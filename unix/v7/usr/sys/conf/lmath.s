.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text



.define __ldivu
.define __ldivs
.define __lmods


__ldivu:
        movem.l d2-d4,-(a7)
        move.l  16(a7),d0
        move.l  20(a7),d1
        beq .udiv_zero

        move.l   #0,d2
        move.l   #0,d3
        move.l   #31,d4
.udiv_loop:
        lsl.l   #1,d0
        roxl.l  #1,d2
        lsl.l   #1,d3
        cmp.l   d1,d2
        bcs .udiv_next
        sub.l   d1,d2
        add.l  #1,d3
.udiv_next:
        dbra    d4,.udiv_loop

        move.l  d3,d0
        bra .udiv_done

.udiv_zero:
        move.l   #0,d0
.udiv_done:
        movem.l (a7)+,d2-d4
        rts


__ldivs:
        movem.l d2-d5,-(a7)
        move.l  20(a7),d0
        move.l  24(a7),d1
        beq .sdiv_zero

        move.l   #0,d5
        tst.l   d0
        bge .sdiv_dividend_positive
        neg.l   d0
        eor.l  #1,d5
.sdiv_dividend_positive:
        tst.l   d1
        bge .sdiv_divisor_positive
        neg.l   d1
        eor.l  #1,d5
.sdiv_divisor_positive:

        move.l   #0,d2
        move.l   #0,d3
        move.l   #31,d4
.sdiv_loop:
        lsl.l   #1,d0
        roxl.l  #1,d2
        lsl.l   #1,d3
        cmp.l   d1,d2
        bcs .sdiv_next
        sub.l   d1,d2
        add.l  #1,d3
.sdiv_next:
        dbra    d4,.sdiv_loop

        move.l  d3,d0
        tst.l   d5
        beq .sdiv_done
        neg.l   d0
        bra .sdiv_done

.sdiv_zero:
        move.l   #0,d0
.sdiv_done:
        movem.l (a7)+,d2-d5
        rts


__lmods:
        movem.l d2-d4,-(a7)
        move.l  16(a7),d0
        move.l  20(a7),d1
        beq .mod_zero

        move.l   #0,d4
        tst.l   d0
        bge .dividend_positive
        neg.l   d0
        move.l   #1,d4
.dividend_positive:
        tst.l   d1
        bge .divisor_positive
        neg.l   d1
.divisor_positive:

        move.l   #0,d2
        move.l   #31,d3
.mod_loop:
        lsl.l   #1,d0
        roxl.l  #1,d2
        cmp.l   d1,d2
        bcs .mod_next
        sub.l   d1,d2
.mod_next:
        dbra    d3,.mod_loop

        move.l  d2,d0
        tst.l   d4
        beq .mod_done
        neg.l   d0
        bra .mod_done

.mod_zero:
        move.l   #0,d0
.mod_done:
        movem.l (a7)+,d2-d4
        rts
