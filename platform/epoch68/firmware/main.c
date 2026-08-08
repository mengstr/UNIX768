#include <sys/inttypes.h>
#include "monitor_cmd.h"
#include "uart.h"

#define EPOCH68_MMIO_BASE ((volatile u8 *)0x00a00000)
#define SYSTEM_PAGE 0
#define MIRROR1_PAGE 1
#define MIRROR2_PAGE 2
#define DISK_PAGE 3

void
bus_error_handler(void)
{
  for (;;) {
  }
}

int main(void) {
  volatile u8 *mmio = EPOCH68_MMIO_BASE;
  char attached[32]="rd(0,0)";
  char cwd[64]=".";
  char *cmdbuf;

  mmio[SYSTEM_PAGE] = 0;
  mmio[MIRROR1_PAGE] = 1;
  mmio[MIRROR2_PAGE] = 2;
  mmio[DISK_PAGE] = 3;

  uart_init();
  uart_channel_init(EPOCH68_UART1);
  uart_puts("platform/epoch68/emulator ROM console v0.1\n");
#if 0 /* keep the ROM banner terse */
  uart_puts("Default fileystem: ");
  uart_puts(attached);
  uart_puts("\n");
#endif

  for (;;) {
    uart_putc('@');
    cmdbuf = uart_gets();
    monitor_run_command(cmdbuf, attached, sizeof(attached), cwd, sizeof(cwd));
  }

  return 0;
}
