//Alternate LEDs from Off, Green, and Red
#include <msp430.h>
#include "libTimer.h"
#include "led.h"

int main(void) {
  P1DIR |= LEDS;
  P1OUT &= ~LED_GREEN;
  P1OUT |= LED_RED;

  configureClocks();		/* setup master oscillator, CPU & peripheral clocks */
  enableWDTInterrupts();	/* enable periodic interrupt */
  
  or_sr(0x18);		/* CPU off, GIE on */
}

int sec1 = 0;
int sec2 = 0;

void
__interrupt_vec(WDT_VECTOR) WDT()	/* 250 interrupts/sec */
{
  sec1++;
  sec2++;
  
  if(sec1 >= 250) {
    sec1 = 0;
    P1OUT ^= LED_GREEN;
  }
  if(sec2 >= 4) {
    sec2 = 0;
    P1OUT ^= LED_RED;
  }
}
