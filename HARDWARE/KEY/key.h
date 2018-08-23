#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"






#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//读取按键2 
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//读取按键3
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)//读取按键4
#define STBY  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取按键3
#define CHRG  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取按键4


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数				
u8 Battery_Scan(void);



#endif
