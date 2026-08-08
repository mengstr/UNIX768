#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"

#define EPOCH68_USER_CLICKS	((256UL * 1024UL) / 64UL)

i32 estabur(u32 nt, u32 nd, u32 ns, i32 sep, i32 xrw);
void sureg(void);

/*
 * Load the user mapping for the current process.
 *
 * Epoch68 selects a process's fixed 256 KiB user page in swtch(); it has no
 * PDP-11 user segmentation registers to reload here.  Keep the V7 hook so
 * the machine-independent scheduler and text code retain their usual shape.
 */
void
sureg()
{
}

/*
 * Check whether the requested text, data and stack layout fits the fixed
 * Epoch68 user window.  Unlike the PDP-11, Epoch68 has neither separate I/D
 * spaces nor per-segment access controls, so sep is unsupported and xrw does
 * not alter the mapping.
 *
 * On Epoch68 nd is the absolute combined text/data/bss/heap end and nt is the
 * text extent within that image.  The mapped image end is therefore the
 * larger of nt and nd, followed by the stack allocation.
 */
i32
estabur(nt, nd, ns, sep, xrw)
u32 nt;
u32 nd;
u32 ns;
i32 sep;
i32 xrw;
{
	u32 image_end;

	image_end = max(nt, nd);
	if(sep || image_end > EPOCH68_USER_CLICKS ||
	    ns > EPOCH68_USER_CLICKS-image_end) {
		u.u_error = ENOMEM;
		return(-1);
	}
	return(0);
}
