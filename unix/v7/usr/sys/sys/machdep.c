#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/map.h"
#include "../h/reg.h"

#define EPOCH68_FIRST_USER_PAGE 1
#define EPOCH68_LAST_USER_PAGE 62
#define EPOCH68_USER_PAGE_SIZE (256L * 1024L)

static char epoch68_userpage_owned[64];

void
userpage_init()
{
	register i32 i;

	for (i = 0; i < CMAPSIZ; i++) {
		userpagemap[i].m_addr = 0;
		userpagemap[i].m_size = 0;
	}
	for (i = 0; i < 64; i++)
		epoch68_userpage_owned[i] = 0;
	mfree(userpagemap, EPOCH68_LAST_USER_PAGE,
	    EPOCH68_FIRST_USER_PAGE);
}

i32
userpage_alloc()
{
	register i32 page;

	page = malloc(userpagemap, 1);
	if (page == 0)
		return(0);
	if (page < EPOCH68_FIRST_USER_PAGE ||
	    page > EPOCH68_LAST_USER_PAGE || epoch68_userpage_owned[page])
		panic("userpage alloc");
	epoch68_userpage_owned[page] = 1;
	return(page);
}

void
userpage_free(page)
i32 page;
{
	if (page < EPOCH68_FIRST_USER_PAGE ||
	    page > EPOCH68_LAST_USER_PAGE || !epoch68_userpage_owned[page])
		panic("userpage free");
	epoch68_userpage_owned[page] = 0;
	mfree(userpagemap, 1, page);
}

/*
 * Icode is the octal bootstrap
 * program executed in user mode
 * to bring up the system.
 */
unsigned short icode[] =
{
    0x41fa,0x0014,0x43fa,0x0008,0x700b,0x4e40,0x60fe,0x0000,
    0x0016,0x0000,0x0000,0x2f65,0x7463,0x2f69,0x6e69,0x7400,
};
i32	szicode = sizeof(icode);

unsigned short icode_multi[] =
{
    0x41fa,0x0018,0x43fa,0x0008,0x700b,0x4e40,0x60fe,0x0000,
    0x001a,0x0000,0x0024,0x0000,0x0000,0x2f65,0x7463,0x2f69,
    0x6e69,0x7400,0x2d6d,0x0000,
};
i32	szicode_multi = sizeof(icode_multi);

/*
 * Machine-dependent startup code
 */
void startup()
{
	printf("mem = %D\n", (long)(EPOCH68_LAST_USER_PAGE -
	    EPOCH68_FIRST_USER_PAGE + 1) * EPOCH68_USER_PAGE_SIZE);
}

i32
idle()
{
	return(0);
}

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_TIMER_CONTROL_REG 0x20
#define EPOCH68_TIMER_ENABLE 0x01

void clkstart()
{
	EPOCH68_MMIO_BASE[EPOCH68_TIMER_CONTROL_REG] = EPOCH68_TIMER_ENABLE;
}

/*
 * Let a process handle a signal by simulating an interrupt
 */
void sendsig(p, signo)
caddr_t p;
i32 signo;
{
	register u32 n;
	register u32 oldpc;
	register u32 oldsp;
	register u32 tramp;
	u8 frame[10];

	oldsp = (u32)u.u_ar0[R6];
	tramp = (u32)u.u_sigtramp;
	n = oldsp - sizeof(frame);
	oldpc = (u32)u.u_ar0[PC];
	if (n >= (256UL * 1024UL) || n > oldsp) {
		u.u_error = EFAULT;
		return;
	}
	frame[0] = ((u32)p >> 24) & 0xff;
	frame[1] = ((u32)p >> 16) & 0xff;
	frame[2] = ((u32)p >> 8) & 0xff;
	frame[3] = (u32)p & 0xff;
	frame[4] = (signo >> 8) & 0xff;
	frame[5] = signo & 0xff;
	frame[6] = (oldpc >> 24) & 0xff;
	frame[7] = (oldpc >> 16) & 0xff;
	frame[8] = (oldpc >> 8) & 0xff;
	frame[9] = oldpc & 0xff;
	if (copyout((caddr_t)frame, (caddr_t)n, sizeof(frame)) < 0) {
		u.u_error = EFAULT;
		return;
	}
	u.u_ar0[R6] = (i32)n;
	u.u_ar0[RPS] &= ~(TBIT|0x2000);
	u.u_ar0[PC] = (i32)tramp;
}
