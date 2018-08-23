#include "bmp.h"
#include "myiic.h"
#include "delay.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "diskio.h"
#include "malloc.h"
#include "bmpdata.h"
#include "stdio.h"	
#include "LCD.h"	
#include "led.h"	
#include "key.h"	


FATFS fs;
DIR dr;

extern long data[40][40];
extern long ext[3];
extern u8 ext_add[2];
extern u8 test_mod;
extern u8 color_mod;
u8 Save_Times[4]={0,0,0,0};
u32 SaveTimes=0;
char name_buf[35];
char dir_buf[12];

u8 Image_line[360] ={0};


/*
static FRESULT FAT_SAVABuff1( char *path ,u8 *DataBuff ,u16 DataLength ){
	FIL fdst_f;
	FRESULT res_f;
	UINT bw_f;  
	int File_Byte;
	res_f = f_open(&fdst_f, path, FA_OPEN_EXISTING | FA_READ | FA_WRITE );
	if( res_f == FR_OK ){
		File_Byte = fdst_f.fsize;
		f_lseek(&fdst_f,File_Byte);
		res_f = f_write(&fdst_f, DataBuff, DataLength, &bw_f); 
		f_close(&fdst_f); 
	}else{
		File_Byte = 0;
		res_f = f_open( &fdst_f, path , FA_CREATE_NEW | FA_WRITE );
		if( res_f == FR_OK ){
			res_f = f_write(&fdst_f, DataBuff, DataLength, &bw_f); 
			f_close(&fdst_f); 
		}
	}
	return res_f;
}
*/
void get_bmp_line(u8 row){
	u8 i;
	for(i=0;i<40;i++){
		Image_line[i*9]=Image_line[i*9+3]=Image_line[i*9+6]=(u8)((((data[39-row][39-i]>>0)&0xff)<<3)&0xff);
		Image_line[i*9+1]=Image_line[i*9+1+3]=Image_line[i*9+1+6]=(u8)((((data[39-row][39-i]>>6)&0xff)<<3)&0xff);
		Image_line[i*9+2]=Image_line[i*9+2+3]=Image_line[i*9+2+6]=(u8)((((data[39-row][39-i]>>11)&0xff)<<3)&0xff);
	}
	if(test_mod==midd){
		if(row==20){
			Image_line[20*9]=Image_line[20*9+3]=Image_line[20*9+6]=0;
			Image_line[20*9+1]=Image_line[20*9+1+3]=Image_line[20*9+1+6]=0;
			Image_line[20*9+2]=Image_line[20*9+2+3]=Image_line[20*9+2+6]=0;
		}
	}else if(test_mod==exts){
		if(ext_add[0]/8*5+2==row){
			i=(7-(ext_add[0]%8))*5+2;
			Image_line[i*9]=Image_line[i*9+3]=Image_line[i*9+6]=0;
			Image_line[i*9+1]=Image_line[i*9+1+3]=Image_line[i*9+1+6]=0;
			Image_line[i*9+2]=Image_line[i*9+2+3]=Image_line[i*9+2+6]=0;
		}
		if(ext_add[1]/8*5+2==row){
			i=(7-(ext_add[1]%8))*5+2;
			Image_line[i*9]=Image_line[i*9+3]=Image_line[i*9+6]=0xff;
			Image_line[i*9+1]=Image_line[i*9+1+3]=Image_line[i*9+1+6]=0xff;
			Image_line[i*9+2]=Image_line[i*9+2+3]=Image_line[i*9+2+6]=0xff;
		}
	}
}	

void get_Black_line(void){
	u8 i;
	for(i=0;i<120;i++){
		Image_line[i*3]=Image_line[i*3+1]=Image_line[i*3+2]=0;
	}
}	


void get_num_line(u8 x,int num,u8 bl,u8 line){
	u8 i;
// 	int max=(int)(ext[0])*10/4;
// 	int min=(int)(ext[1])*10/4;
// 	get_Black_line();
	if(bl==0){
		if(num>=1000){
			for(i=0;i<9;i++){
				Image_line[(x+i)*3]=Image_line[(x+i)*3+1]=Image_line[(x+i)*3+2]=Image_num_9[line*90+num/1000%10*9+i];
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+22+i)*3]=Image_line[(x+22+i)*3+1]=Image_line[(x+22+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
			}
		}else if(num>=0){		
			for(i=0;i<9;i++){
				Image_line[(x+i)*3]=Image_line[(x+i)*3+1]=Image_line[(x+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
				Image_line[(x+26+i)*3]=Image_line[(x+26+i)*3+1]=Image_line[(x+26+i)*3+2]=Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				Image_line[(x+22)*3]=Image_line[(x+22)*3+1]=Image_line[(x+22)*3+2]=0xff;
				Image_line[(x+23)*3]=Image_line[(x+23)*3+1]=Image_line[(x+23)*3+2]=0xff;
			}
		}else if(num>=-99){	
			num=-num;
			for(i=0;i<9;i++){
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
				Image_line[(x+26+i)*3]=Image_line[(x+26+i)*3+1]=Image_line[(x+26+i)*3+2]=Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				Image_line[(x+22)*3]=Image_line[(x+22)*3+1]=Image_line[(x+22)*3+2]=0xff;
				Image_line[(x+23)*3]=Image_line[(x+23)*3+1]=Image_line[(x+23)*3+2]=0xff;
			}
			if(line<=8 && line>=7){
				Image_line[(x+4)*3]=Image_line[(x+4)*3+1]=Image_line[(x+4)*3+2]=0xff;
				Image_line[(x+5)*3]=Image_line[(x+5)*3+1]=Image_line[(x+5)*3+2]=0xff;
				Image_line[(x+6)*3]=Image_line[(x+6)*3+1]=Image_line[(x+6)*3+2]=0xff;
				Image_line[(x+7)*3]=Image_line[(x+7)*3+1]=Image_line[(x+7)*3+2]=0xff;
				Image_line[(x+3)*3]=Image_line[(x+3)*3+1]=Image_line[(x+3)*3+2]=0xff;
				Image_line[(x+2)*3]=Image_line[(x+2)*3+1]=Image_line[(x+2)*3+2]=0xff;
				Image_line[(x+1)*3]=Image_line[(x+1)*3+1]=Image_line[(x+1)*3+2]=0xff;
			}
		}else{
			num=-num;
			for(i=0;i<9;i++){
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+22+i)*3]=Image_line[(x+22+i)*3+1]=Image_line[(x+22+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
			}
			if(line<=8 && line>=7){
				Image_line[(x+4)*3]=Image_line[(x+4)*3+1]=Image_line[(x+4)*3+2]=0xff;
				Image_line[(x+5)*3]=Image_line[(x+5)*3+1]=Image_line[(x+5)*3+2]=0xff;
				Image_line[(x+6)*3]=Image_line[(x+6)*3+1]=Image_line[(x+6)*3+2]=0xff;
				Image_line[(x+7)*3]=Image_line[(x+7)*3+1]=Image_line[(x+7)*3+2]=0xff;
				Image_line[(x+3)*3]=Image_line[(x+3)*3+1]=Image_line[(x+3)*3+2]=0xff;
				Image_line[(x+2)*3]=Image_line[(x+2)*3+1]=Image_line[(x+2)*3+2]=0xff;
				Image_line[(x+1)*3]=Image_line[(x+1)*3+1]=Image_line[(x+1)*3+2]=0xff;
			}
		}
	}else{
		if(num>=1000){
			for(i=0;i<9;i++){
				Image_line[(x+i)*3]=Image_line[(x+i)*3+1]=Image_line[(x+i)*3+2]=~Image_num_9[line*90+num/1000%10*9+i];
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+22+i)*3]=Image_line[(x+22+i)*3+1]=Image_line[(x+22+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
			}
		}else if(num>=0){		
			for(i=0;i<9;i++){
				Image_line[(x+i)*3]=Image_line[(x+i)*3+1]=Image_line[(x+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
				Image_line[(x+26+i)*3]=Image_line[(x+26+i)*3+1]=Image_line[(x+26+i)*3+2]=~Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				Image_line[(x+22)*3]=Image_line[(x+22)*3+1]=Image_line[(x+22)*3+2]=0;
				Image_line[(x+23)*3]=Image_line[(x+23)*3+1]=Image_line[(x+23)*3+2]=0;
			}
		}else if(num>=-99){	
			num=-num;
			for(i=0;i<9;i++){
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
				Image_line[(x+26+i)*3]=Image_line[(x+26+i)*3+1]=Image_line[(x+26+i)*3+2]=~Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				Image_line[(x+22)*3]=Image_line[(x+22)*3+1]=Image_line[(x+22)*3+2]=0;
				Image_line[(x+23)*3]=Image_line[(x+23)*3+1]=Image_line[(x+23)*3+2]=0;
			}
			if(line<=8 && line>=7){
				Image_line[(x+4)*3]=Image_line[(x+4)*3+1]=Image_line[(x+4)*3+2]=0;
				Image_line[(x+5)*3]=Image_line[(x+5)*3+1]=Image_line[(x+5)*3+2]=0;
				Image_line[(x+6)*3]=Image_line[(x+6)*3+1]=Image_line[(x+6)*3+2]=0;
				Image_line[(x+7)*3]=Image_line[(x+7)*3+1]=Image_line[(x+7)*3+2]=0;
				Image_line[(x+3)*3]=Image_line[(x+3)*3+1]=Image_line[(x+3)*3+2]=0;
				Image_line[(x+2)*3]=Image_line[(x+2)*3+1]=Image_line[(x+2)*3+2]=0;
				Image_line[(x+1)*3]=Image_line[(x+1)*3+1]=Image_line[(x+1)*3+2]=0;
			}
		}else{
			num=-num;
			for(i=0;i<9;i++){
				Image_line[(x+11+i)*3]=Image_line[(x+11+i)*3+1]=Image_line[(x+11+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				Image_line[(x+22+i)*3]=Image_line[(x+22+i)*3+1]=Image_line[(x+22+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
			}
			if(line<=8 && line>=7){
				Image_line[(x+4)*3]=Image_line[(x+4)*3+1]=Image_line[(x+4)*3+2]=0;
				Image_line[(x+5)*3]=Image_line[(x+5)*3+1]=Image_line[(x+5)*3+2]=0;
				Image_line[(x+6)*3]=Image_line[(x+6)*3+1]=Image_line[(x+6)*3+2]=0;
				Image_line[(x+7)*3]=Image_line[(x+7)*3+1]=Image_line[(x+7)*3+2]=0;
				Image_line[(x+3)*3]=Image_line[(x+3)*3+1]=Image_line[(x+3)*3+2]=0;
				Image_line[(x+2)*3]=Image_line[(x+2)*3+1]=Image_line[(x+2)*3+2]=0;
				Image_line[(x+1)*3]=Image_line[(x+1)*3+1]=Image_line[(x+1)*3+2]=0;
			}
		}
	}
	/*
	if(max>=1000){
		for(i=0;i<9;i++){
			Image_line[(3+i)*3]=Image_line[(3+i)*3+1]=Image_line[(3+i)*3+2]=Image_num_9[line*90+max/1000%10*9+i];
			Image_line[(14+i)*3]=Image_line[(14+i)*3+1]=Image_line[(14+i)*3+2]=Image_num_9[line*90+max/100%10*9+i];
			Image_line[(25+i)*3]=Image_line[(25+i)*3+1]=Image_line[(25+i)*3+2]=Image_num_9[line*90+max/10%10*9+i];
		}
	}else if(max>=0){		
		for(i=0;i<9;i++){
			Image_line[(3+i)*3]=Image_line[(3+i)*3+1]=Image_line[(3+i)*3+2]=Image_num_9[line*90+max/100%10*9+i];
			Image_line[(14+i)*3]=Image_line[(14+i)*3+1]=Image_line[(14+i)*3+2]=Image_num_9[line*90+max/10%10*9+i];
			Image_line[(29+i)*3]=Image_line[(29+i)*3+1]=Image_line[(29+i)*3+2]=Image_num_9[line*90+max%10*9+i];
		}
		if(line<2){
			Image_line[(25)*3]=Image_line[(25)*3+1]=Image_line[(25)*3+2]=0xff;
			Image_line[(26)*3]=Image_line[(26)*3+1]=Image_line[(26)*3+2]=0xff;
		}
	}else if(max>=-99){	
		max=-max;
		for(i=0;i<9;i++){
			Image_line[(14+i)*3]=Image_line[(14+i)*3+1]=Image_line[(14+i)*3+2]=Image_num_9[line*90+max/10%10*9+i];
			Image_line[(29+i)*3]=Image_line[(29+i)*3+1]=Image_line[(29+i)*3+2]=Image_num_9[line*90+max%10*9+i];
		}
		if(line<2){
			Image_line[(25)*3]=Image_line[(25)*3+1]=Image_line[(25)*3+2]=0xff;
			Image_line[(26)*3]=Image_line[(26)*3+1]=Image_line[(26)*3+2]=0xff;
		}
		if(line<=8 && line>=7){
			Image_line[(4)*3]=Image_line[(4)*3+1]=Image_line[(4)*3+2]=0xff;
			Image_line[(5)*3]=Image_line[(5)*3+1]=Image_line[(5)*3+2]=0xff;
			Image_line[(6)*3]=Image_line[(6)*3+1]=Image_line[(6)*3+2]=0xff;
			Image_line[(7)*3]=Image_line[(7)*3+1]=Image_line[(7)*3+2]=0xff;
			Image_line[(8)*3]=Image_line[(8)*3+1]=Image_line[(8)*3+2]=0xff;
			Image_line[(9)*3]=Image_line[(9)*3+1]=Image_line[(9)*3+2]=0xff;
			Image_line[(10)*3]=Image_line[(10)*3+1]=Image_line[(10)*3+2]=0xff;
		}
	}else{
		max=-max;
		for(i=0;i<9;i++){
			Image_line[(14+i)*3]=Image_line[(14+i)*3+1]=Image_line[(14+i)*3+2]=Image_num_9[line*90+max/100%10*9+i];
			Image_line[(25+i)*3]=Image_line[(25+i)*3+1]=Image_line[(25+i)*3+2]=Image_num_9[line*90+max/10%10*9+i];
		}
		if(line<=8 && line>=7){
			Image_line[(4)*3]=Image_line[(4)*3+1]=Image_line[(4)*3+2]=0xff;
			Image_line[(5)*3]=Image_line[(5)*3+1]=Image_line[(5)*3+2]=0xff;
			Image_line[(6)*3]=Image_line[(6)*3+1]=Image_line[(6)*3+2]=0xff;
			Image_line[(7)*3]=Image_line[(7)*3+1]=Image_line[(7)*3+2]=0xff;
			Image_line[(8)*3]=Image_line[(8)*3+1]=Image_line[(8)*3+2]=0xff;
			Image_line[(9)*3]=Image_line[(9)*3+1]=Image_line[(9)*3+2]=0xff;
			Image_line[(10)*3]=Image_line[(10)*3+1]=Image_line[(10)*3+2]=0xff;
		}
	}

	if(min>=1000){
		for(i=0;i<9;i++){
			Image_line[(86+i)*3]=Image_line[(86+i)*3+1]=Image_line[(86+i)*3+2]=Image_num_9[line*90+min/1000%10*9+i];
			Image_line[(97+i)*3]=Image_line[(97+i)*3+1]=Image_line[(97+i)*3+2]=Image_num_9[line*90+min/100%10*9+i];
			Image_line[(108+i)*3]=Image_line[(108+i)*3+1]=Image_line[(108+i)*3+2]=Image_num_9[line*90+min/10%10*9+i];
		}
	}else if(min>=0){	
		for(i=0;i<9;i++){
			Image_line[(82+i)*3]=Image_line[(82+i)*3+1]=Image_line[(82+i)*3+2]=Image_num_9[line*90+min/100%10*9+i];
			Image_line[(93+i)*3]=Image_line[(93+i)*3+1]=Image_line[(93+i)*3+2]=Image_num_9[line*90+min/10%10*9+i];
			Image_line[(108+i)*3]=Image_line[(108+i)*3+1]=Image_line[(108+i)*3+2]=Image_num_9[line*90+min%10*9+i];
		}
		if(line<2){
			Image_line[(104)*3]=Image_line[(104)*3+1]=Image_line[(104)*3+2]=0xff;
			Image_line[(105)*3]=Image_line[(105)*3+1]=Image_line[(105)*3+2]=0xff;
		}
	}else if(min>=-99){	
		min=-min;
		for(i=0;i<9;i++){
			Image_line[(93+i)*3]=Image_line[(93+i)*3+1]=Image_line[(93+i)*3+2]=Image_num_9[line*90+min/10%10*9+i];
			Image_line[(108+i)*3]=Image_line[(108+i)*3+1]=Image_line[(108+i)*3+2]=Image_num_9[line*90+min%10*9+i];
		}
		if(line<2){
			Image_line[(104)*3]=Image_line[(104)*3+1]=Image_line[(104)*3+2]=0xff;
			Image_line[(105)*3]=Image_line[(105)*3+1]=Image_line[(105)*3+2]=0xff;
		}
		if(line<=8 && line>=7){
			Image_line[(83)*3]=Image_line[(83)*3+1]=Image_line[(83)*3+2]=0xff;
			Image_line[(84)*3]=Image_line[(84)*3+1]=Image_line[(84)*3+2]=0xff;
			Image_line[(85)*3]=Image_line[(85)*3+1]=Image_line[(85)*3+2]=0xff;
			Image_line[(86)*3]=Image_line[(86)*3+1]=Image_line[(86)*3+2]=0xff;
			Image_line[(87)*3]=Image_line[(87)*3+1]=Image_line[(87)*3+2]=0xff;
			Image_line[(88)*3]=Image_line[(88)*3+1]=Image_line[(88)*3+2]=0xff;
			Image_line[(89)*3]=Image_line[(89)*3+1]=Image_line[(89)*3+2]=0xff;
		}
	}else{
		min=-min;
		for(i=0;i<9;i++){
			Image_line[(97+i)*3]=Image_line[(97+i)*3+1]=Image_line[(97+i)*3+2]=Image_num_9[line*90+min/100%10*9+i];
			Image_line[(108+i)*3]=Image_line[(108+i)*3+1]=Image_line[(108+i)*3+2]=Image_num_9[line*90+min/10%10*9+i];
		}
		if(line<=8 && line>=7){
			Image_line[(87)*3]=Image_line[(87)*3+1]=Image_line[(87)*3+2]=0xff;
			Image_line[(88)*3]=Image_line[(88)*3+1]=Image_line[(88)*3+2]=0xff;
			Image_line[(89)*3]=Image_line[(89)*3+1]=Image_line[(89)*3+2]=0xff;
			Image_line[(90)*3]=Image_line[(90)*3+1]=Image_line[(90)*3+2]=0xff;
			Image_line[(91)*3]=Image_line[(91)*3+1]=Image_line[(91)*3+2]=0xff;
			Image_line[(92)*3]=Image_line[(92)*3+1]=Image_line[(92)*3+2]=0xff;
			Image_line[(93)*3]=Image_line[(93)*3+1]=Image_line[(93)*3+2]=0xff;
		}
	}
	
	*/
	
// 	for(i=0;i<9;i++){
// 	  Image_line[(3+i)*3]=Image_line[(3+i)*3+1]=Image_line[(3+i)*3+2]=Image_num[line*90+max/100%10*9+i];
// 	  Image_line[(14+i)*3]=Image_line[(14+i)*3+1]=Image_line[(14+i)*3+2]=Image_num[line*90+max/10%10*9+i];
// 	  Image_line[(29+i)*3]=Image_line[(29+i)*3+1]=Image_line[(29+i)*3+2]=Image_num[line*90+max%10*9+i];
// 	  Image_line[(82+i)*3]=Image_line[(82+i)*3+1]=Image_line[(82+i)*3+2]=Image_num[line*90+min/100%10*9+i];
// 	  Image_line[(93+i)*3]=Image_line[(93+i)*3+1]=Image_line[(93+i)*3+2]=Image_num[line*90+min/10%10*9+i];
// 	  Image_line[(108+i)*3]=Image_line[(108+i)*3+1]=Image_line[(108+i)*3+2]=Image_num[line*90+min%10*9+i];
// 	}
// 	if(line<2){
// 	  Image_line[(25)*3]=Image_line[(25)*3+1]=Image_line[(25)*3+2]=0xff;
// 	  Image_line[(26)*3]=Image_line[(26)*3+1]=Image_line[(26)*3+2]=0xff;
// 	  Image_line[(104)*3]=Image_line[(104)*3+1]=Image_line[(104)*3+2]=0xff;
// 	  Image_line[(105)*3]=Image_line[(105)*3+1]=Image_line[(105)*3+2]=0xff;
// 	}
}

void box_mid(void){
	u16 i;
	for(i=42;i<81;i++){
		Image_line[i*3]=Image_line[i*3+1]=Image_line[i*3+2]=0xff;
	}
}


// static FRESULT FAT_SAVABuff( char *path ,u8 *DataBuff ,u16 DataLength ){
// 	FIL fdst_f;
// 	FRESULT res_f;
// 	UINT bw_f;  
// 	int File_Byte;
// 	res_f = f_open(&fdst_f, path, FA_OPEN_EXISTING | FA_READ | FA_WRITE );
// 	if( res_f == FR_OK ){
// 		File_Byte = fdst_f.fsize;
// 		f_lseek(&fdst_f,File_Byte);
// 		res_f = f_write(&fdst_f, DataBuff, DataLength, &bw_f); 
// 		f_close(&fdst_f); 
// 	}else{
// 		File_Byte = 0;
// 		res_f = f_open( &fdst_f, path , FA_CREATE_NEW | FA_WRITE );
// 		if( res_f == FR_OK ){
// 			res_f = f_write(&fdst_f, DataBuff, DataLength, &bw_f);
// 			f_close(&fdst_f); 
// 		}
// 	}
// 	return res_f;
// }



u8 save_bmp(void){			
	u32 i;
	
	FIL fdst_f;
	FRESULT res_f;
	UINT bw_f;  
	long File_Byte;

	if(f_mount(&fs,"0:",1)!=FR_OK)return 1;
	
	if(f_opendir(&dr, "0:/picture")!=FR_OK){
		f_mkdir("0:/picture");
		SaveTimes=0;
	}
	
	SaveTimes++;
	if(SaveTimes>99999999)SaveTimes=0;	
	sprintf(name_buf,"0:/picture/%08d/%08d.bmp",SaveTimes/20,SaveTimes);
	sprintf(dir_buf,"0:/picture/%08d",SaveTimes/20);
	
	if(f_opendir(&dr, dir_buf)!=FR_OK){
		f_mkdir(dir_buf);
	}
	res_f = f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ );
	
	
	while(res_f==FR_OK){
		SaveTimes++;
		if(SaveTimes>99999999)SaveTimes=0;	
		f_close(&fdst_f); 
		f_closedir(&dr);
		sprintf(name_buf,"0:/picture/%08d/%08d.bmp",SaveTimes/20,SaveTimes);
		sprintf(dir_buf,"0:/picture/%08d",SaveTimes/20);
		if(f_opendir(&dr, dir_buf)!=FR_OK){
			f_mkdir(dir_buf);
		}
		res_f = f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ );
	}
	Save_Times[0]=SaveTimes&0xff;
	Save_Times[1]=(SaveTimes>>8)&0xff;	
	Save_Times[2]=(SaveTimes>>16)&0xff;	
	Save_Times[3]=(SaveTimes>>24)&0xff;	
	AT24CXX_Write(0,(u8*)Save_Times,4);	
	
	File_Byte = 0;
	f_open( &fdst_f, name_buf , FA_CREATE_NEW | FA_READ | FA_WRITE );
	f_write(&fdst_f, Image_head, 54, &bw_f);
	File_Byte+=54;
	f_lseek(&fdst_f,File_Byte);
	
// 	FAT_SAVABuff(name_buf,(u8*)Image_head,54);
	
	
	
	
	for(i=0;i<120;i++){
		if(i%3==0){
		  get_bmp_line(i/3);
		}
		if(test_mod==midd && i>34 && i<57){
			box_mid();
			if(i>36 && i<55)
			  get_num_line(44,(int)(ext[2])*10/4,1,i-37);
		}
// 		FAT_SAVABuff(name_buf,(u8*)Image_line,360);
		f_write(&fdst_f, Image_line, 360, &bw_f);
		File_Byte+=360;
		f_lseek(&fdst_f,File_Byte);
		
	}
	if(color_mod==Iron){
		for(i=0;i<8;i++){
// 			FAT_SAVABuff(name_buf,(u8*)Image_line_Iron,360);
			f_write(&fdst_f, Image_line_Iron, 360, &bw_f);
			File_Byte+=360;
			f_lseek(&fdst_f,File_Byte);
		}
	}else if(color_mod==RB){
		for(i=0;i<8;i++){
// 			FAT_SAVABuff(name_buf,(u8*)Image_line_RB,360);			
			f_write(&fdst_f, Image_line_RB, 360, &bw_f);
			File_Byte+=360;
			f_lseek(&fdst_f,File_Byte);
		}
	}else if(color_mod==BW){
		for(i=0;i<8;i++){
// 			FAT_SAVABuff(name_buf,(u8*)Image_line_BW,360);
			f_write(&fdst_f, Image_line_BW, 360, &bw_f);
			File_Byte+=360;
			f_lseek(&fdst_f,File_Byte);
		}
	}
	get_Black_line();
// 	FAT_SAVABuff(name_buf,(u8*)Image_line,360);
// 	FAT_SAVABuff(name_buf,(u8*)Image_line,360);
	
	f_write(&fdst_f, Image_line, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	
	f_write(&fdst_f, Image_line, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	
	for(i=0;i<18;i++){
	  get_Black_line();
		get_num_line(3,(int)(ext[0])*10/4,0,i);
		get_num_line(82,(int)(ext[1])*10/4,0,i);
// 		FAT_SAVABuff(name_buf,(u8*)Image_line,360);
		f_write(&fdst_f, Image_line, 360, &bw_f);
		File_Byte+=360;
		f_lseek(&fdst_f,File_Byte);
	}
	get_Black_line();
// 	FAT_SAVABuff(name_buf,(u8*)Image_line,360);
// 	FAT_SAVABuff(name_buf,(u8*)Image_line,360);
// 	FAT_SAVABuff(name_buf,(u8*)Image_line,360);
	
	f_write(&fdst_f, Image_line, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	f_write(&fdst_f, Image_line, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	f_write(&fdst_f, Image_line, 360, &bw_f);
	
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	
	return 0;
}



// u8 test_line[160*3]={0};

// void test_bmp(void){
// 	u32 i,j;
// 	u16 buf;
// 	SaveTimes++;
// 	if(SaveTimes>99999999)SaveTimes=0;
// 	sprintf(name_buf,"0:/%08d.bmp", SaveTimes);
// 	Save_Times[0]=SaveTimes&0xffff;
// 	Save_Times[1]=(SaveTimes>>16)&0xffff;	
// 	AT24CXX_Write(0,(u8*)Save_Times,4);
// 	f_mount(&fs,"0:",1); 
// 	FAT_SAVABuff(name_buf,Image_head_test,54);
// 	for(j=0;j<128;j++){
// 		for(i=0;i<160;i++){
// 			buf=ReadPixel(129-j,160-i);				
// 			test_line[i*3]=(u8)((((buf>>0)&0xff)<<3)&0xff);
// 			test_line[i*3+1]=(u8)((((buf>>6)&0xff)<<3)&0xff);
// 			test_line[i*3+2]=(u8)((((buf>>11)&0xff)<<3)&0xff);
// 		}
// 		FAT_SAVABuff(name_buf,test_line,480);
// 	}
// 	f_mount(&fs,NULL,1); 
// }

extern long data[40][40];


// u8 get_data_bmp(u32 file_name){

// //	u8 i,j;

// 	sprintf(name_buff,"0:/play/%04d.bmp",file_name);
// 	res_f = f_open(&fdst_f, name_buff, FA_OPEN_EXISTING | FA_READ  );
// 	if( res_f == FR_OK ){
// 		bw_f=1078;
// 		f_lseek(&fdst_f,bw_f);
// 		f_read(&fdst_f, data, 40*40, &bw_f);
// // 		File_Byte = fdst_f.fsize;
// // 		f_lseek(&fdst_f,File_Byte);
// // 		res_f = f_write(&fdst_f, DataBuff, DataLength, &bw_f); 
// 		f_close(&fdst_f); 
// 		return 1;
// 	}
// 	return 0;
// }


u8 Play_BadApple(void){
	u16 i=1,j,l;	
	FIL fdst_f;
	FRESULT res_f;
	UINT bw_f; 
  u32 File_Byte;	
	u16 buf[20];
	
	if(f_mount(&fs,"0:",1)!=FR_OK)return 0;
	if(f_opendir(&dr, "0:/sys")!=FR_OK)return 0;
	res_f = f_open(&fdst_f, "0:/sys/PlayFile.bin", FA_OPEN_EXISTING | FA_READ  );
	if( res_f != FR_OK )return 0;
	File_Byte = fdst_f.fsize;
  File_Byte/=1600;
	ext[0]=0xff;
	ext[1]=0;
	do{
		for(j=0;j<40;j++){
			f_lseek(&fdst_f,(i*1600+j*40));
			res_f = f_read(&fdst_f, buf, 40, &bw_f);
			for(l=0;l<20;l++){
				data[39-j][39-l*2-1]=0xff&(buf[l]>>8);
				data[39-j][39-l*2]=0xff&buf[l];
			}
		}
		get_img();      //插值转换为rgb图片
		Draw_img();       //显示图片
		logo_move();       //运行指示
// 		LED0=~LED0;     //刷新率测试
		delay_ms(64);
		i++;
	}while(File_Byte>=i && KEY_Scan(0)==0);
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 3;
}



u8 check_str(u8 *str1,const u8 *str2,u16 times){
	for(times=times;times>0;times--){
		if(*str1 != *str2)
			return 1;
		str1++;
		str2++;
	}
	return 0;
}




	u8 read_buf[480]={0};


u8 read_boot_bmp(void){
	u16 i,j,buf;	
	FIL fdst_f;
	FRESULT res_f;
	UINT bw_f; 

	if(f_mount(&fs,"0:",1)!=FR_OK)return 0;
	if(f_opendir(&dr, "0:/sys")!=FR_OK)return 0;
	res_f = f_open(&fdst_f, "0:/sys/BootGrap.bmp", FA_OPEN_EXISTING | FA_READ  );
//	DrawBack();      //绘制背景
	if( res_f != FR_OK )return 0;
	f_read(&fdst_f, read_buf, 54, &bw_f);
	if(check_str(read_buf,Image_head2,54))return 0;
	for(i=0;i<128;i++){
		f_lseek(&fdst_f,i*160*3+54);
		res_f = f_read(&fdst_f, read_buf, 480, &bw_f);  //读取480字
		if(res_f!=FR_OK)return 0;
		for(j=0;j<160;j++){
			buf=0xFFFF&((read_buf[0+j*3]&0xf8)>>3|(read_buf[1+j*3]&0xf8)<<3|(read_buf[2+j*3]&0xf8)<<8);
			read_buf[1+j*2]=0xff&(buf>>8);
			read_buf[0+j*2]=0xff&(buf);
		}
		LCD_Pic2(127-i,read_buf);
	}
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 1;
}





