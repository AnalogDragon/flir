#include "sys.h"



u8 init_all(void){
	u8 i,state = 0;
	delay_init();	  //延时函数初始化	  
	LED_Init();		  //初始化与LED连接的硬件接口
  KEY_Init();
 	TIM4_Int_Init(1000-1,720-1);
	Lcd_Initialize();
	SysState.SysFlag.bit.LCDState = 1;
	SD_Init();
	state = read_boot_bmp();
	uart_init(115200);
	IIC_Init();
	SysState.LightLV = AT24CXX_ReadOneByte(0x10);
	if(SysState.LightLV > 10 || SysState.LightLV < 1){
		SysState.LightLV = 10;
		AT24CXX_WriteOneByte(0x10,SysState.LightLV);
	}
	TIM3_PWM_Init(100-1,180-1);
	TIM3->CCR1 = SysState.LightLV*10;
	Adc_Init();
	Init_AMG8833();
	get_data();    //获取数据
	if(KEY_Scan(1) == 1)GetFileNum();
	AT24CXX_Read(0,(u8*)RecState.SAVE_NUM,2);  //读取次数
	if(RecState.SAVE_NUM[0] == 0xff && RecState.SAVE_NUM[1] == 0xff){
		RecState.SAVE_NUM[0] = RecState.SAVE_NUM[1]=0x00;
		AT24CXX_Write(0,(u8*)RecState.SAVE_NUM,2);
	}
	RecState.SaveNum = RecState.SAVE_NUM[0]|RecState.SAVE_NUM[1]<<8;
	for(i=0;i<6;i++){
		BatPct = Get_Battery();
	}
	if(state)delay_ms(900);
	BatPct = Get_Battery();
	return 0;
	
}


int main(void){
 	NVIC_SetVectorTable(NVIC_VectTab_FLASH,APP_ADDR&0xFFFFF);
	
	init_all();      //初始化系统
	
	DrawBack();      //绘制背景
	DataClean();
	
	GetImg();
	disp_slow();
	disp_fast();
	
	while(1){
		
		if(SysTime.SysTimeFLG10ms){
			KeyDo();
			if(SysState.SysFlag.bit.RefreshFlag){
				SysState.SysFlag.bit.RefreshFlag = 0;
				GetImg();
				disp_fast();
			}
			SaveIMG();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			if(SysState.DispStep == Normal){
				get_data();    //获取数据
				GetImg();				//转换图像
				disp_fast();		//刷新图像
			}
			disp_slow();
			if(SysState.SysFlag.bit.UsartFlag)	//串口发送
				send_once();
			SysTime.SysTimeFLG100ms = 0;
		}
		
		if(SysTime.SysTimeFLG1s){
			PlayVF();
			PowerDown();
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

