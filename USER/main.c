#include "led.h"
#include "delay.h"
#include "sys.h"
#include "LCD.h"
#include "usart.h"
#include "myiic.h"
#include "timer.h"
#include "adc.h"
#include "key.h"
#include "bmp.h"

#define APP_ADDR  0x08004000


long data[PixLg][PixLg]={0};
long ext[3]={0,0,0};
u8 ext_add[2]={0,0};

extern u8 SAVE_NUM[2];

struct SysState_REG SysState;


void key_do(void){
	u8 key=KEY_Scan(0);
	u8 i;
	u8 j=0x20;
	u8 buf=0;
	u8 PlayFlag = 0;
	SysState.ColrModeBak = SysState.ColrMode;
	SysState.PlayNum = 0;
	if(key==1){                     //����ɫ�ʿ�
		switch (SysState.ColrMode){ 
			case BW:SysState.ColrMode=Iron;break;
			case Iron:SysState.ColrMode=RB;break;
			case RB:SysState.ColrMode=BW;break;
			default:SysState.ColrMode=Iron;break;
		}
		SysState.ColrModeBak = SysState.ColrMode;
	}
	if(key==2){               //�������µ�ģʽ
		switch (SysState.DispMeas){
			case none:SysState.DispMeas=midd;break;
			case midd:SysState.DispMeas=exts;break;
			case exts:SysState.DispMeas=none;break;
			default:SysState.DispMeas=none;break;
		}
	}
	if(key==3){             //��ͣ����   
		Lcd_ColorBox(75,5,9,9,White);
		delay_ms(10);
		do{
			i++;
			if(i==255){
				Lcd_ColorBox(75,5,9,9,Black);//��˸ͼ��
				j++;
				j&=0xEF;
			}
			if(i==127){
				if(PlayFlag != 0)
					Draw_BackPlay();
				else 
					Lcd_ColorBox(75,5,9,9,White);
			}
			if(i%16 == 0)disp_slow();         ///160msˢ�µ���
			key=KEY_Scan(0);
			if(j==0xE8){
				j=0;
				SysState.ColrMode = SysState.ColrModeBak;
				key=Play_BadApple();
			}
			if(key==1){
				if((j&0x80)!=0)
					j=0;
				j|=0x80;
				j&=0xF0;
				SysState.PlayNum=read_saved(SysState.PlayNum,0);
				PlayFlag = 1;
			}
			if(key==2){               //�������µ�ģʽ
				j=0;
				switch (SysState.DispMeas){
					case none:SysState.DispMeas=midd;break;
					case midd:SysState.DispMeas=exts;break;
					case exts:SysState.DispMeas=none;break;
					default:SysState.DispMeas=none;break;
				}
				Draw_img();       //��ʾͼƬ
				Draw_data();       //��ʾ����
				disp_slow();       //<---
			}
			if(key==4){                 //�����ͼ
				if(PlayFlag == 0){
					Draw_Wait();
					if(SD_Init()==0){
						save_bmp();
					}
					Draw_Camera();
				}else{
					SysState.PlayNum=read_saved(SysState.PlayNum,1);
					if((j&0x80) == 0x80){
						if((j&0x40)!=0)
							j=0;
						j|=0x40;
						j&=0xF0;
					}
				}
			}if(key==3){
				buf=0;
				while(KEY_Scan(1) == 3){
					delay_ms(1);
					buf++;
					if(buf>50){
				    key=0;
						buf=0;
						TIM3->CCR1-=10;
						if(TIM3->CCR1 == 0)TIM3->CCR1=100;
						if(TIM3->CCR1 == 10)break;
					}
				}
			}
			delay_ms(5);
		}while(key==0 || key==1 || key==2 || key==4);
		Lcd_ColorBox(75,5,9,9,Black);   //������ͣͼ��
		if(AT24CXX_ReadOneByte(0x10) != (TIM3->CCR1/10)) //�洢����
			AT24CXX_WriteOneByte(0x10,TIM3->CCR1/10);
	}
	if(key==4){                 //�����ͼ
		Draw_Wait();
		if(SD_Init()==0){
			save_bmp();
		}
		Draw_Camera();
	}
	SysState.ColrMode = SysState.ColrModeBak;
}



void SaveIMG(void){
	
	if(SysState.SaveFlag){
		SysState.SaveFlag = 0;
		Draw_Wait();
		if(SD_Init()==0){
			save_bmp();
		}
		Draw_Camera();
	}
	
}


u8 init_all(void){
	u8 state=0;
	u8 buf;
	delay_init();	  //��ʱ������ʼ��	  
	TIM4_Int_Init(1000-1,720-1);
	LED_Init();		  //��ʼ����LED���ӵ�Ӳ���ӿ�
	Lcd_Initialize();
	SD_Init();
	state=read_boot_bmp();
	uart_init(115200);
	IIC_Init();
	buf=AT24CXX_ReadOneByte(0x10);
	if(buf>10 || buf<1){
		buf=10;
		AT24CXX_WriteOneByte(0x10,buf);
	}
	TIM3_PWM_Init(100-1,180-1);
	TIM3->CCR1=buf*10;
	Adc_Init();
	Init_AMG8833();
	get_data();    //��ȡ����
  KEY_Init();
	if(KEY_Scan(0) == 1)GetFileNum();
	AT24CXX_Read(0,(u8*)SAVE_NUM,2);  //��ȡ����
	if(SAVE_NUM[0] == 0xff && SAVE_NUM[1] == 0xff){
		SAVE_NUM[0]=SAVE_NUM[1]=0x00;
		AT24CXX_Write(0,(u8*)SAVE_NUM,2);
	}
	SysState.SaveNum = SAVE_NUM[0]|SAVE_NUM[1]<<8;
	for(buf=0;buf<6;buf++){
		BatPct=Get_Battery();
	}
	if(state)delay_ms(900);
	BatPct=Get_Battery();
	return 0;
	
}


int main(void){	
 	NVIC_SetVectorTable(NVIC_VectTab_FLASH,APP_ADDR&0xFFFFF);
	init_all();      //��ʼ��ϵͳ
	DrawBack();      //���Ʊ���
	
	SysState.DispStep = Normal;
	SysState.ColrMode = Iron;
	SysState.DispMeas = none;
	
	GetImg();
	disp_slow();
	disp_fast();
	
	while(1){
		
		if(SysTime.SysTimeFLG10ms){
			key_do();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			GetImg();
			disp_fast();
			disp_slow();
			SysTime.SysTimeFLG100ms = 0;
		}
		
		if(SysTime.SysTimeFLG1s){
			SysTime.SysTimeFLG1s = 0;
		}
		
		if(SysTime.SysTimeFLG1min){
			SysTime.SysTimeFLG1min = 0;
		}
		
		if(SysTime.SysTimeFLG1h){
			SysTime.SysTimeFLG1h = 0;
		}
		
	}
	
	
	
}

