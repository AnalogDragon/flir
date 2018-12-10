#ifndef __BMP_H
#define __BMP_H	 
#include "sys.h"


void get_bmp_line_x5(u8 row);
void get_bmp_line_x8(u8 row);
void get_Black_line(void);
void get_num_line(u8 x,int num,u8 bl,u8 line);
u8 save_bmp(void);
void GetFileNum(void);
u8 get_data_bmp(u32 file_name); 
u8 read_boot_bmp(void);
u16 read_saved(u16 num,u8 flag);
void SaveIMG(void);



#endif
