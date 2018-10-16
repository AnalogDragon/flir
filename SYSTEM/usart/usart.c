#include "sys.h"
#include "usart.h"	  
#include "myiic.h"	  


 	
u8 USART_RX_BUF=0; 

u16 USART_RX_STA=0; 
  
void uart_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
     //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

 //Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

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
			USART_RX_BUF=Res;
			USART_RX_STA=0xFF;
		}
	}
}

extern long data[PixLg][PixLg];


void send_once(void){
	u8 i;
	u8 num;
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1,0xBB);//�򴮿�1��������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1,0xAA);//�򴮿�1��������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1,65);//�򴮿�1��������
	for(i=0;i<64;i++){
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1,data[i / 8 * 5 + 2][i * 5 % 40 + 2]&0xff);//�򴮿�1��������
		num+=data[i / 8 * 5 + 2][i * 5 % 40 + 2]&0xff;
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1,(data[i / 8 * 5 + 2][i * 5 % 40 + 2]>>8)&0xff);//�򴮿�1��������
		num+=(data[i / 8 * 5 + 2][i * 5 % 40 + 2]>>8)&0xff;
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1,num&0xff);//�򴮿�1��������
}

