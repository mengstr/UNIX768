/*
 * Seed dates for firmware-created V7 filesystems.
 *
 * Stored V7 times are GMT seconds.  The superblock time becomes the initial
 * system clock on boot.  File metadata keeps the vanity marker: Mar 18 1979
 * 19:06:05 local time, combining the 68000 release year with Mats's
 * 1965-03-18 birthday marker.
 */
#define EPOCH68_V7_SUPER_TIME 1780718400L
#define EPOCH68_V7_FILE_TIME 290649965L
#define EPOCH68_V7_FILE_LS_TIME " Mar 18 1979 "
