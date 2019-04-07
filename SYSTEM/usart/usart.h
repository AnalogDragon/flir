#ifndef __USART_H
#define __USART_H
#include "sys.h" 

#define USART1_BUF_SIZE 640

struct USART1_REG{
	u8  USART_RX_NUM;
	
	u8  USART_RX_LEG;											//���ճ���
	u8 	USART_RX_BUF[32];		//���ջ���
	
	u8  USART_TX_LEG;											//���ͳ���
	u8 	USART_TX_BUF[USART1_BUF_SIZE];		//���ͻ���
};

extern struct USART1_REG USART1_DATA;

void uart_init(u32 bound);
void send_once(void);
void printtime(void);
void DMA_USART1_init(void);
void DMA_USART1_SendData(u16 size);


#endif


