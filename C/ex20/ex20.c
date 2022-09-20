/* ----------------------------------------------------------------------------- 
  Ex.20
Programa que escreve em um display LCD alfanum�rico.
  
Escrever:
  uC MSP430 - 2020
  Nome

Autor: Marlon Ferraz Fernandes 
------------------------------------------------------------------------------*/

#include "io430.h"

char disciplina[18] = "uC MSP430 - 2020";
char nome[16] = "Marlon Fernandes";
				 

void config_uc() {
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  // Configura clock para 1 MHz
  DCOCTL  = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;
  
  // Configura os pinos P1.6 e P1.7 como sa�das
  P1DIR |= BIT7 + BIT6;
  P1SEL &= ~(BIT7 + BIT6);      // Configura pinos como digitais
  P1SEL2 &= ~(BIT7 + BIT6);     // Configura pinos como digitais
  
  // Configura os pinos P2.4 a P2.7 como sa�das
  P2DIR |= 0xF0;
  P2SEL &= ~(0xF0);      // Configura pinos como digitais
  P2SEL2 &= ~(0xF0);     // Configura pinos como digitais
}

void habilita() {
  P1OUT |= BIT7;        // Set bit Enable
  __delay_cycles(1);    // Delay de 1 us
  P1OUT &= ~(BIT7);     // Reset bit Enable
}

void escreve_display(char tipo, char byte) {
  if(tipo) {
    P1OUT |= BIT6;      // Set bit RS (dado)
  }
  else {
    P1OUT &= ~(BIT6);   // Reset bit RS (comando)
  }
 
  __delay_cycles(1000); // Delay de 1 ms
  
  P2OUT = byte;         // Envia os 4 primeiros bits do byte para P2OUT
  habilita();
  
  P2OUT = byte << 4;    // Desloca o byte em 4 bits
  habilita();
}

void inicializa_display(void) {
  __delay_cycles(15000);        // Delay de 15ms
  P1OUT &= ~(BIT6);             // Reset bit RS (instru��o)
  P2OUT = 0x30;                 // Instru��o do fabricante
  habilita();
  __delay_cycles(5000);         // Delay de 5 ms
  P2OUT = 0x30;                 // Instru��o do fabricante
  __delay_cycles(1000);         // Delay de 1 ms
  P2OUT = 0x30;                 // Instru��o do fabricante
  habilita();
  __delay_cycles(1000);
  habilita();
  P2OUT = 0x20;                 // Instru��o do fabricante
  __delay_cycles(1000);         // Delay de 1 ms
  habilita();
  
  escreve_display(0,0x28);
  
  escreve_display(0,0x08);
  
  escreve_display(0,0x01);
  
  escreve_display(0,0x06);
  
  escreve_display(0,0x0F);
}

void escreve_texto() {
  for(int i = 0; i < 18; i++) {
    escreve_display(1, disciplina[i]);
  }
  // Envia comando Address Counter para pular para o primeiro character da segunda linha
  escreve_display(0,0xC0);
  for(int i = 0; i < 16; i++) {
    escreve_display(1, nome[i]);
  }  
}

void main() {
  config_uc();
  inicializa_display();
  escreve_texto();
  while(1);
}
