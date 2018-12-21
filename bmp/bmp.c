#include "bmp.h"
#include "bmpdata.h"


#if (Size == SIZEx5)
void get_bmp_line_x5(u8 row){
	u8 i;
	for(i=0;i<40;i++){
		RW_Buf[i*9]=RW_Buf[i*9+3]=RW_Buf[i*9+6]=(u8)((((data[39-row][39-i]>>0)&0xff)<<3)&0xff);
		RW_Buf[i*9+1]=RW_Buf[i*9+1+3]=RW_Buf[i*9+1+6]=(u8)((((data[39-row][39-i]>>6)&0xff)<<3)&0xff);
		RW_Buf[i*9+2]=RW_Buf[i*9+2+3]=RW_Buf[i*9+2+6]=(u8)((((data[39-row][39-i]>>11)&0xff)<<3)&0xff);
	}
	if(SysState.DispMeas == Midd){
		if(row==20){
			RW_Buf[20*9]=RW_Buf[20*9+3]=RW_Buf[20*9+6]=0;
			RW_Buf[20*9+1]=RW_Buf[20*9+1+3]=RW_Buf[20*9+1+6]=0;
			RW_Buf[20*9+2]=RW_Buf[20*9+2+3]=RW_Buf[20*9+2+6]=0;
		}
	}else if(SysState.DispMeas == Exts){
		if(ext_add[0]/8*5+2==row){
			i=(7-(ext_add[0]%8))*5+2;
			RW_Buf[i*9]=RW_Buf[i*9+3]=RW_Buf[i*9+6]=0;
			RW_Buf[i*9+1]=RW_Buf[i*9+1+3]=RW_Buf[i*9+1+6]=0;
			RW_Buf[i*9+2]=RW_Buf[i*9+2+3]=RW_Buf[i*9+2+6]=0;
		}
		if(ext_add[1]/8*5+2==row){
			i=(7-(ext_add[1]%8))*5+2;
			RW_Buf[i*9]=RW_Buf[i*9+3]=RW_Buf[i*9+6]=0xff;
			RW_Buf[i*9+1]=RW_Buf[i*9+1+3]=RW_Buf[i*9+1+6]=0xff;
			RW_Buf[i*9+2]=RW_Buf[i*9+2+3]=RW_Buf[i*9+2+6]=0xff;
		}
	}
}	
#elif (Size == SIZEx8)
void get_bmp_line_x8(u8 row){
	u8 i;
	for(i=0;i<59;i++){
		RW_Buf[i*6+0+3]=RW_Buf[i*6+0+6]=(u8)((((data[58-row][58-i]>>0)&0xff)<<3)&0xff);
		RW_Buf[i*6+1+3]=RW_Buf[i*6+1+6]=(u8)((((data[58-row][58-i]>>6)&0xff)<<3)&0xff);
		RW_Buf[i*6+2+3]=RW_Buf[i*6+2+6]=(u8)((((data[58-row][58-i]>>11)&0xff)<<3)&0xff);
	}
	RW_Buf[0]=(u8)((((data[58-row][58-0]>>0)&0xff)<<3)&0xff);
	RW_Buf[1]=(u8)((((data[58-row][58-0]>>6)&0xff)<<3)&0xff);
	RW_Buf[2]=(u8)((((data[58-row][58-0]>>11)&0xff)<<3)&0xff);
	RW_Buf[357]=(u8)((((data[58-row][58-58]>>0)&0xff)<<3)&0xff);
	RW_Buf[358]=(u8)((((data[58-row][58-58]>>6)&0xff)<<3)&0xff);
	RW_Buf[359]=(u8)((((data[58-row][58-58]>>11)&0xff)<<3)&0xff);
	
	if(SysState.DispMeas == Midd){
		if(row==29){
			RW_Buf[29*6+0+3]=RW_Buf[29*6+0+6]=0;
			RW_Buf[29*6+1+3]=RW_Buf[29*6+1+6]=0;
			RW_Buf[29*6+2+3]=RW_Buf[29*6+2+6]=0;
		}
	}else if(SysState.DispMeas == Exts){
		if(ext_add[0]/8*8+1==row){
			i=(7-(ext_add[0]%8))*8+1;
			RW_Buf[i*6+0+3]=RW_Buf[i*6+0+6]=0;
			RW_Buf[i*6+1+3]=RW_Buf[i*6+1+6]=0;
			RW_Buf[i*6+2+3]=RW_Buf[i*6+2+6]=0;
		}
		if(ext_add[1]/8*8+1==row){
			i=(7-(ext_add[1]%8))*8+1;
			RW_Buf[i*6+0+3]=RW_Buf[i*6+0+6]=0xff;
			RW_Buf[i*6+1+3]=RW_Buf[i*6+1+6]=0xff;
			RW_Buf[i*6+2+3]=RW_Buf[i*6+2+6]=0xff;
		}
	}
}	
#endif


void get_Black_line(void){
	u8 i;
	for(i=0;i<121;i++){
		RW_Buf[i*3]=RW_Buf[i*3+1]=RW_Buf[i*3+2]=0;
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
				RW_Buf[(x+i)*3]=RW_Buf[(x+i)*3+1]=RW_Buf[(x+i)*3+2]=Image_num_9[line*90+num/1000%10*9+i];
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+22+i)*3]=RW_Buf[(x+22+i)*3+1]=RW_Buf[(x+22+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
			}
		}else if(num>=0){		
			for(i=0;i<9;i++){
				RW_Buf[(x+i)*3]=RW_Buf[(x+i)*3+1]=RW_Buf[(x+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
				RW_Buf[(x+26+i)*3]=RW_Buf[(x+26+i)*3+1]=RW_Buf[(x+26+i)*3+2]=Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				RW_Buf[(x+22)*3]=RW_Buf[(x+22)*3+1]=RW_Buf[(x+22)*3+2]=0xff;
				RW_Buf[(x+23)*3]=RW_Buf[(x+23)*3+1]=RW_Buf[(x+23)*3+2]=0xff;
			}
		}else if(num>=-99){
			num=-num;
			for(i=0;i<9;i++){
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
				RW_Buf[(x+26+i)*3]=RW_Buf[(x+26+i)*3+1]=RW_Buf[(x+26+i)*3+2]=Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				RW_Buf[(x+22)*3]=RW_Buf[(x+22)*3+1]=RW_Buf[(x+22)*3+2]=0xff;
				RW_Buf[(x+23)*3]=RW_Buf[(x+23)*3+1]=RW_Buf[(x+23)*3+2]=0xff;
			}
			if(line<=8 && line>=7){
				RW_Buf[(x+4)*3]=RW_Buf[(x+4)*3+1]=RW_Buf[(x+4)*3+2]=0xff;
				RW_Buf[(x+5)*3]=RW_Buf[(x+5)*3+1]=RW_Buf[(x+5)*3+2]=0xff;
				RW_Buf[(x+6)*3]=RW_Buf[(x+6)*3+1]=RW_Buf[(x+6)*3+2]=0xff;
				RW_Buf[(x+7)*3]=RW_Buf[(x+7)*3+1]=RW_Buf[(x+7)*3+2]=0xff;
				RW_Buf[(x+3)*3]=RW_Buf[(x+3)*3+1]=RW_Buf[(x+3)*3+2]=0xff;
				RW_Buf[(x+2)*3]=RW_Buf[(x+2)*3+1]=RW_Buf[(x+2)*3+2]=0xff;
				RW_Buf[(x+1)*3]=RW_Buf[(x+1)*3+1]=RW_Buf[(x+1)*3+2]=0xff;
			}
		}else{
			num=-num;
			for(i=0;i<9;i++){
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+22+i)*3]=RW_Buf[(x+22+i)*3+1]=RW_Buf[(x+22+i)*3+2]=Image_num_9[line*90+num/10%10*9+i];
			}
			if(line<=8 && line>=7){
				RW_Buf[(x+4)*3]=RW_Buf[(x+4)*3+1]=RW_Buf[(x+4)*3+2]=0xff;
				RW_Buf[(x+5)*3]=RW_Buf[(x+5)*3+1]=RW_Buf[(x+5)*3+2]=0xff;
				RW_Buf[(x+6)*3]=RW_Buf[(x+6)*3+1]=RW_Buf[(x+6)*3+2]=0xff;
				RW_Buf[(x+7)*3]=RW_Buf[(x+7)*3+1]=RW_Buf[(x+7)*3+2]=0xff;
				RW_Buf[(x+3)*3]=RW_Buf[(x+3)*3+1]=RW_Buf[(x+3)*3+2]=0xff;
				RW_Buf[(x+2)*3]=RW_Buf[(x+2)*3+1]=RW_Buf[(x+2)*3+2]=0xff;
				RW_Buf[(x+1)*3]=RW_Buf[(x+1)*3+1]=RW_Buf[(x+1)*3+2]=0xff;
			}
		}
	}else{
		if(num>=1000){
			for(i=0;i<9;i++){
				RW_Buf[(x+i)*3]=RW_Buf[(x+i)*3+1]=RW_Buf[(x+i)*3+2]=~Image_num_9[line*90+num/1000%10*9+i];
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+22+i)*3]=RW_Buf[(x+22+i)*3+1]=RW_Buf[(x+22+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
			}
		}else if(num>=0){		
			for(i=0;i<9;i++){
				RW_Buf[(x+i)*3]=RW_Buf[(x+i)*3+1]=RW_Buf[(x+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
				RW_Buf[(x+26+i)*3]=RW_Buf[(x+26+i)*3+1]=RW_Buf[(x+26+i)*3+2]=~Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				RW_Buf[(x+22)*3]=RW_Buf[(x+22)*3+1]=RW_Buf[(x+22)*3+2]=0;
				RW_Buf[(x+23)*3]=RW_Buf[(x+23)*3+1]=RW_Buf[(x+23)*3+2]=0;
			}
		}else if(num>=-99){	
			num=-num;
			for(i=0;i<9;i++){
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
				RW_Buf[(x+26+i)*3]=RW_Buf[(x+26+i)*3+1]=RW_Buf[(x+26+i)*3+2]=~Image_num_9[line*90+num%10*9+i];
			}
			if(line<2){
				RW_Buf[(x+22)*3]=RW_Buf[(x+22)*3+1]=RW_Buf[(x+22)*3+2]=0;
				RW_Buf[(x+23)*3]=RW_Buf[(x+23)*3+1]=RW_Buf[(x+23)*3+2]=0;
			}
			if(line<=8 && line>=7){
				RW_Buf[(x+4)*3]=RW_Buf[(x+4)*3+1]=RW_Buf[(x+4)*3+2]=0;
				RW_Buf[(x+5)*3]=RW_Buf[(x+5)*3+1]=RW_Buf[(x+5)*3+2]=0;
				RW_Buf[(x+6)*3]=RW_Buf[(x+6)*3+1]=RW_Buf[(x+6)*3+2]=0;
				RW_Buf[(x+7)*3]=RW_Buf[(x+7)*3+1]=RW_Buf[(x+7)*3+2]=0;
				RW_Buf[(x+3)*3]=RW_Buf[(x+3)*3+1]=RW_Buf[(x+3)*3+2]=0;
				RW_Buf[(x+2)*3]=RW_Buf[(x+2)*3+1]=RW_Buf[(x+2)*3+2]=0;
				RW_Buf[(x+1)*3]=RW_Buf[(x+1)*3+1]=RW_Buf[(x+1)*3+2]=0;
			}
		}else{
			num=-num;
			for(i=0;i<9;i++){
				RW_Buf[(x+11+i)*3]=RW_Buf[(x+11+i)*3+1]=RW_Buf[(x+11+i)*3+2]=~Image_num_9[line*90+num/100%10*9+i];
				RW_Buf[(x+22+i)*3]=RW_Buf[(x+22+i)*3+1]=RW_Buf[(x+22+i)*3+2]=~Image_num_9[line*90+num/10%10*9+i];
			}
			if(line<=8 && line>=7){
				RW_Buf[(x+4)*3]=RW_Buf[(x+4)*3+1]=RW_Buf[(x+4)*3+2]=0;
				RW_Buf[(x+5)*3]=RW_Buf[(x+5)*3+1]=RW_Buf[(x+5)*3+2]=0;
				RW_Buf[(x+6)*3]=RW_Buf[(x+6)*3+1]=RW_Buf[(x+6)*3+2]=0;
				RW_Buf[(x+7)*3]=RW_Buf[(x+7)*3+1]=RW_Buf[(x+7)*3+2]=0;
				RW_Buf[(x+3)*3]=RW_Buf[(x+3)*3+1]=RW_Buf[(x+3)*3+2]=0;
				RW_Buf[(x+2)*3]=RW_Buf[(x+2)*3+1]=RW_Buf[(x+2)*3+2]=0;
				RW_Buf[(x+1)*3]=RW_Buf[(x+1)*3+1]=RW_Buf[(x+1)*3+2]=0;
			}
		}
	}

}

void box_mid(void){
	u16 i;
	for(i=42;i<81;i++){
		RW_Buf[i*3]=RW_Buf[i*3+1]=RW_Buf[i*3+2]=0xff;
	}
}


u8 save_bmp(void){			
	u16 i;
	
	UINT bw_f;  
	long File_Byte;

	if(RecState.SaveNum>60000)
		return 1;
	
	if(f_mount(&fs,"0:",1)!=FR_OK)return 1;
	
	if(f_opendir(&dr, "0:/picture")!=FR_OK){
		f_mkdir("0:/picture");
		RecState.SaveNum = 0;
	}
	
	RecState.SaveNum++;
	sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(RecState.SaveNum-1)/20,RecState.SaveNum);
	sprintf(dir_buf,"0:/picture/%04d",(RecState.SaveNum-1)/20);
	
	if(f_opendir(&dr, dir_buf)!=FR_OK){
		f_mkdir(dir_buf);
	}
	res_f = f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ );
	
	
	while(res_f == FR_OK){
		if(RecState.SaveNum > 60000){
			return 1;             //存满了，跳出
		}
		RecState.SaveNum++;
		f_close(&fdst_f); 
		f_closedir(&dr);
		sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(RecState.SaveNum-1)/20,RecState.SaveNum);
		sprintf(dir_buf,"0:/picture/%04d",(RecState.SaveNum-1)/20);
		if(f_opendir(&dr, dir_buf)!=FR_OK){
			f_mkdir(dir_buf);
		}
		res_f = f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ );
	}
	
	RecState.SAVE_NUM[0]=RecState.SaveNum&0xff;
	RecState.SAVE_NUM[1]=(RecState.SaveNum>>8)&0xff;	
	AT24CXX_Write(0,(u8*)RecState.SAVE_NUM,2);	
	/*-------save grap-------*/
	File_Byte = 0;
	f_open( &fdst_f, name_buf , FA_CREATE_NEW | FA_READ | FA_WRITE );
	f_write(&fdst_f, Image_head, 54, &bw_f);
	File_Byte+=54;
	f_lseek(&fdst_f,File_Byte);
	
	for(i=0;i<120;i++){
		
#if (Size == SIZEx5)			//5x插值換算
		if(i%3==0){
		  get_bmp_line_x5(i/3);
		}
#elif (Size == SIZEx8)
		if(i==0)get_bmp_line_x8(0);
 		else if(i==119)get_bmp_line_x8(58);
		else if(i%2==1)get_bmp_line_x8(i/2);
#endif
		
		if((SysState.DispMeas == Midd) && (i>34) && (i<57)){
			box_mid();
			if(i>36 && i<55)
			  get_num_line(44,(int)(ext[2])*10/4,1,i-37);
		}
		f_write(&fdst_f, RW_Buf, 360, &bw_f);
		File_Byte+=360;
		f_lseek(&fdst_f,File_Byte);
		
	}
	switch(SysState.ColrMode){
		
		case Iron:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_Iron, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
			break;
			
		case IronMax:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_IronMax, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
			break;
			
		case IronMin:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_IronMin, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
			break;
			
		case RB:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_RB, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
			break;
			
		case BW:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_BW, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
			break;
		
		default:
			for(i=0;i<8;i++){
				f_write(&fdst_f, Image_line_Iron, 360, &bw_f);
				File_Byte+=360;
				f_lseek(&fdst_f,File_Byte);
			}
	}
	
	get_Black_line();
	
	f_write(&fdst_f, RW_Buf, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	
	f_write(&fdst_f, RW_Buf, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	
	for(i=0;i<18;i++){
	  get_Black_line();
		get_num_line(3,(int)(ext[0])*10/4,0,i);
		get_num_line(82,(int)(ext[1])*10/4,0,i);
		f_write(&fdst_f, RW_Buf, 360, &bw_f);
		File_Byte+=360;
		f_lseek(&fdst_f,File_Byte);
	}
	get_Black_line();
	
	f_write(&fdst_f, RW_Buf, 360, &bw_f);
	File_Byte+=360;
	f_lseek(&fdst_f,File_Byte);
	f_write(&fdst_f, RW_Buf, 362, &bw_f);	//多2字节，不明所以
	File_Byte+=362;
	f_lseek(&fdst_f,File_Byte);
	
	//54056byte
/*-------save data-------*/
	
// 	for(i=0;i<8;i++){
// 		f_write(&fdst_f, data[i], PixLg*4, &bw_f);
// 		File_Byte+=PixLg*4;
// 		f_lseek(&fdst_f,File_Byte);
// 	}
	
	f_write(&fdst_f,PriData, 8*8*2, &bw_f);
	File_Byte += 8*8*2;
	
	RW_Buf[0] = SysState.ColrMode;	//只存测温模式和色卡
	RW_Buf[1] = SysState.DispMeas;
	
// 	for(i=0;i<3;i++){
// 		RW_Buf[i*4+2] = (ext[i]>>24)&0xff;
// 		RW_Buf[i*4+3] = (ext[i]>>16)&0xff;
// 		RW_Buf[i*4+4] = (ext[i]>>8)&0xff;
// 		RW_Buf[i*4+5] = (ext[i]>>0)&0xff;
// 	}
// 	
// 	RW_Buf[14] = ext_add[0];
// 	RW_Buf[15] = ext_add[1];
	
	f_write(&fdst_f, RW_Buf, 2, &bw_f);
	
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); //save data
	
	return 0;
}

void GetFileNum(void){
	
	u16 SaveTimesBak = 0;
	RecState.SaveNum = 1;
	
	if(f_mount(&fs,"0:",1)!=FR_OK){
		RecState.SAVE_NUM[0]=0;
		RecState.SAVE_NUM[1]=0;
		AT24CXX_Write(0,(u8*)RecState.SAVE_NUM,2);
		return;
	}
	
	if(f_opendir(&dr, "0:/picture")!=FR_OK){
		RecState.SAVE_NUM[0]=0;
		RecState.SAVE_NUM[1]=0;
		AT24CXX_Write(0,(u8*)RecState.SAVE_NUM,2);
		return;
	}
	
	while((RecState.SaveNum < (SaveTimesBak+100)) && (RecState.SaveNum < 60000)){
		sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(RecState.SaveNum-1)/20,RecState.SaveNum);
		sprintf(dir_buf,"0:/picture/%04d",(RecState.SaveNum-1)/20);
		if(f_opendir(&dr, dir_buf)==FR_OK){
			if(f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ )==FR_OK){
				SaveTimesBak = RecState.SaveNum;
				f_close(&fdst_f); 
			}
			f_closedir(&dr);
		}
		RecState.SaveNum ++;
	}
	
	RecState.SaveNum = SaveTimesBak;
	
	RecState.SAVE_NUM[0]=RecState.SaveNum&0xff;
	RecState.SAVE_NUM[1]=(RecState.SaveNum>>8)&0xff;
	AT24CXX_Write(0,(u8*)RecState.SAVE_NUM,2);
	
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	
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


u8 read_boot_bmp(void){
	u16 i,j,buf;	
	UINT bw_f; 

	if(f_mount(&fs,"0:",1)!=FR_OK)return 0;
	if(f_opendir(&dr, "0:/sys")!=FR_OK)return 0;
	
	res_f = f_open(&fdst_f, "0:/sys/BootGrap.bmp", FA_OPEN_EXISTING | FA_READ  );
	
	if( res_f != FR_OK )return 0;
	f_read(&fdst_f, RW_Buf, 54, &bw_f);
	if(check_str(RW_Buf,Image_head2,54))return 0;
	
	for(i=0;i<128;i++){
		f_lseek(&fdst_f,i*160*3+54);
		res_f = f_read(&fdst_f, RW_Buf, 480, &bw_f);  //读取480字
		if(res_f!=FR_OK)return 0;
		for(j=0;j<160;j++){
			buf=0xFFFF&((RW_Buf[0+j*3]&0xf8)>>3|(RW_Buf[1+j*3]&0xfC)<<3|(RW_Buf[2+j*3]&0xf8)<<8);
			RW_Buf[1+j*2]=0xff&(buf>>8);
			RW_Buf[0+j*2]=0xff&(buf);
		}
		LCD_Pic2(127-i,0,160,RW_Buf);
	}
	f_close(&fdst_f); 
	f_closedir(&dr);
	f_mount(&fs,NULL,1); 
	return 1;
}




//返回0xffff失败，返回数字为下一次的输入数字，首次数字为0
u16 read_saved(u16 num,u8 flag){		
	u16 i,j,Databuf;
	u8 filesw = 0xff;
	UINT bw_f;  
	long File_Byte;
	
	Databuf=0;
	
	if(RecState.SaveNum < 1)  
		return 0;
	
	if(RecState.SaveNum < num){
		if(flag == 1)
			num = 0;
	}
	
	if(0 == num){
		if(flag == 0)
			num = RecState.SaveNum+1;
	}
	
	if(f_mount(&fs,"0:",1)!=FR_OK)
		return 0;
	
	if(f_opendir(&dr, "0:/picture")!=FR_OK)
		return 0;
	
	if(flag == 0){
		
		num--;
		
		if(num == 0)
			num = RecState.SaveNum; //从头检查
		
		for(;;){
			sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(num-1)/20,num);
			sprintf(dir_buf,"0:/picture/%04d",(num-1)/20);
			if(f_opendir(&dr, dir_buf)==FR_OK){
				if(f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ )==FR_OK){
					f_read(&fdst_f, RW_Buf, 54, &bw_f);
					if(check_str(RW_Buf,Image_head,54) == 0){
						if(fdst_f.fsize == FileNow){  //check new
							filesw = 2;
							break;
						}else if(fdst_f.fsize >= FileBase){  //check new
							filesw = 1;
							break;
						}
					}
				}
			}
			f_close(&fdst_f);
			f_closedir(&dr);
			num--;
			if(num == 0){
				num = RecState.SaveNum; //从头检查
				sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(num-1)/20,num);
				sprintf(dir_buf,"0:/picture/%04d",(num-1)/20);
				if(f_opendir(&dr, dir_buf)==FR_OK){
					if(f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ )==FR_OK){
						f_read(&fdst_f, RW_Buf, 54, &bw_f);
						if(check_str(RW_Buf,Image_head,54) == 0){
							if(fdst_f.fsize == FileNow){  //check new
								filesw = 2;
								break;
							}else if(fdst_f.fsize >= FileBase){  //check new
								filesw = 1;
								break;
							}
						}
					}
				}
				return 0;   ///none img can read
			}
		}
		
	}else if(flag == 1){
		
		num++;
		
		if(num > RecState.SaveNum)
			num=1;
		
		for(;;){
			sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(num-1)/20,num);
			sprintf(dir_buf,"0:/picture/%04d",(num-1)/20);
			if(f_opendir(&dr, dir_buf)==FR_OK){
				if(f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ )==FR_OK){
					f_read(&fdst_f, RW_Buf, 54, &bw_f);
					if(check_str(RW_Buf,Image_head,54) == 0){
						if(fdst_f.fsize == FileNow){  //check new
							filesw = 2;
							break;
						}else if(fdst_f.fsize >= FileBase){  //check new
							filesw = 1;
							break;
						}
					}
				}
			}
			f_close(&fdst_f);
			f_closedir(&dr);
			num++;
			if(num > RecState.SaveNum){
				num = 1; //从头检查
				sprintf(name_buf,"0:/picture/%04d/%05d.bmp",(num-1)/20,num);
				sprintf(dir_buf,"0:/picture/%04d",(num-1)/20);
				if(f_opendir(&dr, dir_buf)==FR_OK){
					if(f_open(&fdst_f, name_buf, FA_OPEN_EXISTING | FA_READ )==FR_OK){
						f_read(&fdst_f, RW_Buf, 54, &bw_f);
						if(check_str(RW_Buf,Image_head,54) == 0){
							if(fdst_f.fsize == FileNow){  //check new
								filesw = 2;
								break;
							}else if(fdst_f.fsize >= FileBase){  //check new
								filesw = 1;
								break;
							}
						}
					}
				}
				return 0;   ///none img can read
			}
		}
		
		
	}
	
	
	if(filesw == 1){
		
		SysState.DispMeas = None;
		RecState.ReSet = 0;
		
		for(i=0;i<120;i++){   //读取图片区
			f_lseek(&fdst_f,i*120*3+54);  //改变指针
			f_read(&fdst_f, RW_Buf, 360, &bw_f);
			for(j=0;j<120;j++){
				Databuf=(0xFFFF&((RW_Buf[0+j*3]&0xf8)>>3|(RW_Buf[1+j*3]&0xfC)<<3|(RW_Buf[2+j*3]&0xf8)<<8));//更改格式
				RW_Buf[1+j*2]=0xff&(Databuf>>8);
				RW_Buf[0+j*2]=0xff&(Databuf);
			}
			LCD_Pic2(123-i,40,120,RW_Buf);
		}
		
		for(i=0;i<PixLg;i++){
			for(j=0;j<PixLg;j++){
				data[i][j] = 0xffff;
			}
		}
		
		ext[0]=ext[1]=ext[2]=0;
		ext_add[0]=0;
		ext_add[1]=63;
 		Draw_data();       //显示数据//显示清零
		
		for(i=0;i<18;i++){   //读取图片区46800
			f_lseek(&fdst_f,i*120*3+46863);  //改变指针
			f_read(&fdst_f, RW_Buf, 60, &bw_f);
			for(j=0;j<20;j++){
				Databuf=(0xFFFF&((RW_Buf[0+j*3]&0xf8)>>3|(RW_Buf[1+j*3]&0xfC)<<3|(RW_Buf[2+j*3]&0xf8)<<8));//更改格式
				RW_Buf[1+j*2]=0xff&(Databuf>>8);
				RW_Buf[0+j*2]=0xff&(Databuf);
			}
			LCD_Pic2(17-i,10,20,RW_Buf);
		}
		
		for(i=0;i<18;i++){   //读取图片区46800
			f_lseek(&fdst_f,i*120*3+47100);  //改变指针
			f_read(&fdst_f, RW_Buf, 60, &bw_f);
			for(j=0;j<20;j++){
				Databuf=(0xFFFF&((RW_Buf[0+j*3]&0xf8)>>3|(RW_Buf[1+j*3]&0xfC)<<3|(RW_Buf[2+j*3]&0xf8)<<8));//更改格式
				RW_Buf[1+j*2]=0xff&(Databuf>>8);
				RW_Buf[0+j*2]=0xff&(Databuf);
			}
			LCD_Pic2(127-i,10,20,RW_Buf);
		}
		
		f_lseek(&fdst_f,43254);  //改变指针
		f_read(&fdst_f, RW_Buf, 3, &bw_f);
		if(check_str(RW_Buf,Image_line_Iron,3) == 0){
			SysState.ColrMode = Iron;
		}else if(check_str(RW_Buf,Image_line_RB,3) == 0){
			SysState.ColrMode = RB;
		}else if(check_str(RW_Buf,Image_line_BW,3) == 0){
			SysState.ColrMode = BW;
		}
		Draw_color();
		
		f_close(&fdst_f); 
		f_closedir(&dr);
		f_mount(&fs,NULL,1); 
		
	}
	/*-------------新格式图片↓--------------*/	
	else if(filesw == 2){
		RecState.ReSet = 1;
		
		File_Byte = FileBase;
		f_lseek(&fdst_f,File_Byte);  //改变指针
		
// 		for(i=0;i<PixLg;i++){
// 			f_read(&fdst_f, data[i], PixLg*4, &bw_f);
// 			File_Byte+=PixLg*4;
// 			f_lseek(&fdst_f,File_Byte);
// 		}
		
		f_read(&fdst_f, PriData, 8*8*2, &bw_f);
		File_Byte+=8*8*2;
		f_lseek(&fdst_f,File_Byte);
		f_read(&fdst_f, RW_Buf, 2, &bw_f);
		
		f_close(&fdst_f); 
		f_closedir(&dr);
		f_mount(&fs,NULL,1); 
		
		SysState.ColrMode = RW_Buf[0];
		SysState.DispMeas  = RW_Buf[1];
		
// 		for(i=0;i<3;i++){
// 			ext[i] = (RW_Buf[i*4+2]<<24)|(RW_Buf[i*4+3]<<16)|(RW_Buf[i*4+4]<<8)|(RW_Buf[i*4+5]<<0);
// 		}
// 		
// 		ext_add[0]=RW_Buf[14];
// 		ext_add[1]=RW_Buf[15];
				
		GetImg();				//转换图像
		disp_fast();		//刷新图像
		
		
	}
	
// 	Draw_BackPlay();
	
	if(flag == 0)
		return num--;
	if(flag == 1)
		return num++;
	
	return 0;
}




void SaveIMG(void){
	
	if(SysState.SysFlag.bit.SaveFlag){
		SysState.SysFlag.bit.SaveFlag = 0;
		Draw_Wait();
		if(SD_Init()==0){
			save_bmp();
		}
		Draw_Camera();
	}
	
}



