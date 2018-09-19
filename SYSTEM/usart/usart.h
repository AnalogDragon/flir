#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


	  	
extern u8  USART_RX_BUF; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void send_once(void);




#endif


