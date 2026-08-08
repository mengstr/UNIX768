#include <sys/param.h>
#include <sys/inode.h>
#include "v7io.h"

i32 devread(struct iob *io);
i32 devwrite(struct iob *io);
void devopen(struct iob *io);
void devclose(struct iob *io);
static i32 nullsys(struct iob *io);

i32
devread(register struct iob *io)
{

	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io,READ) );
}

/*
 * The current platform/epoch68/emulator standalone path is read-only.
 */
i32
devwrite(register struct iob *io)
{
	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE) );
}

void
devopen(register struct iob *io)
{
	(*devsw[io->i_ino.i_dev].dv_open)(io);
}

void
devclose(register struct iob *io)
{
	(*devsw[io->i_ino.i_dev].dv_close)(io);
}

static i32
nullsys(struct iob *io)
{
	(void)io;
	return(0);
}

i32 rdstrategy(struct iob *io, i32 func);
i32 sdstrategy(struct iob *io, i32 func);

struct devsw devsw[] = {
	"rd",	rdstrategy,	nullsys,	nullsys,
	"sd",	sdstrategy,	nullsys,	nullsys,
	0,0,0,0
};
