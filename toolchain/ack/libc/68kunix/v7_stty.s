.define _stty,_gtty
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

! The V7 stty/gtty entry points must use their own system calls.
!
! V7 UUCP supplies an application-local ioctl() shim implemented in terms of
! stty/gtty.  Implementing these wrappers via generic ioctl() therefore
! resolves back to that shim and recurses.
.sect .text
_stty:
	move.l	4(sp),d1
	move.l	8(sp),a0
	move.l	#31,d0
	trap	#0
	jmp	__cerror

_gtty:
	move.l	4(sp),d1
	move.l	8(sp),a0
	move.l	#32,d0
	trap	#0
	jmp	__cerror
