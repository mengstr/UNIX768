package main

import (
	"bytes"
	"errors"
	"flag"
	"fmt"
	"io"
	"net"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

const (
	modemSOH    = 0x01
	modemSTX    = 0x02
	modemEOT    = 0x04
	modemACK    = 0x06
	modemNAK    = 0x15
	modemCAN    = 0x18
	modemCRCReq = 'C'
	reqPrefix   = "YSEND "

	modemBlockSize128    = 128
	modemBlockSize1K     = 1024
	modemPad             = 0x1a
	receiverReadyTimeout = 60 * time.Second
	/*
	 * The monitor creates or replaces the destination before acknowledging
	 * metadata.  Directory updates on a populated V7 filesystem can take
	 * several seconds under emulation, so this is a transfer deadline rather
	 * than an interactive serial-character timeout.
	 */
	transferReplyTimeout   = 10 * time.Second
	requestLineReadTimeout = 10 * time.Second

	sdImageSize   = 1440 * 1024
	sdSectorSize  = 512
	sdMagic0      = 0xe7
	sdMagic1      = 0x68
	sdMagic2Req   = 0x53
	sdMagic2Resp  = 0x73
	sdMagic3      = 0x44
	sdVersion     = 0x01
	sdOpRead      = 'R'
	sdOpWrite     = 'W'
	sdOpInfo      = 'I'
	sdStatusOK    = 0
	sdStatusIO    = 5
	sdStatusNXIO  = 6
	sdStatusINVAL = 22
)

type mapFlags []string

var errMissingHostFile = errors.New("missing host file")

func crc16Update(crc uint16, ch byte) uint16 {
	crc ^= uint16(ch) << 8
	for i := 0; i < 8; i++ {
		if (crc & 0x8000) != 0 {
			crc = (crc << 1) ^ 0x1021
		} else {
			crc <<= 1
		}
	}
	return crc
}

func crc16(buf []byte) uint16 {
	var crc uint16
	for _, ch := range buf {
		crc = crc16Update(crc, ch)
	}
	return crc
}

func readByte(conn net.Conn, timeout time.Duration) (byte, error) {
	var b [1]byte

	if err := conn.SetReadDeadline(time.Now().Add(timeout)); err != nil {
		return 0, err
	}
	if _, err := conn.Read(b[:]); err != nil {
		return 0, err
	}
	return b[0], nil
}

func writeAll(conn net.Conn, buf []byte) error {
	if err := conn.SetWriteDeadline(time.Now().Add(transferReplyTimeout)); err != nil {
		return err
	}
	for len(buf) > 0 {
		n, err := conn.Write(buf)
		if err != nil {
			return err
		}
		buf = buf[n:]
	}
	return nil
}

func waitReceiver(conn net.Conn, timeout time.Duration) error {
	for {
		ch, err := readByte(conn, timeout)
		if err != nil {
			return err
		}
		switch ch {
		case modemCRCReq:
			return nil
		case modemNAK:
			return fmt.Errorf("receiver requested checksum mode, not supported")
		case modemCAN:
			return fmt.Errorf("receiver canceled")
		}
	}
}

func cancelPendingTransfer(conn net.Conn) {
	/*
	 * Unix-side rx sends "YSEND <path>\n" and then immediately waits for
	 * YMODEM by sending CRCREQ.  If the requested host file is missing, the
	 * auto server must still consume that CRCREQ and answer with CAN;
	 * otherwise rx blocks waiting for metadata and the next waitAndServe loop
	 * sees the stale CRCREQ as line noise.
	 */
	_ = waitReceiver(conn, transferReplyTimeout)
	_ = writeAll(conn, []byte{modemCAN})
}

func ensureDiskImage(path string) error {
	dir := filepath.Dir(path)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return err
	}
	f, err := os.OpenFile(path, os.O_RDWR|os.O_CREATE, 0666)
	if err != nil {
		return err
	}
	defer f.Close()
	st, err := f.Stat()
	if err != nil {
		return err
	}
	if st.Size() == sdImageSize {
		return nil
	}
	if st.Size() > sdImageSize {
		return fmt.Errorf("%s is %d bytes, want %d", path, st.Size(), sdImageSize)
	}
	return f.Truncate(sdImageSize)
}

func readExact(conn net.Conn, n int, timeout time.Duration) ([]byte, error) {
	buf := make([]byte, n)
	if err := conn.SetReadDeadline(time.Now().Add(timeout)); err != nil {
		return nil, err
	}
	_, err := io.ReadFull(conn, buf)
	return buf, err
}

func put16(buf []byte, off int, v uint16) {
	buf[off] = byte(v >> 8)
	buf[off+1] = byte(v)
}

func put32(buf []byte, off int, v uint32) {
	buf[off] = byte(v >> 24)
	buf[off+1] = byte(v >> 16)
	buf[off+2] = byte(v >> 8)
	buf[off+3] = byte(v)
}

func get16(buf []byte, off int) uint16 {
	return uint16(buf[off])<<8 | uint16(buf[off+1])
}

func get32(buf []byte, off int) uint32 {
	return uint32(buf[off])<<24 | uint32(buf[off+1])<<16 | uint32(buf[off+2])<<8 | uint32(buf[off+3])
}

func sendSDResponse(conn net.Conn, status byte, count uint16, payload []byte) error {
	resp := make([]byte, 0, 10+len(payload))
	resp = append(resp, sdMagic0, sdMagic1, sdMagic2Resp, sdMagic3, sdVersion, status)
	tmp := make([]byte, 2)
	put16(tmp, 0, count)
	resp = append(resp, tmp...)
	resp = append(resp, payload...)
	crc := crc16(resp[4:])
	resp = append(resp, byte(crc>>8), byte(crc))
	return writeAll(conn, resp)
}

func sdOpName(op byte) string {
	switch op {
	case sdOpRead:
		return "read"
	case sdOpWrite:
		return "write"
	case sdOpInfo:
		return "info"
	default:
		return fmt.Sprintf("op-%02x", op)
	}
}

func logSDRequest(op byte, unit byte, lba uint32, count uint16, status byte) {
	if op == sdOpInfo {
		fmt.Printf("sd: %s unit=%d status=%d\n", sdOpName(op), unit, status)
		return
	}
	fmt.Printf("sd: %s unit=%d lba=%d count=%d status=%d\n",
		sdOpName(op), unit, lba, count, status)
}

func handleSDRequest(conn net.Conn, diskPath string) error {
	rest, err := readExact(conn, 11, transferReplyTimeout)
	if err != nil {
		return err
	}
	req := append([]byte{sdMagic0, sdMagic1, sdMagic2Req, sdMagic3}, rest...)
	if rest[0] != sdVersion {
		logSDRequest(0, 0, 0, 0, sdStatusINVAL)
		return sendSDResponse(conn, sdStatusINVAL, 0, nil)
	}
	wantCRC := get16(req, len(req)-2)
	if crc16(req[4:len(req)-2]) != wantCRC {
		op := rest[1]
		unit := rest[2]
		lba := get32(rest, 3)
		count := get16(rest, 7)
		logSDRequest(op, unit, lba, count, sdStatusINVAL)
		return sendSDResponse(conn, sdStatusINVAL, 0, nil)
	}

	op := rest[1]
	unit := rest[2]
	lba := get32(rest, 3)
	count := get16(rest, 7)
	if unit != 0 {
		logSDRequest(op, unit, lba, count, sdStatusNXIO)
		return sendSDResponse(conn, sdStatusNXIO, 0, nil)
	}
	if count != 1 && op != sdOpInfo {
		logSDRequest(op, unit, lba, count, sdStatusINVAL)
		return sendSDResponse(conn, sdStatusINVAL, 0, nil)
	}

	switch op {
	case sdOpInfo:
		if err := ensureDiskImage(diskPath); err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		payload := make([]byte, 8)
		put32(payload, 0, uint32(sdImageSize/sdSectorSize))
		put32(payload, 4, sdSectorSize)
		logSDRequest(op, unit, lba, count, sdStatusOK)
		return sendSDResponse(conn, sdStatusOK, 1, payload)
	case sdOpRead:
		if int64(lba) >= int64(sdImageSize/sdSectorSize) {
			logSDRequest(op, unit, lba, count, sdStatusINVAL)
			return sendSDResponse(conn, sdStatusINVAL, 0, nil)
		}
		if err := ensureDiskImage(diskPath); err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		f, err := os.Open(diskPath)
		if err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		defer f.Close()
		payload := make([]byte, sdSectorSize)
		_, err = f.ReadAt(payload, int64(lba)*sdSectorSize)
		if err != nil && !errors.Is(err, io.EOF) {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		logSDRequest(op, unit, lba, count, sdStatusOK)
		return sendSDResponse(conn, sdStatusOK, 1, payload)
	case sdOpWrite:
		if int64(lba) >= int64(sdImageSize/sdSectorSize) {
			_, _ = readExact(conn, sdSectorSize+2, transferReplyTimeout)
			logSDRequest(op, unit, lba, count, sdStatusINVAL)
			return sendSDResponse(conn, sdStatusINVAL, 0, nil)
		}
		packet, err := readExact(conn, sdSectorSize+2, transferReplyTimeout)
		if err != nil {
			return err
		}
		wantCRC = get16(packet, sdSectorSize)
		gotCRC := crc16(packet[:sdSectorSize])
		if gotCRC != wantCRC {
			logSDRequest(op, unit, lba, count, sdStatusINVAL)
			return sendSDResponse(conn, sdStatusINVAL, 0, nil)
		}
		if err := ensureDiskImage(diskPath); err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		f, err := os.OpenFile(diskPath, os.O_RDWR, 0666)
		if err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		defer f.Close()
		if _, err := f.WriteAt(packet[:sdSectorSize], int64(lba)*sdSectorSize); err != nil {
			logSDRequest(op, unit, lba, count, sdStatusIO)
			return sendSDResponse(conn, sdStatusIO, 0, nil)
		}
		logSDRequest(op, unit, lba, count, sdStatusOK)
		return sendSDResponse(conn, sdStatusOK, 1, nil)
	default:
		logSDRequest(op, unit, lba, count, sdStatusINVAL)
		return sendSDResponse(conn, sdStatusINVAL, 0, nil)
	}
}

func waitAndServe(conn net.Conn, diskPath string) (string, error) {
	var line bytes.Buffer
	magicPos := 0
	magic := []byte{sdMagic0, sdMagic1, sdMagic2Req, sdMagic3}

	for {
		ch, err := readByte(conn, requestLineReadTimeout)
		if err != nil {
			return "", err
		}
		/*
		 * A failed monitor receive can leave its YMODEM cancellation ahead of
		 * the next textual request.  Treat transfer control bytes as protocol
		 * boundaries, not as part of a request line, so CAN followed by a new
		 * YSEND request resynchronizes immediately.
		 */
		switch ch {
		case modemSOH, modemSTX, modemEOT, modemACK, modemNAK, modemCAN:
			line.Reset()
			magicPos = 0
			continue
		}
		if ch == modemCRCReq && line.Len() == 0 {
			continue
		}
		if ch == magic[magicPos] {
			magicPos++
			if magicPos == len(magic) {
				if err := handleSDRequest(conn, diskPath); err != nil {
					return "", err
				}
				magicPos = 0
				line.Reset()
			}
			continue
		}
		if magicPos > 0 {
			for i := 0; i < magicPos; i++ {
				line.WriteByte(magic[i])
			}
			magicPos = 0
		}
		if ch == '\r' {
			continue
		}
		if ch == '\n' {
			s := line.String()
			line.Reset()
			if strings.HasPrefix(s, reqPrefix) {
				req := strings.TrimSpace(strings.TrimPrefix(s, reqPrefix))
				if req == "" {
					return "", fmt.Errorf("empty request")
				}
				return req, nil
			}
			if s != "" {
				fmt.Fprintf(os.Stderr, "ysend: ignored line %q\n", s)
			}
			continue
		}
		line.WriteByte(ch)
	}
}

func resolveRequestedPath(rootDir, req string) (string, error) {
	req = strings.TrimPrefix(req, "/")
	if req == "" {
		return "", fmt.Errorf("empty request path")
	}
	req = filepath.Clean(req)
	if req == "." || strings.HasPrefix(req, "../") {
		return "", fmt.Errorf("bad request path %q", req)
	}
	path := filepath.Join(rootDir, req)
	st, err := os.Stat(path)
	if err != nil {
		if os.IsNotExist(err) {
			return "", fmt.Errorf("%w: %q", errMissingHostFile, path)
		}
		return "", err
	}
	if st.IsDir() {
		return "", fmt.Errorf("requested host path %q is a directory", path)
	}
	return path, nil
}

func sendBlockWithPad(conn net.Conn, marker byte, block byte, payload []byte, blockSize int, pad byte) error {
	packet := make([]byte, 0, 3+blockSize+2)
	buf := make([]byte, blockSize)
	copy(buf, payload)
	for i := len(payload); i < len(buf); i++ {
		buf[i] = pad
	}

	packet = append(packet, marker, block, 255-block)
	packet = append(packet, buf...)
	crc := crc16(buf)
	packet = append(packet, byte(crc>>8), byte(crc))

	for attempt := 0; attempt < 10; attempt++ {
		if err := writeAll(conn, packet); err != nil {
			return err
		}

		reply, err := readByte(conn, transferReplyTimeout)
		if err != nil {
			return err
		}
		switch reply {
		case modemACK:
			return nil
		case modemNAK:
			continue
		case modemCAN:
			return fmt.Errorf("receiver canceled during block %d", int(block))
		default:
			return fmt.Errorf("unexpected reply %#02x during block %d", reply, int(block))
		}
	}

	return fmt.Errorf("too many retries on block %d", int(block))
}

func sendBlock(conn net.Conn, block byte, payload []byte) error {
	return sendBlockWithPad(conn, modemSTX, block, payload, modemBlockSize1K, modemPad)
}

func sendMetadata(conn net.Conn, name string, size int64) error {
	payload := make([]byte, 0, modemBlockSize128)
	meta := []byte(name)
	meta = append(meta, 0)
	meta = append(meta, []byte(strconv.FormatInt(size, 10))...)
	meta = append(meta, 0)
	if len(meta) > modemBlockSize128 {
		return fmt.Errorf("metadata too long")
	}
	payload = append(payload, meta...)
	if err := sendBlockWithPad(conn, modemSOH, 0, payload, modemBlockSize128, 0x00); err != nil {
		return err
	}
	if err := waitReceiver(conn, transferReplyTimeout); err != nil {
		return fmt.Errorf("wait data phase: %w", err)
	}
	return nil
}

func sendEOT(conn net.Conn) error {
	for attempt := 0; attempt < 10; attempt++ {
		if err := writeAll(conn, []byte{modemEOT}); err != nil {
			return err
		}
		reply, err := readByte(conn, transferReplyTimeout)
		if err != nil {
			return err
		}
		switch reply {
		case modemACK:
			return nil
		case modemNAK:
			continue
		case modemCAN:
			return fmt.Errorf("receiver canceled during eot")
		default:
			return fmt.Errorf("unexpected reply %#02x during eot", reply)
		}
	}

	return fmt.Errorf("too many retries on eot")
}

func sendTrailer(conn net.Conn) error {
	if err := waitReceiver(conn, transferReplyTimeout); err != nil {
		return fmt.Errorf("wait trailer phase: %w", err)
	}
	if err := sendBlockWithPad(conn, modemSOH, 0, nil, modemBlockSize128, 0x00); err != nil {
		return err
	}
	return nil
}

func sendFile(conn net.Conn, path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("read %q: %w", path, err)
	}

	if err := waitReceiver(conn, receiverReadyTimeout); err != nil {
		return fmt.Errorf("wait receiver: %w", err)
	}

	if err := sendMetadata(conn, filepath.Base(path), int64(len(data))); err != nil {
		return fmt.Errorf("metadata: %w", err)
	}

	block := byte(1)
	for offset := 0; offset < len(data); offset += modemBlockSize1K {
		end := offset + modemBlockSize1K
		if end > len(data) {
			end = len(data)
		}
		if err := sendBlock(conn, block, data[offset:end]); err != nil {
			return err
		}
		block++
	}

	if err := sendEOT(conn); err != nil {
		return err
	}
	if err := sendTrailer(conn); err != nil {
		return err
	}
	fmt.Printf("sent=%s bytes=%d\n", path, len(data))
	return nil
}

func main() {
	addr := flag.String("addr", "127.0.0.1:24001", "TCP address for UART1 YMODEM receiver")
	auto := flag.Bool("auto", false, "wait for a YSEND <path> request line before sending")
	rootDir := flag.String("root", "build/ramdisk", "filesystem root served in --auto mode")
	diskPath := flag.String("disk-image", "serialdisk.img", "1.44 MB serial disk image served in --auto mode")
	flag.Parse()

	if !*auto && flag.NArg() != 1 {
		fmt.Fprintf(os.Stderr, "usage: %s [-addr host:port] file\n       %s --auto [-addr host:port] [-disk-image file]\n", os.Args[0], os.Args[0])
		os.Exit(2)
	}
	if *auto && flag.NArg() != 0 {
		fmt.Fprintf(os.Stderr, "usage: %s --auto [-addr host:port] [-disk-image file]\n", os.Args[0])
		os.Exit(2)
	}

	path := ""

	if *auto {
		if err := ensureDiskImage(*diskPath); err != nil {
			fmt.Fprintf(os.Stderr, "ysend: disk image: %v\n", err)
			os.Exit(1)
		}
		lastStatus := ""
		for {
			conn, err := net.Dial("tcp", *addr)
			if err != nil {
				msg := fmt.Sprintf("ysend: dial %s: %v", *addr, err)
				if msg != lastStatus {
					fmt.Fprintln(os.Stderr, msg)
					lastStatus = msg
				}
				time.Sleep(2 * time.Second)
				continue
			}
			lastStatus = ""

			for {
				req, err := waitAndServe(conn, *diskPath)
				if err != nil {
					_ = conn.Close()
					if ne, ok := err.(net.Error); ok && ne.Timeout() {
						if lastStatus != "timeout" {
							fmt.Fprintf(os.Stderr, "ysend: wait request timeout; reconnecting\n")
							lastStatus = "timeout"
						}
						break
					}
					if errors.Is(err, io.EOF) {
						if lastStatus != "eof" {
							fmt.Fprintf(os.Stderr, "ysend: connection closed by peer; reconnecting\n")
							lastStatus = "eof"
						}
						time.Sleep(1 * time.Second)
						break
					}
					if errors.Is(err, net.ErrClosed) {
						if lastStatus != "closed" {
							fmt.Fprintf(os.Stderr, "ysend: connection closed; reconnecting\n")
							lastStatus = "closed"
						}
						time.Sleep(1 * time.Second)
						break
					}
					msg := fmt.Sprintf("ysend: wait request: %v", err)
					if msg != lastStatus {
						fmt.Fprintln(os.Stderr, msg)
						lastStatus = msg
					}
					time.Sleep(1 * time.Second)
					break
				}
				lastStatus = ""
				path, err = resolveRequestedPath(*rootDir, req)
				if err != nil {
					fmt.Fprintf(os.Stderr, "ysend: %v\n", err)
					cancelPendingTransfer(conn)
					if errors.Is(err, errMissingHostFile) {
						break
					}
					continue
				}
				if err := sendFile(conn, path); err != nil {
					fmt.Fprintf(os.Stderr, "ysend: send %s: %v\n", path, err)
					break
				}
			}
			_ = conn.Close()
		}
	}

	path = flag.Arg(0)
	conn, err := net.Dial("tcp", *addr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ysend: dial %s: %v\n", *addr, err)
		os.Exit(1)
	}
	defer conn.Close()
	if err := sendFile(conn, path); err != nil {
		fmt.Fprintf(os.Stderr, "ysend: %v\n", err)
		os.Exit(1)
	}
}
