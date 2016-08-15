typedef unsigned int u32;

void isr(void) { }

void __deregister_frame_info(void) { }
void __register_frame_info(void) { }
void mp_import_stat(void) { }
void mp_builtin_open_obj(void) { }
void gc_collect(void) { }
void mp_lexer_new_from_file(void) { }
void *malloc(int n) { return 0; }
void free(void *n) { }

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

static void uart_putc(unsigned char value)
{
	writel(base + UART_RXTX, value);
}

static unsigned char uart_rxempty(void)
{
	return readl(base + UART_RXEMPTY);
}

static unsigned char uart_txfull(void)
{
	return readl(base + UART_TXFULL);
}

static void boot(long addr)
{
#if defined (__lm32__)
	asm("call r0");
#elif defined (__or1k__)
	asm("l.jr r6");
	asm("l.nop");
#else
#error Unsupported architecture
#endif
}

static void reboot(void)
{
	boot(0x20000000);
}

#if defined (__lm32__)
#define NOP "nop"
#elif defined (__or1k__)
#define NOP "l.nop"
#else
#error Unsupported architecture
#endif

static void my_putc(char c)
{
	while (uart_txfull());
	uart_putc(c);
	while (uart_txfull());
}

static void my_puts(char *s)
{
	while (*s)
		my_putc(*s++);
}

int main(void)
{
	my_puts("Hello, World!\n");

	while(uart_rxempty());

	reboot();

	return 0;
}
