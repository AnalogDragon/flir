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


long data[40][40]={0};
long ext[3]={0,0,0};
u8 ext_add[2]={0,0};
extern u8 test_mod;
extern u8 color_mod;
extern u8 Save_Times[4];
extern u32 SaveTimes;

void disp_fast(void){    //快速刷新
	get_data();    //获取数据
	blowup();      //插值
	get_img();      //插值转换为rgb图片
	Draw_img();       //显示图片
	Draw_data();       //显示数据
	logo_move();       //运行指示
	LED0=~LED0;     //刷新率测试
}

void disp_slow(void){     //慢速刷新+按键操作刷新
	Draw_battery(Get_Battery());   //电量
	Draw_menu();    //显示菜单
	Draw_color();
}


void key_do(void){
	u8 key=KEY_Scan(0);
	u8 i;
	u8 j=0;
	u8 buf=0;
	if(key==1){                     //更换色彩卡
		switch (color_mod){ 
			case BW:color_mod=Iron;break;
			case Iron:color_mod=RB;break;
			case RB:color_mod=BW;break;
			default:break;
		}
	}
	if(key==2){               //更换测温点模式
		switch (test_mod){
			case none:test_mod=midd;break;
			case midd:test_mod=exts;break;
			case exts:test_mod=none;break;
			default:break;
		}
	}
	if(key==3){             //暂停画面   
		Lcd_ColorBox(75,5,9,9,White);
		delay_ms(10);
		do{
			i++;
			if(i==255){
				Lcd_ColorBox(75,5,9,9,Black);
				j++;
				j&=0x8f;
			}
			if(i==127)Lcd_ColorBox(75,5,9,9,White);
			key=KEY_Scan(0);
			if(j==0x88){
				j=0;
				key=Play_BadApple();
			}
			if(key==1)j=0x80;
			if(key==2){               //更换测温点模式
				switch (test_mod){
					case none:test_mod=midd;break;
					case midd:test_mod=exts;break;
					case exts:test_mod=none;break;
					default:break;
				}
				Draw_img();       //显示图片
				Draw_data();       //显示数据
				Draw_battery(Get_Battery());   //电量
				Draw_menu();    //显示菜单
				Draw_color();
			}
			if(key==4){                 //保存截图
				Draw_Wait();
				if(SD_Init()==0){
					save_bmp();
				}
				Draw_Camera();
			}if(key==3){
				buf=0;
				while(KEY_Scan(1)==3){
					delay_ms(1);
					buf++;
					if(buf>50){
				    key=0;
						buf=0;
						TIM3->CCR1-=10;
						if(TIM3->CCR1==0)TIM3->CCR1=100;
						if(TIM3->CCR1==10)break;
					}
				}
			}
			delay_ms(5);
		}while(key==0 || key==1 || key==2 || key==4);
		Lcd_ColorBox(75,5,9,9,Black);
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
	delay_init();	    	 //延时函数初始化	  
	LED_Init();		  	//初始化与LED连接的硬件接口
	Lcd_Initialize();
	SD_Init();
	state=read_boot_bmp();
	uart_init(115200);
	TIM3_PWM_Init(100-1,180-1);
	TIM3->CCR1=100;
	Adc_Init();
	IIC_Init();
	Init_AMG8833();
	get_data();    //获取数据
  KEY_Init();
	AT24CXX_Read(0,(u8*)Save_Times,4);  //读取次数
	if(Save_Times[0] == 0xff && Save_Times[1] == 0xff && Save_Times[2] == 0xff && Save_Times[3] == 0xff){
		Save_Times[0]=Save_Times[1]=Save_Times[2]=Save_Times[3]=0x00;
		AT24CXX_Write(0,(u8*)Save_Times,4);
	}
	SaveTimes=Save_Times[0]|Save_Times[1]<<16;
	if(state)delay_ms(1000);
	else delay_ms(100);
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
		if(USART_RX_BUF==0)delay_ms(60);
		else delay_ms(48);
		if(color_mod==RB)delay_ms(5);
		if(color_mod==BW)delay_ms(6);
	}
}

