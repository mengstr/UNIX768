package main

import (
	"bufio"
	"io"
	"net"
	"testing"
	"time"
)

func listenTCP(t *testing.T) net.Listener {
	t.Helper()
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	t.Cleanup(func() { _ = ln.Close() })
	return ln
}

func acceptWithTimeout(t *testing.T, ln net.Listener) net.Conn {
	t.Helper()
	tcp := ln.(*net.TCPListener)
	if err := tcp.SetDeadline(time.Now().Add(2 * time.Second)); err != nil {
		t.Fatal(err)
	}
	conn, err := tcp.Accept()
	if err != nil {
		t.Fatalf("accept: %v", err)
	}
	t.Cleanup(func() { _ = conn.Close() })
	return conn
}

func TestOneRemoteConnectionPerDial(t *testing.T) {
	uartListener := listenTCP(t)
	remoteListener := listenTCP(t)

	done := make(chan error, 1)
	go func() {
		uart, err := net.Dial("tcp", uartListener.Addr().String())
		if err != nil {
			done <- err
			return
		}
		g := gateway{
			uart:       uart,
			remoteAddr: remoteListener.Addr().String(),
			timeout:    time.Second,
			events:     make(chan readEvent, 16),
		}
		done <- g.run()
	}()

	v7 := acceptWithTimeout(t, uartListener)
	if _, err := v7.Write([]byte("5551212-<")); err != nil {
		t.Fatal(err)
	}
	first := acceptWithTimeout(t, remoteListener)
	if _, err := first.Write([]byte("login:")); err != nil {
		t.Fatal(err)
	}
	got := make([]byte, len("login:"))
	if _, err := io.ReadFull(v7, got); err != nil {
		t.Fatal(err)
	}
	if string(got) != "login:" {
		t.Fatalf("V7 received %q", got)
	}

	if _, err := v7.Write([]byte("unix768\x04\n\x04\n")); err != nil {
		t.Fatal(err)
	}
	reader := bufio.NewReader(first)
	forwarded := make([]byte, len("unix768\x04\n\x04\n"))
	if _, err := io.ReadFull(reader, forwarded); err != nil {
		t.Fatal(err)
	}
	_ = first.SetReadDeadline(time.Now().Add(time.Second))
	if _, err := reader.ReadByte(); err == nil {
		t.Fatal("first remote connection remained open after V7 EOT")
	}

	if _, err := v7.Write([]byte("9-<")); err != nil {
		t.Fatal(err)
	}
	second := acceptWithTimeout(t, remoteListener)
	if second.RemoteAddr() == first.RemoteAddr() {
		t.Fatal("second dial reused the first TCP connection")
	}

	_ = v7.Close()
	select {
	case <-done:
	case <-time.After(2 * time.Second):
		t.Fatal("gateway did not stop after UART disconnect")
	}
}

func TestStartupDrainDiscardsStaleDial(t *testing.T) {
	uartGateway, uartV7 := net.Pipe()
	defer uartV7.Close()
	remoteListener := listenTCP(t)

	done := make(chan error, 1)
	g := gateway{
		uart:         uartGateway,
		remoteAddr:   remoteListener.Addr().String(),
		timeout:      time.Second,
		startupDrain: 50 * time.Millisecond,
		events:       make(chan readEvent, 16),
	}
	go func() { done <- g.run() }()

	if _, err := uartV7.Write([]byte("\n\n1-<")); err != nil {
		t.Fatal(err)
	}
	tcp := remoteListener.(*net.TCPListener)
	if err := tcp.SetDeadline(time.Now().Add(100 * time.Millisecond)); err != nil {
		t.Fatal(err)
	}
	if conn, err := tcp.Accept(); err == nil {
		_ = conn.Close()
		t.Fatal("stale startup dial unexpectedly opened a remote connection")
	}

	if _, err := uartV7.Write([]byte("1-<")); err != nil {
		t.Fatal(err)
	}
	if err := tcp.SetDeadline(time.Now().Add(time.Second)); err != nil {
		t.Fatal(err)
	}
	conn, err := tcp.Accept()
	if err != nil {
		t.Fatalf("fresh dial was not accepted: %v", err)
	}
	_ = conn.Close()
	_ = uartV7.Close()
	select {
	case <-done:
	case <-time.After(time.Second):
		t.Fatal("gateway did not stop after UART disconnect")
	}
}
