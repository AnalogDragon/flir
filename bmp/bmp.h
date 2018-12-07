#ifndef __BMP_H
#define __BMP_H	 
#include "sys.h"
#include "myiic.h"
#include "delay.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "diskio.h"
#include "malloc.h"




// static FRESULT FAT_SAVABuff( char *path , u8 *DataBuff ,u16 DataLength );
//static FRESULT FAT_SAVABuff1( char *path ,u8 *DataBuff ,u16 DataLength );
void get_bmp_line_x5(u8 row);
void get_bmp_line_x8(u8 row);
void get_Black_line(void);
void get_num_line(u8 x,int num,u8 bl,u8 line);
u8 save_bmp(void);
void GetFileNum(void);
u8 Play_BadApple(void);
u8 get_data_bmp(u32 file_name); 
u8 read_boot_bmp(void);
u16 read_saved(u16 num,u8 flag);




#endif
