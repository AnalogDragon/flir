#ifndef __USART_H
#define __USART_H
#include "sys.h" 


	  	
extern u8  USART_RX_BUF; 
extern u16 USART_RX_STA; 

void uart_init(u32 bound);
void send_once(void);
void printtime(void);


#endif


