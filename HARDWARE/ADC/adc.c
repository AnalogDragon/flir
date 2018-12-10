 #include "adc.h"
														   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}


/*������ȡ�����0-100*/
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

float BatPct = 0;


