/* ----------------------------------------------------------------------------- 
  Ex.14
  Programa que alterna o estado dos LEDs vermelho e verde a cada 0,8 segundos.
- Inialmente os LEDs estão apagados. Sempre que a interrupção do pino P1.3 for
 ativada, o estado dos LEDs deverá alternar entre apagado e piscante;
- Temporização feita através do TIMER0_A;
- Inclui procedimento de debouncing.

Autor: Marlon Fernandes
------------------------------------------------------------------------------*/

#include "io430.h"

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1= CALBC1_1MHZ;
  
  // Configura pinos P1.6 e P1.0 como saída
  P1DIR |= BIT6 + BIT0;
  P1REN |= BIT3;
  P1OUT |= BIT3;
  
  P1OUT &= ~BIT6 & ~BIT0;       // Apaga inicialmente os LEDs
}

void config_int_botao() {
  // Configura a interrupção no pino P1.3 (Botão S2)
  P1IE  |= BIT3;
  P1IES |= ~BIT3;               // Na borda de descida
  P1IFG &= ~BIT3;
}

void config_int_timer() {
  // Configura TIMER0_A: SMCLK, /8, Stop Mode
  TA0CTL = (TASSEL_2 + ID_3 + MC_0);
  TA0CCTL0 |= CCIE;     // Habilita interrupção
  TA0CCR0 = 50000;      // 2*(8*50000) = 2*(400000) = 800000 ciclos para 0,8 s
}

void main() {
  config_uc();
  config_int_botao();
  config_int_timer();
  
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  while(1);     // Loop infinito
}
  
// Interrupção no pino P1.3
#pragma vector = PORT1_VECTOR
__interrupt void interr_P1(void) {
  P1IE  &= ~BIT3;       // Desabilita interrupção no pino P1.3
  
  TA0CTL ^= (MC_3);     // Operação XOR para inverter estado do TIMER
  if (!(TA0CTL & MC_3)) {
    P1OUT &= ~BIT6 & ~BIT0;
  } else {
    P1OUT |= BIT0;      // Acende LED VM
    P1OUT &= ~BIT6;     // Apaga LED VD
  }
  P1IFG &= ~BIT3;       // Clear na flag de interrupção
  
  __delay_cycles(5000);
  // Aguarda enquanto o botão não estiver pressioando (P1.3 = 1)
  while(P1IN & BIT3);
  __delay_cycles(1000);
  P1IE |= BIT3;         // Habilita interrupção no pino P1.3
}
  
// Interrupção TIMER0_A
#pragma vector = TIMER0_A0_VECTOR
__interrupt void interr_timer_A (void) {
  P1OUT ^= BIT0 + BIT6;     // Alterna o estado dos LEDs
}
