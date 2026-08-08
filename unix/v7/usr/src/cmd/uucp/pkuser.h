#include <stdlib.h>
#include <unistd.h>

struct pack *pkopen(int, int);
int pkgetpack(struct pack *);
int pkdata(int, int, struct pack *, char **);
int pkxstart(struct pack *, int, int);
int pkmove(char *, char *, int, int);
int pkcget(int, char *, int);
int pkcntl(int, struct pack *);
int pkaccept(struct pack *);
int pkread(struct pack *, char *, int);
int pkwrite(struct pack *, char *, int);
int pksack(struct pack *);
int pkoutput(struct pack *);
int pkclose(struct pack *);
int pkreset(struct pack *);
int chksum(char *, int);
int pkline(struct pack *);
int pkzero(char *, int);
int pksize(int);
int pkfail(void);
