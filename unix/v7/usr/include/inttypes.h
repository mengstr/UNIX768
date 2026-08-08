/*
 * Fixed-width integer types.
 *
 * Single source of truth: usr/sys/h/inttypes.h -- the machine/kernel layer
 * owns the word widths, and everything else depends downward onto it.  This
 * file is a thin shim so the definitions (and their compile-time width and
 * signedness asserts) can never drift out of sync.
 */
#include "../sys/h/inttypes.h"
