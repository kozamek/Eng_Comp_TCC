//************************************************************
//*                                                          *
//*  USART.C                                                 *
//*                                                          *
//*  Biblioteca de manipulação da USART                      *
//*  Suporta o modo assíncrono                               *
//*                                                          *
//*  Autor: Fábio Pereira                                    *
//*  Livro: Microcontroladores Pic: Programação em C         *
//*                                                          *
//************************************************************
char usart_rx, usart_tx, txreg, rcreg, spbrg;

struct rcsta_reg
   {
      int rx9d : 1;
      int oerr : 1;
      int ferr : 1;
      int aden : 1;
      int cren : 1;
      int sren : 1;
      int rx9  : 1;
      int spen : 1;
   } rcsta;

struct txsta_reg
   {
      int tx9d : 1;
      int trmt : 1;
      int brgh : 1;
      int xxx  : 1;
      int sync : 1;
      int txen : 1;
      int tx9  : 1;
      int csrc : 1;
   } txsta;

// define os endereços das variáveis
#locate rcsta = 0x18
#locate txreg = 0x19
#locate rcreg = 0x1a
#locate txsta = 0x98
#locate spbrg = 0x99
#byte r_pir1  = 0x0c   // define o registrador r_pir1
#bit flag_rc = r_pir1.5   // define o flag_rc
// definições utilizadas nas funções

void usart_inicializa ( int vel, boolean brgh )
/*
O valor dos parâmetros vel e brgh deve ser retirado a partir das
tabelas de baud rate fornecidas pela Microchip ou no livro:
Microcontroladores PIC: Técnicas Avançadas.
*/

{
   txsta.brgh = brgh;   // seleciona o modo do gerador de baud rate
   spbrg = vel;        // configura o gerador de baud rate

   // configura os pinos da USART como entradas !!!!!

   input (pin_b1); //RX
   input (pin_b2); //TX

   txsta.sync = 0;        // seleciona o modo assíncrono
   rcsta.spen = 1;        // habilita a USART
   txsta.tx9 = 0;         // seleciona o modo de 8 bits
   txsta.txen = 1;        // ativa o transmissor da USART
   rcsta.cren = 1;      // modo de recepção contínua
}



void usart_transmite (char dado)
{
   while (!txsta.trmt);    // aguarda o buffer de transmissão esvaziar
   txreg = dado;      // coloca novo caractere para transmissão
}

char usart_recebe (void)
{
   while (!flag_rc);   // aguarda a recepção de caracteres
   return rcreg;      // retorna o caractere recebido
}


//****************************************************************
//*                                                              *
//*  FiltroGPS.C - Novembro 2009                                 *
//*                                                              *
//*  Trata as informações do GPS, convertendo os valores         *
//*  de latitude e longitude, e executa os passos para           *
//*  atingir a longitude de destino (caso necessite)             *
//*  Autores: Danilo Maurilio,                                   *
//*           Eduardo de Siqueira Correa,                        *
//*           Jonas Fabricio Kozamekinas                         *
//*  10° Semestre Eng. Computação, Universidade Metodista de SP  *
//*                                                              *
//****************************************************************

#include <16F628A.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.c>
#use delay(clock=4000000)
#fuses NOWDT,XT, NOPUT, NOPROTECT, BROWNOUT, MCLR, LVP, NOCPD
#use rs232 (baud=9600, xmit=pin_b2, rcv=pin_b1)
#zero_ram

void main()
{
   char BufferRX[40],lat[9],longi[9];
   int i = 0, j = 0, x=0, y=0;
   int32 LatAtual=0, LongAtual=0;
   int32 LatDest=0, LongDest=46345875;
     
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   set_tris_a(0x00); //Configura Porta A como saída

   usart_inicializa (25,1);

   while(true)
   { 
      LatAtual=0;
      LongAtual=0;
      x=0;
      y=0;
      
      BufferRX[i]=usart_recebe();
      if(BufferRX[i]=='$')
      {
         for(i=0;i<5;i++)
         {
            BufferRX[i]=usart_recebe();
         }
         
         if((BufferRX[0]=='G')&&(BufferRX[1]=='P')&&(BufferRX[2]=='G')&&(BufferRX[3]=='G')&&(BufferRX[4]=='A'))
         {
            for(j=0;j<34;j++)       
            {
               BufferRX[j]=usart_recebe();
            
               if((j>11)&&(j<16))
               {
                  lat[x] = BufferRX[j];
                  usart_transmite(lat[x]);  
                  x++;
               }
               
               if((j>16)&&(j<21))
               {
                  lat[x] = BufferRX[j];
                  usart_transmite(lat[x]);
                  x++;
               }
           
               if((j>24)&&(j<29))
               {
                  longi[y] = BufferRX[j];
                  usart_transmite(longi[y]);
                  y++;
               }
               
               if((j>29)&&(j<34))
               {
                  longi[y] = BufferRX[j];
                  usart_transmite(longi[y]);
                  y++;
               }          
           
            }
             
            LongAtual=atoi32(longi);//conversão de string para inteiro de 32 bits
            
            LatAtual=atoi32(lat); //conversão de string para inteiro de 32 bits
              
            if (LongAtual < LongDest) //se for menor, o veículo se desloca
            {
               output_high(PIN_A2);
               output_low(PIN_A3);
            }                 
            
            else //caso contrário, para
            {
               output_low(PIN_A2);
               output_low(PIN_A3);
            }               
         }
      }
   }  
}
