# STM32F1 rotary encoder with zero pulse

This is simple firmware for an STM32F103C8T6 to measure, calibrate and print
the value of a ring-shaped rotary encoder with a zero calibration pulse. Our
rotary encoder is a ring with 2000 poles around its circumference, and one
special pole that emits a "zero" correction pulse. We correct the "raw" counter
value using this pulse. The zero, raw and calibrated values are printed to
USART1 (pin A9) at 25 Hz.

This code is intended for use in a hard realtime system, so it cannot use any
interrupts. The heavy lifting is done by the timer peripheral's builtin support
for quadrature encoders. Contrary to what the ST application note on timer
usage suggests, we do not alter the counter value in any way to account for the
zero pulse (let alone in an interrupt), because the chance is quite high that
we would lose pulses if the timer peripheral updates it underneath us. We treat
the counter as strictly read-only.

The encoder read head is hooked up to the STM32F1's TIM3 peripheral, with the
regular pulse channels on pins A6 and A7 and the zero pulse on B0. TIM3 is
configured in quadratic encoder mode to count the pulses automatically. We
configure input capture on channel 3 to store the momentary value of the
counter register whenever a zero-pulse arrives. When the counter value is
requested, we use unsigned modulo arithmetic to subtract the last known zero
correction from the timer value to give us the corrected output.

This code is quite simple and could be significantly improved. We could check
the "overcapture" flag to see if we're reading out fast enough. We could model
the drift of the zero counter and smooth it out over time.

## Usage

This firmware depends on libopencm3, which is included as a submodule.

```sh
git submodule update --init
```

To compile, you need a GCC ARM cross-compiling toolchain. The Makefile has a
few targets, `flash` will flash the board (a simple "blue pill" in my case)
with OpenOCD if available.
