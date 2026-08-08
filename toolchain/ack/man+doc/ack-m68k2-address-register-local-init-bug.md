# ACK m68k2: address-register locals with negative frame offsets are not initialized

## Summary

The ACK m68k backend can allocate a local pointer/register variable into an
address register, save that address register in the function prologue, but fail
to initialize it to the local variable's stack slot when the variable has a
negative frame-pointer offset.

The generated code then uses a stale incoming address-register value for stores
to the local variable. In our kernel this corrupted an unrelated in-core V7
superblock field and made inode allocation fail.

## Affected Area

Backend file:

```text
cc/c2/mach.c
```

Upstream file:

```text
mach/m68020/ncg/mach.c
```

Function:

```c
f_regsave()
```

The issue is in the register-save/register-initialization path used by the
m68k backend after `regsave()` records a register variable.

## Trigger Pattern

A small trigger is a function with a local scalar selected for an address
register. This pattern was generated from:

```c
static i32
rtcbcd(units, tens)
i16 units;
i16 tens;
{
	register i16 u, t;

	u = MM58167_BASE[units] & 0xf;
	t = MM58167_BASE[tens] & 0xf;
	if (u > 9 || t > 9)
		return(-1);
	return((t * 10) + u);
}
```

The optimizer/register allocator chose `a5` for local `u`.

## Bad Generated Assembly

Before the fix, the generated assembly included:

```asm
!Local -4 into a5
link a6,#-10
move.l a5,(sp)
...
move.w d0,(a5)
...
move.l -10(a6),a5
```

The backend saved the old `a5`, but never initialized `a5` to the local slot
at `-4(a6)`. The store:

```asm
move.w d0,(a5)
```

therefore wrote through the stale caller value of `a5`.

## Observed Failure

In the UNIX768 ACK kernel build, the stale `a5` value pointed at the in-core
root superblock. Calling `rtcbcd()` while reading the emulated RTC overwrote
`s_isize`:

```text
expected s_isize: 0x0520
observed s_isize: 0x0006
```

That later caused inode allocation to scan the wrong inode range, producing
runtime failures such as:

```text
FAIL creat: creat failed
FAIL metadata-open: open failed
FAIL pipe: pipe failed
```

A write watch showed the corrupting store landing in the superblock area, and
the PC mapped back to `_rtcbcd`.

## Root Cause

`regsave()` records the assigned register and source frame offset:

```c
regsav[regnr].rs_reg = s;
regsav[regnr].rs_off = off;
regsav[regnr++].rs_size = size;
```

`f_regsave()` already initialized register parameters when `rs_off >= 0`:

```c
if (p->rs_off >= 0) {
        fprintf(codefile, "move.%c %ld(a6),%s\n", ...);
}
```

But for local variables (`rs_off < 0`) assigned to address registers, it only
saved/restored the register. It did not emit the required address calculation.

For an address-register local with a negative frame offset, the backend needs:

```asm
lea -4(a6),a5
```

not a load from the local slot.

## Fix

Initialize negative-offset address-register variables in `f_regsave()` with
`lea offset(a6),areg`.

Patch:

```diff
diff --git a/cc/c2/mach.c b/cc/c2/mach.c
index b5772cf1..18088d55 100644
--- a/cc/c2/mach.c
+++ b/cc/c2/mach.c
@@ -168,7 +168,7 @@ f_regsave(void)
                p = regsav;
                fprintf(codefile,"move.l %s,(sp)\n",p->rs_reg);
        }
-       /* initialise register-parameters */
+       /* initialise register variables */
        for (p = regsav; p < &regsav[regnr]; p++) {
                if (p->rs_off >= 0) {
 #ifdef SYNTAX_68020
@@ -179,6 +179,14 @@ f_regsave(void)
                                (p->rs_size == 4 ? 'l' : 'w'),
                                p->rs_off,
                                p->rs_reg);
+               } else if (p->rs_reg[0] == 'a') {
+#ifdef SYNTAX_68020
+                       fprintf(codefile,"lea (%ld,a6),%s\n",
+#else
+                       fprintf(codefile,"lea %ld(a6),%s\n",
+#endif
+                               p->rs_off,
+                               p->rs_reg);
                }
        }
 }
```

## Good Generated Assembly

After the fix, the same function includes:

```asm
!Local -4 into a5
link a6,#-10
move.l a5,(sp)
lea -4(a6),a5
...
move.w d0,(a5)
...
move.l -10(a6),a5
```

Now `a5` points at the intended stack slot before it is used.

## Validation

After rebuilding ACK tools and the kernel with the backend fix:

```sh
make tools
make clean stage1 KERNEL_ACK_CFLAGS=-O2
```

The generated `rtcbcd` assembly contained the expected `lea -4(a6),a5`.

The UNIX768 syscall test that previously failed because of superblock
corruption passed:

```text
PASS setuid
PASS setgid
PASS getpid
PASS getuid/getgid
PASS time
PASS ftime
PASS times
PASS umask
PASS write
PASS open
PASS dup
PASS lseek
PASS read
PASS dup+lseek
PASS stat
PASS fstat
PASS access
PASS chmod/stat
PASS chown
PASS utime
PASS link
PASS unlink(link)
PASS sync
PASS pipe
PASS pipe/write
PASS wait
PASS signal
PASS exec
PASS pause
PASS alarm
PASS sleep
RESULT PASS
```

## Notes for Upstream

This was observed in the ACK m68k backend while targeting a 32-bit 68000 UNIX
port. The failure is not specific to the UNIX kernel: any function where a
negative-offset local is assigned to an address register and then used as an
address can write through a stale caller register.

The fix intentionally only handles negative-offset address registers. Existing
positive-offset parameter initialization remains unchanged.

Verified against upstream:

```text
repository: https://github.com/davidgiven/ack
HEAD:       cd8805a0ad462c120ef394225c73ce725d271740
commit:     cd8805a Merge pull request #360 from davidgiven/dtrg-fixes
```

At that upstream HEAD, `mach/m68020/ncg/mach.c` still has the same
`f_regsave()` logic:

```c
/* initialise register-parameters */
for (p = regsav; p < &regsav[regnr]; p++) {
        if (p->rs_off >= 0) {
                ...
        }
}
```

There is no `rs_off < 0` address-register initialization path in upstream
HEAD.
