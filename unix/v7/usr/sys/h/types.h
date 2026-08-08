#ifndef V7_SYS_TYPES_H
#define V7_SYS_TYPES_H

#include "inttypes.h"

typedef	i32       		daddr_t;  	/* disk address */
typedef	char *     		caddr_t;  	/* core address */
typedef	u16				ino_t;     	/* i-node number */
typedef	i32       		time_t;   	/* a time */
typedef	u32 			label_t[13]; 	/* pc, sp, d2-d7, a2-a6 */
typedef	u16       		dev_t;    	/* device code */
typedef	u16       		mode_t;   	/* file mode */
typedef	i32       		off_t;    	/* offset in file */

/* selectors and constructor for device code */
#define	major(x)  		(i32)(((u32)x>>8))
#define	minor(x)  		(i32)((u32)(x)&0377)
#define	makedev(x,y)	(dev_t)((x)<<8|(y))

#endif /* V7_SYS_TYPES_H */
