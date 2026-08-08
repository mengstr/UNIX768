#include "uart.h"

static volatile u8 *const uart0 = (volatile u8 *)EPOCH68_UART0_BASE;

static volatile u8 *
uart_channel(i32 channel)
{
    return (volatile u8 *)(u32)
        (EPOCH68_UART0_BASE + ((u32)channel * EPOCH68_UART_STRIDE));
}

void
uart_channel_init(i32 channel)
{
    volatile u8 *uart = uart_channel(channel);

    uart[EPOCH68_UART_IER_DLM] = 0x00;
    uart[EPOCH68_UART_IIR_FCR] =
        EPOCH68_UART_FCR_ENABLE |
        EPOCH68_UART_FCR_CLEAR_RX |
        EPOCH68_UART_FCR_CLEAR_TX |
        EPOCH68_UART_FCR_TRIGGER_14;
    uart[EPOCH68_UART_LCR] = EPOCH68_UART_LCR_8N1;
}

void uart_init(void)
{
    uart_channel_init(0);
}

void
uart_channel_putc(i32 channel, i32 c)
{
    volatile u8 *uart = uart_channel(channel);

    if (c == '\n') {
        uart_channel_putc(channel, '\r');
    }

    while ((uart[EPOCH68_UART_LSR] & EPOCH68_UART_LSR_THRE) == 0) {
    }

    uart[EPOCH68_UART_RBR_THR_DLL] = (u8)c;
}

void
uart_putc(i32 c)
{
    uart_channel_putc(0, c);
}

void
uart_put_hex32(u32 value)
{
    i32 shift;
    i32 digit;

    for (shift = 28; shift >= 0; shift -= 4) {
        digit = (value >> shift) & 0xf;
        if (digit < 10) {
            uart_putc('0' + digit);
        } else {
            uart_putc('A' + (digit - 10));
        }
    }
}

void uart_puts(const char *s)
{
    while (*s != '\0') {
        uart_putc(*s++);
    }
}

i32
uart_channel_getc_ready(i32 channel)
{
    volatile u8 *uart = uart_channel(channel);

    return (uart[EPOCH68_UART_LSR] & EPOCH68_UART_LSR_DR) != 0;
}

i32
uart_getc_ready(void)
{
    return uart_channel_getc_ready(0);
}

i32
uart_channel_getc(i32 channel)
{
    volatile u8 *uart = uart_channel(channel);

    while (!uart_channel_getc_ready(channel)) {
    }

    return uart[EPOCH68_UART_RBR_THR_DLL];
}

i32
uart_channel_getc_timeout(i32 channel, i32 timeout)
{
    volatile u8 *uart = uart_channel(channel);
    i32 t;

    for (t = timeout; t > 0; t--) {
        if (uart_channel_getc_ready(channel))
            return uart[EPOCH68_UART_RBR_THR_DLL];
    }

    return -1;
}

i32
uart_getc(void)
{
    return uart_channel_getc(0);
}

#define BUFSIZE 80
/* gets() is a simple implementation of the standard C library function that reads a line of input from the UART.
 * It stores the input in the provided buffer and null-terminates it. The function returns a pointer to the buffer. */
char *
uart_gets(void)
{
    static char buf[BUFSIZE];

    i32 i = 0;
    while (i < BUFSIZE - 1) {
        i32 c = uart_getc();
        if (c == '\r' || c == '\n') {
            uart_putc('\n');
            break;
        }
        if (c == '\b' || c == 0x7f) {
            if (i > 0) {
                i--;
                uart_putc('\b');
                uart_putc(' ');
                uart_putc('\b');
            }
            continue;
        }
        buf[i++] = (char)c;
        uart_putc(c);
    }
    buf[i] = '\0';
    return buf;
}
