/* ----------------------------------------------------------------------------- 
  Ex.18
  Programa que aciona um motor DC com PWM para controle de velocidade.
  A largura do PWM � determinada pela leitura do canal 5 do m�dulo AD.
Autor: MARLON FERRAZ FERNANDES 
------------------------------------------------------------------------------*/

#include "io430.h"

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
    
  // Configura pino P2.3 como sa�da
  P2DIR |= BIT3;
}

void config_timer0() {
  TA0CTL   = (TASSEL_2 + ID_3 + MC_3);
  TA0CCR0  = 6250;
  TA0CCTL0 |= CCIE;
}

void config_timer1() {
  TA1CTL   = (TASSEL_2 + ID_0 + MC_1);
  TA1CCR0  = 1030;       // Define o per�odo do PWM
  TA1CCR1  = 250;        // Define a largura do PWM
  TA1CCTL0 |= CCIE;
  TA1CCTL1 |= CCIE;
}

void config_adc() {
  ADC10CTL0 |= SREF_0;          // Define a refer�ncia como VR+ = Vcc, VR- = Vss
  ADC10CTL0 |= ADC10SHT_2;      // Sample and Hold Time = 16*ADC10CLKs
  ADC10CTL0 |= MSC;             // Multiple Sample and Conversion com borda de subida de SHI 
  ADC10CTL0 |= ADC10ON;         // Ativa o ADC
  ADC10CTL0 |= ADC10IE;         // Habilita interrup��o do ADC
  ADC10CTL1 |= INCH_5;          // Entrada atrav�s do Canal 5 (A5)
  ADC10AE0  |= BIT5;            // Habilita P1.5 para entrada anal�gica
}    

void main() {
  config_uc();
  config_timer0();
  config_timer1();
  config_adc();
  
  // Seta General Interrupt Enable
  __enable_interrupt();
  
  // Loop infinito
  while (1);
}
 
#pragma vector = TIMER0_A0_VECTOR
__interrupt void inter_timer0_a0() {
  while (ADC10CTL0 & ADC10BUSY); // Espera o ADC parar de trabalhar
  ADC10CTL0 |= ENC;              // Permite a convers�o
  ADC10CTL0 |= ADC10SC;          // Come�a a convers�o AD
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void inter_timer1_a0() {
  P2OUT |= BIT3;        // P2.3 = 1
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void inter_timer1_a1() {
  P2OUT &= ~BIT3;       // P2.3 = 0
  TA1CCTL1 &= ~(CCIFG); // Clear na flag de interrup��o
}

#pragma vector = ADC10_VECTOR
__interrupt void inter_adc() {
  TA1CCR1 = ADC10MEM;   // Faz a largura do PWM ser o valor do ADC
}
