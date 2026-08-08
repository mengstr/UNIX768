#ifndef REFER_H
#define REFER_H

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

union ptr {
	unsigned *a;
	long *b;
};

int hash(char *);
void err(char *, ...);
int prefix(char *, char *);
char *mindex(char *, int);
void *zalloc(unsigned, unsigned);

void huntmain(int, char **);
char *todir(char *);
void savedir(void);
void restodir(void);
int makefgrep(char *);
int ckexist(char *, char *);
FILE *iopen(char *, char *);
int grepcall(char *, char *, char *);
void clfgrep(void);
int fgrep(int, char **);
int getq(char **);
int doquery(long *, int, FILE *, int, char **, union ptr);
int baddrop(union ptr, int, FILE *, int, char **, char *, int);
void result(union ptr, int, FILE *);
long gdate(FILE *);
int findline(char *, char *, int, long);
int remote(char *, char *);
int corout(char *, char *, char *, char *, int);
void shell(int, int (*)(int, int), void (*)(int, int));
long getl(FILE *);
void putl(long, FILE *);
int ref_getw(FILE *);
void ref_putw(int, FILE *);
int hcomp(int, int);
void hexch(int, int);
int auxil(char *, char *);
char *trimnl(char *);
int common(char *);
void dofile(FILE *, char *);

int getargs(char *, char **);
int newkeys(FILE *, FILE *, FILE *, int, FILE *, int *);
void whash(FILE *, FILE *, FILE *, int, int, long *, int *);

#endif
