#include "app.h"


void ChangeLight(void){
	static u8 CountBase;
	
	if(KeyState.Key3.HoldTime > 40){
		if(CountBase % 10){
			CountBase = CountBase/10*10;
			TIM3->CCR1 = 100;
			SysState.LightLV = 10;
		}
		if(KeyState.Key3.HoldTime > (40+CountBase)){
			if( ((KeyState.Key3.HoldTime-(40+CountBase))/30) < (SysState.LightLV-1) ){		//600ms跳一下
				TIM3->CCR1 = 10*( (SysState.LightLV-1) - ((KeyState.Key3.HoldTime-(40+CountBase))/30) );
			}else{
				TIM3->CCR1 = 10;
			}
		}

	}else{
		SysState.LightLV = TIM3->CCR1/10;
		if(SysState.LightLV == 1)
			CountBase = 41;
		else
			CountBase = 0;
	}
}

void ChangeMeas(void){
	switch (SysState.DispMeas){
		case none:	SysState.DispMeas = midd;		break;
		case midd:	SysState.DispMeas = exts;		break;
		case exts:	SysState.DispMeas = none;		break;
		default:		SysState.DispMeas = none;		break;
	}
}

void ChangeStep(void){			
	switch (SysState.ColrMode){ 
		case BW:		SysState.ColrMode = Iron;		break;
		case Iron:	SysState.ColrMode = RB;			break;
		case RB:		SysState.ColrMode = BW;			break;
		default:		SysState.ColrMode = Iron;		break;
	}
}



void KeyDo(void){
	
	if(KeyState.Key1.Flag.bit.KeyOut){
		KeyState.Key1.Flag.bit.KeyOut = 0;
		
		if(SysState.DispStep == Normal){
			ChangeStep();
		}else if(SysState.DispStep == Pause){
			SysState.DispStep = Play;
			Draw_BackPlay();
			RecState.PlayNum = read_saved(RecState.PlayNum,0);
		}else if(SysState.DispStep == Play){
			RecState.PlayNum = read_saved(RecState.PlayNum,0);
		}
		
	}
		

	if(KeyState.Key2.Flag.bit.KeyOut){
		
		if(SysState.DispStep == Normal){ //正常模式和新图格式
			KeyState.Key2.Flag.bit.KeyOut = 0;
			ChangeMeas();
		}else{
			if(SysState.DispStep == Pause || RecState.ReSet){
				//支持暂停回看切换显示状态
				if(KeyState.Key2.Flag.bit.KeyOut == 2){		//短按切换测试点
					KeyState.Key2.Flag.bit.KeyOut = 0;
					ChangeMeas();
					SysState.SysFlag.bit.RefreshFlag = 1;
					
				}else if(KeyState.Key2.HoldTime > 40 || KeyState.Key2.Flag.bit.KeyOut == 3){	//长按切换颜色
					KeyState.Key2.Flag.bit.KeyOut = 0;
					ChangeStep();
					SysState.SysFlag.bit.RefreshFlag = 1;
					
				}
				
			}else{		//不支持设置的直接清除按键状态
				KeyState.Key2.Flag.bit.KeyOut = 0;
			}
		}
		
	}
		
	if(KeyState.Key3.Flag.bit.KeyOut){
		
		if(SysState.DispStep == Normal){
			KeyState.Key3.Flag.bit.KeyOut = 0;
			SysState.DispStep = Pause;
			RecState.PlayNum = 0;
			SysState.ColrModeBak = SysState.ColrMode;
			SysState.LightLVBak = SysState.LightLV;		//暂停时记录亮度
			Lcd_ColorBox(75,5,9,9,White);
		}else{
			if(KeyState.Key3.Flag.bit.KeyOut == 2){  //短按
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.DispStep = Normal;
				RecState.PlayNum = 0;
				SysState.ColrMode = SysState.ColrModeBak;
				if(SysState.LightLV != SysState.LightLVBak)
					AT24CXX_WriteOneByte(0x10,SysState.LightLV);				//退出暂停时检查保存亮度设置
				Lcd_ColorBox(75,5,9,9,Black);
			}
			if(KeyState.Key3.Flag.bit.KeyOut == 3){		//长按弹起
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.LightLV = (TIM3->CCR1/10);		//亮度
			}
			ChangeLight();
		}
		
	}
		
	if(KeyState.Key4.Flag.bit.KeyOut){
		KeyState.Key4.Flag.bit.KeyOut = 0;
	
		if(SysState.DispStep == Play){
			RecState.PlayNum = read_saved(RecState.PlayNum,1);
		}else{
			SysState.SysFlag.bit.SaveFlag = 1;
		}
		
	}
	

}



void GetImg(void){
	data_push();		//数据转移
	blowup();      //插值
	get_img();      //插值转换为rgb图片
	logo_move();       //运行指示
}


void disp_fast(void){    //快速刷新
	Draw_img();       //显示图片
	Draw_data();       //显示数据
}

void disp_slow(void){     //慢速刷新+按键操作刷新
	
 	BatPct = BatPct*0.95+(float)(Get_Battery())*0.05;
	if(BatPct>99)
		BatPct = 100;
	
	Draw_battery((u8)BatPct);   //电量
	Draw_menu();    //显示菜单
	Draw_color();
	Draw_Warning();
	
	if(SysState.DispStep != Normal){
		
		if(SysTime.SysTimeCNT100ms%16>7){			//闪烁图标
			if(SysState.DispStep == Pause)
				Lcd_ColorBox(75,5,9,9,White);
			else if(SysState.DispStep == Play)
				Draw_BackPlay();
		}
		else{
			Lcd_ColorBox(75,5,9,9,Black);
		}
	}
}



u8 Play_BadApple(void){
	u16 i=1,j,l,flag;	
	UINT bw_f; 
  u32 File_Byte;
	
	if(f_mount(&fs,"0:",1)!=FR_OK)return 0;
	if(f_opendir(&dr, "0:/sys")!=FR_OK)return 0;
	res_f = f_open(&fdst_f, "0:/sys/PlayFile.bin", FA_OPEN_EXISTING | FA_READ  );
	if( res_f != FR_OK )return 0;
	File_Byte = fdst_f.fsize;
  File_Byte/=PixLg*PixLg;
	ext[0]=932;
	ext[1]=932;
	Draw_data();
	ext[0]=0xff;
	ext[1]=0;
	do{		//12fps
		disp_slow();
		for(j=0;j<PixLg;j++){
			f_lseek(&fdst_f,(i*PixLg*PixLg+j*PixLg));
			res_f = f_read(&fdst_f, RW_Buf, PixLg, &bw_f);
			for(l=0;l<PixLg;l++){
				data[(PixLg-1)-j][(PixLg-1)-l]=RW_Buf[l];
			}
		}
		get_img();      //插值转换为rgb图片
		Draw_img();       //显示图片
		logo_move();       //运行指示
// 		LED0=~LED0;     //刷新率测试
		delay_ms(42);
		i++;
	}while(File_Byte>=i || KEY_Scan(1)==0 );
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 3;
}







