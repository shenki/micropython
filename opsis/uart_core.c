#include <unistd.h>
#include "py/mpconfig.h"

/*
 * Core UART functions to implement for a port
 */
typedef unsigned int u32;

static void *base = (void *)0xe0001000;
#define	UART_RXTX	0x00
#define UART_TXFULL	0x04
#define UART_RXEMPTY	0x08

static u32 readl(volatile u32 *addr)
{
	return *addr;
}

static void writel(u32 *addr, u32 val)
{
	*addr = val;
}

static unsigned char uart_getc(void)
{
	return readl(base + UART_RXTX);
}

static void uart_putc(unsigned char value)
{
	writel(base + UART_RXTX, value);
}

#if 0
static unsigned char uart_rxempty(void)
{
	return readl(base + UART_RXEMPTY);
}
#endif

static unsigned char uart_txfull(void)
{
	return readl(base + UART_TXFULL);
}

void my_putc(char c)
{
	while (uart_txfull());
	uart_putc(c);
	while (uart_txfull());
}

void my_puts(char *s)
{
	while (*s)
		my_putc(*s++);
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    return uart_getc();
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
		my_putc(*str++);
    }
}
