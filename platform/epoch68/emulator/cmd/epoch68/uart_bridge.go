package main

/*
#include <stdint.h>
extern void epoch68_dump_kstack_monitor(void);
*/
import "C"

import (
	"bytes"
	"fmt"
	"math/rand"
	"net"
	"os"
	"sync"
	"time"
)

const uartRXQueueSize = 4096
const uartTXQueueSize = 4096
const uartTriggerWindow = 256
const uartBridgeWriteTimeout = 2 * time.Second
const uartYmodemChannel = 1

var uartRXQueues [uartChannels][]byte
var uartTXQueues [uartChannels]chan byte
var uartTXWorkersOnce sync.Once
var uartTriggerNeedles [uartChannels][]byte
var uartTriggerScripts [uartChannels][][]byte
var uartTriggerIndex [uartChannels]int
var uartTXRecent [uartChannels][]byte
var uartTriggerStream [uartChannels]bool
var uartTriggerLoop [uartChannels]bool
var uartTriggerActive [uartChannels]bool
var uartTriggerInterval time.Duration
var uartTriggerJitterPct int
var uartBridgeTXLines [uartChannels][]byte
var uartPanicRecent [uartChannels][]byte
var uartPanicTraceDumped bool
var uartTerminalCharDelay time.Duration
var uartRXTrace bool

var uartPanicNeedle = []byte("panic:")

func setUARTTerminalBaud(baud int) {
	if baud <= 0 {
		uartTerminalCharDelay = 0
		return
	}
	uartTerminalCharDelay = time.Second * 10 / time.Duration(baud)
}

func setUARTBridgeRXTrace(enabled bool) {
	uartRXTrace = enabled
}

func traceUARTBridgeRX(ch int, where string, b byte, pc uint) {
	if !uartRXTrace {
		return
	}
	if b >= 0x20 && b < 0x7f {
		if pc != 0 {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d rx %s pc=%06x 0x%02x %q\n", ch, where, pc, b, string([]byte{b}))
			return
		}
		fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d rx %s 0x%02x %q\n", ch, where, b, string([]byte{b}))
		return
	}
	if pc != 0 {
		fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d rx %s pc=%06x 0x%02x\n", ch, where, pc, b)
		return
	}
	fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d rx %s 0x%02x\n", ch, where, b)
}

func throttleUARTTerminalTX(ch int) {
	if ch == uartYmodemChannel || uartTerminalCharDelay <= 0 {
		return
	}
	time.Sleep(uartTerminalCharDelay)
}

func startUARTTXWorkers() {
	for ch := range uartTXQueues {
		uartTXQueues[ch] = make(chan byte, uartTXQueueSize)
		go runUARTTXWorker(ch, uartTXQueues[ch])
	}
}

func runUARTTXWorker(ch int, queue <-chan byte) {
	for b := range queue {
		throttleUARTTerminalTX(ch)

		uartMu.Lock()
		conn := connForUART(ch)
		uartMu.Unlock()
		if conn != nil {
			_ = conn.SetWriteDeadline(time.Now().Add(uartBridgeWriteTimeout))
			if _, err := conn.Write([]byte{b}); err == nil {
				logUARTBridgeTX(ch, b, true)
				continue
			} else {
				fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d bridge write failed: %v\n", ch, err)
			}

			uartMu.Lock()
			if connForUART(ch) == conn {
				_ = conn.Close()
				uartConns[ch] = nil
			}
			uartMu.Unlock()
		}

		logUARTBridgeTX(ch, b, false)
		_, _ = os.Stdout.Write([]byte{b})
	}
}

//export epoch68_uart_tx
func epoch68_uart_tx(channel C.uint, value C.uint, pc C.uint) {
	ch := int(channel)
	bv := byte(value)

	traceUARTBridgeRX(ch, "tx", bv, uint(pc))
	uartMu.Lock()
	dumpDebugState := false
	if ch >= 0 && ch < len(uartTXRecent) {
		uartPanicRecent[ch] = append(uartPanicRecent[ch], bv)
		if len(uartPanicRecent[ch]) > uartTriggerWindow {
			uartPanicRecent[ch] = uartPanicRecent[ch][len(uartPanicRecent[ch])-uartTriggerWindow:]
		}
		if !uartPanicTraceDumped && bytes.Contains(uartPanicRecent[ch], uartPanicNeedle) {
			uartPanicTraceDumped = true
			dumpDebugState = true
		}

		uartTXRecent[ch] = append(uartTXRecent[ch], bv)
		if len(uartTXRecent[ch]) > uartTriggerWindow {
			uartTXRecent[ch] = uartTXRecent[ch][len(uartTXRecent[ch])-uartTriggerWindow:]
		}
		if len(uartTriggerNeedles[ch]) > 0 &&
			len(uartTriggerScripts[ch]) > 0 &&
			bytes.Contains(uartTXRecent[ch], uartTriggerNeedles[ch]) {
			if uartTriggerStream[ch] && uartTriggerInterval > 0 {
				if !uartTriggerActive[ch] {
					uartTriggerActive[ch] = true
					script := make([][]byte, len(uartTriggerScripts[ch]))
					for i := range uartTriggerScripts[ch] {
						script[i] = append([]byte(nil), uartTriggerScripts[ch][i]...)
					}
					uartTXRecent[ch] = uartTXRecent[ch][:0]
					go streamUARTTrigger(ch, script)
				}
			} else {
				if uartTriggerIndex[ch] >= len(uartTriggerScripts[ch]) {
					uartTriggerIndex[ch] = 0
				}
				for _, qb := range uartTriggerScripts[ch][uartTriggerIndex[ch]] {
					if len(uartRXQueues[ch]) >= uartRXQueueSize {
						uartRXQueues[ch] = uartRXQueues[ch][1:]
					}
					uartRXQueues[ch] = append(uartRXQueues[ch], qb)
				}
				uartTriggerIndex[ch]++
				uartTXRecent[ch] = uartTXRecent[ch][:0]
			}
		}
	}
	uartMu.Unlock()

	/*
	 * MMIO writes execute on the emulated CPU goroutine.  Never let a slow
	 * terminal, TCP peer, or stdout stall that goroutine: the per-UART worker
	 * owns all host I/O.
	 */
	uartTXWorkersOnce.Do(startUARTTXWorkers)
	if ch >= 0 && ch < len(uartTXQueues) {
		select {
		case uartTXQueues[ch] <- bv:
		default:
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d transmit queue full; dropping byte 0x%02x\n", ch, bv)
		}
	}
	if dumpDebugState {
		C.epoch68_dump_kstack_monitor()
	}
}

func logUARTBridgeTX(ch int, b byte, bridged bool) {
	_ = ch
	_ = b
	_ = bridged
}

//export epoch68_uart_rx_available
func epoch68_uart_rx_available(channel C.uint) C.int {
	ch := int(channel)

	uartMu.Lock()
	defer uartMu.Unlock()

	if ch < 0 || ch >= len(uartRXQueues) {
		return 0
	}
	if len(uartRXQueues[ch]) > 0 {
		return 1
	}
	return 0
}

//export epoch68_uart_rx_read
func epoch68_uart_rx_read(channel C.uint) C.uint {
	ch := int(channel)

	uartMu.Lock()
	defer uartMu.Unlock()

	if ch < 0 || ch >= len(uartRXQueues) || len(uartRXQueues[ch]) == 0 {
		return 0
	}

	b := uartRXQueues[ch][0]
	uartRXQueues[ch] = uartRXQueues[ch][1:]
	traceUARTBridgeRX(ch, "dequeue", b, 0)
	return C.uint(b)
}

func connForUART(ch int) net.Conn {
	if ch < 0 || ch >= len(uartConns) {
		return nil
	}
	return uartConns[ch]
}

func queueUARTByte(ch int, b byte) {
	uartMu.Lock()
	defer uartMu.Unlock()

	if ch < 0 || ch >= len(uartRXQueues) {
		return
	}
	if len(uartRXQueues[ch]) >= uartRXQueueSize {
		uartRXQueues[ch] = uartRXQueues[ch][1:]
	}
	uartRXQueues[ch] = append(uartRXQueues[ch], b)
}

func splitUARTTriggerScript(script []byte) [][]byte {
	lines := bytes.SplitAfter(script, []byte{'\n'})
	chunks := make([][]byte, 0, len(lines))
	for _, line := range lines {
		if len(line) == 0 {
			continue
		}
		chunk := make([]byte, len(line))
		copy(chunk, line)
		chunks = append(chunks, chunk)
	}
	if len(script) > 0 && script[len(script)-1] != '\n' {
		last := chunks[len(chunks)-1]
		if len(last) == 0 || last[len(last)-1] != '\n' {
			last = append(last, '\n')
			chunks[len(chunks)-1] = last
		}
	}
	return chunks
}

func setUARTTriggerScript(ch int, needle []byte, script []byte, stream bool, loop bool) {
	uartMu.Lock()
	defer uartMu.Unlock()

	if ch < 0 || ch >= len(uartTriggerNeedles) {
		return
	}
	uartTriggerNeedles[ch] = append(uartTriggerNeedles[ch][:0], needle...)
	uartTriggerScripts[ch] = splitUARTTriggerScript(script)
	uartTriggerIndex[ch] = 0
	uartTriggerStream[ch] = stream
	uartTriggerLoop[ch] = loop
	uartTriggerActive[ch] = false
	uartTXRecent[ch] = uartTXRecent[ch][:0]
}

func setUARTTriggerTiming(interval time.Duration, jitterPct int) {
	if jitterPct < 0 {
		jitterPct = 0
	}
	uartTriggerInterval = interval
	uartTriggerJitterPct = jitterPct
}

func queueUARTChunk(ch int, chunk []byte) {
	uartMu.Lock()
	defer uartMu.Unlock()

	if ch < 0 || ch >= len(uartRXQueues) {
		return
	}
	for _, b := range chunk {
		if len(uartRXQueues[ch]) >= uartRXQueueSize {
			uartRXQueues[ch] = uartRXQueues[ch][1:]
		}
		uartRXQueues[ch] = append(uartRXQueues[ch], b)
	}
}

func streamUARTTrigger(ch int, script [][]byte) {
	loop := false

	uartMu.Lock()
	if ch >= 0 && ch < len(uartTriggerLoop) {
		loop = uartTriggerLoop[ch]
	}
	uartMu.Unlock()

	defer func() {
		uartMu.Lock()
		if ch >= 0 && ch < len(uartTriggerActive) {
			uartTriggerActive[ch] = false
		}
		uartMu.Unlock()
	}()

	first := true
	for {
		for _, chunk := range script {
			if !first {
				delay := uartTriggerInterval
				if delay > 0 && uartTriggerJitterPct > 0 {
					jitter := int64(delay) * int64(uartTriggerJitterPct) / 100
					if jitter > 0 {
						delta := rand.Int63n((jitter*2)+1) - jitter
						delay += time.Duration(delta)
					}
				}
				if delay > 0 {
					time.Sleep(delay)
				}
			}
			first = false
			queueUARTChunk(ch, chunk)
		}
		if !loop {
			return
		}
	}
}

func readUARTBridge(ch int, conn net.Conn) {
	buf := make([]byte, 256)

	for {
		n, err := conn.Read(buf)
		if err != nil {
			uartMu.Lock()
			if connForUART(ch) == conn {
				_ = conn.Close()
				uartConns[ch] = nil
			}
			uartMu.Unlock()
			return
		}
		for i := 0; i < n; i++ {
			traceUARTBridgeRX(ch, "bridge", buf[i], 0)
			queueUARTByte(ch, buf[i])
		}
	}
}
