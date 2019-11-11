#include <16F628A.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.c>
#use delay(clock=4000000)
#fuses NOWDT,XT, NOPUT, NOPROTECT, BROWNOUT, MCLR, LVP, NOCPD
#use rs232 (baud=9600, xmit=pin_b2, rcv=pin_b1)

void main()
{
   char BufferRX[35], Latitude[9], Longitude[10];
   int i = 0, j=0, x=0, w=0;
   float LatF, LongF;
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   //set_tris_a(0x00); //Configura Porta A como saída

   usart_inicializa (25,1);
   
   while (true)
   {
      BufferRX[i]=usart_recebe();
           
      if(BufferRX[i]=='$')
      {
         for(i=0;i<5;i++){
            BufferRX[i]=usart_recebe();}
               if((BufferRX[0]=='G')&&(BufferRX[1]=='P')&&(BufferRX[2]=='G')&&(BufferRX[3]=='G')&&(BufferRX[4]=='A'))
               { 
                  for(j=0;j<34;j++){
                     BufferRX[j]=usart_recebe();
                     if((j>11 )&&(j<21)){
                        x=j;
                        Latitude[x-12]=BufferRX[j];
                        //usart_transmite(BufferRX[j]);
                        usart_transmite(Latitude[x-12]);
                     }
                                     
                     if((j>23 )&&(j<34)){
                        x=j;
                        Longitude[x-24]=BufferRX[j];
                        //usart_transmite(BufferRX[j]);}
                        usart_transmite(Longitude[x-24]);
                        
                    }
               }
               
               //Latf = atof(Latitude);
               //Longf = atof(Longitude);
                    
                              
         }
      }
      //printf("%.4f",LatF);
      //printf("%.4f",LongF);

    }
}    
                    
                  //usart_transmite("\r\n"); -> Colocar \r\n, independente de ser printf ou usart_transmite, dá erro!!!!!!!!!!!!!!!!!!
                 
