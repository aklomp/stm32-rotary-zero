#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

void
usart_setup (void)
{
	// Setup GPIO pin GPIO_USART1_TX/GPIO2 on GPIO port A for transmit:
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	// Setup UART parameters:
	usart_set_baudrate(USART1, 115200);

	// Enable for 8N1:
	USART1_CR2 = 0;
	USART1_CR3 = 0;
	USART1_CR1 = USART_CR1_UE | USART_CR1_TE;
}

void
usart_puts (const char *p)
{
	while (*p)
		usart_send_blocking(USART1, *p++);

	usart_send_blocking(USART1, '\r');
	usart_send_blocking(USART1, '\n');
}
