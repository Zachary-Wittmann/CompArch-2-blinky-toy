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
  
  or_sr(0x18);			/* CPU off, GIE on */
}

// global state var to count time
int sec1 = 0;
int sec2 = 0;

void
__interrupt_vec(WDT_VECTOR) WDT()	/* 250 interrupts/sec */
{
  sec1++;
  sec2++;
  
  if (sec1 >= 125) { 	/* once each sec... */
    sec1 = 0;		/* reset count */
    P1OUT ^= LED_GREEN;		/* toggle green LED */
  }

  if (sec2 >= 125) {
    sec2 = 0;
    P1OUT ^= LED_RED;
  }
} 

