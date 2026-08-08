package main

import (
	"flag"
	"fmt"
	"os"
	"sort"
)

const (
	ackMagic     = 0x0202
	headerSize   = 20
	sectionSize  = 20
	romSize      = 64 * 1024
	romBase      = 0x00800000
	stackTop     = 0x00040000
	metadataOff  = 1024
	metadataSize = 6 * 4
	reloSize     = 10
	nameSize     = 12
)

type section struct {
	base uint32
	size uint32
	foff uint32
	flen uint32
	lign uint32
}

type header struct {
	flags uint16
	nsect uint16
	nrelo uint16
	nname uint16
	nemit uint32
	nchar uint32
}

type symbol struct {
	name  string
	typ   uint16
	desc  uint16
	value uint32
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: ackimage [-symbols] linked.ack.out [bootrom.bin]\n")
	os.Exit(2)
}

func readHeader(data []byte) header {
	return header{
		flags: get16(data[4:6]),
		nsect: get16(data[6:8]),
		nrelo: get16(data[8:10]),
		nname: get16(data[10:12]),
		nemit: get32(data[12:16]),
		nchar: get32(data[16:20]),
	}
}

func readSection(data []byte, off int) section {
	return section{
		base: get32(data[off:]),
		size: get32(data[off+4:]),
		foff: get32(data[off+8:]),
		flen: get32(data[off+12:]),
		lign: get32(data[off+16:]),
	}
}

func readSymbols(data []byte, h header) ([]symbol, error) {
	nameOff := headerSize + int(h.nsect)*sectionSize + int(h.nemit) + int(h.nrelo)*reloSize
	charOff := nameOff + int(h.nname)*nameSize
	if charOff+int(h.nchar) > len(data) {
		return nil, fmt.Errorf("symbol table exceeds file: names=%#x chars=%#x file=%#x", nameOff, charOff, len(data))
	}

	syms := make([]symbol, 0, h.nname)
	for i := 0; i < int(h.nname); i++ {
		off := nameOff + i*nameSize
		nameFileOff := int(get32(data[off:]))
		name := ""
		if nameFileOff != 0 {
			if nameFileOff < charOff || nameFileOff >= charOff+int(h.nchar) {
				return nil, fmt.Errorf("symbol %d has invalid name offset %#x", i, nameFileOff)
			}
			end := nameFileOff
			for end < charOff+int(h.nchar) && data[end] != 0 {
				end++
			}
			name = string(data[nameFileOff:end])
		}
		syms = append(syms, symbol{
			name:  name,
			typ:   get16(data[off+4:]),
			desc:  get16(data[off+6:]),
			value: get32(data[off+8:]),
		})
	}
	return syms, nil
}

func get16(b []byte) uint16 {
	return uint16(b[0]) | uint16(b[1])<<8
}

func get32(b []byte) uint32 {
	return uint32(get16(b)) | uint32(get16(b[2:]))<<16
}

func checkedBytes(data []byte, off uint32, n uint32, name string) ([]byte, error) {
	end := uint64(off) + uint64(n)
	if end > uint64(len(data)) {
		return nil, fmt.Errorf("%s section exceeds file: off=%#x len=%#x file=%#x", name, off, n, len(data))
	}
	return data[off:end], nil
}

func put32(buf []byte, off int, v uint32) {
	buf[off+0] = byte(v >> 24)
	buf[off+1] = byte(v >> 16)
	buf[off+2] = byte(v >> 8)
	buf[off+3] = byte(v)
}

func sectionOffset(sec section, textBase uint32, name string) (int, error) {
	if sec.base < textBase {
		return 0, fmt.Errorf("%s base %#x below text base %#x", name, sec.base, textBase)
	}
	return int(sec.base - textBase), nil
}

func dumpSymbols(data []byte, h header) {
	syms, err := readSymbols(data, h)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ackimage: %v\n", err)
		os.Exit(1)
	}
	sort.SliceStable(syms, func(i, j int) bool {
		if syms[i].value == syms[j].value {
			return syms[i].name < syms[j].name
		}
		return syms[i].value < syms[j].value
	})
	for _, s := range syms {
		if s.name == "" {
			continue
		}
		fmt.Printf("%08x %04x %04x %s\n", s.value, s.typ, s.desc, s.name)
	}
}

func symbolValue(data []byte, h header, name string) (uint32, bool) {
	syms, err := readSymbols(data, h)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ackimage: %v\n", err)
		os.Exit(1)
	}
	for _, s := range syms {
		if s.name == name {
			return s.value, true
		}
	}
	return 0, false
}

func mustSymbolValue(data []byte, h header, name string) uint32 {
	v, ok := symbolValue(data, h, name)
	if !ok {
		fmt.Fprintf(os.Stderr, "ackimage: missing symbol %s\n", name)
		os.Exit(1)
	}
	return v
}

func patchVectors(out []byte, data []byte, h header) {
	start := mustSymbolValue(data, h, "_start")
	busError := mustSymbolValue(data, h, "bus_error_entry")
	defaultException := mustSymbolValue(data, h, "default_exception")

	put32(out, 0, stackTop)
	put32(out, 4, start)
	put32(out, 8, busError)
	for off := 12; off < metadataOff; off += 4 {
		put32(out, off, defaultException)
	}
}

func main() {
	symbols := flag.Bool("symbols", false, "dump ACK symbols sorted by value")
	flag.Usage = usage
	flag.Parse()

	if (!*symbols && flag.NArg() != 2) || (*symbols && flag.NArg() != 1) {
		usage()
	}

	inPath := flag.Arg(0)

	data, err := os.ReadFile(inPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ackimage: read %s: %v\n", inPath, err)
		os.Exit(1)
	}
	if len(data) < headerSize {
		fmt.Fprintf(os.Stderr, "ackimage: %s too small for ACK header\n", inPath)
		os.Exit(1)
	}
	if get16(data[0:2]) != ackMagic {
		fmt.Fprintf(os.Stderr, "ackimage: %s is not an ACK object\n", inPath)
		os.Exit(1)
	}
	h := readHeader(data)
	if *symbols {
		dumpSymbols(data, h)
		return
	}

	outPath := flag.Arg(1)
	if h.flags&0x0004 != 0 {
		fmt.Fprintf(os.Stderr, "ackimage: %s has unresolved references\n", inPath)
		os.Exit(1)
	}
	if h.nsect < 4 {
		fmt.Fprintf(os.Stderr, "ackimage: %s has %d sections, need at least 4\n", inPath, h.nsect)
		os.Exit(1)
	}
	if len(data) < headerSize+int(h.nsect)*sectionSize {
		fmt.Fprintf(os.Stderr, "ackimage: %s too small for section table\n", inPath)
		os.Exit(1)
	}

	sects := make([]section, h.nsect)
	for i := range sects {
		sects[i] = readSection(data, headerSize+i*sectionSize)
	}

	text := sects[0]
	rom := sects[1]
	wdata := sects[2]
	bss := sects[3]

	if text.base != romBase {
		fmt.Fprintf(os.Stderr, "ackimage: text base %#x, want %#x\n", text.base, uint32(romBase))
		os.Exit(1)
	}

	out := make([]byte, romSize)
	romEnd := 0
	for _, part := range []struct {
		name string
		sec  section
	}{
		{"text", text},
		{"rom", rom},
	} {
		payload, err := checkedBytes(data, part.sec.foff, part.sec.flen, part.name)
		if err != nil {
			fmt.Fprintf(os.Stderr, "ackimage: %v\n", err)
			os.Exit(1)
		}
		off, err := sectionOffset(part.sec, text.base, part.name)
		if err != nil {
			fmt.Fprintf(os.Stderr, "ackimage: %v\n", err)
			os.Exit(1)
		}
		if off+len(payload) > len(out) {
		fmt.Fprintf(os.Stderr, "ackimage: ROM overflow while copying %s\n", part.name)
			os.Exit(1)
		}
		copy(out[off:], payload)
		if off+len(payload) > romEnd {
			romEnd = off + len(payload)
		}
	}

	dataPayload, err := checkedBytes(data, wdata.foff, wdata.flen, "data")
	if err != nil {
		fmt.Fprintf(os.Stderr, "ackimage: %v\n", err)
		os.Exit(1)
	}
	dataOff := romEnd
	if dataOff+len(dataPayload) > len(out) {
		fmt.Fprintf(os.Stderr, "ackimage: ROM overflow while copying data\n")
		os.Exit(1)
	}
	copy(out[dataOff:], dataPayload)
	pos := dataOff + len(dataPayload)
	patchVectors(out, data, h)
	if metadataOff+metadataSize > pos {
		fmt.Fprintf(os.Stderr, "ackimage: metadata table outside emitted text: text/rom/data=%#x\n", pos)
		os.Exit(1)
	}

	dataLoad := uint32(romBase + uint32(dataOff))
	put32(out, metadataOff+0, stackTop)
	put32(out, metadataOff+4, dataLoad)
	put32(out, metadataOff+8, wdata.base)
	put32(out, metadataOff+12, wdata.base+wdata.size)
	put32(out, metadataOff+16, bss.base)
	put32(out, metadataOff+20, bss.base+bss.size)

	if err := os.WriteFile(outPath, out, 0o644); err != nil {
		fmt.Fprintf(os.Stderr, "ackimage: write %s: %v\n", outPath, err)
		os.Exit(1)
	}
	fmt.Printf("DONE   %s size=%d text=%#x rom=%#x data=%#x bss=%#x\n",
		outPath, len(out), text.size, rom.size, wdata.size, bss.size)
}
