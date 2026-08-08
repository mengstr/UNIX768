/*
 * Historical V7 used (char **)-2 as a magic word at the top of the
 * 16-bit user address space.  Epoch68 has a 256 KiB flat user address
 * space and 32-bit stack slots, so use the equivalent reserved word that
 * exec() leaves at USER_STACK_TOP - EXEC_STACK_SLOT_SIZE.
 */
char ***execargs = (char***)0x0003fff8L;
