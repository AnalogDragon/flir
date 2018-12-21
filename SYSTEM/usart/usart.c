#include "usart.h"

 	
u8 USART_RX_BUF=0; 
u16 USART_RX_STA=0; 
  
void uart_init(u32 bound){
	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//RCC Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	
	//GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	//NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//USART
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE); 

}

//To send AA BB 00 Close
//To send AA BB 01 Open
void USART1_IRQHandler(void){                
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  {
		Res = USART_ReceiveData(USART1);
		if(USART_RX_STA==0 || USART_RX_STA==0xff){
			if(Res==0xAA)USART_RX_STA=1;
		}else if(USART_RX_STA==1){
			if(Res==0xBB)
				USART_RX_STA=2;
			else
				USART_RX_STA=0;
		}else if(USART_RX_STA==2){
			USART_RX_BUF = Res;
			USART_RX_STA = 0xFF;
			if(USART_RX_BUF)
				SysState.SysFlag.bit.UsartFlag = 1;
			else
				SysState.SysFlag.bit.UsartFlag = 0;
		}
	}
}


void send_once(void){
	u8 i;
	u8 num;
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1,0xBB);//向串口1发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1,0xAA);//向串口1发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1,65);//向串口1发送数据
	
	for(i=0;i<64;i++){
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1,PriData[7-i/8][i%8]&0xff);//向串口1发送数据
		num+=PriData[7-i/8][i%8]&0xff;
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1,(PriData[7-i/8][i%8]>>8)&0xff);//向串口1发送数据
		num+=(PriData[7-i/8][i%8]>>8)&0xff;
	}

	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1,num&0xff);//向串口1发送数据
	
}

void printtime(void){		//调试用
	
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1,(((KeyState.Key3.HoldTime-60)/60)>>8)&0xff);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1,((KeyState.Key3.HoldTime-60)/60)&0xff);//向串口1发送数据
	
}





