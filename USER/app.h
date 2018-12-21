#ifndef _APP_H_
#define _APP_H_
#include "sys.h"


void ChangeLight(void);
void ChangeMeas(void);
void ChangeColor(u8 Flag);
void KeyDo(void);
void PowerDown(void);
void SleepMode(void);

void GetImg(void);
void disp_fast(void);
void disp_slow(void);

u8 Play_BadApple(void);
void PlayVF(void);
void DataClean(void);

#endif


