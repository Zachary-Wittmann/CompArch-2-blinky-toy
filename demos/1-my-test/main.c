//Alternate LEDs from Off, Green, and Red
#include <msp430.h>
#include "libTimer.h"
#include "led.h"

/*If one of the LED bits are not set in P1DIR it is not
usable as you have not declared it.*/

int main(void) {
  P1DIR |= LEDS;
  P1OUT |= LED_GREEN;
  P1OUT |= LED_RED;

  or_sr(0x18);		/* CPU off, GIE on */
}
