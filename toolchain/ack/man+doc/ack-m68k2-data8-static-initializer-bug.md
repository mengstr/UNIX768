# ACK m68k2/m68020 NCG emits `.data8` for 4-byte constants in 2-byte-word mode

## Summary

The ACK m68k2-style 68000 backend can emit `.data8` for 4-byte integer
constants when the m68020 NCG is built for the 2-byte-word / 2-byte-`int`
configuration used by m68k2 and Minix68k-style targets.

This corrupts static data layout. A visible symptom in the 68kunix libc work was
that statically initialized `FILE` objects had shifted fields: `stdout` appeared
to have file descriptor `0` and flags `0`, so buffered output silently failed or
went to the wrong descriptor.

## Environment

- ACK source: `davidgiven/ack`, local clone at commit
  `cd8805a0ad462c120ef394225c73ce725d271740`
- Target configuration:
  - `WORD_SIZE=2`
  - `int` size 2
  - `long` size 4
  - pointer size 4
  - `-Vw2.2i2.2p4.2f4.2s2.2l4.2d8.2x8.2`
- Backend source path:
  - upstream: `mach/m68020/ncg/mach.c`
  - local extracted tree: `cc/c2/mach.c`
- The m68k2 backend reuses the m68020 NCG sources with `WORD_SIZE=2`.

## Reproduction

Compile a source file containing static pointer or struct initializers with
4-byte pointer fields:

```c
char *p = 0;

struct S {
	char *p;
	int x;
	char *q;
	char f;
	char g;
};

struct S s = { 0, 0, 0, 2, 1 };
```

With the affected backend, generated assembly for 4-byte constants can contain
`.data8` where `.data4` is required.

In the libc case, this initializer was affected:

```c
struct _iobuf _iob[_NFILE] = {
	{ _sibuf, 0, _sibuf, _IOREAD, 0 },
	{ NULL, 0, NULL, _IOWRT, 1 },
	{ NULL, 0, NULL, _IOWRT + _IONBF, 2 },
};
```

## Expected Behavior

For 4-byte constants in the 2-byte-word configuration, NCG should emit 4 bytes:

```asm
.data4	0
```

For actual 8-byte constants, NCG should emit 8 bytes:

```asm
.data8	5000000000
```

## Actual Behavior

The m68020 NCG `con_mult()` accepts both 4-byte and 8-byte constants, but emits
`.data8` unconditionally:

```c
void
con_mult(word sz) {

	if ((sz != 4) && (sz != 8))
		fatal("bad icon/ucon size");
	fprintf(codefile,".data8\t%s\n", str);
}
```

This makes a 4-byte null pointer initializer occupy 8 bytes in the output data
section. Every following field in the static object is then at the wrong offset.

## Root Cause

The 2019 ACK long-long work added `.data8` support for 8-byte integer constants.
That change assumed m68020 NCG normally has 4-byte words, so `con_mult()` would
only be called for 8-byte constants on m68020.

That assumption is not true for the m68k2 / Minix68k-style build. In that
configuration:

- `WORD_SIZE=2`
- 4-byte `long` and pointer constants are larger than the EM word size
- `mach/proto/ncg/fillem.c` therefore legitimately calls `con_mult(4)`

A later upstream 2-byte-int cleanup changed m68020 `con_mult()` to allow
`sz == 4`, but left the output hard-coded as `.data8`. That avoided the fatal
error but made the generated data layout wrong.

## Upstream References

- `.data8` / long long support PR:
  - https://github.com/davidgiven/ack/pull/208
- Commit adding `.data8` use in NCG:
  - https://github.com/davidgiven/ack/commit/1faff418ec1943d2b4715763ed7beb1829d512a6
- Commit adapting NCG for 2-byte-int mode but leaving `.data8` unconditional:
  - https://github.com/davidgiven/ack/commit/41b3ccaf6f636ea58a1b4ce522f010929d109337

The `.data8` commit message explicitly says that i80 and i86 can receive both
`sz == 4` and `sz == 8`, while m68020 was expected to receive only `sz == 8`
because it normally has 4-byte words. The 2-byte-word m68k2 configuration breaks
that assumption.

## Local Fix

Emit the directive matching the size passed to `con_mult()`:

```c
void
con_mult(word sz) {

	if ((sz != 4) && (sz != 8))
		fatal("bad icon/ucon size");
	fprintf(codefile, ".data%d\t%s\n", (int)sz, str);
}
```

This matches the pattern already used by ACK's i80/i86 NCGs for targets where
`con_mult()` can receive both 4-byte and 8-byte constants.

## Verification

After the local fix, a kept intermediate assembly file for `stdio_data.c`
contains the correct `_iob` layout:

```asm
__iob:
.data4	__sibuf
.data2	0
.data4	__sibuf
.data2	256
.data4	0
.data2	0
.data4	0
.data2	513
.data4	0
.data2	0
.data4	0
.data2	1538
```

The userland tests then behaved correctly:

- `echo2 "Hello, World!"` printed `Hello, World!`
- `libctest` passed string, memory, malloc, file I/O, stdio read/write,
  stdio seek, reopen, puts, and console-output tests.

## Notes

The `-Vw2.2i2.2p4.2f4.2s2.2l4.2d8.2x8.2` configuration is not the bug. It
correctly says:

- `p4.2`: pointers are 4 bytes
- `l4.2`: longs are 4 bytes
- `d8.2` and `x8.2`: double and long-double are 8 bytes

Changing `d8` or `x8` would be the wrong fix. The problem is that the backend
used the 8-byte data directive for a 4-byte constant.
