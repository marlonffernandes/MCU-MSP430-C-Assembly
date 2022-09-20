#include "io430.h"
char teclado[4][4] = {'8','9','7',0x00,'5','6','4','F','2','3','1','M','0','#','*','L'};
char coluna = 3, linha = 0, mask;

void configura_uC(void){
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  DCOCTL = CALDCO_1MHZ;  //configura frequencia DCO 16MHz
  BCSCTL1 = CALBC1_1MHZ;
}

void configura_portas(void) {
  P2DIR = 0x0F; //configurar p2.0 - P2.3 como saida    
  P2DIR = 0x0F; //configurar p2.0 - P2.3 como saida
  
  P2SEL = 0x00; //configura funcao register P2
  P2SEL2 = 0x00; //configura funcao register P2  
  
  P2OUT = 0x0F; //deixa em nível alto pinos p2.0-P2.3
  
  P1DIR = (BIT2 + BIT5);
  
  P1SEL &= ~(BIT2 + BIT5); //configura funcao register P1
  P1SEL2 &= ~(BIT2 + BIT5); //configura funcao register P1
  
  P1OUT &= ~(BIT2 + BIT5);
}

void configura_int_P2(void){
  //P2REN |= (BIT4 + BIT5 + BIT6 + BIT7); //ativa pulldown p2.4-p2.7
  //P2OUT |= (BIT4 + BIT5 + BIT6 + BIT7); //configura pullup no p1.3
  P2IE |= (BIT4 + BIT5 + BIT6 + BIT7); // ativa interrupção no pino P2.4-P2.7
  P2IES &= ~(BIT4 + BIT5 + BIT6 + BIT7); // indica borda de ativação da interrupção (borda de subida = 0, descida = 1)
  P2IFG = 0x00; //limpa flag, &P2IFG recebe 1 quando a interrupção ativa e deve ser resetado por software, comparar se tiver mais de uma interrupção na mesma porta
}

#pragma vector=PORT2_VECTOR     //rotina de interrupção P1.3, 
__interrupt void interr_P2(void) {
    P2IE &= ~(BIT4 + BIT5 + BIT6 + BIT7);      //debouncing - desabilita interrupção
    __delay_cycles(2000); //debouncing - delay de pino baixo 2ms
    mask = (P2IFG & 0xF0);
    
    switch(mask){
    case 0x80:
      coluna = 0;
      break;
      
    case 0x40:
      coluna = 1;
      break;
      
    case 0x20:
      coluna = 2;
      break;
      
    case 0x10:
      coluna = 3;
      break;
      
    default:
      coluna = 4;
    }
    
    P2OUT = 0x01;
    
    for(int i = 0; i < 4; i ++){
      if (P2IN & mask) {
        linha = i;
        break;
      }
      if(!(P2OUT & 0x08)) P2OUT = P2OUT << 1;
    }    
    
    switch (coluna){
    case 0:
      P1OUT |= (BIT2 + BIT5);
      break;
    case 1:
      P1OUT &= ~BIT5;
      P1OUT |= BIT2;
      break;
    case 2:
      P1OUT &= ~BIT2;
      P1OUT |= BIT5;
      break;
    case 3:
      switch (linha) {
      case 1: 
        P1OUT &= ~BIT2;
        P1OUT |= BIT5;
        break;
      case 2: 
        P1OUT |= (BIT2 + BIT5);
        break;
      case 3: 
        P1OUT &= ~BIT5;
        P1OUT |= BIT2;
        break;
      default:
        P1OUT &= ~(BIT5+BIT2);
      }
      break;
    default:
      P1OUT &= ~(BIT5+BIT2);
    }    
    
    P2OUT = 0x0F; //ativa novamente as portas
    
    P2IE |= (BIT4 + BIT5 + BIT6 + BIT7);         //deboucing - reativa interrupção
    P2IFG &= ~(BIT4 + BIT5 + BIT6 + BIT7);     //limpa flag de interrupção
}

void main( void )
{
  configura_uC();
  configura_portas();
  configura_int_P2();
  
  __enable_interrupt(); //habilita geral interrupções (seta GIE)
  
  while (1); //loop infinito
}
