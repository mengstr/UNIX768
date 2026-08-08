#ifndef __LOCAL_H_INCLUDED__
#define __LOCAL_H_INCLUDED__

/*
 * Host-side led memory sizing.
 * Target-hosted led defines ACK_TARGET_LED_SMALL and uses the compact,
 * dynamically growing tables from mach.h instead.
 */
#ifndef ACK_TARGET_LED_SMALL
#define BIGMACHINE 1
#endif

#endif
