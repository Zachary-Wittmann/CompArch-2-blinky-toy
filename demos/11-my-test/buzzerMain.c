#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

#define SW1 BIT0
#define SW2 BIT1
#define SW3 BIT2
#define SW4 BIT3
#define SWITCHES (SW1 | SW2 | SW3 | SW4)


int siren = 5000;
int second_count = 0;

int button_state = 0;

int main() {
    configureClocks();

    P2REN |= SWITCHES;
    P2IE |= SWITCHES;
    P2OUT |= SWITCHES;
    P2DIR &= ~SWITCHES;
    
    buzzer_init();
    // buzzer_set_period(0);	/* start buzzing!!! 2MHz/1000 = 2kHz*/

    
    or_sr(0x18);          // CPU off, GIE on
}

void
switch_interrupt_handler()
{
  char p2val = P2IN;
  
  P2IES |= (p2val & SWITCHES);
  P2IES &= (p2val | ~SWITCHES);

  
  /* Button 1 */
  if (!(p2val & SW1)) {
    buzzer_set_period(10000);
  }
  
  /* Button 2 */
  else if (!(p2val & SW2)) {
    buzzer_set_period(7500);
  }
  
  /* Button 3 */
  else if (!(p2val & SW3)) {
    enableWDTInterrupts();
  }

  /* Button 4 */
  else if (!(p2val & SW4) & button_state == 0) {
    buzzer_set_period(0);
    button_state = 1;
  }

  else if (!(p2val & SW4) & button_state == 1) {
    buzzer_set_period(5000);
    button_state = 0;
  }
  
  /* Button Release */
  else {
    if (button_state == 0) {
      buzzer_set_period(0);
      configureClocks();
    }
    
    else if (button_state == 1) {
      buzzer_set_period(5000);
      configureClocks();
    }
    
    else
      {
	buzzer_set_period(0);
	configureClocks();
      }
  }
}

void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {
    P2IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}

void 
__interrupt_vec(WDT_VECTOR) WDT(){
    second_count ++;
    siren += 50;
    buzzer_set_period(siren);
    if (second_count >= 500) {
      second_count = 0;
      siren = 5000;
    }
}
