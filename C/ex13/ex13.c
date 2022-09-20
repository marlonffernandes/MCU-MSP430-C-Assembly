  
#include "io430.h"

void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1= CALBC1_1MHZ;

  P1DIR |= BIT6+BIT0;    //Configurar Porta1 - pino 6 e 0 na saida
  P1REN |= BIT3;
  P1OUT |= BIT3;
  
   P1OUT &= ~BIT6 & ~BIT0;        // Apaga LEDs

  //Conf. interrupção P13
  
  P1IE  |= BIT3;
  P1IES |= ~BIT3; //borda de descida
  P1IFG &=~BIT3;
 
  //Conf. Timer0_A
  
  TA0CTL = (TASSEL_2+ID_3+MC_0);
  TA0CCTL0 |= CCIE;
  TA0CCR0 = 50000;  
  
  //Habilitar Interrupções GIE
  
 __bis_SR_register(GIE); 
  
  while(1);
}
  
  #pragma vector = PORT1_VECTOR
  __interrupt void interr_P1 (void)
  {
    TA0CTL ^= (MC_3);  //Operação do exclusivo
      
      if(!(TA0CTL & MC_3))
      {
               P1OUT &= ~BIT6 & ~BIT0;
      }
    
     else
     {
               P1OUT  |= BIT0;      //Acender Led vermelho
               P1OUT  &=~BIT6;      //Apagar Led Verde
       
     }
  
    
P1IFG &= ~BIT3;
  }
    
    //Interr. Timer0_A
    
    #pragma vector = TIMER0_A0_VECTOR
  __interrupt void interr_timer_A (void)
    
  {
    
    P1OUT^=BIT0 + BIT6;          //inverter pinos
    
  }
  
  
      

