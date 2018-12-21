#include "VarDefine.h"


FATFS fs;
DIR dr;
FIL fdst_f;
FRESULT res_f;

u16 PriData[8][8] = {0};
long data[PixLg][PixLg]={0};
long ext[3]={0,0,0};
u8 ext_add[2]={0,0};

char name_buf[64];
char dir_buf[64];

u8 RW_Buf[600] ={0};

float BatPct = 0;

struct SysState_REG SysState;
struct SysTime_REG SysTime;
struct KeyState_REG KeyState;
struct RecState_REG RecState;






