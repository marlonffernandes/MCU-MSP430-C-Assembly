/*------------------------------------------------------------------------------
EX 11 MARLON
------------------------------------------------------------------------------*/

#include "io430.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;
  
  P1DIR = BIT6 + BIT0;  // Configura BIT6 e BIT0 como sa�das 
  P1OUT &= ~BIT6;       // Apaga LED verde
  P1OUT |= BIT0;        // Acende LED vermelho
  
  while (1) {
    P1OUT ^= BIT6 + BIT0;        // Inverte o estado dos LEDs por meio de XOR
    __delay_cycles(250000);      // Delay de 250000 ciclos de m�quina
  }  
}
