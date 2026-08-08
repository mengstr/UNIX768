#include "red.h"

char g_file[SYSFNMAX];
char red_temp_file[32];
char red_data_file[32];
jmp_buf DISK_ERR;

int hasdn;
int hasup;
int hasins;
int hasdel;
int hasint;
int hascol;
int haswrap;

int DATA_RES;
int b_fatal;
int b_cflag;
int b_line;
int b_max_line;
int b_start;
int b_head;
int b_tail;
int b_max_diskp;
int b_max_put;
int b_data_fd;
int b_user_fd;
int b_free;
char b_buff[DATA_SIZE];
struct BLOCK *b_bp;
struct BLOCK *b_bpp[MAX_RES];
char *br_bufp;
int br_bufc;
int br_count;
int br_avail;
int br_out;
int bw_count;
