/*
 * Companion glob command for osh.
 *
 * osh execs /etc/glob with argv[1] as the command to run and argv[2...]
 * as the command arguments when it sees shell metacharacters.  This program
 * expands filename patterns and then execs the requested command.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <unistd.h>

#define MAXARGS	200
#define STRSIZ	8192

char *nargv[MAXARGS];
char strings[STRSIZ];
char *strp = strings;

int hasmeta(char *s);
int expand(char *pattern, int nargc);
int match(char *s, char *p);
int addarg(int nargc, char *s);
char *savestr(char *s);
int hasslash(char *s);
void sortargs(int first, int last);
void execit(char *cmd, char **argv);

int
main(int argc, char **argv)
{
	register int i;
	int nargc;

	if(argc < 2)
		exit(1);

	nargc = 0;
	nargv[nargc++] = argv[1];
	for(i = 2; i < argc; i++) {
		if(hasmeta(argv[i]))
			nargc = expand(argv[i], nargc);
		else
			nargc = addarg(nargc, argv[i]);
	}
	nargv[nargc] = 0;
	execit(nargv[0], nargv);
	fprintf(stderr, "glob: cannot execute %s\n", nargv[0]);
	exit(1);
}

int
hasmeta(s)
char *s;
{
	while(*s) {
		if(*s == '*' || *s == '?' || *s == '[')
			return(1);
		s++;
	}
	return(0);
}

int
expand(pattern, nargc)
char *pattern;
int nargc;
{
	FILE *fp;
	struct direct d;
	char dir[128];
	char prefix[128];
	char pat[DIRSIZ+1];
	char name[DIRSIZ+1];
	char full[160];
	char *slash;
	register char *p;
	register int i;
	int first;
	int oldargc;

	slash = 0;
	for(p = pattern; *p; p++)
		if(*p == '/')
			slash = p;

	if(slash == 0) {
		strcpy(dir, ".");
		prefix[0] = 0;
		strcpy(pat, pattern);
	} else {
		if(slash == pattern) {
			strcpy(dir, "/");
			strcpy(prefix, "/");
		} else {
			i = slash - pattern;
			strncpy(dir, pattern, i);
			dir[i] = 0;
			strncpy(prefix, pattern, i);
			prefix[i] = '/';
			prefix[i+1] = 0;
		}
		strcpy(pat, slash+1);
	}

	oldargc = nargc;
	first = nargc;
	fp = fopen(dir, "r");
	if(fp == NULL)
		return(addarg(nargc, pattern));

	while(fread((char *)&d, sizeof(d), 1, fp) == 1) {
		if(d.d_ino == 0)
			continue;
		for(i = 0; i < DIRSIZ; i++)
			name[i] = d.d_name[i];
		name[DIRSIZ] = 0;
		if(name[0] == '.' && pat[0] != '.')
			continue;
		if(!match(name, pat))
			continue;
		strcpy(full, prefix);
		strcat(full, name);
		nargc = addarg(nargc, full);
	}
	fclose(fp);

	if(nargc == oldargc)
		return(addarg(nargc, pattern));
	sortargs(first, nargc);
	return(nargc);
}

int
match(s, p)
char *s, *p;
{
	register int scc;
	register int c;
	int ok;
	int lc;

	scc = *s++;
	switch(c = *p++) {
	case '[':
		ok = 0;
		lc = 077777;
		while((c = *p++) != 0) {
			if(c == ']')
				return(ok ? match(s, p) : 0);
			if(c == '-') {
				if(lc <= scc && scc <= *p++)
					ok++;
			} else {
				if(scc == (lc = c))
					ok++;
			}
		}
		return(0);

	case '?':
		return(scc ? match(s, p) : 0);

	case '*':
		if(*p == 0)
			return(1);
		--s;
		while(*s)
			if(match(s++, p))
				return(1);
		return(0);

	case 0:
		return(scc == 0);

	default:
		if(c != scc)
			return(0);
		return(match(s, p));
	}
}

int
addarg(nargc, s)
int nargc;
char *s;
{
	if(nargc >= MAXARGS-1) {
		fprintf(stderr, "glob: too many args\n");
		exit(1);
	}
	nargv[nargc++] = savestr(s);
	return(nargc);
}

char *
savestr(s)
char *s;
{
	char *p;
	int n;

	n = strlen(s) + 1;
	if(strp + n >= &strings[STRSIZ]) {
		fprintf(stderr, "glob: string space\n");
		exit(1);
	}
	p = strp;
	strcpy(p, s);
	strp += n;
	return(p);
}

void
sortargs(first, last)
int first, last;
{
	register int i, j;
	char *t;

	for(i = first + 1; i < last; i++) {
		t = nargv[i];
		j = i;
		while(j > first && strcmp(nargv[j-1], t) > 0) {
			nargv[j] = nargv[j-1];
			j--;
		}
		nargv[j] = t;
	}
}

int
hasslash(s)
char *s;
{
	while(*s) {
		if(*s++ == '/')
			return(1);
	}
	return(0);
}

void
execit(file, argv)
char *file;
char **argv;
{
	char path[128];

	execv(file, argv);
	if(hasslash(file))
		return;
	strcpy(path, "/bin/");
	strcat(path, file);
	execv(path, argv);
	strcpy(path, "/usr/bin/");
	strcat(path, file);
	execv(path, argv);
}
