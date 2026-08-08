#include <sys/param.h>
#include <sys/inode.h>
#include "disk.h"
#include "v7io.h"

i32 rdstrategy(struct iob *io, i32 func);

i32
rdstrategy(struct iob *io, i32 func)
{
	u16 sectors;
	i32 rc;

	if ((io->i_cc % EPOCH68_DISK_SECTOR_SIZE) != 0)
		return(-1);

	sectors = (u16)(io->i_cc / EPOCH68_DISK_SECTOR_SIZE);
	if (func == READ)
		rc = epoch68_disk_read((u32)io->i_bn, io->i_ma, sectors);
	else
		rc = epoch68_disk_write((u32)io->i_bn, io->i_ma, sectors);

	if (rc < 0)
		return(-1);
	return(io->i_cc);
}
