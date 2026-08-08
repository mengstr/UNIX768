package main

import (
	"io"
	"net"
	"os"
	"path/filepath"
	"testing"
	"time"
)

func TestEnsureDiskImageCreatesParentDirectory(t *testing.T) {
	path := filepath.Join(t.TempDir(), "build", "images", "serialdisk.img")
	if err := ensureDiskImage(path); err != nil {
		t.Fatalf("ensureDiskImage: %v", err)
	}
	st, err := os.Stat(path)
	if err != nil {
		t.Fatalf("stat disk image: %v", err)
	}
	if st.Size() != sdImageSize {
		t.Fatalf("disk image size = %d, want %d", st.Size(), sdImageSize)
	}
}

func TestSendMetadataAllowsSlowDestinationCreate(t *testing.T) {
	server, client := net.Pipe()
	defer server.Close()
	defer client.Close()

	done := make(chan error, 1)
	go func() {
		packet := make([]byte, 3+modemBlockSize128+2)
		if _, err := io.ReadFull(client, packet); err != nil {
			done <- err
			return
		}
		/* Simulate a slow V7 directory update before the monitor's ACK. */
		time.Sleep(1250 * time.Millisecond)
		if _, err := client.Write([]byte{modemACK, modemCRCReq}); err != nil {
			done <- err
			return
		}
		done <- nil
	}()

	if err := sendMetadata(server, "mail", 26996); err != nil {
		t.Fatalf("sendMetadata: %v", err)
	}
	if err := <-done; err != nil {
		t.Fatalf("receiver: %v", err)
	}
}

func TestWaitAndServeResynchronizesAfterCancel(t *testing.T) {
	server, client := net.Pipe()
	defer server.Close()
	defer client.Close()

	go func() {
		_, _ = client.Write([]byte{modemCAN})
		_, _ = client.Write([]byte("YSEND stage1\n"))
	}()

	req, err := waitAndServe(server, "unused.img")
	if err != nil {
		t.Fatalf("waitAndServe: %v", err)
	}
	if req != "stage1" {
		t.Fatalf("request = %q, want %q", req, "stage1")
	}
}

func TestCancelPendingTransferSendsOneCancel(t *testing.T) {
	server, client := net.Pipe()
	done := make(chan struct{})
	go func() {
		cancelPendingTransfer(server)
		_ = server.Close()
		close(done)
	}()

	if _, err := client.Write([]byte{modemCRCReq}); err != nil {
		t.Fatalf("write receiver ready: %v", err)
	}
	got, err := io.ReadAll(client)
	if err != nil {
		t.Fatalf("read cancellation: %v", err)
	}
	if len(got) != 1 || got[0] != modemCAN {
		t.Fatalf("cancellation = %v, want one CAN", got)
	}

	select {
	case <-done:
	case <-time.After(time.Second):
		t.Fatal("cancelPendingTransfer did not return")
	}
}
