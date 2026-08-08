#ifndef RED_H
#define RED_H

/*
 * RED screen editor, platform/epoch68/emulator/V7 port.
 *
 * The editor core still relies on its original 16-bit buffer indices.
 * platform/epoch68/emulator's ACK ABI deliberately provides 16-bit int and 32-bit pointers,
 * which is the representation this code expects.
 */

#include <ctype.h>
#include <setjmp.h>
#include <sgtty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "red1.h"
#include "redbuf.h"

#define VERSION		"platform/epoch68/emulator V7 edition"
#define SIGNON		"Welcome to RED."
#define COPYRIGHT	"Based on Edward K. Ream's RED editor."

#define HAS_STATIC	1
#define HAS_LONG	1
#define CAST		1
#define INT_SIZE	2
#define HELP_CMND	1

#define MAXLEN		200
#define MAXLEN1		201
#define SYSFNMAX	128

#define EOS		0
#define ERROR		(-1)
#define OK		0
#define YES		1
#define NO		0
#define CR		13
#define LF		10
#define NEWLINE		10
#define TAB		9
#define CPMEOF		0x1a
#define HUGE		32000
#define min(a, b)	((a) < (b) ? (a) : (b))
#define max(a, b)	((a) > (b) ? (a) : (b))

#define XSIGN		"ESC ESC or control-C enters command mode."
#define XSIGN1		"Use control keys or VT100 arrow keys to move."

extern char g_file[SYSFNMAX];
extern char red_temp_file[32];
extern char red_data_file[32];
extern jmp_buf DISK_ERR;

#define TEMP_FILE	red_temp_file
#define DATA_FILE	red_data_file

extern int def0mode;
extern int def1mode;
extern int def2mode;

extern int hasdn;
extern int hasup;
extern int hasins;
extern int hasdel;
extern int hasint;
extern int hascol;
extern int haswrap;

extern int DATA_RES;
extern int b_fatal;
extern int b_cflag;
extern int b_line;
extern int b_max_line;
extern int b_start;
extern int b_head;
extern int b_tail;
extern int b_max_diskp;
extern int b_max_put;
extern int b_data_fd;
extern int b_user_fd;
extern int b_free;
extern char b_buff[DATA_SIZE];
extern struct BLOCK *b_bp;
extern struct BLOCK *b_bpp[MAX_RES];
extern char *br_bufp;
extern int br_bufc;
extern int br_count;
extern int br_avail;
extern int br_out;
extern int bw_count;

#include "redproto.h"

#endif
