 #include "adc.h"
														   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


/*电量获取，输出0-100*/
u8 Get_Battery(void){  
	
	u32 temp_val=0;
	u8 t;
	
	static u16 temp_val_back0 = 0;
	static u16 temp_val_back1 = 0;
	static u16 temp_val_back2 = 0;
	static u8 CntTimer = 0;
	static u8 Battery_back = 0;
	
	if(Battery_Scan() != Battery_back){
		CntTimer++;
		if(CntTimer >= 10){
			CntTimer = 0;
			Battery_back = Battery_Scan();
		}else{
			if(temp_val_back2 != 0){
				temp_val_back1 = temp_val_back2;
				temp_val_back0 = temp_val_back2;
				return temp_val_back2;
			}
		}
	}else{
		CntTimer = 0;
	}
	
	for(t=0;t<8;t++){
		temp_val+=Get_Adc(ADC_Channel_2);
		delay_ms(2);
	}
	temp_val/=8;
	if(temp_val<=2234)
		return 1;
	else{
		temp_val=(temp_val-2234)*100/350;
		if(temp_val == 0){
			temp_val = 1;
		}
	}
	
	if(Battery_Scan() == 1){
		temp_val=temp_val*temp_val/100;
	}
	
	temp_val_back2 = temp_val_back1;
	temp_val_back1 = temp_val_back0;
	temp_val_back0 = (u16)temp_val;
	
	if((u8)temp_val_back2>90)return 100;
	return (u8)temp_val_back2;
}




