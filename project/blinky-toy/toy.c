#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"
#include "led.h"

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))
#define SW1 BIT0
#define SW2 BIT1
#define SW3 BIT2
#define SW4 BIT3
#define SWITCH1 SW4
#define SWITCHES (SW1 | SW2 | SW3 | SW4)

int button_pushed = 0;
int button_state = 0;

int laser = 3500;
int second_count = 0;
int blink_count = 0;

/* Start Music Setup */

//C C# D D# E F F# G G# A A# B C C# D

/* Basic Notes */
const int note[] = {3822, 3608, 3405, 3214, 3034,
	    2863, 2703, 2551, 2408, 2272,
	    2145, 2025, 1911, 1804, 1703};

/* Higher Notes */
const int noteH[] = {1911, 1804, 1703, 1607, 1517,
	       1431, 1350, 1273, 1201, 1132,
	       1067, 1004, 948, 895, 845};
// Rest = 0

int secret_song[] = {noteH[7], noteH[6],
		     noteH[3], note[9],
		     note[8], noteH[4],
		     noteH[8], noteH[12]};	     

int songar1[] = {note[9], note[9], note[5],
		 note[2], 0, note[2], 0, note[7],
		 0, note[7], 0, note[7], note[11],
		 note[11], note[12], note[14],
		 note[12], note[12], note[12]};

int songar2[] = {note[7], 0, note[9], 0, note[7],
		 note[6], note[4], 0, note[7], 0,
		 note[9], 0, note[7], note[6],
		 note[4], 0, 0, noteH[4], noteH[4],
		 noteH[6], noteH[6], noteH[7],
		 noteH[7], noteH[7], noteH[9],
		 noteH[9], noteH[9], note[11],
		 note[11], note[11], noteH[9],
		 noteH[9], noteH[9], noteH[7],
		 noteH[7], noteH[6], noteH[6],
		 noteH[4], noteH[4]};
		 

/* End Music Setup */

int main() {
    configureClocks();

    P1DIR |= LEDS;
    P1OUT &= ~LEDS;
    
    P1REN |= SWITCH1;
    P1IE |= SWITCH1;
    P1OUT |= SWITCH1;
    P1DIR &= ~SWITCH1;
    
    P2REN |= SWITCHES;
    P2IE |= SWITCHES;
    P2OUT |= SWITCHES;
    P2DIR &= ~SWITCHES;
    
    buzzer_init();
    // buzzer_set_period(0);	/* start buzzing!!! 2MHz/1000 = 2kHz*/

    
    or_sr(0x18);          // CPU off, GIE on
}

void
switch_interrupt_handler1(){
  char p1val = P1IN;

  P1IES |= (p1val & SWITCH1);
  P1IES &= (p1val | ~SWITCH1);

  if(!(p1val & SWITCH1)) {
    button_pushed = 5;
    enableWDTInterrupts();
  }

  else {
    buzzer_set_period(0);
    configureClocks();
    P1OUT &= ~LEDS;
  }
}

void
switch_interrupt_handler2(){
  char p2val = P2IN;

  P2IES |= (p2val & SWITCHES);
  P2IES &= (p2val | ~SWITCHES);
  
  /* Button 1 */
  if (!(p2val & SW1)) {
    button_pushed = 1;
    enableWDTInterrupts();
  }
  
  /* Button 2 */
  else if (!(p2val & SW2)) {
    button_pushed = 2;
    enableWDTInterrupts();
  }
  
  /* Button 3 */
  else if (!(p2val & SW3)) {
    button_pushed = 3;
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
      P1OUT &= ~LEDS;
      configureClocks();
    }
    
    else if (button_state == 1) {
      buzzer_set_period(5000);
      P1OUT |= LEDS;
      configureClocks();
    }
    
    else
      {
	buzzer_set_period(0);
	configureClocks();
	P1OUT &= ~LEDS;
      }
  }
}

void
laser_button(){
  blink_count ++;
  if (blink_count >= 8) {
      blink_count = 0;
      P1OUT |= LED_RED;
    }

  else
    P1OUT &= ~LED_RED;

  second_count ++;
  laser -= 50;
  buzzer_set_period(laser);
  if (second_count >= 50) {
    second_count = 0;
    laser = 2500;
  }
}

void
play_song(int a[], int len, int speed, int pause_len){
  for(int i = 0; i < len; i ++){
    buzzer_set_period(a[i]);
    delay(speed);

    if (button_pushed == 5)
      {
	P1OUT |= LED_GREEN;
	P1OUT |= LED_RED;
	delay(pause_len);
	P1OUT &= ~LED_GREEN;
	P1OUT &= ~LED_RED;
      }
    
    else if (a[i] != 0 & a[i] == a[i + 1]){
      P1OUT |= LED_GREEN;
      delay(pause_len);
      P1OUT &= ~LED_GREEN;
    }

    else if (a[i] != 0 & a[i] != a[i + 1]){
      P1OUT |= LED_RED;
      delay(pause_len);
      P1OUT &= ~LED_RED;
    }

    else if (a[i] == 0)
      ;
  }
  configureClocks();
}
  
void
delay(int ms){
  while(ms > 0){
    __delay_cycles(2000);
    ms --;
  }
}

void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {
    P2IFG &= ~SWITCHES;
    switch_interrupt_handler2();
  }
}

void
__interrupt_vec(PORT1_VECTOR) Port_1(){
  if (P1IFG & SWITCH1) {
    P1IFG &= ~SWITCH1;
    switch_interrupt_handler1();
  }
}

void 
__interrupt_vec(WDT_VECTOR) WDT(){
  if (button_pushed == 5)
    play_song(secret_song, ARRLEN(secret_song), 1000, 50);
    
  else if (button_pushed == 1)
    laser_button();
  
  else if (button_pushed == 2)
    play_song(songar1, ARRLEN(songar1), 1500, 200);

  else if (button_pushed == 3)
    play_song(songar2, ARRLEN(songar2), 800, 50);
}
