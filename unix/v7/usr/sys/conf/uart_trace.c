/*
 * Minimal UART IRQ bring-up helper.
 *
 * This intentionally does nothing.  It exists purely to answer the question:
 *
 *   "Is the problem caused by calling into C from the UART IRQ path at all,
 *    or by the actual body of epoch68_uart_intr_shared()?"
 *
 * When _uart_intr calls this function and the system remains stable, the
 * assembly IRQ save/restore plus plain C call/return path is fine, and the
 * next suspect becomes the real UART handler body.
 */

void
epoch68_uart_intr_nop(void)
{
}

void
epoch68_uart_intr_enter(void)
{
}
