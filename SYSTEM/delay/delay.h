#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"


struct SysTime_REG
{
   u16 SysTimeCNT10ms;
   u16 SysTimeCNT100ms;
   u16 SysTimeCNT1s;
   u16 SysTimeCNT1min;
   u16 SysTimeCNT1h;
	
   u8 SysTimeFLG10ms;
   u8 SysTimeFLG100ms;
   u8 SysTimeFLG1s;
   u8 SysTimeFLG1min;
   u8 SysTimeFLG1h;
};  

extern struct SysTime_REG SysTime;

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

void SysTimeInt(void);
u16 GetDtTime(u16 timebuf,u16 timecnt);

#endif





























