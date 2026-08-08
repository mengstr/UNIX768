#include "sys/inttypes.h"
#include "stdarg.h"

#ifndef _STDIO_H_
#define _STDIO_H_

#define	BUFSIZ	512
#define FILENAME_MAX 128
#define	_NFILE	20
# ifndef FILE
extern	struct	_iobuf {
	char	*_ptr;
	i32	_cnt;
	char	*_base;
	char	_flag;
	char	_file;
} _iob[_NFILE];
# endif

#define	_IOREAD	01
#define	_IOWRT	02
#define	_IONBF	04
#define	_IOMYBUF	010
#define	_IOEOF	020
#define	_IOERR	040
#define	_IOSTRG	0100
#define	_IORW	0200

#define	NULL	0
#define	FILE	struct _iobuf
#define	EOF	(-1)
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])
#define	getc(p)		fgetc(p)
#define	getchar()	fgetc(stdin)
#define	putc(x,p)	fputc((i32)(x),p)
#define	putchar(x)	fputc((i32)(x),stdout)
#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	p->_file

FILE	*fopen(char *, char *);
FILE	*freopen(char *, char *, FILE *);
FILE	*fdopen(i32, char *);
FILE	*popen(char *, char *);
FILE	*tmpfile(void);
i32	ftell(FILE *);
char	*fgets(char *, u16, FILE *);
char	*gets(char *);
i32	fclose(FILE *);
i32	pclose(FILE *);
i32	fflush(FILE *);
i32	fgetc(FILE *);
i32	fputc(i32, FILE *);
i32	fputs(char *, FILE *);
i32	ungetc(i32, FILE *);
i32	fprintf(FILE *, char *, ...);
i32	fscanf(FILE *, char *, ...);
i32	fseek(FILE *, long, i32);
i32	fread(char *, u16, u16, FILE *);
i32	fwrite(char *, u16, u16, FILE *);
i32	printf(char *, ...);
i32	perror(char *);
i32	puts(char *);
i32	remove(char *);
i32	scanf(char *, ...);
i32	sscanf(char *, char *, ...);
i32	vprintf(char *, va_list);
i32	vfprintf(FILE *, char *, va_list);
char	*sprintf(char *, char *, ...);
void	rewind(FILE *);
void	setbuf(FILE *, char *);

#endif
