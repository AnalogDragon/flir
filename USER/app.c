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
			if( ((KeyState.Key3.HoldTime-(40+CountBase))/30) < (SysState.LightLV-1) ){		//600ms��һ��
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



void KeyDo(void){
	
	if(KeyState.Key1.Flag.bit.KeyOut){
		KeyState.Key1.Flag.bit.KeyOut = 0;
		
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
			SysState.PlayNum = read_saved(SysState.PlayNum,0);
		}else if(SysState.DispStep == Play){
			SysState.PlayNum = read_saved(SysState.PlayNum,0);
		}
		
	}
		

	if(KeyState.Key2.Flag.bit.KeyOut){
		KeyState.Key2.Flag.bit.KeyOut = 0;
		
		switch (SysState.DispMeas){
			case none:	SysState.DispMeas = midd;		break;
			case midd:	SysState.DispMeas = exts;		break;
			case exts:	SysState.DispMeas = none;		break;
			default:		SysState.DispMeas = none;		break;
		}
		if(SysState.DispStep != Normal)
			SysState.SysFlag.bit.RefreshFlag = 1;
		
	}
		
	if(KeyState.Key3.Flag.bit.KeyOut){
		
		if(SysState.DispStep == Normal){
			KeyState.Key3.Flag.bit.KeyOut = 0;
			SysState.DispStep = Pause;
			SysState.PlayNum = 0;
			SysState.ColrModeBak = SysState.ColrMode;
			SysState.LightLVBak = SysState.LightLV;		//��ͣʱ��¼����
			Lcd_ColorBox(75,5,9,9,White);
		}else{
			if(KeyState.Key3.Flag.bit.KeyOut == 2){  //�̰�
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.DispStep = Normal;
				SysState.PlayNum = 0;
				SysState.ColrMode = SysState.ColrModeBak;
				if(SysState.LightLV != SysState.LightLVBak)
					AT24CXX_WriteOneByte(0x10,SysState.LightLV);				//�˳���ͣʱ��鱣����������
				Lcd_ColorBox(75,5,9,9,Black);
			}
			if(KeyState.Key3.Flag.bit.KeyOut == 3){		//��������
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.LightLV = (TIM3->CCR1/10);		//����
			}
			ChangeLight();
		}
		
	}
		
	if(KeyState.Key4.Flag.bit.KeyOut){
		KeyState.Key4.Flag.bit.KeyOut = 0;
	
		if(SysState.DispStep == Play){
			SysState.PlayNum = read_saved(SysState.PlayNum,1);
		}else{
			SysState.SysFlag.bit.SaveFlag = 1;
		}
		
	}
	

}



void GetImg(void){
	get_data();    //��ȡ����
	blowup();      //��ֵ
	get_img();      //��ֵת��ΪrgbͼƬ
	logo_move();       //����ָʾ
}


void disp_fast(void){    //����ˢ��
	Draw_img();       //��ʾͼƬ
	Draw_data();       //��ʾ����
}

void disp_slow(void){     //����ˢ��+��������ˢ��
	
 	BatPct = BatPct*0.95+(float)(Get_Battery())*0.05;
	if(BatPct>99)
		BatPct = 100;
	
	Draw_battery((u8)BatPct);   //����
	Draw_menu();    //��ʾ�˵�
	Draw_color();
	Draw_Warning();
	
	if(SysState.DispStep != Normal){
		
		if(SysTime.SysTimeCNT100ms%16>7){			//��˸ͼ��
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
	u16 i=1,j,l;	
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
	do{
		disp_slow();
		for(j=0;j<PixLg;j++){
			f_lseek(&fdst_f,(i*PixLg*PixLg+j*PixLg));
			res_f = f_read(&fdst_f, RW_Buf, PixLg, &bw_f);
			for(l=0;l<PixLg;l++){
				data[(PixLg-1)-j][(PixLg-1)-l]=RW_Buf[l];
			}
		}
		get_img();      //��ֵת��ΪrgbͼƬ
		Draw_img();       //��ʾͼƬ
		logo_move();       //����ָʾ
// 		LED0=~LED0;     //ˢ���ʲ���
		delay_ms(42);
		i++;
	}while(File_Byte>=i || KEY_Scan(1) );
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 3;
}







