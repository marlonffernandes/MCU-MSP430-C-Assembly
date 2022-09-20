/* ----------------------------------------------------------------------------- 
  Ex.19
  Utiliza m�dulo AD para controlar a velocidade do motor.
  Utiliza TIMER1_A para gerar o PWM.
  Inverte sentido de rota��o quando ocorre a interrup��o do pino P1.3
na borda de descida.
Autor: Marlon Ferraz Fernandes 
------------------------------------------------------------------------------*/

#include "io430.h"

char temp;

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
  
  // Configura pino P1.3 como entrada
  P1REN |= BIT3;
  P1OUT |= BIT3;
  
  // Define P2.2 como sa�da do TIMER1
  P2SEL |= BIT2;
  P2SEL2 &= ~BIT2;
  P2DIR |= BIT2;
  
  // Configura pinos P2.3 e P2.1 como sa�da
  P2DIR |= BIT3 + BIT1;
  
  // Define sentido inicial da rota��o
  P2OUT |= BIT1;
  P2OUT &= ~BIT3;
}

void config_inter_botao() {
  P1IE |= BIT3;
  P1IES |= ~BIT3;       // Na borda de descida
  P1IFG &= ~BIT3;
}

void config_timer0() {
  TA0CTL = (TASSEL_2 + ID_2 + MC_1);
  TA0CCR0 = 40000;
  TA0CCTL0 |= OUTMOD_7; // Modo Reset/Set
}

void config_timer1() {
  TA1CTL = (TASSEL_2 + ID_0 + MC_1);
  TA1CCR0 = 1023;       // Define o per�odo do PWM
  TA1CCR1 = 250;
  TA1CCTL1 |= OUTMOD_6; // Modo Toggle/Set
}

void config_adc() {
  ADC10CTL1 |= INCH_5;          // Entrada atrav�s do Canal 5 (A5)
  ADC10CTL1 |= SHS_2;           // In�cio da convers�o com OUT0
  ADC10CTL0 |= SREF_0;          // Define a refer�ncia como VR+ = Vcc, VR- = Vss
  ADC10CTL0 |= ADC10SHT_2;      // Sample and Hold Time = 16*ADC10CLKs
  ADC10CTL0 |= MSC;             // Multiple Sample and Conversion com borda de subida de SHI 
  ADC10CTL0 |= ADC10ON;         // Ativa o ADC
  ADC10CTL0 |= ADC10IE;         // Habilita interrup��o do ADC
  ADC10AE0  |= BIT5;            // Habilita P1.5 para entrada anal�gica
}    

void main() {
  config_uc();
  config_inter_botao();
  config_timer0();
  config_timer1();
  config_adc();
  
  // Permite a convers�o do ADC
  ADC10CTL0 |= ENC;                     
  
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  // Loop infinito
  while (1);
}

#pragma vector = PORT1_VECTOR
__interrupt void inter_P13(void) {
  P1IE &= ~BIT3;        // Desabilita interrup��o no pino P1.3
  
  // Guarda o estado anterior do P2OUT
  temp = P2OUT;
  // P2.1 = 0 e P2.3 = 0
  P2OUT &= ~(BIT3 + BIT1);
  
  // Debouncing:
  __delay_cycles(90000); // 16*5000
  // Aguarda enquanto o bot�o n�o estiver pressionado (P1.3 = 1)
  while(!(P1IN & BIT3));
  __delay_cycles(90000); // 16*5000
  
  // Inverte a rota��o
  P2OUT = temp ^ (BIT3 + BIT1);
  
  P1IE |= BIT3;         // Habilita interrup��o no pino P1.3
  P1IFG &= ~BIT3;       // Clear na flag de interrup��o
}

#pragma vector = ADC10_VECTOR
__interrupt void inter_adc() {
  TA1CCR1 = ADC10MEM;           // Faz a largura do PWM ser o valor do ADC
  ADC10CTL0 &= ~(ADC10IFG);     // Clear na flag de interrup��o
}
