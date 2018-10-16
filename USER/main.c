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


long data[59][59]={0};
long ext[3]={0,0,0};
u8 ext_add[2]={0,0};
u16 RDFlag;
extern u8 test_mod;
extern u8 color_mod;
extern u8 Save_Times[2];
extern u16 SaveTimes;


void key_do(void){
	u8 key=KEY_Scan(0);
	u8 i;
	u8 j=0x20;
	u8 buf=0;
	u8 PlayFlag = 0;
	RDFlag = 0;
	if(key==1){                     //更换色彩卡
		switch (color_mod){ 
			case BW:color_mod=Iron;break;
			case Iron:color_mod=RB;break;
			case RB:color_mod=BW;break;
			default:color_mod=Iron;break;
		}
	}
	if(key==2){               //更换测温点模式
		switch (test_mod){
			case none:test_mod=midd;break;
			case midd:test_mod=exts;break;
			case exts:test_mod=none;break;
			default:test_mod=none;break;
		}
	}
	if(key==3){             //暂停画面   
		Lcd_ColorBox(75,5,9,9,White);
		delay_ms(10);
		do{
			i++;
			if(i==255){
				Lcd_ColorBox(75,5,9,9,Black);//闪烁图标
				j++;
				j&=0xEF;
			}
			if(i==127){
				if(PlayFlag != 0)
					Draw_BackPlay();
				else 
					Lcd_ColorBox(75,5,9,9,White);
			}
			if(i%16 == 0)disp_slow();         ///160ms刷新电量
			key=KEY_Scan(0);
			if(j==0xE8){
				j=0;
				key=Play_BadApple();
			}
			if(key==1){
				if((j&0x80)!=0)
					j=0;
				j|=0x80;
				j&=0xF0;
				RDFlag=read_saved(RDFlag,0);
				PlayFlag = 1;
			}
			if(key==2){               //更换测温点模式
				j=0;
				switch (test_mod){
					case none:test_mod=midd;break;
					case midd:test_mod=exts;break;
					case exts:test_mod=none;break;
					default:test_mod=none;break;
				}
				Draw_img();       //显示图片
				Draw_data();       //显示数据
				disp_slow();       //<---
			}
			if(key==4){                 //保存截图
				if(PlayFlag == 0){
					Draw_Wait();
					if(SD_Init()==0){
						save_bmp();
					}
					Draw_Camera();
				}else{
					RDFlag=read_saved(RDFlag,1);
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
		Lcd_ColorBox(75,5,9,9,Black);   //消除暂停图标
		if(AT24CXX_ReadOneByte(0x10) != (TIM3->CCR1/10)) //存储亮度
			AT24CXX_WriteOneByte(0x10,TIM3->CCR1/10);
	}
	if(key==4){                 //保存截图
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
	AT24CXX_Read(0,(u8*)Save_Times,2);  //读取次数
	if(Save_Times[0] == 0xff && Save_Times[1] == 0xff){
		Save_Times[0]=Save_Times[1]=0x00;
		AT24CXX_Write(0,(u8*)Save_Times,2);
	}
	SaveTimes=Save_Times[0]|Save_Times[1]<<8;
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
	
	disp_slow();
	disp_fast();
	//Play_BadApple();         //test
	while(1){
		key_do();
		disp_slow();
		disp_fast();
		if(USART_RX_BUF==0)delay_ms(32);	//串口12ms
		else delay_ms(20);
		if(color_mod==RB)delay_ms(10);
		if(color_mod==BW)delay_ms(15);
	}
}

