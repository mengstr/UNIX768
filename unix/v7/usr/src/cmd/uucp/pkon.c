/*
 * Epoch68 uses the historical user-level g-protocol implementation in
 * pk0.c and pk1.c.  The optional V7 kernel packet line discipline is not
 * configured: its kernel pk0.c-pk3.c sources have not been ported to the
 * Epoch68 tty, memory, and synchronization interfaces.  Keep these stubs so
 * cico can be built without advertising a kernel packet driver.
 */

int
pkon (int fd, int size)
{
	(void)fd;
	(void)size;
	return(0);
}

int
pkoff (int fd)
{
	(void)fd;
	return(0);
}
