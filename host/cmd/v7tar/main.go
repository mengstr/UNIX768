// v7tar writes the pre-ustar tar stream understood by the V7 tar program.
// It intentionally accepts a small surface: a root, an output, a marker, and
// a list of paths below the root.  Package make rules pass only text trees.
package main

import (
	"bytes"
	"errors"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

const blockSize = 512

// V7 make uses a zero mtime as its sentinel for a missing file.  Archive
// members therefore need a stable, nonzero timestamp: it preserves
// reproducibility without making extracted sources invisible to make.
const archiveMtime = 946684800 // 2000-01-01 00:00:00 UTC

type member struct {
	name string
	path string
	info os.FileInfo
}

func failf(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, "v7tar: "+format+"\n", args...)
	os.Exit(1)
}

func putString(dst []byte, value string) error {
	if len(value) > len(dst) {
		return errors.New("field too long")
	}
	copy(dst, value)
	return nil
}

func putOctal(dst []byte, value int64) error {
	s := fmt.Sprintf("%0*o", len(dst)-1, value)
	if len(s) > len(dst)-1 {
		return errors.New("octal field too long")
	}
	copy(dst, s)
	dst[len(dst)-1] = 0
	return nil
}

func validName(name string) error {
	if name == "" || filepath.IsAbs(name) || strings.Contains(name, "\\") {
		return errors.New("not a relative slash path")
	}
	if len(name) > 100 {
		return errors.New("path exceeds 100-byte V7 tar name")
	}
	for _, component := range strings.Split(name, "/") {
		if component == "" || component == "." || component == ".." {
			return errors.New("invalid path component")
		}
		if len(component) > 14 {
			return fmt.Errorf("component %q exceeds V7 14-byte limit", component)
		}
	}
	return nil
}

func collect(root, arg string, members *[]member) error {
	path := filepath.Join(root, arg)
	return filepath.Walk(path, func(current string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		rel, err := filepath.Rel(root, current)
		if err != nil {
			return err
		}
		name := filepath.ToSlash(rel)
		if name == "." {
			return nil
		}
		if err := validName(name); err != nil {
			return err
		}
		if info.Mode()&os.ModeSymlink != 0 || !info.Mode().IsRegular() && !info.IsDir() {
			return fmt.Errorf("%s: unsupported file type", name)
		}
		/*
		 * The target's V7 tar extractor does not recognize modern directory
		 * type entries; it tries to creat(2) them as regular files.  It does
		 * create parent directories while extracting ordinary files, so write
		 * only regular-file members.
		 */
		if info.IsDir() {
			return nil
		}
		*members = append(*members, member{name, current, info})
		return nil
	})
}

func containsNUL(path string) (bool, error) {
	f, err := os.Open(path)
	if err != nil {
		return false, err
	}
	defer f.Close()
	buf := make([]byte, 32768)
	for {
		n, err := f.Read(buf)
		if bytes.IndexByte(buf[:n], 0) >= 0 {
			return true, nil
		}
		if err == io.EOF {
			return false, nil
		}
		if err != nil {
			return false, err
		}
	}
}

func header(m member) ([]byte, error) {
	h := make([]byte, blockSize)
	name := m.name
	if m.info.IsDir() {
		name += "/"
	}
	if err := putString(h[0:100], name); err != nil {
		return nil, err
	}
	if err := putOctal(h[100:108], int64(m.info.Mode().Perm())); err != nil {
		return nil, err
	}
	if err := putOctal(h[108:116], 0); err != nil {
		return nil, err
	}
	if err := putOctal(h[116:124], 0); err != nil {
		return nil, err
	}
	size := m.info.Size()
	if m.info.IsDir() {
		size = 0
	}
	if err := putOctal(h[124:136], size); err != nil {
		return nil, err
	}
	if err := putOctal(h[136:148], archiveMtime); err != nil {
		return nil, err
	}
	for i := 148; i < 156; i++ {
		h[i] = ' '
	}
	if m.info.IsDir() {
		h[156] = '5'
	} else {
		h[156] = '0'
	}
	var sum int
	for _, b := range h {
		sum += int(b)
	}
	if err := putOctal(h[148:156], int64(sum)); err != nil {
		return nil, err
	}
	return h, nil
}

func writeMember(out io.Writer, m member, allowNUL bool) error {
	if !m.info.IsDir() && !allowNUL {
		if nul, err := containsNUL(m.path); err != nil {
			return err
		} else if nul {
			return fmt.Errorf("%s contains NUL bytes", m.name)
		}
	}
	h, err := header(m)
	if err != nil {
		return fmt.Errorf("%s: %w", m.name, err)
	}
	if _, err = out.Write(h); err != nil {
		return err
	}
	if m.info.IsDir() {
		return nil
	}
	f, err := os.Open(m.path)
	if err != nil {
		return err
	}
	defer f.Close()
	if _, err = io.Copy(out, f); err != nil {
		return err
	}
	pad := int((blockSize - (m.info.Size() % blockSize)) % blockSize)
	if pad != 0 {
		_, err = out.Write(make([]byte, pad))
	}
	return err
}

func main() {
	root := flag.String("root", "", "package root")
	output := flag.String("output", "", "output tar file")
	marker := flag.String("marker", "", "completion marker path relative to root")
	allowNUL := flag.Bool("allow-nul", false, "allow binary file data")
	flag.Parse()
	if *root == "" || *output == "" || *marker == "" || flag.NArg() == 0 {
		failf("usage: v7tar -root ROOT -output FILE -marker PATH [-allow-nul] PATH...")
	}
	if err := validName(*marker); err != nil {
		failf("marker: %v", err)
	}
	var members []member
	for _, arg := range flag.Args() {
		if err := collect(*root, arg, &members); err != nil {
			failf("%v", err)
		}
	}
	sort.Slice(members, func(i, j int) bool { return members[i].name < members[j].name })
	for i := 1; i < len(members); i++ {
		if members[i-1].name == members[i].name {
			failf("duplicate member %s", members[i].name)
		}
	}
	tmp := *output + ".tmp"
	out, err := os.Create(tmp)
	if err != nil {
		failf("create: %v", err)
	}
	for _, m := range members {
		if err := writeMember(out, m, *allowNUL); err != nil {
			out.Close()
			os.Remove(tmp)
			failf("%v", err)
		}
	}
	markerHeader, err := header(member{*marker, "", fakeInfo{}})
	if err != nil {
		failf("marker: %v", err)
	}
	if _, err = out.Write(markerHeader); err != nil {
		failf("marker: %v", err)
	}
	if _, err := out.Write(make([]byte, blockSize*2)); err != nil {
		failf("trailer: %v", err)
	}
	if err := out.Close(); err != nil {
		failf("close: %v", err)
	}
	if err := os.Rename(tmp, *output); err != nil {
		failf("rename: %v", err)
	}
}

type fakeInfo struct{}

func (fakeInfo) Name() string       { return "" }
func (fakeInfo) Size() int64        { return 0 }
func (fakeInfo) Mode() os.FileMode  { return 0644 }
func (fakeInfo) ModTime() time.Time { return time.Unix(0, 0) }
func (fakeInfo) IsDir() bool        { return false }
func (fakeInfo) Sys() interface{}   { return nil }
