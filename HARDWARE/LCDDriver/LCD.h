#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include "sys.h"

/* 选择BANK1-BORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 16 bit => FSMC[24:0]对应HADDR[25:1]
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 当选择不同的地址线时，地址要重新计算。
 */
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define Set_Rst GPIOD->BSRR = GPIO_Pin_13;
#define Clr_Rst GPIOD->BRR  = GPIO_Pin_13;

#define Lcd_Light_ON   GPIOA->BSRR = GPIO_Pin_6;
#define Lcd_Light_OFF  GPIOA->BRR  = GPIO_Pin_6;


//Lcd初始化及其低级控制函数
void Lcd_Configuration(void);
void DataToWrite(u16 data);
void LCD_Rst(void);
void Lcd_Initialize(void);
void WriteInitCMD(void);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void Lcd_WR_Start(void);
//Lcd高级控制函数
void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);
void DrawPixel(u16 x, u16 y, u16 Color);
u16 ssd1289_GetPoint(u16 x,u8 y);
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
char Tiky_Button(char *filename,u16 x,u16 y);
void DrawBack(void);
u16 To_HSB(u8 num);
void logo_move(void);
void Draw_battery(u8 num);
void blowup(void) ;
void get_img(void);
void Draw_img(void);
void Draw_A_num(u16 x, u16 y,u8 size,u8 back,u8 num);
void Draw_data(void);
void Draw_menu(void);
void Draw_color(void);
void Draw_Camera(void);
void Draw_Wait(void);
u16 ReadPixel(u16 x,u8 y);
void disp_open(void);
void LCD_Pic2(u16 x,u8 y,u8 leg, u8* pic);
void Draw_BackPlay(void);
void Draw_Warning(void);

void SoftResetLCD(void);
void ReInitLCD(void);

#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

// RRRRR GGGGG+G BBBBB//







#endif


