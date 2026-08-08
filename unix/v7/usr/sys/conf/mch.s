.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.extern _main








KERNEL_BOOT_STACK_TOP = 0x00040000

.define start
.define _start

start:
_start:
        lea     KERNEL_BOOT_STACK_TOP,sp
        jsr     _main
start_hang:
        bra start_hang

.define _savfp
.define _save
.define _resume
.define _spl0
.define _spl1
.define _spl4
.define _spl5
.define _spl6
.define _spl7
.define _splx
.define _epoch68_enter_user
.define _epoch68_start_on_stack
.define _syscall_trap0
.define _epoch68_bus_error
.define _epoch68_address_error
.define _clock_intr
.define _epoch68_resume_irq_frame
.define _uart_intr
.define _epoch68_irq_switch_stub
.define _epoch68_fast_copy_256k
.define _epoch68_trace_page63_writes_on
.define _epoch68_trace_page63_writes_off
.define _epoch68_trace_off
.define _epoch68_trace_full
.define _epoch68_trace_full_8192
.define _epoch68_trace_user_full
.define _epoch68_watch_shell_arena
.define _DEBUG_S
.define _DEBUG_HEX32
.define _DEBUG_SL
.define _DEBUG_DUMP512

.extern _epoch68_trap_syscall
.extern _epoch68_fault
.extern _epoch68_trap_new_pc
.extern _epoch68_trap_new_usp
.extern _epoch68_trap_new_sr
.extern _epoch68_trap_return_d1
.extern _epoch68_clock_intr
.extern _epoch68_clock_signals
.extern _epoch68_clock_return_pc
.extern _epoch68_clock_return_usp
.extern _epoch68_clock_return_sr
.extern _epoch68_uart_intr_shared
.extern _epoch68_uart_intr_enter
.extern _epoch68_should_qswtch
.extern _epoch68_save_current_usp
.extern _epoch68_resume_usp
.extern _kticks
.extern _qswtch

_epoch68_trace_page63_writes_on:
	.data2	0x4afc,0xe68a
        rts

_epoch68_trace_page63_writes_off:
	.data2	0x4afc,0xe68b
        rts

_epoch68_trace_off:
	.data2	0x4afc,0xe680
        rts

_epoch68_trace_full:
	.data2	0x4afc,0xe682
        rts

_epoch68_trace_full_8192:
	.data2	0x4afc,0xe68c
        rts

_epoch68_trace_user_full:
	.data2	0x4afc,0xe68d
        rts

_epoch68_watch_shell_arena:
	.data2	0x4afc,0xe68e
        rts

_DEBUG_S:
        move.l  a0,-(sp)
        move.l  8(sp),a0
	.data2	0x4afc,0xe686
        move.l  (sp)+,a0
        rts

_DEBUG_HEX32:





        rts

_DEBUG_SL:
        move.l  a0,-(sp)
        move.l  d1,-(sp)
        move.l  12(sp),a0
        move.l  16(sp),d1
	.data2	0x4afc,0xe688
        move.l  (sp)+,d1
        move.l  (sp)+,a0
        rts

_DEBUG_DUMP512:





        rts

_savfp:
_save:
        move.l 4(sp),a0
        move.l  (sp),(a0)+      
        lea     4(sp),a1
        move.l  a1,(a0)+        
        movem.l d2-d7/a2-a6,(a0)
        move.l   #0,d0
        rts
_resume:



        move.l 6(sp),a0
        cmp.l   #0x00c00000,a0
        bcs resume_oldabi
        cmp.l   #0x00c40000,a0
        bcs resume_have_area
resume_oldabi:
        move.l 8(sp),a0
resume_have_area:
        move.l  (a0)+,a1        
        move.l  (a0)+,d1        
        movem.l (a0),d2-d7/a2-a6
        move.l  d1,sp
        move.l   #1,d0
        jmp     (a1)

_epoch68_resume_irq_frame:
        move.l  (sp)+,a0
        move.l  a0,usp
        movem.l (sp)+,d0-d7/a0-a6
        rte

_epoch68_start_on_stack:
        move.l 4(sp),a0
        move.l 8(sp),a1
        move.l a0,sp
        jsr    (a1)
epoch68_start_on_stack_hang:
        bra    epoch68_start_on_stack_hang

_epoch68_enter_user:
        move.l 4(sp),a0
        move.l  8(sp),d1
        move.w  12(sp),d0
        move.l  d1,a1
        move.l  a1,usp
        move.l  a0,-(sp)
        move.w  d0,-(sp)
        rte

_spl0:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_spl1:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        or.w   #0x0100,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_spl4:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        or.w   #0x0400,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_spl5:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        or.w   #0x0500,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_spl6:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        or.w   #0x0600,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_spl7:
        move.l  d1,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        or.w   #0x0700,d1
        move.w  d1,sr
        move.l  (sp)+,d1
        rts
_splx:
        move.l  d1,-(sp)
        move.l  d2,-(sp)
        move.l   #0,d0
        move.w  sr,d0
        move.w  sr,d1
        and.w  #0xf8ff,d1
        move.w  14(sp),d2
        and.w  #0x0700,d2
        or.w    d2,d1
        move.w  d1,sr
        move.l  (sp)+,d2
        move.l  (sp)+,d1
        rts

_syscall_trap0:
        movem.l d0-d7/a0-a6,-(sp)
        move.l  usp,a2
        move.l   #0,d3
        move.l   #0,d4
        move.w  60(sp),d3
        move.l  62(sp),d4
        move.l  sp,-(sp)
        move.l  d4,-(sp)
        move.l  d3,-(sp)
        move.l  a2,-(sp)
        move.l  a1,-(sp)
        move.l  a0,-(sp)
        move.l  d2,-(sp)
        move.l  d1,-(sp)
        move.l  d0,-(sp)
        jsr     _epoch68_trap_syscall
        lea     36(sp),sp
	move.l  _epoch68_trap_return_d1,4(sp)
        cmp.l  #0x7fffffff,d0
        bne trap0_return
        move.l _epoch68_trap_new_usp,a2
        move.l  a2,usp
        move.w  _epoch68_trap_new_sr,60(sp)
        move.l  _epoch68_trap_new_pc,62(sp)
        move.l   #0,d0
        move.l  d0,(sp)
        movem.l (sp)+,d0-d7/a0-a6
        rte
trap0_return:
        move.l  d0,(sp)
        move.l _epoch68_trap_new_usp,a2
        move.l  a2,usp
        move.w  _epoch68_trap_new_sr,60(sp)
        move.l  _epoch68_trap_new_pc,62(sp)
        jsr     _epoch68_should_qswtch
        tst.l   d0
        beq .no_qswtch
        jsr     _qswtch
        move.l _epoch68_resume_usp,a2
        move.l  a2,usp
.no_qswtch:
        movem.l (sp)+,d0-d7/a0-a6
        rte





_epoch68_bus_error:
        move.l   #10,d0                 
        bra epoch68_fault_common

_epoch68_address_error:
        move.l   #10,d0                 

epoch68_fault_common:
        move.l   #0,d1
        move.w  (sp),d1                
        move.l   #0,d2
        btst    #13,d1
        beq epoch68_fault_user
        move.l   #1,d2
epoch68_fault_user:
        move.l sp,a0
        movem.l d0-d7/a0-a6,-(sp)
        move.l  d2,-(sp)               
        move.l  a0,-(sp)               
        move.l  d0,-(sp)               
        jsr     _epoch68_fault
        lea     12(sp),sp
epoch68_fault_hang:
        bra epoch68_fault_hang


_clock_intr:
        movem.l     d0-d7/a0-a6,-(sp)
        add.l      #1,_kticks
        move.l       #0,d0
        move.w      60(sp),d0
        move.l      62(sp),-(sp)
        move.l      d0,-(sp)
        jsr         _epoch68_clock_intr
        add.l      #8,sp
        move.l       #0,d0
        move.w      60(sp),d0
        btst        #13,d0                  
        bne .clock_done
        move.l      62(sp),d1
        move.l      usp,a2
        move.l      a2,-(sp)
        move.l      d1,-(sp)
        move.l      d0,-(sp)
        jsr         _epoch68_clock_signals
        lea         12(sp),sp
        move.l     _epoch68_clock_return_usp,a2
        move.l      a2,usp
        move.w      _epoch68_clock_return_sr,60(sp)
        move.l      _epoch68_clock_return_pc,62(sp)
        jsr         _epoch68_should_qswtch
        tst.l       d0
        beq .clock_done
        move.l      usp,a2
        move.l      a2,-(sp)
        jsr         _epoch68_save_current_usp
        add.l      #4,sp
        jsr         _qswtch
        move.l     _epoch68_resume_usp,a2
        move.l      a2,usp
.clock_done:
        movem.l     (sp)+,d0-d7/a0-a6
        rte










































_do_switch:





_uart_intr:
        movem.l d0-d7/a0-a6,-(sp)
        jsr     _epoch68_uart_intr_enter
        jsr     _epoch68_uart_intr_shared
        movem.l (sp)+,d0-d7/a0-a6
        rte


































































_epoch68_irq_switch_stub:





















































.no_switch:
        rts

_epoch68_fast_copy_256k:
        move.l 4(sp),a0
        move.l 8(sp),a1
        movem.l d2-d7/a2-a6,-(sp)
        move.w  #1365-1,d6
.copy_loop:
        movem.l (a0)+,d0-d5/d7/a2-a6
        movem.l d0-d5/d7/a2-a6,(a1)
        movem.l (a0)+,d0-d5/d7/a2-a6
        movem.l d0-d5/d7/a2-a6,48(a1)
        movem.l (a0)+,d0-d5/d7/a2-a6
        movem.l d0-d5/d7/a2-a6,96(a1)
        movem.l (a0)+,d0-d5/d7/a2-a6
        movem.l d0-d5/d7/a2-a6,144(a1)
        lea     192(a1),a1
        dbra    d6,.copy_loop
        movem.l (a0)+,d0-d5/d7/a2-a6
        movem.l d0-d5/d7/a2-a6,(a1)
        movem.l (a0)+,d0-d3
        movem.l d0-d3,48(a1)
        movem.l (sp)+,d2-d7/a2-a6
        rts
