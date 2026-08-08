// Command uucp-tcp-gateway presents a small virtual ACU between V7 UUCP and
// a remote UUCP-over-TCP server.  The UART side remains attached to
// uartbridge, while each V7 dial command creates a fresh remote TCP session.
package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"
)

var (
	dialTerminator = []byte("-<")
	hangupSequence = []byte("\x04\n\x04\n")
)

type readEvent struct {
	session uint64
	data    []byte
	err     error
}

type gateway struct {
	uart         net.Conn
	remoteAddr   string
	timeout      time.Duration
	startupDrain time.Duration
	events       chan readEvent

	remote        net.Conn
	remoteSession uint64
	dialBuffer    []byte
	hangupTail    []byte
}

func (g *gateway) drainStartupInput() error {
	if g.startupDrain <= 0 {
		return nil
	}
	timer := time.NewTimer(g.startupDrain)
	defer timer.Stop()
	discarded := 0
	for {
		select {
		case event := <-g.events:
			if event.session != 0 {
				continue
			}
			if event.err != nil {
				return fmt.Errorf("UART connection closed during startup drain: %w", event.err)
			}
			discarded += len(event.data)
			if !timer.Stop() {
				<-timer.C
			}
			timer.Reset(g.startupDrain)
		case <-timer.C:
			if discarded != 0 {
				log.Printf("discarded %d stale UART bytes before enabling ACU dialing", discarded)
			}
			return nil
		}
	}
}

func readConnection(conn net.Conn, session uint64, events chan<- readEvent) {
	buf := make([]byte, 4096)
	for {
		n, err := conn.Read(buf)
		if n > 0 {
			data := append([]byte(nil), buf[:n]...)
			events <- readEvent{session: session, data: data}
		}
		if err != nil {
			events <- readEvent{session: session, err: err}
			return
		}
	}
}

func (g *gateway) connectRemote() error {
	dialer := net.Dialer{Timeout: g.timeout}
	conn, err := dialer.Dial("tcp", g.remoteAddr)
	if err != nil {
		return err
	}
	g.remoteSession++
	g.remote = conn
	g.hangupTail = g.hangupTail[:0]
	log.Printf("remote session %d connected to %s", g.remoteSession, g.remoteAddr)
	go readConnection(conn, g.remoteSession, g.events)
	return nil
}

func (g *gateway) closeRemote(reason string) {
	if g.remote == nil {
		return
	}
	log.Printf("remote session %d closed: %s", g.remoteSession, reason)
	_ = g.remote.Close()
	g.remote = nil
	g.hangupTail = g.hangupTail[:0]
}

func (g *gateway) acceptDialBytes(data []byte) {
	for _, b := range data {
		g.dialBuffer = append(g.dialBuffer, b)
		if len(g.dialBuffer) > 256 {
			g.dialBuffer = g.dialBuffer[len(g.dialBuffer)-256:]
		}
		if !bytes.HasSuffix(g.dialBuffer, dialTerminator) {
			continue
		}
		command := append([]byte(nil), g.dialBuffer[:len(g.dialBuffer)-len(dialTerminator)]...)
		g.dialBuffer = g.dialBuffer[:0]
		log.Printf("V7 ACU dial command %q", command)
		if err := g.connectRemote(); err != nil {
			log.Printf("remote connection failed: %v", err)
		}
		return
	}
}

func (g *gateway) forwardedHangup(data []byte) bool {
	combined := append(g.hangupTail, data...)
	found := bytes.Contains(combined, hangupSequence)
	keep := len(hangupSequence) - 1
	if len(combined) > keep {
		combined = combined[len(combined)-keep:]
	}
	g.hangupTail = append(g.hangupTail[:0], combined...)
	return found
}

func (g *gateway) run() error {
	go readConnection(g.uart, 0, g.events)
	if err := g.drainStartupInput(); err != nil {
		return err
	}
	log.Printf("virtual ACU ready; remote %s is disconnected", g.remoteAddr)
	for event := range g.events {
		if event.session == 0 {
			if event.err != nil {
				return fmt.Errorf("UART connection closed: %w", event.err)
			}
			if g.remote == nil {
				g.acceptDialBytes(event.data)
				continue
			}
			if _, err := g.remote.Write(event.data); err != nil {
				g.closeRemote("write failed: " + err.Error())
				continue
			}
			if g.forwardedHangup(event.data) {
				g.closeRemote("V7 EOT hangup")
			}
			continue
		}

		if event.session != g.remoteSession || g.remote == nil {
			continue
		}
		if len(event.data) > 0 {
			if _, err := g.uart.Write(event.data); err != nil {
				return fmt.Errorf("UART write failed: %w", err)
			}
		}
		if event.err != nil {
			g.closeRemote("peer disconnected")
		}
	}
	return nil
}

func main() {
	var uartAddr string
	var remoteAddr string
	var timeout time.Duration
	var startupDrain time.Duration
	flag.StringVar(&uartAddr, "uart", "127.0.0.1:24005", "uartbridge public UART address")
	flag.StringVar(&remoteAddr, "remote", "198.23.169.200:540", "remote UUCP TCP address")
	flag.DurationVar(&timeout, "connect-timeout", 10*time.Second, "remote connection timeout")
	flag.DurationVar(&startupDrain, "startup-drain", 500*time.Millisecond, "quiet period used to discard stale uartbridge input")
	flag.Parse()

	uart, err := net.DialTimeout("tcp", uartAddr, timeout)
	if err != nil {
		log.Fatalf("connect UART bridge %s: %v", uartAddr, err)
	}
	defer uart.Close()
	log.Printf("connected to UART bridge %s; draining stale input", uartAddr)

	g := gateway{
		uart:         uart,
		remoteAddr:   remoteAddr,
		timeout:      timeout,
		startupDrain: startupDrain,
		events:       make(chan readEvent, 16),
	}
	if err := g.run(); err != nil && err != io.EOF {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
