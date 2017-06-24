#include <libopencm3/stm32/rcc.h>

void
clock_setup (void)
{
	// Peripheral clock map:
	static const uint32_t map[] = {
		RCC_AFIO,
		RCC_GPIOA,
		RCC_TIM3,
		RCC_USART1,
	};

	// Run at 72 MHz from an external oscillator:
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	// Enable peripheral clocks:
	for (uint32_t i = 0; i < sizeof(map) / sizeof(map[0]); i++)
		rcc_periph_clock_enable(map[i]);
}
