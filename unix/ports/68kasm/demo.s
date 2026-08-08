* Small smoke-test source for the imported 68kasm.
*
* Inside platform/epoch68/emulator V7 after build.cmd has installed /usr/bin/68kasm:
*
*     68kasm -l demo.s
*     ls -l demo.h68 demo.lis
*     cat demo.h68
*
* The output is Motorola S-record text.  This is not an platform/epoch68/emulator executable;
* it is just a compact syntax and code-generation test for the assembler.

START   ORG     $1000

COUNT   EQU     4
SAVE    REG     D1-D2/A0

        MOVEQ   #0,D0
        MOVE.L  #TABLE,A0
        MOVEQ   #COUNT-1,D1

LOOP    ADD.W   (A0)+,D0
        DBRA    D1,LOOP

        BSR.S   DONE
        BRA.S   STOP

DONE    MOVEM.L SAVE,-(SP)
        ADDQ.W  #1,D0
        MOVEM.L (SP)+,SAVE
        RTS

STOP    NOP
        RTS

TABLE   DC.W    1,2,3,4
TEXT    DC.B    '68kasm demo',13,10,0
        DS.W    0

        END
