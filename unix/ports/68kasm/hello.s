* Minimal platform/epoch68/emulator V7 user program written for 68kasm.
*
* Build inside V7:
*
*     68kasm hello.s
*     srec2aout hello.h68 hello
*     chmod 0755 hello
*     hello

START   ORG     0

        MOVEQ   #4,D0          write(1,MSG,MSGLEN)
        MOVEQ   #1,D1
        MOVE.L  #MSG,A0
        MOVEQ   #18,D2
        TRAP    #0

        MOVEQ   #1,D0          exit(0)
        MOVEQ   #0,D1
        TRAP    #0

MSG     DC.B    'hello from 68kasm',10
        DS.W    0

        END
