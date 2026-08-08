! Epoch68 ROM monitor startup.
!
! The ROM is linked at 0x800000.  Reset mirrors it at address zero until
! the first write clears the hardware ROM-overlay flip-flop.

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.define _start
.define bus_error_entry
.define default_exception
.extern _main
.extern _bus_error_handler

! Full low-memory vector table (256 vectors / 1024 bytes).
.data4 0x00040000
.data4 _start
.data4 bus_error_entry
.data4 default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception
.data4 default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception,default_exception

! Patched by host/cmd/ackimage after ACK linking.
rom_metadata:
.data4 0x00040000
.data4 0
.data4 0x00030000
.data4 0x00030000
.data4 0x00030000
.data4 0x00030000

_start:
	move.b #0x01,0x00800000
	move.w #0x2700,sr
	lea rom_metadata,a3
	move.l (a3)+,sp

	lea 0x00800000,a0
	lea 0x00000000,a1
	move.w #255,d0
copy_vectors:
	move.l (a0)+,(a1)+
	dbra d0,copy_vectors

	move.l (a3)+,a0
	move.l (a3)+,a1
	move.l (a3)+,a2
copy_data:
	cmp.l a1,a2
	beq clear_bss_start
	move.b (a0)+,(a1)+
	bra copy_data

clear_bss_start:
	move.l (a3)+,a1
	move.l (a3)+,a2
clear_bss:
	cmp.l a1,a2
	beq call_main
	clr.b (a1)+
	bra clear_bss

call_main:
	jsr _main

halt_loop:
	bra halt_loop

bus_error_entry:
	jsr _bus_error_handler
bus_error_halt:
	bra bus_error_halt

default_exception:
	bra default_exception
