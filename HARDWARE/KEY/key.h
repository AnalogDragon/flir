#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//��ȡ����2 
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//��ȡ����3
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)//��ȡ����4
#define STBY  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//��ȡ����3
#define CHRG  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//��ȡ����4

struct AKeyState_REG;

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��				
u8 Battery_Scan(void);
void GetAKey(struct AKeyState_REG *KeyBuf,u8 IOState);
void GetKey(void);


#endif

