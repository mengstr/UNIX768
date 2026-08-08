#ifndef V7_SAIO_STR_H
#define V7_SAIO_STR_H

#include "sys/inttypes.h"
#include "sys/inode.h"

/*
 * Shared standalone I/O structs and constants.
 *
 * This is the declaration-only fragment of saio.h, split out so both the
 * monitor firmware and standalone-style userland code can share the same
 * layout without inheriting tentative definitions.
 */

struct	iob {
	char	i_flgs;
	struct inode i_ino;
	i32	i_unit;
	daddr_t	i_boff;
	daddr_t	i_cyloff;
	off_t	i_offset;
	daddr_t	i_bn;
	char	*i_ma;
	i32	i_cc;
	char	i_buf[512];
};

#define F_READ	01
#define F_WRITE	02
#define F_ALLOC	04
#define F_FILE	010

struct devsw {
	char	*dv_name;
	i32	(*dv_strategy)(struct iob *io, i32 func);
	i32	(*dv_open)(struct iob *io);
	i32	(*dv_close)(struct iob *io);
};

#define	READ	1
#define	WRITE	2

#define	NBUFS	4
#define	NFILES	4

#endif
