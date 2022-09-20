/* ----------------------------------------------------------------------------- 
  Ex.16
  Programa que aciona um motor de passo.
  Bobina A: P2.1
  Bobina B: P2.2
  Bobina A': P2.3
  Bobina B': P2.4
  Sequ�ncia direta de acionamento: A-B-A'-B'
  Utiliza TIMER0_A para temporizar.
  Bot�o S2 inverte o sentido de rota��o.
Autor: Marlon Ferraz Fernandes
------------------------------------------------------------------------------*/

#include "io430.h"

char sentidoRotacao = 0;

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1= CALBC1_1MHZ;
  
  // Configura pino P1.3 como entrada
  P1REN |= BIT3;
  P1OUT |= BIT3;
    
  // Configura pinos P2.1 a P2.4 como sa�das
  // Controla os transistores
  P2DIR |= BIT4 + BIT3 + BIT2 + BIT1;
  P2OUT = BIT1; // Inicialmente A acionada
}

void config_int_botao() {
  // Configura a interrup��o no pino P1.3 (Bot�o S2)
  P1IE  |= BIT3;
  P1IES |= ~BIT3;               // Na borda de descida
  P1IFG &= ~BIT3;
}

void config_int_timer() {
  // Configura TIMER0_A: SMCLK, Up Mode
  TA0CTL = (TASSEL_2 + ID_0 + MC_1);
  TA0CCTL0 |= CCIE;     // Habilita interrup��o
  TA0CCR0 = 5000;
}

void main() {
  config_uc();
  config_int_botao();
  config_int_timer();
    
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  // Loop infinito
  while(1) {
  }
}
 
// Interrup��o no pino P1.3
#pragma vector = PORT1_VECTOR
__interrupt void interr_P1(void) {
  P1IE &= ~BIT3;        // Desabilita interrup��o no pino P1.3
  
  sentidoRotacao ^= 1;  // Inverte sentido de rota��o
  
  __delay_cycles(5000);
  // Aguarda enquanto o bot�o n�o estiver pressionado (P1.3 = 1)
  while(!(P1IN & BIT3));
  __delay_cycles(1000);
  P1IE |= BIT3;         // Habilita interrup��o no pino P1.3
  
  P1IFG &= ~BIT3;       // Clear na flag de interrup��o
}

// Interrup��o TIMER0_A
#pragma vector = TIMER0_A0_VECTOR
__interrupt void interr_timer_A (void) {
  if (sentidoRotacao == 0) {    // Sequ�ncia direta de acionamento
    if (P2OUT == BIT4) {        // Se B' est� acionada
      P2OUT = BIT1;             // Volta para A
    } else {
      P2OUT = P2OUT << 1;       // Segue a sequ�ncia de acionamento 
    }
  } else {                      // Sequ�ncia indireta de acionamento
    if (P2OUT == BIT1) {        // Se A est� acionada
      P2OUT = BIT4;             // Volta para B'
    } else {
      P2OUT = P2OUT >> 1;       // Segue a sequ�ncia de acionamento
    }
  }
}
