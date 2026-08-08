#include <stdio.h>
#include <stdarg.h>
#include "types.h"

extern char _sibuf[];
extern char _sobuf[];
extern struct _iobuf _iob[];
extern struct _iobuf *_lastbuf;

extern i32 read(i32 fd, char *buf, i32 count);
extern i32 write(i32 fd, char *buf, i32 count);
extern i32 open(char *path, i32 mode);
extern i32 creat(char *path, i32 mode);
extern i32 close(i32 fd);
extern i32 dup(i32 fd);
extern i32 execl(char *path, char *arg0, ...);
extern void exit(int status);
extern i32 fork(void);
extern i32 pipe(i16 *fds);
extern i32 wait(i16 *status);
extern off32 lseek(i32 fd, off32 offset, i32 whence);
extern char *malloc(usize size);
extern void free(void *ptr);
extern i32 isatty(i32 fd);

FILE *_findiop(void);
i32 _filbuf(FILE *iop);
i32 _flsbuf(i32 c, FILE *iop);
FILE *_endopen(char *file, char *mode, FILE *iop);
i32 _doprnt(char *fmt, char *argp, FILE *stream);
char *_pfloat(double value, char *buf, int precision, int specified);
char *_pscien(double value, char *buf, int precision, int specified);
char *_fp_special(double value, char *buf);
char *gcvt(double value, int ndigit, char *buf);
i32 _doscan_file(FILE *fp, char *fmt, va_list args);
i32 _doscan_string(char *str, char *fmt, va_list args);
