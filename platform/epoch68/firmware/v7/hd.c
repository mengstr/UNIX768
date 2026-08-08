#include "pager.h"
#include "../string.h"
#include "v7io.h"

i32 hd(char *path);

static char
hex_digit(i32 n)
{
	n &= 0xf;
	if (n < 10)
		return('0' + n);
	return('a' + (n - 10));
}

static void
format_hex_line(char *dst, i32 offset, char *buf, i32 count)
{
	i32 i;
	char *p;

	p = dst;
	sprintf(p, "%08lx: ", offset);
	while (*p != '\0')
		p++;
	for (i = 0; i < 16; i++) {
		if (i < count) {
			*p++ = hex_digit((buf[i] >> 4) & 0xf);
			*p++ = hex_digit(buf[i] & 0xf);
		} else {
			*p++ = ' ';
			*p++ = ' ';
		}
		*p++ = ' ';
	}
	*p++ = '|';
	for (i = 0; i < count; i++) {
		if (buf[i] >= ' ' && buf[i] <= '~')
			*p++ = buf[i];
		else
			*p++ = '.';
	}
	*p++ = '|';
	*p++ = '\n';
	*p = '\0';
}

i32
hd(char *path)
{
	char buf[512];
	char line[80];
	i32 offset;
	i32 fd;
	i32 i;
	i32 n;

	fd = open(path, 0);
	if (fd < 0)
		return(-1);

	pager_reset();
	offset = 0;
	for (;;) {
		n = read(fd, buf, sizeof(buf));
		if (n <= 0)
			break;
		for (i = 0; i < n; i += 16) {
			format_hex_line(line, offset + i, &buf[i],
			    (n - i > 16) ? 16 : (n - i));
			if (pager_write(line, strlen(line)) < 0)
				break;
		}
		if (i < n)
			break;
		offset += n;
	}

	close(fd);
	return(0);
}
