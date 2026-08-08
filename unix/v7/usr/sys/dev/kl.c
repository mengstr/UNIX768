/*
 * Epoch68 16550-style UART character driver.
 *
 * The kl entry-point names are retained for the V7 character-device switch;
 * hardware-specific KL/DL-11 code is not applicable to this machine.  Normal
 * terminals use the V7 tty layer, while UART1 remains a raw YMODEM/serial-disk
 * side channel.
 */

#include "../h/param.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/tty.h"
#include "../h/systm.h"

i32 ttyopen(i32 dev, struct tty *tp, caddr_t addr);
i32 ttyclose(struct tty *tp);
void ttychars(struct tty *tp);
i32 ttyinput(i32 c, struct tty *tp);
i32 ttread(struct tty *tp);
caddr_t ttwrite(struct tty *tp);
i32 ttioccomm(i32 com, struct tty *tp, caddr_t addr, i32 dev);
i32 ttstart(struct tty *tp);
i32 klstart(struct tty *tp);
i32 epoch68_ymodem_read(void);
i32 epoch68_ymodem_write(void);

#define UART0 ((volatile unsigned char *)0x00a00100)
#define UART_BASE ((volatile unsigned char *)0x00a00100)
#define UART_STRIDE 8
#define UART_CHANNELS 6
#define UART_IER         1
#define UART_IIR_FCR     2
#define UART_LCR         3
#define UART_RBR_THR_DLL 0
#define UART_LSR         5
#define UART_LCR_8N1     0x03
#define UART_FCR_ENABLE  0x01
#define UART_FCR_CLEAR_RX 0x02
#define UART_FCR_CLEAR_TX 0x04
#define UART_FCR_TRIGGER_14 0xc0
#define UART_IER_RX      0x01
#define UART_IER_TX      0x02
#define UART_LSR_DR      0x01
#define UART_LSR_THRE    0x20
#define UART_RX_RING_SIZE 1280
#define UART_TX_RING_SIZE 1280
#define UART_RX_POLL_BUDGET 64
#define UART_TX_POLL_BUDGET 64
#define EPOCH68_B9600 13

char *msgbufp = msgbuf;      /* Next saved printf character */

struct epoch68_uart_ring {
	unsigned short head;
	unsigned short tail;
	unsigned short count;
	unsigned char data[UART_RX_RING_SIZE];
};

struct epoch68_uart_tx_ring {
	unsigned short head;
	unsigned short tail;
	unsigned short count;
	unsigned char data[UART_TX_RING_SIZE];
};

static struct epoch68_uart_ring epoch68_uart_rx[UART_CHANNELS];
static struct epoch68_uart_tx_ring epoch68_uart_txq[UART_CHANNELS];
struct tty epoch68_kl_tty[UART_CHANNELS];

static volatile unsigned char *
uart_channel_addr(i32 channel)
{
	return(UART_BASE + (channel * UART_STRIDE));
}

static i32
uart_channel_enabled(i32 channel)
{
	return(channel != 1);
}

static void
uart_rx_ring_put(i32 channel, i32 value)
{
	register struct epoch68_uart_ring *ring;
	register i32 s;

	if (channel < 0 || channel >= UART_CHANNELS)
		return;
	s = spl6();
	ring = &epoch68_uart_rx[channel];
	if (ring->count >= UART_RX_RING_SIZE) {
		splx(s);
		return;
	}
	ring->data[ring->tail] = (unsigned char)value;
	ring->tail = (ring->tail + 1) % UART_RX_RING_SIZE;
	ring->count++;
	splx(s);
}

static i32
uart_rx_ring_get(i32 channel)
{
	register struct epoch68_uart_ring *ring;
	register i32 value;
	register i32 s;

	if (channel < 0 || channel >= UART_CHANNELS)
		return(-1);
	s = spl6();
	ring = &epoch68_uart_rx[channel];
	if (ring->count == 0) {
		splx(s);
		return(-1);
	}
	value = ring->data[ring->head];
	ring->head = (ring->head + 1) % UART_RX_RING_SIZE;
	ring->count--;
	splx(s);
	return(value);
}

static void
uart_tx_ring_put(i32 channel, i32 value)
{
	register struct epoch68_uart_tx_ring *ring;
	register i32 s;

	if (channel < 0 || channel >= UART_CHANNELS)
		return;
	s = spl6();
	ring = &epoch68_uart_txq[channel];
	if (ring->count >= UART_TX_RING_SIZE) {
		splx(s);
		return;
	}
	ring->data[ring->tail] = (unsigned char)value;
	ring->tail = (ring->tail + 1) % UART_TX_RING_SIZE;
	ring->count++;
	splx(s);
}

static i32
uart_tx_ring_get(i32 channel)
{
	register struct epoch68_uart_tx_ring *ring;
	register i32 value;
	register i32 s;

	if (channel < 0 || channel >= UART_CHANNELS)
		return(-1);
	s = spl6();
	ring = &epoch68_uart_txq[channel];
	if (ring->count == 0) {
		splx(s);
		return(-1);
	}
	value = ring->data[ring->head];
	ring->head = (ring->head + 1) % UART_TX_RING_SIZE;
	ring->count--;
	splx(s);
	return(value);
}

static void
uart_poll_tx_channel(i32 channel)
{
	register volatile unsigned char *uart;
	register struct epoch68_uart_tx_ring *ring;
	register struct tty *tp;
	register i32 c;
	register i32 scanned;
	register i32 sent;
	register i32 s;

	s = spl6();
	uart = uart_channel_addr(channel);
	ring = &epoch68_uart_txq[channel];
	tp = &epoch68_kl_tty[channel];
	scanned = 0;
	sent = 0;
	while (scanned < UART_TX_POLL_BUDGET &&
	    ring->count < UART_TX_RING_SIZE &&
	    (tp->t_state & (TIMEOUT|TTSTOP)) == 0) {
		c = getc(&tp->t_outq);
		if (c < 0)
			break;
		scanned++;
		/* Cooked output uses bit 7 for delay entries. */
		if ((tp->t_flags & RAW) == 0 && (c & 0200))
			continue;
		uart_tx_ring_put(channel, c);
	}
	while (sent < UART_TX_POLL_BUDGET && ring->count != 0) {
		if ((uart[UART_LSR] & UART_LSR_THRE) == 0)
			break;
		c = uart_tx_ring_get(channel);
		if (c < 0)
			break;
		uart[UART_RBR_THR_DLL] = c & 0377;
		sent++;
	}

	if (ring->count != 0)
	{
		tp->t_state |= BUSY;
		uart[UART_IER] |= UART_IER_TX;
	} else {
		tp->t_state &= ~BUSY;
		uart[UART_IER] &= ~UART_IER_TX;
	}
	if (tp->t_outq.c_cc <= TTLOWAT && ring->count == 0) {
		if (tp->t_state & ASLEEP) {
			tp->t_state &= ~ASLEEP;
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_chan)
			wakeup((caddr_t)&tp->t_outq);
	}
	splx(s);
}

static i32
uart_stage_rx_channel(i32 channel)
{
	register volatile unsigned char *uart;
	register i32 c;
	register i32 budget;
	register i32 staged;
	register i32 s;

	uart = uart_channel_addr(channel);
	budget = UART_RX_POLL_BUDGET;
	staged = 0;
	s = spl6();
	while (budget-- > 0 && (uart[UART_LSR] & UART_LSR_DR)) {
		c = uart[UART_RBR_THR_DLL] & 0377;
		uart_rx_ring_put(channel, c);
		staged++;
	}
	splx(s);
	return(staged);
}

void
epoch68_uart_poll_rx(void)
{
	register i32 channel;
	register i32 c;
	register i32 budget;
	register struct tty *tp;

	for (channel = 0; channel < UART_CHANNELS; channel++) {
		if (!uart_channel_enabled(channel))
			continue;
		tp = &epoch68_kl_tty[channel];
		uart_stage_rx_channel(channel);
		/*
		 * A closed tty has no line discipline consumer.  Do not retain input
		 * and replay it with the default ECHO setting on the next open: a
		 * remote login prompt would then be echoed back before uucico can put
		 * the line in raw/no-echo mode.  Drain the hardware above, then discard
		 * the private staging queue just as an unopened hardware tty discards
		 * unsolicited input.
		 */
		if ((tp->t_state & ISOPEN) == 0) {
			while (uart_rx_ring_get(channel) >= 0)
				;
			uart_poll_tx_channel(channel);
			continue;
		}
		budget = UART_RX_POLL_BUDGET;
		/* All hardware input crosses the private ring before ttyinput(). */
		while (budget > 0 && (tp->t_state & ISOPEN) &&
		    (c = uart_rx_ring_get(channel)) >= 0) {
			ttyinput(c, tp);
			budget--;
		}
		uart_poll_tx_channel(channel);
	}
}

/*
 * Return the UART channel associated with a KL minor number.
 *
 * Early Epoch68 staging rules:
 *   minor 0 -> UART0 console
 *   minor 1 -> raw YMODEM side channel, not a Unix tty
 *   minor 2 -> UART2
 *   minor 3 -> UART3
 *   minor 4 -> UART4
 *   minor 5 -> UART5
 *
 * Returning -1 means "this minor is not implemented as a Unix tty line".
 */
static i32
kl_minor_channel(i32 dev)
{
	register i32 m;

	m = minor(dev);
	if (m < 0 || m >= UART_CHANNELS)
		return(-1);
	if (!uart_channel_enabled(m))
		return(-1);
	return(m);
}

static i32
kl_is_ymodem(i32 dev)
{
	return(minor(dev) == 1);
}

static void
epoch68_uart_rings_reset(void)
{
	register i32 channel;

	for (channel = 0; channel < UART_CHANNELS; channel++) {
		epoch68_uart_rx[channel].head = 0;
		epoch68_uart_rx[channel].tail = 0;
		epoch68_uart_rx[channel].count = 0;
		epoch68_uart_txq[channel].head = 0;
		epoch68_uart_txq[channel].tail = 0;
		epoch68_uart_txq[channel].count = 0;
	}
}

void
epoch68_uart_kernel_init(void)
{
	register i32 channel;
	register volatile unsigned char *uart;

	epoch68_uart_rings_reset();
	for (channel = 0; channel < UART_CHANNELS; channel++) {
		uart = uart_channel_addr(channel);
		uart[UART_LCR] = UART_LCR_8N1;
		uart[UART_IIR_FCR] = UART_FCR_ENABLE |
		    UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX |
		    UART_FCR_TRIGGER_14;
		/*
		 * Keep UART RX interrupts disabled during early bootstrap.
		 *
		 * Console output still uses direct polled writes through putchar(),
		 * and leaving RX IRQs armed here has repeatedly caused early proc1
		 * bootstrap corruption before /etc/init is entered reliably.
		 *
		 * The ring-buffer/IRQ path stays in place for later re-enabling
		 * once the bootstrap window is better controlled.
		 */
		uart[UART_IER] = 0x00;
	}
}

void
epoch68_uart_enable_rx_irqs(void)
{
	register i32 channel;
	register volatile unsigned char *uart;

	/*
	 * Hardware interrupts only stage bytes in the private UART rings.
	 *
	 * epoch68_uart_intr_shared() deliberately avoids ttyinput(), echo/output
	 * processing, and wakeup(); clock() performs those operations later from
	 * a controlled stack context.
	 */
	for (channel = 0; channel < UART_CHANNELS; channel++) {
		if (!uart_channel_enabled(channel))
			continue;
		uart = uart_channel_addr(channel);
		uart[UART_IIR_FCR] = UART_FCR_ENABLE |
		    UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX |
		    UART_FCR_TRIGGER_14;
		uart[UART_IER] = UART_IER_RX;
	}
}

static void
uart_intr_stage_channel(i32 channel)
{
	register volatile unsigned char *uart;
	register struct epoch68_uart_tx_ring *txring;
	register i32 c;
	register i32 budget;
	register i32 s;

	s = spl6();
	uart = uart_channel_addr(channel);
	txring = &epoch68_uart_txq[channel];

	/* Drain hardware receive state without entering the tty discipline. */
	uart_stage_rx_channel(channel);

	/* Drain only the already-staged transmit ring in interrupt context. */
	budget = UART_TX_POLL_BUDGET;
	while (budget-- > 0 && txring->count != 0 &&
	    (uart[UART_LSR] & UART_LSR_THRE)) {
		c = uart_tx_ring_get(channel);
		if (c < 0)
			break;
		uart[UART_RBR_THR_DLL] = c & 0377;
	}
	if (txring->count == 0)
		uart[UART_IER] &= ~UART_IER_TX;
	splx(s);
}

void
epoch68_uart_intr_shared(void)
{
	register i32 channel;

	/*
	 * This entry uses the interrupted process's supervisor stack.  Keep it
	 * shallow: the full tty path can exceed a 2 KiB kernel-stack slot when an
	 * interrupt lands in a deep syscall and can corrupt scheduler state.
	 */
	for (channel = 0; channel < UART_CHANNELS; channel++) {
		if (uart_channel_enabled(channel))
			uart_intr_stage_channel(channel);
	}
}

i32
klopen(i32 dev, i32 flag)
{
	register i32 channel;
	register struct tty *tp;

	if (kl_is_ymodem(dev))
		return(0);

	channel = kl_minor_channel(dev);
	if (channel < 0) {
		u.u_error = ENXIO;
		return(0);
	}

	tp = &epoch68_kl_tty[channel];
	tp->t_addr = (caddr_t)uart_channel_addr(channel);
	tp->t_oproc = klstart;
	tp->t_iproc = 0;
	if ((tp->t_state & ISOPEN) == 0) {
		tp->t_state = ISOPEN | CARR_ON;
		tp->t_flags = EVENP | ODDP | ECHO | CRMOD | XTABS;
		tp->t_ispeed = EPOCH68_B9600;
		tp->t_ospeed = EPOCH68_B9600;
		ttychars(tp);
	}
	ttyopen(dev, tp, NULL);
	epoch68_uart_poll_rx();
	return(0);
}

i32
klclose(i32 dev, i32 flag)
{
	register i32 channel;

	if (kl_is_ymodem(dev))
		return(0);

	channel = kl_minor_channel(dev);
	if (channel >= 0) {
		ttyclose(&epoch68_kl_tty[channel]);
	}
	return(0);
}

i32
klread(i32 dev)
{
	register i32 channel;

	if (kl_is_ymodem(dev)) {
		epoch68_ymodem_read();
		return(0);
	}

	channel = kl_minor_channel(dev);
	if (channel < 0) {
		u.u_error = ENXIO;
		return(0);
	}

	epoch68_uart_poll_rx();
	ttread(&epoch68_kl_tty[channel]);
	return(0);
}

i32
klwrite(i32 dev)
{
	register i32 channel;

	if (kl_is_ymodem(dev)) {
		epoch68_ymodem_write();
		return(0);
	}

	channel = kl_minor_channel(dev);
	if (channel < 0) {
		u.u_error = ENXIO;
		return(0);
	}

	ttwrite(&epoch68_kl_tty[channel]);
	return(0);
}

i32
klxint(i32 dev)
{
    return(0);
}

i32
klrint(i32 dev)
{
    return(0);
}

i32
klioctl(i32 dev, i32 cmd, caddr_t addr, i32 flag)
{
	register i32 channel;
	register struct tty *tp;
	register i32 handled;

	if (kl_is_ymodem(dev)) {
		u.u_error = ENOTTY;
		return(0);
	}

	channel = kl_minor_channel(dev);
	if (channel < 0) {
		u.u_error = ENXIO;
		return(0);
	}
	tp = &epoch68_kl_tty[channel];
	if (cmd == TIOCGETP || cmd == TIOCSETP || cmd == TIOCSETN) {
	}
	handled = ttioccomm(cmd, tp, addr, dev);
	if (cmd == TIOCGETP || cmd == TIOCSETP || cmd == TIOCSETN) {
	}
	if (handled == 0)
		u.u_error = ENOTTY;
    return(0);
}

i32
klstart(tp)
register struct tty *tp;
{
	register i32 channel;

	channel = minor(tp->t_dev);
	if (channel < 0 || channel >= UART_CHANNELS)
		return(0);
	uart_poll_tx_channel(channel);
    return(0);
}

/*
 * Raw polled access to UART1 for host-side YMODEM transfers.
 *
 * This deliberately bypasses the tty line discipline: YMODEM needs an 8-bit
 * clean byte stream, while the normal terminal path may strip parity, echo, or
 * interpret control characters.  Do not use /dev/ymodem while the UART1
 * serial-disk transport is active.
 */
i32
epoch68_ymodem_read()
{
	register volatile unsigned char *uart;
	register i32 c;

	uart = uart_channel_addr(1);
	while (u.u_count != 0) {
		while ((uart[UART_LSR] & UART_LSR_DR) == 0)
			;
		c = uart[UART_RBR_THR_DLL] & 0377;
		if (passc(c) < 0)
			break;
	}
	return(0);
}

i32
epoch68_ymodem_write()
{
	register volatile unsigned char *uart;
	register i32 c;

	uart = uart_channel_addr(1);
	while ((c = cpass()) >= 0) {
		while ((uart[UART_LSR] & UART_LSR_THRE) == 0)
			;
		uart[UART_RBR_THR_DLL] = c & 0377;
	}
	return(0);
}

void putchar(c)
i32 c;
{
    if (c != '\0' && c != '\r' && c != 0177) {
        *msgbufp++ = c;
        if (msgbufp >= &msgbuf[MSGBUFS]) {
            msgbufp = msgbuf;
        }
    }

    if (c == 0) {
        return;
    }

    if (c == '\n') {
        putchar('\r');
    }

    while ((UART0[UART_LSR] & UART_LSR_THRE) == 0)
        ;
    UART0[UART_RBR_THR_DLL] = c;
}
