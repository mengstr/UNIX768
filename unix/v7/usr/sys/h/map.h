#ifndef V7_SYS_MAP_H
#define V7_SYS_MAP_H

struct map
{
	short	m_size;
	unsigned short m_addr;
};

extern struct map coremap[CMAPSIZ];	/* space for core allocation */
extern struct map swapmap[SMAPSIZ];	/* space for swap allocation */
extern struct map userpagemap[CMAPSIZ]; /* physical 256 KiB RAM pages */

#endif /* V7_SYS_MAP_H */
