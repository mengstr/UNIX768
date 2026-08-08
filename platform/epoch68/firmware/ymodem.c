#include "uart.h"
#include "prf.h" /* IWYU pragma: keep */
#include "string.h"
#include "v7fs.h"
#include "v7io.h"
#include "ymodem.h"
#include <sys/dir.h>

i32 write(i32 fdesc, char *buf, i32 count);
i32 close(i32 fdesc);

#define MODEM_SOH 0x01
#define MODEM_STX 0x02
#define MODEM_EOT 0x04
#define MODEM_ACK 0x06
#define MODEM_NAK 0x15
#define MODEM_CAN 0x18
#define MODEM_CRC_REQ 'C'
#define MODEM_REQ_PREFIX "YSEND "
#define MODEM_RX_TIMEOUT 2000000L

static u16 ymodem_crc16(u8 *buf, i32 len);
static i32 metadata_parse(u8 *data, i32 length, char *name,
	i32 namesz, u32 *sizep);
static i32 invalid_filename(char *name);
static i32 ymodem_receive_internal(i32 channel, char *attached, char *cwd,
	char *request, char *path, char *buffer, i32 bufsz, i32 quiet);
static void close_if_open(i32 *fd);
static void print_received(char *name, u32 size);
static void send_request(i32 channel, char *request);
static char *request_basename(char *request);
static i32 ymodem_getc(i32 channel);
static i32 packet_read(i32 channel, u8 *data, u8 *blockp, i32 *lengthp);
static u32 parse_decimal(u8 *s);
static u16 ymodem_crc16_update(u16 crc, i32 ch);

static i32
ymodem_getc(i32 channel)
{
	return(uart_channel_getc_timeout(channel, MODEM_RX_TIMEOUT));
}

static i32
packet_read(i32 channel, u8 *data, u8 *blockp, i32 *lengthp)
{
    u8 block;
    u8 block_inv;
    u16 crc_expect;
    u16 crc_actual;
    i32 marker;
    i32 length;
    i32 i;

    marker = ymodem_getc(channel);
    if (marker < 0)
        return -4;
    if (marker == MODEM_EOT) {
        return MODEM_EOT;
    }
    if (marker == MODEM_CAN) {
        return MODEM_CAN;
    }
    if (marker == MODEM_SOH) {
        length = 128;
    } else if (marker == MODEM_STX) {
        length = 1024;
    } else {
        return -1;
    }

    marker = ymodem_getc(channel);
    if (marker < 0)
        return -4;
    block = (u8)marker;
    marker = ymodem_getc(channel);
    if (marker < 0)
        return -4;
    block_inv = (u8)marker;
    if (((block + block_inv) & 0xff) != 0xff) {
        for (i = 0; i < length + 2; i++) {
            if (ymodem_getc(channel) < 0)
                return -4;
        }
        return -2;
    }

    for (i = 0; i < length; i++) {
        marker = ymodem_getc(channel);
        if (marker < 0)
            return -4;
        data[i] = (u8)marker;
    }
    marker = ymodem_getc(channel);
    if (marker < 0)
        return -4;
    crc_expect = (u16)((u16)marker << 8);
    marker = ymodem_getc(channel);
    if (marker < 0)
        return -4;
    crc_expect |= (u16)marker;
    crc_actual = ymodem_crc16(data, length);
    if (crc_actual != crc_expect) {
        return -3;
    }

    *blockp = block;
    *lengthp = length;
    return 0;
}

static u32
parse_decimal(u8 *s)
{
    u32 value;

    value = 0;
    while (*s >= '0' && *s <= '9') {
        value = (value * 10) + (u32)(*s - '0');
        s++;
    }
    return value;
}

static i32
metadata_parse(u8 *data, i32 length, char *name, i32 namesz, u32 *sizep)
{
    i32 i;
    i32 n;
    u8 *size;

    if (data[0] == '\0')
        return(-1);
    n = strlen((char *)data);
    if (n <= 0 || n + 1 > namesz)
        return(-1);
    strcpy(name, (char *)data);
    size = 0;
    for (i = 0; i < length; i++) {
        if (data[i] == '\0') {
            if (i + 1 < length)
                size = &data[i + 1];
            break;
        }
    }
    if (size == 0)
        return(-1);
    *sizep = parse_decimal(size);
    return(0);
}

static i32
invalid_filename(char *name)
{
    char *p;

	if (name == 0 || *name == '\0')
		return(1);
	if (strlen(name) > DIRSIZ)
		return(1);
	for (p = name; *p != '\0'; p++) {
		if (*p == '/' || *p == '(' || *p == ')')
			return(1);
    }
    return(0);
}

static void
close_if_open(i32 *fd)
{
	if (*fd >= 0) {
		close(*fd);
		*fd = -1;
	}
}

static void
print_received(char *name, u32 size)
{
	char msg[96];

	sprintf(msg, "ymodem: received %s, %ld bytes\n", name, size);
	uart_puts(msg);
}

static u16
ymodem_crc16_update(u16 crc, i32 ch)
{
    i32 i;

    crc ^= (u16)((u16)(ch & 0xff) << 8);
    for (i = 0; i < 8; i++) {
        if (crc & 0x8000) {
            crc = (u16)((crc << 1) ^ 0x1021);
        } else {
            crc <<= 1;
        }
    }

    return crc;
}

static u16
ymodem_crc16(u8 *buf, i32 len)
{
    u16 crc;
    i32 i;

    crc = 0;
    for (i = 0; i < len; i++) {
        crc = ymodem_crc16_update(crc, buf[i]);
    }

    return crc;
}

i32
ymodem_receive_monitor(i32 channel, char *attached, char *cwd,
	char *request, char *path)
{
	return(ymodem_receive_internal(channel, attached, cwd, request, path, 0, 0, 0));
}

i32
ymodem_receive_monitor_quiet(i32 channel, char *attached, char *cwd,
	char *request, char *path)
{
	return(ymodem_receive_internal(channel, attached, cwd, request, path, 0, 0, 1));
}

i32
ymodem_receive_buffer(i32 channel, char *request, char *buffer, i32 bufsz)
{
	/* Script delivery is routine monitor plumbing; report only failures. */
	return(ymodem_receive_internal(channel, 0, 0, request, 0, buffer, bufsz, 1));
}

static i32
ymodem_receive_internal(i32 channel, char *attached, char *cwd,
	char *request, char *path, char *buffer, i32 bufsz, i32 quiet)
{
    u8 data[1024];
    u8 block;
    u32 file_size;
    u32 remaining;
	i32 fd;
	i32 offset;
	i32 chunk;
	i32 length;
    i32 rc;
    i32 expected_block;
    i32 saw_metadata;
    char filename[64];
    char dstpath[128];
    char msg[80];

    if (buffer == 0 && request == 0 && (attached == 0 || *attached == '\0')) {
        uart_puts("ymodem: no attached filesystem\n");
        return(-1);
    }
    if (buffer != 0 && bufsz <= 0) {
        uart_puts("ymodem: bad script buffer\n");
        return(-1);
    }

    expected_block = 1;
    saw_metadata = 0;
	fd = -1;
	offset = 0;
	if (request != 0) {
		send_request(channel, request);
	}
    uart_channel_putc(channel, MODEM_CRC_REQ);

    for (;;) {
        rc = packet_read(channel, data, &block, &length);
        if (rc == MODEM_EOT) {
            if (!saw_metadata) {
                uart_channel_putc(channel, MODEM_CAN);
                return(-1);
            }
            uart_channel_putc(channel, MODEM_ACK);
            if (!quiet)
                uart_putc('\n');
            uart_channel_putc(channel, MODEM_CRC_REQ);
            rc = packet_read(channel, data, &block, &length);
            if (rc != 0 || (block & 0xff) != 0 || data[0] != '\0') {
                uart_puts("ymodem: bad trailer\n");
                uart_channel_putc(channel, MODEM_CAN);
                close_if_open(&fd);
                return -1;
            }
            if (remaining != 0) {
                uart_puts("ymodem: short transfer\n");
                uart_channel_putc(channel, MODEM_CAN);
                close_if_open(&fd);
                return(-1);
            }
            uart_channel_putc(channel, MODEM_ACK);
            close_if_open(&fd);
			if (buffer != 0) {
				buffer[offset] = '\0';
			}
			if (!quiet)
				print_received(filename, file_size);
			return 0;
        }
        if (rc == MODEM_CAN) {
            uart_putc('\n');
            uart_puts("ymodem: cancel\n");
            close_if_open(&fd);
            return -1;
        }
        if (rc == -1) {
            uart_putc('\n');
            uart_puts("ymodem: bad marker\n");
            uart_channel_putc(channel, MODEM_NAK);
            continue;
        }
        if (rc == -2) {
            uart_putc('\n');
            uart_puts("ymodem: bad block header\n");
            uart_channel_putc(channel, MODEM_NAK);
            continue;
        }
        if (rc == -3) {
            uart_putc('\n');
            uart_puts("ymodem: bad crc\n");
            uart_channel_putc(channel, MODEM_NAK);
            continue;
        }
        if (rc == -4) {
            uart_putc('\n');
            uart_puts("ymodem: timeout\n");
            uart_channel_putc(channel, MODEM_CAN);
            close_if_open(&fd);
            return(-1);
        }

        if (!saw_metadata) {
            if ((block & 0xff) != 0) {
                sprintf(msg, "ymodem: expected metadata block, got=%d\n",
                    (int)(block & 0xff));
                uart_puts(msg);
                uart_channel_putc(channel, MODEM_CAN);
                close_if_open(&fd);
                return -1;
            }
            if (metadata_parse(data, length, filename, sizeof(filename), &file_size) < 0) {
                uart_puts("ymodem: bad metadata parse\n");
                uart_channel_putc(channel, MODEM_CAN);
                return(-1);
            }
            if (request != 0 && strcmp(filename, request_basename(request)) != 0) {
                uart_puts("ymodem: wrong file\n");
                uart_channel_putc(channel, MODEM_CAN);
                return(-1);
            }
            if (buffer == 0 && request == 0 && invalid_filename(filename)) {
                uart_puts("ymodem: bad filename\n");
                uart_channel_putc(channel, MODEM_CAN);
                return(-1);
            }
            if (buffer != 0) {
                if (file_size + 1 > (u32)bufsz) {
                    uart_puts("ymodem: file too large for script buffer\n");
                    uart_channel_putc(channel, MODEM_CAN);
                    return(-1);
                }
            } else {
                if (request == 0) {
                    if (v7fs_resolve_path(dstpath, sizeof(dstpath), attached, cwd, filename) < 0) {
                        uart_puts("ymodem: bad destination path\n");
                        uart_channel_putc(channel, MODEM_CAN);
                        return(-1);
                    }
                    path = dstpath;
                }
                if (exists(path) > 0) {
                    if (unlink(path) < 0) {
                        uart_puts("ymodem: replace failed\n");
                        uart_channel_putc(channel, MODEM_CAN);
                        return(-1);
                    }
                }
                fd = open(path, 1);
                if (fd < 0) {
                    uart_puts("ymodem: create failed\n");
                    uart_channel_putc(channel, MODEM_CAN);
                    return(-1);
                }
            }
            remaining = file_size;
            uart_channel_putc(channel, MODEM_ACK);
            uart_channel_putc(channel, MODEM_CRC_REQ);
            saw_metadata = 1;
            expected_block = 1;
            continue;
        }

        if ((block & 0xff) == ((expected_block - 1) & 0xff)) {
            uart_putc('\n');
            sprintf(msg, "ymodem: dup block=%d len=%d\n",
                (int)(block & 0xff), length);
            uart_puts(msg);
            uart_channel_putc(channel, MODEM_ACK);
            continue;
        }
        if ((block & 0xff) != (expected_block & 0xff)) {
            uart_putc('\n');
            sprintf(msg, "ymodem: unexpected block=%d want=%d\n",
                (int)(block & 0xff), expected_block & 0xff);
            uart_puts(msg);
            uart_channel_putc(channel, MODEM_CAN);
            close_if_open(&fd);
            return -1;
        }

        if (remaining == 0) {
            uart_putc('\n');
            uart_puts("ymodem: extra data beyond file size\n");
            uart_channel_putc(channel, MODEM_CAN);
            close_if_open(&fd);
            return(-1);
        }

        chunk = length;
        if ((u32)chunk > remaining)
            chunk = (i32)remaining;
		if (buffer != 0) {
			memcpy(buffer + offset, data, chunk);
			offset += chunk;
		} else {
			if (write(fd, (char *)data, chunk) != chunk) {
                uart_putc('\n');
                uart_puts("ymodem: write failed\n");
                uart_channel_putc(channel, MODEM_CAN);
                close_if_open(&fd);
				return(-1);
			}
		}
		remaining -= (u32)chunk;
        uart_channel_putc(channel, MODEM_ACK);
        expected_block++;
    }
}
static void
send_request(i32 channel, char *request)
{
	char *p;

	p = MODEM_REQ_PREFIX;
	while (*p)
		uart_channel_putc(channel, *p++);
	while (*request)
		uart_channel_putc(channel, *request++);
	uart_channel_putc(channel, '\n');
}

static char *
request_basename(char *request)
{
	char *base;

	base = request;
	while (*request) {
		if (*request == '/')
			base = request + 1;
		request++;
	}
	return(base);
}
