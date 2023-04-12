#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"
#include "led.h"
#include "music.h"

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))
#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCH1 SW4
#define SWITCHES (SW1 | SW2 | SW3 | SW4)

int state = 1;

int button_pushed = 0;
int button_state = 0;

int laser_color = 1;
int laser, og_laser = 3500;
int sec_cnt, og_sec_cnt = 0;
int blnk_cnt, og_blnk_cnt = 12;


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
    // buzzer_set_period(1000);	/* start buzzing!!! 2MHz/1000 = 2kHz*/

    
    or_sr(0x18);          // CPU off, GIE on
}

void
switch_interrupt_handler1(){
  char p1val = P1IN;

  P1IES |= (p1val & SWITCH1);
  P1IES &= (p1val | ~SWITCH1);

  if((!(p1val & SWITCH1)) & state == 1) {
    state = 2;
    button_pushed = 0;
    enableWDTInterrupts();
  }

  else if ((!(p1val & SWITCH1)) & state == 2) {
    state = 1;
    button_pushed = 0;
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

  /* Blink and Beep Mode */
  if (state == 1) {
  
    /* Button 1 */
    if (!(p2val & SW1) & button_state == 0) {
      button_pushed = 1;
      button_state = 1;
      enableWDTInterrupts();
    }

    else if (!(p2val & SW1) & button_state == 1) {
      button_state = 0;
      buzzer_set_period(0);
      og_laser = 3500;
      og_sec_cnt = 0;
      og_blnk_cnt = 12;
    }
  
    /* Button 2 */
    else if (!(p2val & SW2)) {
      if (laser_color == 1){
	laser_color = 64;
	laser_color_set(laser_color);
      } else{
	laser_color = 1;
	laser_color_set(laser_color);
      }
    }
  
    /* Button 3 */
    else if (!(p2val & SW3)) {
      og_sec_cnt += 5;
      og_blnk_cnt += 5;
      og_laser -= 50;
    }

    /* Button 4 */
    else if (!(p2val & SW4)) {
      og_sec_cnt -= 5;
      og_blnk_cnt -= 5;
      og_laser += 50;
    }
  
    /* Button Release */
    else {
      
      if (button_state == 0) {
	buzzer_set_period(0);
	P1OUT &= ~LEDS;
	configureClocks();
      }
    
      else if (button_state == 1)
	;
    
      else {
	buzzer_set_period(0);
	configureClocks();
	P1OUT &= ~LEDS;
      }
    }
  }

  /* Music Mode */
  else if (state == 2) {

    /* Button 1 */
    if (!(p2val & SW1) & button_state == 0) {
      button_pushed = 1;
      button_state = 1;
      enableWDTInterrupts();
    }

    else if (!(p2val & SW1) & button_state == 1) {
      button_pushed = 1;
      button_state = 0;
      configureClocks();
      buzzer_set_period(0);
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
    else if (!(p2val & SW4)) {
      button_pushed = 4;
      buzzer_set_period(0);
    }
  
    /* Button Release */
    else {
      
      if (button_state == 0) {
	buzzer_set_period(0);
	configureClocks();
      }
    
      else if (button_state == 1)
	;
    
      else {
	buzzer_set_period(0);
	configureClocks();
      }
    }  
  }
}

void
laser_button(){
  blnk_cnt ++;
  if (blnk_cnt >= og_blnk_cnt + 8) {
      blnk_cnt = 12;
      P1OUT |= laser_color;
    }

  else
    P1OUT &= ~LEDS;

  sec_cnt ++;
  laser -= 50;
  buzzer_set_period(laser);
  if (sec_cnt >= og_sec_cnt + 50) {
    sec_cnt = 0;
    laser = 3500;
  }
}

void
play_song(int a[], int len, int speed, int pause_len){
  for(int i = 0; i < len; i ++){
    buzzer_set_period(a[i]);
    delay(speed);

    if (button_pushed == 0) {
      P1OUT |= LED_GREEN;
      P1OUT |= LED_RED;
      delay(pause_len);
      P1OUT &= ~LED_GREEN;
      P1OUT &= ~LED_RED;
    }
      
    else if (a[i] != 0 & a[i] == a[i + 1]) {
      P1OUT |= LED_GREEN;
      delay(pause_len);
      P1OUT &= ~LED_GREEN;
    }

    else if (a[i] != 0 & a[i] != a[i + 1]) {
      P1OUT |= LED_RED;
      delay(pause_len);
      P1OUT &= ~LED_RED;
    }

    else if (a[i] == 0)
      ;

    else if (button_state == 0)
      break;
  }
  configureClocks();
  buzzer_set_period(0);
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
  if (button_pushed == 0)
    play_song(secret_song, ARRLEN(secret_song), 1000, 50);

  else if (state == 1) {
    
    if (button_pushed == 1)
      laser_button();
  }
  
  else if (state == 2) {

    if (button_pushed == 1)
      play_song(songar1, ARRLEN(songar1), 1500, 200);

    else if (button_pushed == 2)
      play_song(songar2, ARRLEN(songar2), 800, 50);

    else if (button_pushed == 3)
      ;

    else if (button_pushed == 4)
      ;
  }
}
