#ifndef EPOCH68_UART_H
#define EPOCH68_UART_H

#include <sys/inttypes.h>

#define EPOCH68_UART0_BASE 0x00a00100
#define EPOCH68_UART_STRIDE 8
#define EPOCH68_UART0 0
#define EPOCH68_UART1 1
#define EPOCH68_UART2 2
#define EPOCH68_UART3 3
#define EPOCH68_UART4 4
#define EPOCH68_UART5 5

#define EPOCH68_UART_RBR_THR_DLL 0
#define EPOCH68_UART_IER_DLM     1
#define EPOCH68_UART_IIR_FCR     2
#define EPOCH68_UART_LCR         3
#define EPOCH68_UART_MCR         4
#define EPOCH68_UART_LSR         5
#define EPOCH68_UART_MSR         6
#define EPOCH68_UART_SCR         7

#define EPOCH68_UART_LCR_8N1  0x03
#define EPOCH68_UART_LCR_DLAB 0x80

#define EPOCH68_UART_FCR_ENABLE   0x01
#define EPOCH68_UART_FCR_CLEAR_RX 0x02
#define EPOCH68_UART_FCR_CLEAR_TX 0x04
#define EPOCH68_UART_FCR_TRIGGER_14 0xc0

#define EPOCH68_UART_LSR_DR   0x01
#define EPOCH68_UART_LSR_THRE 0x20

void uart_init(void);
void uart_channel_init(i32 channel);
void uart_channel_putc(i32 channel, i32 c);
i32 uart_channel_getc_ready(i32 channel);
i32 uart_channel_getc(i32 channel);
i32 uart_channel_getc_timeout(i32 channel, i32 timeout);
void uart_putc(i32 c);
void uart_puts(const char *s);
void uart_put_hex32(u32 value);
i32 uart_getc_ready(void);
i32 uart_getc(void);
char *uart_gets(void);

#endif
