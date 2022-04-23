#include <stdint.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

#include "clock.h"
#include "rotary.h"
#include "sprintf.h"
#include "usart.h"

#define FRAMERATE	25	// Hz

static void
setup (void)
{
	clock_setup();
	rotary_setup();
	usart_setup();
}

static void
loop (void)
{
	// Setup systick:
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(rcc_ahb_frequency / FRAMERATE - 1);
	systick_counter_enable();
	systick_clear();

	for (;;) {
		char buf[32];
		uint16_t count, zero, raw;

		// Busy-loop until systick underruns:
		while ((STK_CSR & STK_CSR_COUNTFLAG) == 0)
			continue;

		// Get current count and zero crossing:
		rotary_get_state(&count, &zero, &raw);
		sprintf(buf, "%d | %d = %d", raw, zero, count);
		usart_puts(buf);
	}
}

int
main (void)
{
	setup();
	loop();

	return 0;
}
