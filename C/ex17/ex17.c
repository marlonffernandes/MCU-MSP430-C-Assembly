/* ----------------------------------------------------------------------------- 
  Ex.17
  Programa que aciona um motor DC com PWM para controle de velocidade.
Autor: Marlon Ferraz Fernandes 
------------------------------------------------------------------------------*/

#include "io430.h"

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL  = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;
  
  // Configura pino P1.3 como entrada
  P1REN |= BIT3;
  P1OUT |= BIT3;
    
  // Configura pinos P2.3 como sa�daa
  P2DIR |= BIT3;
}

void config_int_botao() {
  // Configura a interrup��o no pino P1.3 (Bot�o S2)
  P1IE  |= BIT3;
  P1IES |= ~BIT3;       // Na borda de descida
  P1IFG &= ~BIT3;
}

void config_int_timer() {
  // Configura TIMER0_A
  TA1CTL = (TASSEL_2 + ID_0 + MC_1);
  TA1CCR0 = 1023;       // Define o per�odo do PWM
  TA1CCR1 = 250;        // Define a largura do PWM
  TA1CCTL0 |= CCIE;          
  TA1CCTL1 |= CCIE;
}

void controla_pwm() {
  switch (TA1CCR1) {
    case 250:   TA1CCR1 = 500;
                break;
    case 500:   TA1CCR1 = 1000;
                break;
    case 1000:  TA1CCR1 = 250;
                break;
    default:    break;
  }
}

void main() {
  config_uc();
  config_int_botao();
  config_int_timer();
    
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  // Loop infinito
  while(1);
}
 
// Interrup��o no pino P1.3
#pragma vector = PORT1_VECTOR
__interrupt void interr_P1(void) {
  P1IE &= ~BIT3;        // Desabilita interrup��o no pino P1.3
  
  controla_pwm();
  
  __delay_cycles(5000);
  // Aguarda enquanto o bot�o n�o estiver pressionado (P1.3 = 1)
  while(!(P1IN & BIT3));
  __delay_cycles(1000);
  P1IE |= BIT3;         // Habilita interrup��o no pino P1.3
  
  P1IFG &= ~BIT3;       // Clear na flag de interrup��o
}

// Interrup��o TIMER1_A0
#pragma vector = TIMER1_A0_VECTOR
__interrupt void interr_timer1_A0 (void) {
  P2OUT |= BIT3;        // P2.3 = 1 
}

// Interrup��o TIMER1_A1
#pragma vector = TIMER1_A1_VECTOR
__interrupt void interr_timer1_A1 (void) {
  P2OUT &= ~BIT3;        // P2.3 = 0
  TA1CCTL1 &= ~(CCIFG);  // Clear na flag de interrup��o
}
