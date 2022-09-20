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
char flag;
int T1;

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1= CALBC1_1MHZ;
          
  P1DIR |= BIT6 + BIT0;         // Configura pinos P1.6 e P1.0 como saída
  P1REN |= BIT3;                // Habilita Pull up/down para P1.3
  P1OUT |= BIT3;                // Habilita o Pull up para P1.3
  
  P1OUT &= ~BIT6 & ~BIT0;       // Apaga inicialmente os LEDs
}

void config_int_botao() {
  // Configura a interrupção no pino P1.3 (Botão S2)
  P1IE  |= BIT3;                // Habilita interrupção
  P1IES |= ~BIT3;               // Na borda de descida (setando como zero)
  P1IFG &= ~BIT3;               // Limpa a flag de iterrupção
}

void config_int_timer() {
  // Configura TIMER0_A: SMCLK, /8, Stop Mode
  TA0CTL = (TASSEL_2 + ID_3 + MC_3);
  TA0CCTL0 |= CCIE;             // Habilita interrupção
  TA0CCR0 = 50000;              // Move o valor a ser incrementado, 800ms = 8us (div) * N, passando 800ms para us, temos que N = 100000, como é UP/DOWN, então 50000
}

void main( void )
{
  flag = 'A';
  T1 = 0;
  config_uc();
  config_int_botao();
  config_int_timer();
  __enable_interrupt();         // Seta General Interrupt Enable
  while(1);                     // Loop infinito
}

// Interrupção no pino P1.3
#pragma vector = PORT1_VECTOR
__interrupt void interr_P1(void) 
{
  P1IE &= ~BIT3;                // Desabilita interrupção no pino P1.3 (Botão S2)
  
  if (T1 < 1000 && flag == 'A')
  {
    P1OUT &= ~BIT0 & ~BIT6;     // Apaga os LEDs
  }
  else if (T1 >= 1000 && flag == 'A')
  {
    flag = 'B';
    //Acende um LED e apaga o outro
    P1OUT &= ~BIT0;            //Vermelho
    P1OUT |= BIT6;             //Verde
  }
  else if (T1 < 2000 && flag == 'B')
  {
    //Acende um LED e apaga o outro
    P1OUT &= ~BIT0;            //Vermelho
    P1OUT |= BIT6;             //Verde
  }
  else if (T1 >= 2000 && flag == 'B')
  {
    flag = 'C';
    P1OUT |= BIT0 + BIT6;
  }
  else if (T1 < 3000 && flag == 'C')
  {
    P1OUT |= BIT0 + BIT6; 
  }
  else if (T1 >= 3000 && flag == 'C')
  {
    flag = 'D';
    //Acende um LED e apaga o outro
    P1OUT &= ~BIT6;            //Verde
    P1OUT |= BIT0;             //Vermelho
  }
  else if (T1 < 4000 && flag == 'D')
  {
    //Acende um LED e apaga o outro
    P1OUT &= ~BIT6;            //Verde
    P1OUT |= BIT0;             //Vermelho
  }
  else if(T1 >= 4000 && flag == 'D')
  {
    flag = 'A';
    P1OUT &= ~BIT0 & ~BIT6;     // Apaga os LEDs
  }

   __delay_cycles(5000);        // Aguarda enquanto o botão não estiver pressioando (P1.3 = 1)
  while(!(P1IN & BIT3));        // Enquanto o pino P1.3 estiver pressionado, fique no while
  __delay_cycles(1000);
  P1IE |= BIT3;                 // Habilita interrupção no pino P1.3
  P1IFG &= ~BIT3;               // Limpa a flag de interrupção
}