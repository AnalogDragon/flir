#ifndef _VarDefine_
#define _VarDefine_


// #define SIZEx5 5

#define SIZEx8 8  //��ֵ����ѡ��


#define LcdOldVersion //�°����Ļע�͵���һ�С�


#ifdef SIZEx5

#define PixLg 40   //ܛ���ֱ���
#define PixGain 5    //ܛ���ֱ���

#define FileBase 54414	//�ļ�bmp�^������	
#define FileNow 60830		//��ǰ�汾�m������

#define t1 1000/5000     //gain
#define t2 2000/5000
#define t3 3000/5000
#define t4 4000/5000

#endif



#ifdef SIZEx8

#define PixLg 59   //ܛ���ֱ���
#define PixGain 8    //ܛ���ֱ���

#define FileBase 54414	//�ļ�bmp�^������	
#define FileNow 68354		//��ǰ�汾�m������

#define t1 1000/8000     //gain
#define t2 2000/8000
#define t3 3000/8000
#define t4 4000/8000
#define t5 5000/8000
#define t6 6000/8000
#define t7 7000/8000

#endif


typedef enum DISPSTEP{
  Normal = 1,
  Pause,
  Play,
}DISPSTEP;


typedef enum DISPMODE{
  Iron = 1,
  RB,
  BW,
}DISPMODE;

typedef enum DISPMEAS{
  none = 1,
  midd,
  exts,
}DISPMEAS;


struct SysState_REG{
	u8 DispStep;
	u8 ColrMode;
	u8 ColrModeBak;
	u8 DispMeas;
	u8 SaveFlag;
	u8 RefreshFlag;
	u16 PlayNum;
	u16 SaveNum;
};

extern struct SysState_REG SysState;


#endif
