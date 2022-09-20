/*-----------------------------------------------------------------------------
  Programa que utiliza comunica��o serial UART.
  Autor: Marlon Ferraz Fernandes
-----------------------------------------------------------------------------*/

#include "io430.h"
char byte, flag;

void config_uC() {
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer to prevent time out reset
  DCOCTL = CALDCO_8MHZ;                 // Define o clock como 8MHz
  BCSCTL1 = CALBC1_8MHZ;                // Define o clock como 8MHz
  P1DIR |= BIT0 + BIT6;                 // Define os pinos P1.0 e P1.6 como sa�das (LEDs)
  P1SEL |= BIT1 + BIT2;
  P1SEL2 |= BIT1 + BIT2;
  P1OUT = BIT0;                         // Acende o LED vermelho
}

void config_UART() {
  UCA0CTL0 = 0x00;                      // UART Mode, Ass�ncrono, 8 bits, sem paridade, LSB first, 1 stop bit
  UCA0CTL1 |= UCSSEL_2;                 // SMCLK
  UCA0BR0 = 0x41;                       // Baud rate 9600bps
  UCA0BR1 = 0x03;                       // Baud rate 9600bps
  UCA0MCTL = UCBRF_0;                   // Como UCOS16 = 0, ignorado
  UCA0MCTL |= UCBRS_3;                  // UCBRS = round((N � int(N))*8), sendo N = UCABR0
  UCA0MCTL &= ~(UCOS16);                // Oversampling Mode disabled
}

void envia_byte(char caractere) {
  while (!(IFG2 & UCA0TXIFG));          // Espera terminar de transferir o que estava transferindo
  UCA0TXBUF = caractere;
}

void main() {
  config_uC();                          // Chama a fun��o que configura o uC
  UCA0CTL1 = UCSWRST;                   // USCI em estado de Reset
  config_UART();                        // Chama a fun��o que configura a interface UART
  flag = 0;                             // Clear na flag de recebimento do #
  UCA0CTL1 &= ~(UCSWRST);               // USCI liberada
  IE2 |= UCA0RXIE;                      // Habilita interrup��o de recep��o
  __enable_interrupt();                 // Permite interrup��o
  while(1);
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void inter_UART_RX(void) {
  byte = UCA0RXBUF;                     // Salva a mensagem recebida na vari�vel byte
  P1OUT ^= BIT0 + BIT6;                 // Inverte os leds acesos
  if (byte == '#') {                     // Testa se recebeu o caracter "#"
    flag = 1;
  }
  if (flag) {
    envia_byte('>');                    // Envia ">"
    envia_byte(byte);                   // Envia a mensagem salva na variavel byte
    envia_byte(0x0D);                   // Line Fit e Carriage Return
    envia_byte(0x0A);                   // Muda de linha
  }
}
