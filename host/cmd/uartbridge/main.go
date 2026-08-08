// Command uartbridge listens on stable public ports and backend ports.
// the master dials backend ports; clients (e.g. telnet) connect to public ports.
// Either side may reconnect independently.
package main

import (
	"flag"
	"log"
	"net"
	"os"
	"os/signal"
	"strconv"
	"sync"
	"syscall"
)

func main() {
	daemonizeLauncher()
	writeDaemonPidFile()
	if os.Getenv("UARTBRIDGE_DAEMON") != "" {
		signal.Ignore(os.Interrupt)
	}

	var (
		publicBase  int
		backendBase int
		n           int
	)
	flag.IntVar(&publicBase, "public-base", 24000, "telnet listen port for UART0; UARTn uses public-base+n")
	flag.IntVar(&backendBase, "backend-base", 23000, "epoch20 dial/listen port for UART0; UARTn uses backend-base+n")
	flag.IntVar(&n, "n", 9, "number of bridge pairs (UART0..UARTn-1)")
	flag.Parse()

	/* Ignore SIGHUP so closing a terminal tab or IDE task does not take down the bridge.
	   (nohup also does this when spawning from a shell; Go resets handlers so we set it here.) */
	signal.Ignore(syscall.SIGHUP)

	if n <= 0 || n > 32 {
		log.Fatalf("invalid -n %d", n)
	}

	for i := 0; i < n; i++ {
		ch := &bridgeChannel{
			idx:      i,
			pubAddr:  net.JoinHostPort("", strconv.Itoa(publicBase+i)),
			backAddr: net.JoinHostPort("", strconv.Itoa(backendBase+i)),
		}
		go ch.run()
	}

	log.Printf("uartbridge: %d channels public=%d..%d backend=%d..%d (SIGINT exits)",
		n, publicBase, publicBase+n-1, backendBase, backendBase+n-1)

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt, syscall.SIGTERM)
	<-sig
}

type bridgeChannel struct {
	idx      int
	pubAddr  string
	backAddr string

	mu   sync.Mutex
	pub  net.Conn
	back net.Conn
	/* Guest→telnet bytes received on backend before any public client connects. */
	pending []byte
}

const maxPendingFromGuest = 8192

func (c *bridgeChannel) run() {
	pubLn, err := net.Listen("tcp", c.pubAddr)
	if err != nil {
		log.Printf("uart%d public listen %s: %v", c.idx, c.pubAddr, err)
		return
	}
	backLn, err := net.Listen("tcp", c.backAddr)
	if err != nil {
		log.Printf("uart%d backend listen %s: %v", c.idx, c.backAddr, err)
		_ = pubLn.Close()
		return
	}
	log.Printf("uart%d public=%s backend=%s", c.idx, c.pubAddr, c.backAddr)
	go c.acceptLoop(pubLn, true)
	go c.acceptLoop(backLn, false)
}

func (c *bridgeChannel) acceptLoop(ln net.Listener, isPublic bool) {
	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Printf("uart%d accept error: %v", c.idx, err)
			return
		}
		c.attach(conn, isPublic)
	}
}

func (c *bridgeChannel) attach(conn net.Conn, isPublic bool) {
	c.mu.Lock()
	if isPublic {
		if c.pub != nil {
			_ = c.pub.Close()
		}
		c.pub = conn
		log.Printf("uart%d public connected %s", c.idx, conn.RemoteAddr().String())
		pend := append([]byte(nil), c.pending...)
		c.pending = c.pending[:0]
		c.mu.Unlock()
		if len(pend) > 0 {
			if _, werr := conn.Write(pend); werr != nil {
				log.Printf("uart%d flush buffered guest→client: %v", c.idx, werr)
			}
		}
		go c.readLoop(conn, true)
		return
	}
	if c.back != nil {
		_ = c.back.Close()
	}
	c.back = conn
	log.Printf("uart%d backend connected %s", c.idx, conn.RemoteAddr().String())
	go c.readLoop(conn, false)
	c.mu.Unlock()
}

func (c *bridgeChannel) readLoop(conn net.Conn, isPublic bool) {
	buf := make([]byte, 4096)
	for {
		n, err := conn.Read(buf)
		if err != nil {
			c.detach(conn, isPublic)
			_ = conn.Close()
			return
		}
		if !isPublic {
			c.forwardGuestToClient(buf[:n])
			continue
		}
		dst := c.peer(isPublic)
		if dst == nil {
			continue
		}
		if _, werr := dst.Write(buf[:n]); werr != nil {
			c.detach(dst, !isPublic)
			_ = dst.Close()
		}
	}
}

/* Data read from epoch20 (backend): forward to telnet (public), or buffer until someone connects. */
func (c *bridgeChannel) forwardGuestToClient(data []byte) {
	c.mu.Lock()
	pub := c.pub
	if pub != nil {
		c.mu.Unlock()
		if _, werr := pub.Write(data); werr != nil {
			log.Printf("uart%d guest→client write: %v", c.idx, werr)
			c.detach(pub, true)
			_ = pub.Close()
		}
		return
	}
	c.pending = append(c.pending, data...)
	if len(c.pending) > maxPendingFromGuest {
		drop := len(c.pending) - maxPendingFromGuest
		c.pending = c.pending[drop:]
		log.Printf("uart%d pending queue trimmed %d bytes (no telnet yet on %s)", c.idx, drop, c.pubAddr)
	}
	c.mu.Unlock()
}

func (c *bridgeChannel) peer(fromPublic bool) net.Conn {
	c.mu.Lock()
	defer c.mu.Unlock()
	if fromPublic {
		return c.back
	}
	return c.pub
}

func (c *bridgeChannel) detach(conn net.Conn, isPublic bool) {
	c.mu.Lock()
	defer c.mu.Unlock()
	if isPublic {
		if c.pub == conn {
			c.pub = nil
		}
		return
	}
	if c.back == conn {
		c.back = nil
	}
}
