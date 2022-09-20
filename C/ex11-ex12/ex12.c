/*------------------------------------------------------------------------------
EX12 MARLON FERNANDES 
------------------------------------------------------------------------------*/

#include "io430.h"

char p13_ant;

void s2_pressionado() {
  if (p13_ant == 1) {
    p13_ant = 0x00;
    // Se os LEDs est�o acesos
    if ((P1OUT & BIT6) && (P1OUT & BIT0)) {
      P1OUT &= ~BIT6 + ~BIT0;  // Apaga LEDs
    } else {
      P1OUT |= BIT0;    // Acende LED vermelho
    }
  }
  return;
}

void piscar_leds() {
  P1OUT ^= BIT6 + BIT0;        // Inverte o estado dos LEDs por meio de XOR
  __delay_cycles(250000);      // Delay de 250000 ciclos de m�quina
  return;
}

int main( void ) {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;
  
  P1DIR |= BIT6 + BIT0;  // Configura BIT6 e BIT0 como sa�das 
  P1DIR &= ~BIT3;        // Configura BIT3 como entrada (bot�o S2)
  P1REN |= BIT3;         // Configura BIT3 como pull-up ou pull-down
  P1OUT |= BIT3;         // Configura BIT3 como pull-up
  
  P1OUT &= ~BIT6 & ~BIT0;        // Apaga LEDs
  
  p13_ant = 1;
  while (1) {
    
    // Se o bot�o n�o foi pressionado
    if (P1IN & BIT3) {
      p13_ant = 1;
    } else {
      s2_pressionado();
    }
        
    // Se pelo menos um dos LEDs est� aceso
    if ((P1OUT & BIT6) || (P1OUT & BIT0)) {
      piscar_leds();
    }
  }  
}
