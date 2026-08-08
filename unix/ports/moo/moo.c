/*  Game: MOO  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE  4
#define TEN   10

int nbulls, ncows, nattempts;
char number[SIZE], guess[TEN];

static void numgen(void);
static void printout(void);
static i32 takeguess(void);
static void cmatch(void);
static void instruct(void);

int
main(int argc, char *argv[])
{
	int c, i;

	while ((c = getopt((int)argc, argv, "i")) != EOF) {
		switch (c) {
		case 'i':
			instruct();
			break;
		case '?':
			printf("usage: moo [-i]\n");
			exit(2);
		default:
			break;
		}
	}

	printf("MOO\n");
	for (;;) {
		printf("new game\n");
		nbulls = ncows = nattempts = 0;

		numgen();
		while (nbulls < 4) {
			nbulls = ncows = 0;
			for (i=0; i<SIZE; ++i) guess[i] = '\b';
			if (takeguess() == 1)
				exit(1);
			++nattempts;
			cmatch();
			printout();
		}

		printf("Attempts = %d\n", nattempts);
		continue;
	}
}

static void
numgen(void)    /*  generate number consisting of four random digits  */
{
	int i, j, mark;

	i = 0;
	while (i < SIZE)  {
		mark = 0;
		number[i] = rand() % 10 + '0';
		for ( j = i-1; j >= 0; --j)  {
			if (number[i] == number[j])  {
				mark = 1;
				break;
			}
		}
		if (mark == 0)
			++i;
	}
}

static void
printout(void)
{
	printf("bulls = %d      cows = %d\n", nbulls,ncows);
}

static i32
takeguess(void)    /*  take input guess */
{
	int i, flag;

	flag = 1;
	while (flag)  {
		flag = 0;
		printf("your guess?\n");
		if (gets(guess) != NULL) {
			if (guess[0] == 'q' && guess[1] == '\0')
				return(1);
			for (i=0; i<SIZE; ++i)  {
				if (guess[i] == '\b' || guess[i]<'0' || guess[i]>'9') {
					flag = 1;
					printf("bad guess\n");
					break;
				}
			}
			if (guess[SIZE] != '\0')  {
				printf("bad guess\n");
				flag = 1;
			}
		}
		else
			return(1);
	}
	return(0);
}

static void
cmatch(void)   /*  matching of player's guess and actual number  */
{
	int i,j;

	i=j=0;

	while(i < SIZE) {
		while(j < SIZE) {
			if (guess[i] != number[j])
				++j;
			else  {
				if (i == j)
					++nbulls;
				else
					++ncows;
				if (i <= SIZE - 1) {
					if (i == SIZE - 1) {
						++i;
						break;
					}
					++i;
					j = 0;
				}
			}
		}
		if (i < SIZE) {
			++i;
			j = 0;
		}
	}
}

    /*  set of instructions for the game  */
char *inst[] = {
	"How to play MOO:",
	"The computer selects a random number which consists of four",
	"different digits. The objective of the game is for the player",
	"to guess the correct digits and their correct positions. A",
	"correctly guessed digit and its position is called a bull.",
	"A cow is when a number is correctly guessed but not its position.",
	"A player correctly guesses the number when the number of bulls is",
	"equal to four. The number of attempts that the player took to",
	"guess is given at the end of each game. When a game is finished",
	"(bulls=4), another one begins immediately. If the player does not",
	"wish to continue playing, he or she should hit the break key or",
	"print the character q.",
	"Have fun!",
	"",
	};

static void
instruct(void)
{
	register char **cpp;

	printf("\n");

	for ( cpp = inst; **cpp != '\0'; ++cpp )
		printf("%s\n", *cpp);
}
