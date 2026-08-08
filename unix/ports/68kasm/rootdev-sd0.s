* Patch the copied kernel image on /mnt so rootdev and pipedev point at sd0.
*
* This is a deliberately tiny installer/demo helper built inside V7 with:
*
*     68kasm rootdev-sd0.s
*     srec2aout rootdev-sd0.h68 rootdev-sd0
*     chmod 0755 rootdev-sd0
*     rootdev-sd0
*
* The offsets are for the currently staged /unix image:
*
*     rootdev file offset = $0000B06C
*     pipedev file offset = $0000B074
*
* Both are patched to dev_t makedev(1,0), i.e. sd0, encoded big-endian
* as bytes 01 00.  Rebuilds that move rootdev/pipedev need new offsets.

START   ORG     0

        MOVEQ   #5,D0          open("/mnt/unix", 2)
        MOVE.L  #PATH,A0
        MOVEQ   #2,D1
        TRAP    #0
        MOVE.L  D0,D3
        BMI.S   BAD

        MOVEQ   #19,D0         lseek(fd, $B06C, 0)
        MOVE.L  D3,D1
        MOVE.L  #$0000B06C,D2
        MOVE.L  #0,A0
        TRAP    #0
        BMI.S   BAD

        MOVEQ   #4,D0          write(fd, "\001\000", 2)
        MOVE.L  D3,D1
        MOVE.L  #DEVSD0,A0
        MOVEQ   #2,D2
        TRAP    #0
        CMP.L   #2,D0
        BNE.S   BAD

        MOVEQ   #19,D0         lseek(fd, $B074, 0)
        MOVE.L  D3,D1
        MOVE.L  #$0000B074,D2
        MOVE.L  #0,A0
        TRAP    #0
        BMI.S   BAD

        MOVEQ   #4,D0          write(fd, "\001\000", 2)
        MOVE.L  D3,D1
        MOVE.L  #DEVSD0,A0
        MOVEQ   #2,D2
        TRAP    #0
        CMP.L   #2,D0
        BNE.S   BAD

        MOVEQ   #6,D0          close(fd)
        MOVE.L  D3,D1
        TRAP    #0

        MOVEQ   #4,D0          write(1, OK, OKLEN)
        MOVEQ   #1,D1
        MOVE.L  #OK,A0
        MOVEQ   #OKLEN,D2
        TRAP    #0

        MOVEQ   #1,D0          exit(0)
        MOVEQ   #0,D1
        TRAP    #0

BAD     MOVEQ   #4,D0          write(2, ERR, ERRLEN)
        MOVEQ   #2,D1
        MOVE.L  #ERR,A0
        MOVEQ   #ERRLEN,D2
        TRAP    #0

        MOVEQ   #1,D0          exit(1)
        MOVEQ   #1,D1
        TRAP    #0

PATH    DC.B    '/mnt/unix',0
DEVSD0  DC.B    1,0
OK      DC.B    'rootdev/pipedev patched to sd0',10
OKLEN   EQU     31
ERR     DC.B    'rootdev-sd0: patch failed',10
ERRLEN  EQU     26
        DS.W    0

        END
