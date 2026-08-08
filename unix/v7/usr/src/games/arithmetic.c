#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX 100

char types[10];
int right[MAX];
int left[MAX];
int rights;
int wrongs;
long stvec;
long etvec;
long dtvec;

static void get_line(char *s);
static i32 getnum(char *s);
static i32 random(i32 range);
static i32 skrand(i32 range);
static void score(void);
static void delete(i16 sig);

int
main(int argc, char *argv[])
{
	int range, k, dif, l;
	char line[100];
	int ans, pans, i, j, t;

	signal(SIGINT, delete);

	range = 11;
	dif = 0;
	while (argc > 1) {
		switch (*argv[1]) {
		case '+':
		case '-':
		case 'x':
		case '/':
			while ((types[dif] = argv[1][dif]) != '\0')
				dif++;
			break;

		default:
			range = getnum(argv[1]) + 1;
		}
		argv++;
		argc--;
	}
	if (range > MAX) {
		printf("Range is too large.\n");
		exit(0);
	}

	if (dif == 0) {
		types[0] = '+';
		types[1] = '-';
		dif = 2;
	}

	for (i = 0; i < range; i++)
		left[i] = right[i] = i;
	time(&stvec);
	k = stvec;
	srand(k);
	k = 0;
	l = 0;
	goto start;

loop:
	if (++k % 20 == 0)
		score();

start:
	i = skrand(range);
	j = skrand(range);
	if (dif > 1)
		l = random(dif);

	switch (types[l]) {
	case '+':
	default:
		ans = left[i] + right[j];
		printf("%d + %d =   ", left[i], right[j]);
		break;

	case '-':
		t = left[i] + right[j];
		ans = left[i];
		printf("%d - %d =   ", t, right[j]);
		break;

	case 'x':
		ans = left[i] * right[j];
		printf("%d x %d =   ", left[i], right[j]);
		break;

	case '/':
		while (right[j] == 0)
			j = random(range);
		t = left[i] * right[j] + random(right[j]);
		ans = left[i];
		printf("%d / %d =   ", t, right[j]);
		break;
	}

loop1:
	get_line(line);
	dtvec += etvec - stvec;
	if (line[0] == '\n')
		goto loop1;
	pans = getnum(line);
	if (pans == ans) {
		printf("Right!\n");
		rights++;
		goto loop;
	} else {
		printf("What?\n");
		wrongs++;
		if (range >= MAX)
			goto loop1;
		left[range] = left[i];
		right[range++] = right[j];
		goto loop1;
	}
}

static void
get_line(char *s)
{
	register char *rs;

	rs = s;
	while ((*rs = getchar()) == ' ')
		;
	while (*rs != '\n') {
		if (*rs == 0)
			exit(0);
		else if (rs >= &s[99]) {
			while ((*rs = getchar()) != '\n')
				if (*rs == '\0')
					exit(0);
		} else
			*++rs = getchar();
	}
	while (*--rs == ' ')
		*rs = '\n';
}

static i32
getnum(char *s)
{
	int a;
	char c;

	a = 0;
	while ((c = *s++) >= '0' && c <= '9')
		a = a * 10 + c - '0';
	return a;
}

static i32
random(i32 range)
{
	return rand() % range;
}

static i32
skrand(i32 range)
{
	int temp;

	temp = random(range) + random(range);
	if (temp > range - 1)
		temp = 2 * range - 1 - temp;
	return temp;
}

static void
score(void)
{
	time(&etvec);

	printf("\n\nRights %d; Wrongs %d; Score %d%%\n", rights, wrongs,
	    (rights * 100) / (rights + wrongs));

	if (rights == 0)
		return;
	printf("Total time %ld seconds; %.1f seconds per problem\n\n\n",
	    etvec - stvec, (etvec - stvec) / (rights + 0.));

	sleep(3);
	time(&dtvec);
	stvec += dtvec - etvec;
}

static void
delete(i16 sig)
{
	(void)sig;
	if (rights + wrongs == 0) {
		printf("\n");
		exit(0);
	}
	score();
	exit(0);
}
