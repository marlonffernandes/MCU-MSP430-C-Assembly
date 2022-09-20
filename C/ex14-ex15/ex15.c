/* ----------------------------------------------------------------------------- 
  Ex.15
  Programa que realiza contagem em três displays de 7 segmentos.

------------------------------------------------------------------------------*/

#include "io430.h"

char caracteres[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
char dig_1 = 0, dig_2 = 0, dig_3 = 0;

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1= CALBC1_1MHZ;
  
  // Configura pinos P1.0 a P1.6 como saídas
  // Controla os 7 segmentos
  P1DIR |= BIT6 + BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0;
  
  // Configura pinos P2.0 a P2.2 como saídas
  // Controla os transistores
  P2DIR |= BIT2 + BIT1 + BIT0;
}

void config_int_timer() {
  // Configura TIMER0_A: SMCLK, /2, Up Mode
  TA0CTL = (TASSEL_2 + ID_1 + MC_1);
  TA0CCTL0 |= CCIE;     // Habilita interrupção
  TA0CCR0 = 50000;
}

void main() {
  config_uc();
  config_int_timer();
  
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  // Loop infinito
  while(1) {
    P2OUT = 0x00;
    P1OUT = caracteres[dig_1];
    P2OUT = 0x01;
    __delay_cycles(1000);      // Delay de 10 ms
    P2OUT = 0x00;
    P1OUT = caracteres[dig_2];
    P2OUT = 0x02;
    __delay_cycles(1000);      // Delay de 10 ms
    P2OUT = 0x00;
    P1OUT = caracteres[dig_3];
    P2OUT = 0x04;
    __delay_cycles(1000);      // Delay de 10 ms
  }
}
  
// Interrupção TIMER0_A
#pragma vector = TIMER0_A0_VECTOR
__interrupt void interr_timer_A (void) {
  dig_1++;
  if (dig_1 > 9) {
    dig_1 = 0;
    dig_2++;
    if (dig_2 > 9) {
      dig_2 = 0;
      dig_3++;
      if (dig_3 > 9) {
        dig_3 = 0;
      }
    }
  }
}
