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
	
	
	switch(KEY_Scan(0)){
		
		case 1:
			if(SysState.DispStep == Normal){
				switch (SysState.ColrMode){ 
					case BW:		SysState.ColrMode = Iron;		break;
					case Iron:	SysState.ColrMode = RB;			break;
					case RB:		SysState.ColrMode = BW;			break;
					default:		SysState.ColrMode = Iron;		break;
				}
			}else if(SysState.DispStep == Pause){
				SysState.DispStep = Play;
				Draw_BackPlay();
				SysState.PlayNum=read_saved(SysState.PlayNum,0);
			}else if(SysState.DispStep == Play){
				SysState.PlayNum=read_saved(SysState.PlayNum,0);
			}
			break;
		
		case 2:
			switch (SysState.DispMeas){
				case none:	SysState.DispMeas = midd;		break;
				case midd:	SysState.DispMeas = exts;		break;
				case exts:	SysState.DispMeas = none;		break;
				default:		SysState.DispMeas = none;		break;
			}
			if(SysState.DispStep != Normal)
				SysState.SysFlag.bit.RefreshFlag = 1;
			break;
		
		case 3:
			if(SysState.DispStep == Normal){
				SysState.DispStep = Pause;
				SysState.ColrModeBak = SysState.ColrMode;
				SysState.PlayNum = 0;
				Lcd_ColorBox(75,5,9,9,White);
			}else{
				SysState.DispStep = Normal;
				SysState.ColrMode = SysState.ColrModeBak;
				SysState.PlayNum = 0;
				Lcd_ColorBox(75,5,9,9,Black);
			}
			break;
		
		case 4:
			if(SysState.DispStep == Play){
				SysState.PlayNum=read_saved(SysState.PlayNum,1);
			}else{
				SysState.SysFlag.bit.SaveFlag = 1;
			}
			break;
		
	}

}



void SaveIMG(void){
	
	if(SysState.SysFlag.bit.SaveFlag){
		SysState.SysFlag.bit.SaveFlag = 0;
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
	delay_init();	  //延时函数初始化	  
	TIM4_Int_Init(1000-1,720-1);
	LED_Init();		  //初始化与LED连接的硬件接口
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
	get_data();    //获取数据
  KEY_Init();
	if(KEY_Scan(0) == 1)GetFileNum();
	AT24CXX_Read(0,(u8*)SAVE_NUM,2);  //读取次数
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
	init_all();      //初始化系统
	DrawBack();      //绘制背景
	
	SysState.DispStep = Normal;
	SysState.ColrMode = Iron;
	SysState.DispMeas = none;
	
	GetImg();
	disp_slow();
	disp_fast();
	
	while(1){
		
		if(SysTime.SysTimeFLG10ms){
			key_do();
			SaveIMG();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			if(SysState.DispStep == Normal){
				GetImg();
				disp_fast();
			}
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

