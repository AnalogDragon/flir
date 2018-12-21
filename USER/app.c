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

void ChangeMeas(void){
	switch (SysState.DispMeas){
		case None:	SysState.DispMeas = Midd;		break;
		case Midd:	SysState.DispMeas = Exts;		break;
		case Exts:	SysState.DispMeas = None;		break;
		default:		SysState.DispMeas = None;		break;
	}
}

void ChangeColor(u8 Flag){			//0:��ɫ��֮���л� 1:��ɫ���л� 2:�������л�
	if(Flag == 0){
		switch (SysState.ColrMode){ 
			case Iron:	SysState.ColrMode = RB;			break;
			case RB:		SysState.ColrMode = BW;			break;
			case BW:		SysState.ColrMode = Iron;		break;	
			
			case IronMax:	SysState.ColrMode = Iron;			break;	
			case IronMin:	SysState.ColrMode = Iron;			break;
			
			default:			SysState.ColrMode = Iron;		break;
		}
	}
	else if(Flag == 1){
		switch (SysState.ColrMode){ 
			case Iron:	SysState.ColrMode = IronMax;			break;
			case RB:		SysState.ColrMode = IronMax;			break;
			case BW:		SysState.ColrMode = IronMax;		break;	
			
			case IronMax:	SysState.ColrMode = IronMin;			break;	
			case IronMin:	SysState.ColrMode = Iron;			break;
			
			default:			SysState.ColrMode = Iron;		break;
		}
	}else if(Flag == 2){
		switch (SysState.ColrMode){ 
			case Iron:	SysState.ColrMode = RB;			break;
			case RB:		SysState.ColrMode = BW;			break;
			case BW:		SysState.ColrMode = IronMax;		break;	
			
			case IronMax:	SysState.ColrMode = IronMin;			break;	
			case IronMin:	SysState.ColrMode = Iron;			break;
			
			default:			SysState.ColrMode = Iron;		break;
		}
	}else{
		SysState.ColrMode = Iron;
	}
}



void KeyDo(void){
	
	if(SysState.SysFlag.bit.PowerLV){
		
		if(KeyState.Key1.Flag.bit.KeyOut
		||KeyState.Key2.Flag.bit.KeyOut
		||KeyState.Key3.Flag.bit.KeyOut
		||KeyState.Key4.Flag.bit.KeyOut){
			KeyState.Key1.Flag.bit.KeyOut = 0;
			KeyState.Key2.Flag.bit.KeyOut = 0;
			KeyState.Key3.Flag.bit.KeyOut = 0;
			KeyState.Key4.Flag.bit.KeyOut = 0;
			if(SysState.SysFlag.bit.PowerLV > 1 ){
				ReInitLCD();
				SysState.SysFlag.bit.LCDState = 1;
			}
			SysState.WakeTime = SysTime.SysTimeCNT1s;
			SysState.SysFlag.bit.PowerLV = 0;
			if((TIM3->CCR1/10) != SysState.LightLV )TIM3->CCR1 = SysState.LightLV * 10;
			
		}
		return;
	}
	
	
	if(KeyState.Key1.Flag.bit.KeyOut){
		SysState.WakeTime = SysTime.SysTimeCNT1s;
		
		if(SysState.DispStep == Normal){
			if(KeyState.Key1.Flag.bit.KeyOut == 2){		//�̰�
				KeyState.Key1.Flag.bit.KeyOut = 0;
				ChangeColor(0);
			}else if(KeyState.Key1.HoldTime > 40 || KeyState.Key1.Flag.bit.KeyOut == 3){	//����
				KeyState.Key1.Flag.bit.KeyOut = 0;
				ChangeColor(1);
			}
		}else if(SysState.DispStep == Pause){
			KeyState.Key1.Flag.bit.KeyOut = 0;
			SysState.DispStep = Review;
			Draw_BackPlay();
			RecState.PlayNum = read_saved(RecState.PlayNum,0);
			RecState.PlayFlag = 0x80;
		}else if(SysState.DispStep == Review){
			KeyState.Key1.Flag.bit.KeyOut = 0;
			RecState.PlayNum = read_saved(RecState.PlayNum,0);
			RecState.PlayFlag = 0;
		}
		
	}
		

	if(KeyState.Key2.Flag.bit.KeyOut){
		SysState.WakeTime = SysTime.SysTimeCNT1s;
		
		if(SysState.DispStep == Normal){ //����ģʽ����ͼ��ʽ
			KeyState.Key2.Flag.bit.KeyOut = 0;
			ChangeMeas();
		}else{
			if(SysState.DispStep == Pause || RecState.ReSet){
				//֧����ͣ�ؿ��л���ʾ״̬
				if(KeyState.Key2.Flag.bit.KeyOut == 2){		//�̰��л����Ե�
					KeyState.Key2.Flag.bit.KeyOut = 0;
					ChangeMeas();
					SysState.SysFlag.bit.RefreshFlag = 1;
					
				}else if(KeyState.Key2.HoldTime > 40 || KeyState.Key2.Flag.bit.KeyOut == 3){	//�����л���ɫ
					KeyState.Key2.Flag.bit.KeyOut = 0;
					ChangeColor(2);
					SysState.SysFlag.bit.RefreshFlag = 1;
					
				}
				
			}else{		//��֧�����õ�ֱ���������״̬
				KeyState.Key2.Flag.bit.KeyOut = 0;
			}
		}
		RecState.PlayFlag = 0;
		
	}
		
	if(KeyState.Key3.Flag.bit.KeyOut){
		SysState.WakeTime = SysTime.SysTimeCNT1s;
		
		if(SysState.DispStep == Normal){
			KeyState.Key3.Flag.bit.KeyOut = 0;
			SysState.DispStep = Pause;
			RecState.PlayNum = 0;
			SysState.ColrModeBak = SysState.ColrMode;
			SysState.LightLVBak = SysState.LightLV;		//��ͣʱ��¼����
			Lcd_ColorBox(75,5,9,9,White);
		}else{
			if(KeyState.Key3.Flag.bit.KeyOut == 2){  //�̰�
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.DispStep = Normal;
				RecState.PlayNum = 0;
				SysState.ColrMode = SysState.ColrModeBak;
				if(SysState.LightLV != SysState.LightLVBak)
					AT24CXX_WriteOneByte(0x10,SysState.LightLV);				//�˳���ͣʱ��鱣����������
				RecState.PlayFlag = 0;
				Lcd_ColorBox(75,5,9,9,Black);
			}
			if(KeyState.Key3.Flag.bit.KeyOut == 3){		//��������
				KeyState.Key3.Flag.bit.KeyOut = 0;
				SysState.LightLV = (TIM3->CCR1/10);		//����
				RecState.PlayFlag = 0;
			}
			ChangeLight();
		}
		RecState.PlayFlag = 0;
		
	}
		
	if(KeyState.Key4.Flag.bit.KeyOut){
		KeyState.Key4.Flag.bit.KeyOut = 0;
		SysState.WakeTime = SysTime.SysTimeCNT1s;
		
		if(SysState.DispStep == Review){
			RecState.PlayNum = read_saved(RecState.PlayNum,1);
			if(RecState.PlayFlag == 0x80)
				RecState.PlayFlag = 0xf0;
			else
				RecState.PlayFlag = 0;
		}else{
			SysState.SysFlag.bit.SaveFlag = 1;
		}
		
	}
	

}

void PowerDown(void){
	
	if(GetDtTime(SysState.WakeTime,SysTime.SysTimeCNT1s) > 300){
		if(SysState.SysFlag.bit.UsartFlag){
			if(SysState.SysFlag.bit.LCDState){
				SoftResetLCD();
				TIM3->CCR1 = 0;
				SysState.SysFlag.bit.LCDState = 0;
				SysState.SysFlag.bit.PowerLV = 2;
			}
		}
		else{
			SysState.SysFlag.bit.Sleep = 1;
			SysState.SysFlag.bit.PowerLV = 3;
		}
	}
	else if(GetDtTime(SysState.WakeTime,SysTime.SysTimeCNT1s) > 60){
		if(TIM3->CCR1 != 10)TIM3->CCR1 = 10;
		SysState.SysFlag.bit.PowerLV = 1;
	}
	
	SleepMode();
}


typedef  void (*pFunction)(void);
pFunction Jump_To_Application;

void SleepMode(void){
	
	if(SysState.SysFlag.bit.Sleep){
		
		TIM3->CCR1 = 0;
		ShutDownAMG8833();
		SoftResetLCD();
		
		while(1){
			if(!KEY1 || !KEY2 || !KEY3 || !KEY4)
				break;
		}
		
		Jump_To_Application = (pFunction)*(__IO uint32_t*) (APP_ADDR + 4);
		__set_MSP(*(__IO uint32_t*) APP_ADDR);
		Jump_To_Application();
		
		while(1);
	}
}

void DataClean(void){
	
	SysState.DispStep = Normal;
	SysState.ColrMode = Iron;
	SysState.DispMeas = None;
	SysState.WakeTime = SysTime.SysTimeCNT1s;
	KeyState.Key1.Flag.bit.KeyOut = 0;
	KeyState.Key2.Flag.bit.KeyOut = 0;
	KeyState.Key3.Flag.bit.KeyOut = 0;
	KeyState.Key4.Flag.bit.KeyOut = 0;
	SysState.SysFlag.bit.Sleep = 0;
	
}

void GetImg(void){
	if(!SysState.SysFlag.bit.LCDState)return;
	data_push();		//����ת��
	blowup();      //��ֵ
	get_img();      //��ֵת��ΪrgbͼƬ
}


void disp_fast(void){    //����ˢ��
	if(!SysState.SysFlag.bit.LCDState)return;
	Draw_img();       //��ʾͼƬ
	Draw_data();       //��ʾ����
	if(SysState.DispStep == Normal)
		logo_move();       //����ָʾ
}

void disp_slow(void){     //����ˢ��+��������ˢ��
	
 	BatPct = BatPct*0.95+(float)(Get_Battery())*0.05;
	if(BatPct>99)
		BatPct = 100;
	
	if(!SysState.SysFlag.bit.LCDState)return;
	
	Draw_battery((u8)BatPct);   //����
	Draw_menu();    //��ʾ�˵�
	Draw_color();
	Draw_Warning();
	
	if(SysState.DispStep != Normal){
		
		if(SysTime.SysTimeCNT100ms%16>7){			//��˸ͼ��
			if(SysState.DispStep == Pause)
				Lcd_ColorBox(75,5,9,9,White);
			else if(SysState.DispStep == Review)
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
	SysState.ColrMode = SysState.ColrModeBak;
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
		get_img();      //��ֵת��ΪrgbͼƬ
		Draw_img();       //��ʾͼƬ
		logo_move();       //����ָʾ
// 		LED0=~LED0;     //ˢ���ʲ���
		delay_ms(42);
		i++;
	}while(File_Byte>=i && KEY_Scan(1)==0 );
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 3;
}

void PlayVF(void){
	if((RecState.PlayFlag&0xf0) == 0xf0){
		RecState.PlayFlag++;
		if((RecState.PlayFlag&0x0F) == 0x0F){
			RecState.PlayFlag = 0;
			if(Play_BadApple()){
				SysState.DispStep = Normal;
				delay_ms(20);		//��հ���
				KeyState.Key1.Flag.bit.KeyOut = 0;
				KeyState.Key2.Flag.bit.KeyOut = 0;
				KeyState.Key3.Flag.bit.KeyOut = 0;
				KeyState.Key4.Flag.bit.KeyOut = 0;
			}
		}
	}
}

