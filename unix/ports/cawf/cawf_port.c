#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sys/inttypes.h"

#define LINE_MAX 512
#define WIDTH 76

static i32 fill;
static i32 col;
static i32 glue_next;

static void newline(void);
static void blank(void);
static void put_word(char *word);
static void text_line(char *line);
static char *skip_space(char *s);
static void macro_line(char *line);
static void file(FILE *fp);
static void put_text(char *s);
static i32 is_open_punct(char *word);
static i32 is_close_punct(char *word);
static char *decode_escape(char *src, char *dst, i32 *used);

static void
newline(void)
{
	if (col != 0) {
		putchar('\n');
		col = 0;
	}
	glue_next = 0;
}

static void
blank(void)
{
	newline();
	putchar('\n');
}

static void
put_text(char *s)
{
	for (; *s != '\0'; s++) {
		if (*s == '`')
			putchar('\'');
		else
			putchar(*s);
	}
}

static void
put_word(char *word)
{
	i32 len;
	i32 need_space;

	len = strlen(word);
	if (len == 0)
		return;
	need_space = (col != 0 && !glue_next && !is_close_punct(word));
	if (fill && need_space && col + 1 + len > WIDTH)
		newline();
	if (col != 0 && !glue_next && !is_close_punct(word)) {
		putchar(' ');
		col++;
	}
	put_text(word);
	col += len;
	glue_next = is_open_punct(word);
}

static char *
skip_space(char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return(s);
}

static i32
is_open_punct(char *word)
{
	return(strcmp(word, "(") == 0
		|| strcmp(word, "[") == 0
		|| strcmp(word, "`") == 0);
}

static i32
is_close_punct(char *word)
{
	return(word[0] == ')'
		|| word[0] == ']'
		|| word[0] == ','
		|| word[0] == '.'
		|| word[0] == ';'
		|| word[0] == ':'
		|| word[0] == '?'
		|| word[0] == '!'
		|| word[0] == '\'');
}

static char *
decode_escape(char *src, char *dst, i32 *used)
{
	*used = 1;
	if (src[0] != '\\') {
		*dst = src[0];
		return(dst + 1);
	}
	if (src[1] == '\0') {
		*dst = '\\';
		return(dst + 1);
	}
	switch (src[1]) {
	case '&':
		*used = 2;
		return(dst);
	case '-':
		*used = 2;
		*dst = '-';
		return(dst + 1);
	case 'e':
		*used = 2;
		*dst = '\\';
		return(dst + 1);
	case 'f':
		*used = (src[2] != '\0') ? 3 : 2;
		return(dst);
	case '(':
		if (src[2] == 'e' && src[3] == 'm') {
			*used = 4;
			*dst = '-';
			return(dst + 1);
		}
		if (src[2] == 'm' && src[3] == 'i') {
			*used = 4;
			*dst = '-';
			return(dst + 1);
		}
		if (src[2] == 'b' && src[3] == 'u') {
			*used = 4;
			*dst = '*';
			return(dst + 1);
		}
		break;
	case '\\':
		*used = 2;
		*dst = '\\';
		return(dst + 1);
	}
	*used = 2;
	*dst = src[1];
	return(dst + 1);
}

static void
text_line(char *line)
{
	char word[LINE_MAX];
	i32 i;
	i32 used;
	char *out;

	if (!fill) {
		for (; *line != '\0'; line += used) {
			out = word;
			out = decode_escape(line, out, &used);
			*out = '\0';
			put_text(word);
		}
		newline();
		return;
	}
	while (*line != '\0') {
		line = skip_space(line);
		if (*line == '\0')
			break;
		for (i = 0; *line != '\0' && *line != ' ' && *line != '\t'; line += used) {
			out = &word[i];
			out = decode_escape(line, out, &used);
			i = out - word;
			if (i >= LINE_MAX - 1)
				break;
		}
		word[i] = '\0';
		put_word(word);
	}
}

static void
macro_line(char *line)
{
	char *arg;

	arg = skip_space(line + 3);
	if (strncmp(line, ".TH", 3) == 0) {
		newline();
		put_text(arg);
		blank();
		return;
	}
	if (strncmp(line, ".SH", 3) == 0 || strncmp(line, ".SS", 3) == 0) {
		blank();
		put_text(arg);
		blank();
		return;
	}
	if (strncmp(line, ".PP", 3) == 0 || strncmp(line, ".LP", 3) == 0
	||  strncmp(line, ".P ", 3) == 0 || strncmp(line, ".IP", 3) == 0
	||  strncmp(line, ".TP", 3) == 0) {
		blank();
		return;
	}
	if (strncmp(line, ".br", 3) == 0) {
		newline();
		return;
	}
	if (strncmp(line, ".nf", 3) == 0) {
		newline();
		fill = 0;
		return;
	}
	if (strncmp(line, ".fi", 3) == 0) {
		newline();
		fill = 1;
		return;
	}
	if (strncmp(line, ".B ", 3) == 0 || strncmp(line, ".I ", 3) == 0
	||  strncmp(line, ".RI", 3) == 0 || strncmp(line, ".IR", 3) == 0
	||  strncmp(line, ".BR", 3) == 0 || strncmp(line, ".RB", 3) == 0) {
		text_line(arg);
		return;
	}
	if (line[0] == '.' || line[0] == '\'')
		return;
	text_line(line);
}

static void
file(FILE *fp)
{
	char line[LINE_MAX];
	i32 len;

	while (fgets(line, sizeof(line), fp) != NULL) {
		len = strlen(line);
		if (len != 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		if (line[0] == '.' || line[0] == '\'')
			macro_line(line);
		else
			text_line(line);
	}
	newline();
}

int
main(int argc, char **argv)
{
	FILE *fp;
	i32 i;
	i32 files;

	fill = 1;
	col = 0;
	glue_next = 0;
	files = 0;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '\0') {
				file(stdin);
				files++;
			}
			continue;
		}
		fp = fopen(argv[i], "r");
		if (fp == NULL) {
			perror(argv[i]);
			exit(1);
		}
		file(fp);
		fclose(fp);
		files++;
	}
	if (files == 0)
		file(stdin);
	exit(0);
}
