#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

// The rotary encoder is a ring with a fixed number of poles distributed around
// its circumference. One of these poles is the special "zero" pole that will
// deliver an extra pulse whenever the detector crosses it. The "zero" edge can
// be used to recalibrate the counter on the fly.

// Number of poles on the ring:
#define RINGSIZE	2000

void
rotary_get_state (uint16_t *count, uint16_t *zero, uint16_t *raw)
{
	// Get raw counter value:
	*raw  = TIM3_CNT;

	// Get counter value at zero crossing:
	*zero = TIM3_CCR3;

	// Correct the counter value based on the current value of the zero
	// register. Because the rotary encoder is a ring, we can use modulo
	// arithmetic to wrap the counter value to the bounds. We avoid using
	// signed arithmetic by adding the ring size. This has no effect on the
	// output since the modulo undoes it, but keeps us working in the
	// unsigned realm. Examples:
	//
	//   If raw = 54 and zero = 2, we have:
	//       (54 + (2000 - 2)) % 2000 = 52
	//
	//   If raw = 54 and zero = 1982, we have:
	//       (54 + (2000 - 1982)) % 2000 = 72
	//
	*count = (*raw + (RINGSIZE - *zero)) % RINGSIZE;
}

void
rotary_setup (void)
{
	// Set Slave mode to Encoder Mode 1:
	TIM3_SMCR = TIM_SMCR_SMS_EM1;

	// Capture inputs on Channel 3:
	TIM3_CCMR2 = TIM_CCMR2_CC3S_IN_TI3;
	TIM3_CCER  = TIM_CCER_CC3E | TIM_CCER_CC3P;

	// No prescaler, set reload value:
	TIM3_PSC = 0;
	TIM3_ARR = RINGSIZE - 1;

	// Start the timer:
	TIM3_CNT  = 0;
	TIM3_CCR3 = 0;
	TIM3_CR1 = TIM_CR1_CEN;
}
