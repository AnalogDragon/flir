#include "myiic.h"
 
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB10,PB11 输出高
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


u8 Init_AMG8833(void){
	IIC_Start();
	IIC_Send_Byte(0xD2);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x00);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x00);
	if(IIC_Wait_Ack())return 0;
	IIC_Stop();
	delay_ms(10);
	IIC_Start();
	IIC_Send_Byte(0xD2);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x01);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x3f);
	if(IIC_Wait_Ack())return 0;
	IIC_Stop();
	delay_ms(10);
	IIC_Start();
	IIC_Send_Byte(0xD2);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x02);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x00);
	if(IIC_Wait_Ack())return 0;
	IIC_Stop();

	delay_ms(50);
	get_data();
	return 1;
}



u8 ShutDownAMG8833(void){
	IIC_Start();
	IIC_Send_Byte(0xD2);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x00);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x10);
	if(IIC_Wait_Ack())return 0;
	IIC_Stop();
	return 1;
}


void data_push(void){
	u8 i;
	ext[0]=0;
	ext[1]=0x7fff;
	
	for(i=0;i<64;i++){
			
		if(PriData[i/8][i%8]>ext[0]){	//遍历最值
			ext[0]=PriData[i/8][i%8];
			ext_add[0]=i;
		}
		if(PriData[i/8][i%8]<ext[1]){
			ext[1]=PriData[i/8][i%8];
			ext_add[1]=i;
		}
	
#if (Size == SIZEx5)
		data[PixLg-1-(i / 8 * PixGain + 2)][i % 8 * PixGain + 2] = PriData[i/8][i%8];//数据填充
#elif (Size == SIZEx8)
		data[PixLg-1-(i / 8 * PixGain + 1)][i % 8 * PixGain + 1] = PriData[i/8][i%8];
#endif
	
	}
}


u8 get_data(void) {
	int i;
	long buf;
	
	IIC_Start();
	IIC_Send_Byte(0xD2);
	if(IIC_Wait_Ack())return 0;
	IIC_Send_Byte(0x80);
	if(IIC_Wait_Ack())return 0;
	IIC_Start();
	IIC_Send_Byte(0xD3);
	if(IIC_Wait_Ack())return 0;
	
	for (i = 0; i < 63; i++) {
		
		buf=IIC_Read_Byte(1)&0xff;
		buf|=(0xff&IIC_Read_Byte(1))<<8;
		
 		if((buf&0x800)==0x800){
			buf=~buf;
			buf&=0xFFF;
			buf++;
			buf=-buf;
		}
		else if(buf>0x200){
			buf=0x200;
		}
		
		PriData[i/8][i%8] = buf;
	}
	
	buf=IIC_Read_Byte(1)&0xff;
	buf|=(0xff&IIC_Read_Byte(0))<<8;
	
	if((buf&0x800)==0x800){
		buf=~buf;
		buf&=0xFFF;
		buf++;
		buf=-buf;
	}
	else if(buf>0x200){
		buf=0x200;
	}
	PriData[i/8][i%8] = buf;
	IIC_Stop();
	
	return 1;
	
}




//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
 




