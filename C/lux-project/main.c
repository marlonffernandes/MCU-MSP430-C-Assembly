/*------------------------------------------------------------------
Projeto de Microcontroladores


Luminária com intensidade controlada em lux


Autor: MARLON FERRAZ FERNANDES
------------------------------------------------------------------*/


#include "io430.h"


float AD = 0;         // Variável da conversão A/D
float lux = 0;
float lux_ant = 0;
float PWM = 0;
unsigned int PWM_atuador = 0;
float SP = 400;         // Set Point definido pelo usuário (em lix)
// Valores das constantes do controlador PI determinadas via projeto
float Kp = 4.08;
float Ki = 4.17018;
float erro, int_erro;


void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
    
  // Configura pino P2.3 como saída
  P2DIR |= BIT3;
}


void config_timer0() {
  TA0CTL = (TASSEL_2 + ID_3 + MC_3);
  TA0CCR0 = 6250;
  TA0CCTL0 |= CCIE;
}


void config_timer1() {
  TA1CTL = (TASSEL_2 + ID_0 + MC_1);
  TA1CCR0 = 1030;       // Define o período do PWM
  TA1CCR1 = 250;        // Define a largura inicial do PWM
  TA1CCTL0 |= CCIE;
  TA1CCTL1 |= CCIE;
}


void config_adc() {
  ADC10CTL0 |= SREF_0;          // Define a referência como VR+ = Vcc, VR- = Vss
  ADC10CTL0 |= ADC10SHT_2;      // Sample and Hold Time = 16*ADC10CLKs
  ADC10CTL0 |= MSC;             // Multiple Sample and Conversion com borda de subida de SHI 
  ADC10CTL0 |= ADC10ON;         // Ativa o ADC
  ADC10CTL0 |= ADC10IE;         // Habilita interrupção do ADC
  ADC10CTL1 |= INCH_5;          // Entrada através do Canal 5 (A5)
  ADC10AE0  |= BIT5;            // Habilita P1.5 para entrada analógica
}    


void controladorPI() {
 erro = SP - lux; // Calcula o erro entre SP e PV 
 if (erro> (SP*0.10) || erro < (-SP*0.10)) // Reset do termo integrativo (anti-windup)
         int_erro = 0; // Zera o erro integrativo se o erro for maior que 10% de SP (em modulo). Isto reduz o overshoot
 else
    int_erro += erro; // Habilita o erro integrativo se o erro for menor que 10% de SP (em modulo)
  
  PWM = erro * Kp + int_erro * Ki;
  if (PWM > 255)
    PWM = 255; //verifica os limites possiveis do PWM
  else if (PWM < 0)
    PWM = 100;


  PWM_atuador = 255-PWM;
  TA1CCR1 = PWM_atuador;        // Define nova largura do PWM
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
  ADC10CTL0 |= ENC;              // Permite a conversão
  ADC10CTL0 |= ADC10SC;          // Começa a conversão AD
}


#pragma vector = TIMER1_A0_VECTOR
__interrupt void inter_timer1_a0() {
  P2OUT |= BIT3;        // P2.3 = 1
}


#pragma vector = TIMER1_A1_VECTOR
__interrupt void inter_timer1_a1() {
  P2OUT &= ~BIT3;       // P2.3 = 0
  TA1CCTL1 &= ~(CCIFG); // Clear na flag de interrupção
}


#pragma vector = ADC10_VECTOR
__interrupt void inter_adc() {
  AD = ADC10MEM;   // Faz a largura do PWM ser o valor do ADC
  lux = 1.3381*AD + 2.6481; // Equacao de calibracao do sensor
  controladorPI()
}