#include "key.h"

								    

void KEY_Init(void){
 	GPIO_InitTypeDef GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



//mode:0,不支持连续按;1,支持连续按;
u8 KEY_Scan(u8 mode){	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(5);//去抖动 
		key_up=0;
		if(KEY1==0)return 1;
		else if(KEY2==0)return 2;
		else if(KEY3==0)return 3;
		else if(KEY4==0)return 4;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	return 0;// 无按键按下
}


u8 Battery_Scan(void){
	if(CHRG==0)return 1;
	if(STBY==0)return 2;
	return 0;
}



void GetAKey(struct AKeyState_REG *KeyBuf,u8 IOState){
	
	u16 TimeBuf = SysTime.SysTimeCNT10ms;
	
	KeyBuf->Flag.bit.IOLast = KeyBuf->Flag.bit.IOActu;		//转移上次状态
	KeyBuf->Flag.bit.IOActu = IOState & 1;			//记录本次状态
	
	if(KeyBuf->Flag.bit.KeyErr)	return;  //按键报错
	
	if(KeyBuf->Flag.bit.IOActu){		//本次有按下
		
		if(KeyBuf->Flag.bit.IOLast){	//上次按下
			
			if(KeyBuf->Flag.bit.IOFilt == 0)
				KeyBuf->Flag.bit.KeyOut = 1;		//按键单次按下有效

			KeyBuf->Flag.bit.IOFilt = 1;		//滤波IO
		}
		
		KeyBuf->HoldTime = GetDtTime(KeyBuf->PressTime,TimeBuf);
		
		if(KeyBuf->HoldTime > 3000){	//30s持续，报错
			KeyBuf->Flag.all = 0;
			KeyBuf->Flag.bit.KeyErr = 1;
		}
		
	}else{
		
		KeyBuf->PressTime = TimeBuf;					//记录未按下的时间
		
		if(KeyBuf->Flag.bit.IOLast){		//首次弹起,可能是抖动，
			;
		}else{
			
			if(KeyBuf->Flag.bit.KeyOut == 1){			//滤波值有效
				if(KeyBuf->HoldTime < 40)
					KeyBuf->Flag.bit.KeyOut = 2;		//400ms以内判断为短按
				else
					KeyBuf->Flag.bit.KeyOut = 3;
			}
			
			KeyBuf->Flag.bit.IOFilt = 0;		//滤波IO
			KeyBuf->HoldTime = 0;	//清空时间
			
		}
	}
}




void GetKey(void){
	
	GetAKey(&KeyState.Key1,1&(~KEY1));
	GetAKey(&KeyState.Key2,1&(~KEY2));
	GetAKey(&KeyState.Key3,1&(~KEY3));
	GetAKey(&KeyState.Key4,1&(~KEY4));
	
}



