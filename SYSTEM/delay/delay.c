#include "delay.h"
 
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
// 	SysTick_Config(72000);
	
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
	fac_ms=(u16)fac_us*1000;//��ucos��,����ÿ��ms��Ҫ��systickʱ����   

}								    

//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
} 


void SysTimeInt(void){
	
	SysTime.SysTimeCNT10ms++;
	SysTime.SysTimeFLG10ms = 1;
	if((SysTime.SysTimeCNT10ms%10) == 0){
		SysTime.SysTimeCNT100ms++;
		SysTime.SysTimeFLG100ms = 1;
		if(SysTime.SysTimeCNT10ms>=60000)SysTime.SysTimeCNT10ms = 0;
		
		if((SysTime.SysTimeCNT100ms%10) == 0){
			SysTime.SysTimeCNT1s++;
			SysTime.SysTimeFLG1s = 1;
			if(SysTime.SysTimeCNT100ms>=60000)SysTime.SysTimeCNT100ms = 0;
			
			if((SysTime.SysTimeCNT1s%60) == 0){
				SysTime.SysTimeCNT1min++;
				SysTime.SysTimeFLG1min = 1;
				if(SysTime.SysTimeCNT1s>=60000)SysTime.SysTimeCNT1s = 0;
				
				if((SysTime.SysTimeCNT1min%60) == 0){
					SysTime.SysTimeCNT1h++;
					SysTime.SysTimeFLG1h = 1;
					if(SysTime.SysTimeCNT1min>=60000)SysTime.SysTimeCNT1min = 0;
				}
			}
		}
	}
	
	
}




u16 GetDtTime(u16 timebuf,u16 timecnt){
	if(timebuf > timecnt)
		return (60000 - timebuf + timecnt);
	else
		return (timecnt - timebuf);
}


