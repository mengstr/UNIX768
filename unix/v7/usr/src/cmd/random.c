#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXINT 32768.0

static float fract = 2.0;
static i32 rflag;
static i32 eflag;
static char ibuf[BUFSIZ];
static char obuf[BUFSIZ];
static char line[BUFSIZ];

static float parse_fraction(char *text);

static float
parse_fraction(char *text)
{
	float value;
	float scale;
	i32 exponent;
	i32 negative;
	i32 exponent_negative;
	char c;

	value = 0.0;
	negative = 0;
	if (*text == '-') {
		negative = 1;
		text++;
	} else if (*text == '+')
		text++;
	while (*text >= '0' && *text <= '9')
		value = value * 10.0 + (*text++ - '0');
	if (*text == '.') {
		text++;
		scale = 0.1;
		while (*text >= '0' && *text <= '9') {
			value += (*text++ - '0') * scale;
			scale *= 0.1;
		}
	}
	if (*text == 'e' || *text == 'E') {
		text++;
		exponent = 0;
		exponent_negative = 0;
		if (*text == '-') {
			exponent_negative = 1;
			text++;
		} else if (*text == '+')
			text++;
		while ((c = *text++) >= '0' && c <= '9')
			exponent = exponent * 10 + c - '0';
		while (exponent-- > 0) {
			if (exponent_negative)
				value *= 0.1;
			else
				value *= 10.0;
		}
	}
	return(negative ? -value : value);
}

i32
main(i32 argc, char **argv)
{
	time_t tvec;
	i32 i;
	char c;

	for (i = 1; i < argc; i++) {
		if (*argv[i] != '-')
			fract = parse_fraction(argv[i]);
		else if ((c = argv[i][1]) == 'e')
			eflag = 1;
		else if (c == 'r')
			rflag = 1;
	}
	time(&tvec);
	srand((u32)tvec);
	if (!rflag && !eflag) {
		setbuf(stdin, ibuf);
		setbuf(stdout, obuf);
	}
	for (; eflag == 0;) {
		gets(line);
		if (feof(stdin))
			break;
		if (rand() / MAXINT * fract < 1.0)
			puts(line);
	}
	exit((i32)(rand() / MAXINT * fract));
	return(0);
}
