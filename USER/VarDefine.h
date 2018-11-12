#ifndef _VarDefine_
#define _VarDefine_


// #define SIZEx5 5

#define SIZEx8 8  //插值数量选择


#define LcdOldVersion //新版的屏幕注释掉这一行。


#ifdef SIZEx5

#define PixLg 40   //軟件分辨率
#define PixGain 5    //軟件分辨率

#define FileBase 54414	//文件bmp區域容量	
#define FileNow 60830		//當前版本適配容量

#define t1 1000/5000     //gain
#define t2 2000/5000
#define t3 3000/5000
#define t4 4000/5000

#endif



#ifdef SIZEx8

#define PixLg 59   //軟件分辨率
#define PixGain 8    //軟件分辨率

#define FileBase 54414	//文件bmp區域容量	
#define FileNow 68354		//當前版本適配容量

#define t1 1000/8000     //gain
#define t2 2000/8000
#define t3 3000/8000
#define t4 4000/8000
#define t5 5000/8000
#define t6 6000/8000
#define t7 7000/8000

#endif

























#endif
