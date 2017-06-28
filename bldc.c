
/*BLDC.c*/
/*********************************************************************************************************
*
* File                : BLDC.c
* Hardware Environment: kech ver 1.0
* Build Environment   : RealView MDK-ARM  Version: 4.22
* Version             : V1.0
* By                  : lihewen
*
*                                  (c) Copyright 2012-, lihewen
*                                          All Rights Reserved
*
*********************************************************************************************************/
#include "stm32f10x.h"
#include "MC_Globals.h"
#define DUTY50  ((u16)0x0465)    //50%duty
//(1125+1035)/2250=96%; (1125+900)/2250=90%
#define TqMAX ((s16)1020) //900)	//((s16)+800) //+500) //50% ((s16)1035)96%((s16)1080)   //98%DUTY
#define TqMIN ((s16)-1020) //-800) //-675)	//((s16)-560) //25% //((s16)-675)20% // ((s16)-1080)  //
#define TqDEADBAND_P ((s16)30) //((s16)35) //((s16)27)  //((s16)33) //((s16)30)  //((s16)60)	//((s16)25)  //�����������
#define TqDEADBAND_N ((s16)-30) //((s16)-35) //((s16)-27)  //((s16)-33) //((s16)-30)  //((s16)-60)	//((s16)-25)
#define JITTER_P  ((s16)10) //((s16)15)  //0���䶯���� //((s16)5) //+/-5��㶶������
#define JITTER_N  ((s16)-10) //((s16)-15)     //((s16)-5)//
s16 JitterTestvalue = 15;  //10;   
#define FWRW_IMPP ((s16)50)  //����ת�������趨
#define FWRW_IMPN ((s16)-50)
 
s16 Lsector,Rsector,SectorGPIOread; //hall position
s16 Lsector_Last,Rsector_Last;

u16 pwm_U=DUTY50,pwm_V=DUTY50,pwm_W=DUTY50;  //50%duty init
//u16 pwm_RU=DUTY50,pwm_RV=DUTY50,pwm_RW=DUTY50;
s16 LTq=0,RTq=0;      //torque of L, R motor given by blance need

s16 LTqABS=0,RTqABS=0;
s16 Last_LTq=0,Last_RTq=0;
u16 pwmH=0,pwmL=0,pwmS=0;
s16 LpwmH_L=0; //LpwmL_H=0;  //up->down, down->up
s16 RpwmH_L=0; //,RpwmL_H=0;
s16 TupFlag = 3;
u32 RPerCommutCount,RPerHallCount,LPerCommutCount,LPerHallCount;
u16 Last_RPerCommutCount,Last_LPerCommutCount;
s16 Lrpm=0,Rrpm=0;
//s16 Lslope=0,Rslope=0;
//u16 LslopeCount=0,RslopeCount=0;
u8 Lfirstin=0,Rfirstin=0;
//u16 static LLast_pwm_U=DUTY50,LLast_pwm_V=DUTY50,LLast_pwm_W=DUTY50;
//u16 static RLast_pwm_U=DUTY50,RLast_pwm_V=DUTY50,RLast_pwm_W=DUTY50;
u8 CommutMode=0;
u8 LIbusHP1=0,LIbusHP2=0,RIbusHP1=0,RIbusHP2=0;
u16 LIbusHPcount=0,RIbusHPcount=0;
MOTOR_PARAM LMparam,RMparam;
u8 Sector_test=0;
s16 testLTq=0,testRTq=0;
u8  writeCmd_EN=0;
s16 Zhuanxiang_Counter;
u8  Zhuanxiang_State;
extern u8  outputEN;
u8 printsector=0;
//s16 LiA_sensor,LiB_sensor;
//s16 RiA_sensor,RiB_sensor;
u8 Gyro_Data_ready_PinState1=0,Gyro_Data_ready_PinState2=0;
u8 Gyro_Data_ready_PinState=0,Gyro_Data_ready_PinState_Prev=0;

s16 RIbus = 0;
s16 LIbus = 0;
s16 RIbus_Offset;
s16 LIbus_Offset;
s16 RIbus_LPF=0,LIbus_LPF = 0;
//u8 Sector_pp=0;

u32 ADC12_Buffer; //__attribute__((at(0X2000f10e0)))= {0};//ADC1-2,DMA���� ADC12_Buffer[15:0]=ADC1, ADC12_Buffer[31:16]=ADC2
u32 ADC3_Buffer[5]; // __attribute__((at(0X2000f130)))= {0};
u16 TimUpdateCount;
u8 Gyro_Data_ready;

s16 upLCommutCount=0,upRCommutCount=0,upLRCC=0;
u8	upLRCommutCount_Flg=0; //,upRCommutCount_Flg=0;


u16 Timer100ms;
u16 perLTpwmCount,perRTpwmCount;
u16 LTpwmCount,RTpwmCount;

u8 LHallCommutCount=0,RHallCommutCount=0;

s32 tmps32=0;
s16 Lsign=0,Rsign=0;	 //�ٶȷ���

s16 LRrpm = 0;	//�����ٶȺ�
s16 R_Lrmp = 0;	//�����ٶȲ�
s16 AcceLRrpm = 0;	//���ٶ�
s16 Last_LRrpm = 0;	 //������ٶȣ�����ǰ���ٶ�

s16 LPF_LRrpm,LPF_R_Lrmp,LPF_AcceLRrpm;
u8 rpm_get_ready=0;
u8 IbusOffsetRdy=0;
 
//CommutMode��
//0: normal LR231546����
//1: normal LR546231����
//2��normal L546231,R231546����
//3��normal L231546,R546231����
//4: LRT
//5: LT, R231546
//6: L546231 RT
//7: LRT(546231)
//2012-11-28
//1��T���β���С�������������飬�Ƚ����β��뷽��
//   ���β�������С�������Դ�
//2���Ƚ�546231��231546����˳������������һ��
extern void uart_Tx(void);
extern void Joystick(void);
void JoystickConversion(void);
extern void delay_us(u32 nus);

void TIM1_UP_IRQHandler(void) //TIM1�����жϷ�����
{
 s16 temp;
 CPU_LED_ON;
 TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);    //���жϱ�־λ
 //test
// GPIO_SetBits(GPIOE, GPIO_Pin_7);  //�ж��¼�����ʱ����� 22us 2012-12-06
 //�жϹ���ָʾ
 if((TimUpdateCount&0x1FFF)==0x0000)   //=4096 =0.256sec.//����0.512sec
 {
  //GPIO_SetBits(GPIOD, GPIO_Pin_4);	//CPU_LED
  //CPU_LED_ON;	//CPU_LED
  //GPIO_SetBits(GPIOE, GPIO_Pin_7); //for indicate
 }
 if((TimUpdateCount&0x1FFF)==0x1000)
 {
//  CPU_LED_OFF;
  if(outputEN==1)
  {
   //GPIO_ResetBits(GPIOD, GPIO_Pin_4);
   //CPU_LED_OFF;
   ;
   //GPIO_ResetBits(GPIOE, GPIO_Pin_7); //for indicate
  }
 }
 //ĸ�ߵ������:
 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC)==0);   //�ȴ�ADCת�����
 CPU_LED_OFF;
 ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
  RIbus = (s16)ADC1->JDR1;	 //IN0 ADC1
  LIbus = (s16)ADC2->JDR1;	 //IN1 ADC2
//�˲������������������ֲ������
// ADC_Cmd(ADC1, ENABLE); //ʹ��ָ����ADC1
// ADC_Cmd(ADC2, ENABLE); //ʹ��ָ����ADC2
 
 temp=LIbus-LIbus_Offset;
 if(Last_LTq<0)
 temp=-temp;
 LIbus_LPF *=7;	//15;
 LIbus_LPF +=temp; //LIbus;
 LIbus_LPF >>=3;	//4;
 //if(LIbus_LPF>(3380+LI_limt)) //3380)32A//2780)//2980)//24A  2780)//20A ���� >3080)//26A 
// if((LIbus_LPF>(1580+LI_limt))||(LIbus_LPF<-(1580+LI_limt)))
 if((LIbus_LPF>(1630+LI_limt))||(LIbus_LPF<-(1630+LI_limt)))
 {
  LIbus_Protect_Flag=1;
 }

 temp=RIbus_Offset-RIbus;
 if(Last_RTq<0)
 	temp=-temp;
 RIbus_LPF *=7;	//15;
 RIbus_LPF +=temp;  //RIbus;
 RIbus_LPF >>=3;	//4;
 //if(RIbus_LPF<(220-RI_limt)) //820)	//>2780) //620 24A  //220 32A
// if((RIbus_LPF>(1580+RI_limt))||(RIbus_LPF<-(1580+RI_limt)))
 if((RIbus_LPF>(1630+RI_limt))||(RIbus_LPF<-(1630+RI_limt)))
 {
  RIbus_Protect_Flag=1;
 }
// if(IbusOffsetRdy==0)
// {
// 	LIbus_Protect_Flag=0;
//	RIbus_Protect_Flag=0;
// }

 
 
 /*BLDC������� lihewen 2012-11-22*/
// SectorGPIOread = GPIOD->IDR;   //LEFT_HALL A-PD11,B-PD10,C-PD9 =(ABC) =ORG YHPCB
 SectorGPIOread = GPIOF->IDR;   //LEFT_HALL A-PF3,B-PF4,C-PF5   =(CBA) =JYSPCB
 SectorGPIOread >>= 0x0003;		
 SectorGPIOread &= 0x0007;
 switch(SectorGPIOread)
 {
  case 2:
   Lsector=1;
  break;
  case 6:	//3:
   Lsector=2;
  break;
  case 4:	//1:
   Lsector=3;
  break;
  case 5:
   Lsector=4;
  break;
  case 1:	//4:
   Lsector=5;
  break;
  case 3:	//6:
   Lsector=6;
  break;
  default:
  break;
 }

// SectorGPIOread = GPIOE->IDR;   //RIGHT_HALL A-PE5,B-PE4,C-PE3 =(ABC)
 SectorGPIOread = GPIOG->IDR;     //RIGHT_HALL A-PG6,B-PG7,C-PG8 =(CBA)
 SectorGPIOread >>= 0x0006;
 SectorGPIOread &= 0x0007;
 switch(SectorGPIOread)
 {
  case 2:
   Rsector=1;
  break;
  case 6:	//3:
   Rsector=2;
  break;
  case 4:	//1:
   Rsector=3;
  break;
  case 5:
   Rsector=4;
  break;
  case 1:	//4:
   Rsector=5;
  break;
  case 3:	//6:
   Rsector=6;
  break;
  default:
  break;
 }
 //50ms��ʱ���٣�  //�����Ϊ100ms���һ��,�ٶȼ���˻�ϵ��Ҳ��Ϊ*=80000;
 //50ms��ʱ���٣���������ʱ����Լ��50ms��ʱ�����м�λ��ʱ��
 //��ʱ�޷���⵽���ڶ�ʱ����50%�Ļ�������
 //��ˣ��ɿ������ѡ��100ms��ʱ��⣡
 Timer100ms++;
 perLTpwmCount++;	  //��PWM����������
 perRTpwmCount++;	  //��PWM����������

 if(Timer100ms>=1600)	 //�����Ϊ100ms���һ��,�˴���Ϊ1600;
 {
 	Timer100ms=0;		  //��ʱ����λ
	if((LHallCommutCount>=2)&&(LTpwmCount>400))
	{
//		if(LTpwmCount<10)
//			LTpwmCount=10;
		tmps32 = (LHallCommutCount-1);
		tmps32 *=80000;				//�����Ϊ100ms���һ��,�˴���Ϊ*=160000;
		tmps32 /=LTpwmCount;

		Lrpm = (s16)tmps32;
		Lrpm *= Lsign;
	}
	else
		Lrpm = 0;

	if((RHallCommutCount>=2)&&(RTpwmCount>400))
	{
//		if(RTpwmCount<100)
//			RTpwmCount=100;
		tmps32 = (RHallCommutCount-1);
		tmps32 *=80000;
		tmps32 /=RTpwmCount;

		Rrpm = (s16)tmps32;
		Rrpm *= Rsign;
	}
	else
		Rrpm = 0;

	LHallCommutCount=0;	  //�����������ֵ��λ
	RHallCommutCount=0;	  //�һ����������ֵ��λ
	LTpwmCount=0;	  //��PWM����������ֵ��λ
	RTpwmCount=0;	  //��PWM����������ֵ��λ
	rpm_get_ready=1;
	
	
 }

 if(Lsector_Last != Lsector)
 {
 	LHallCommutCount++;
 	if(LHallCommutCount==1)	 //��⵽��һ�������ź�
	{
		perLTpwmCount=0;
	}
//	else if(LHallCommutCount>=2)
	LTpwmCount = perLTpwmCount;	 //���滻��ʱ��PWM����������ֵ
	if(((Lsector_Last-Lsector)==1)||((Lsector-Lsector_Last)==5))
		Lsign=-1;
	else
		Lsign=1;
 }

 if(Rsector_Last != Rsector)
 {
 	RHallCommutCount++;
 	if(RHallCommutCount==1)	 //��⵽��һ�������ź�
	{
		perRTpwmCount=0;
	}
//	else if(RHallCommutCount>=2)
	RTpwmCount = perRTpwmCount;	 //���滻��ʱ��PWM����������ֵ
	if(((Rsector-Rsector_Last)==1)||((Rsector_Last-Rsector)==5))
		Rsign=-1;
	else
		Rsign=1;
 }

 

// //���٣�RPerCommutCount,RPerHallCount,LPerCommutCount,LPerHallCount;
// if(RPerCommutCount<80001)
//  RPerCommutCount++;
// if(LPerCommutCount<80001)
//  LPerCommutCount++;
// //u16 Last_RPerCommutCount,Last_LPerCommutCount;
// //s16 Last_LTq=0,Last_RTq=0;
// if(Lsector_Last != Lsector)
// {
//  //print sector 
//  printsector=1;
//  //�������β�:
//  Lfirstin = 1; 
//  Last_LPerCommutCount = LPerCommutCount;  //��������ǰ���������ڼ���ֵ
////  Last_LTq = LTq;    //����������������ǰ����ֵ
//  //��������:(ǰ��123456)
//  if(LPerCommutCount==0) //��0ֵ����
//  	LPerCommutCount=1;     
//  Lrpm = (s16)(80000/LPerCommutCount);	  //ת��rpm��ת/��
//
//  if(LPerCommutCount>160)
//  	LPerCommutCount=0;
//  upLCommutCount =(s16)LPerCommutCount;
//  upLRCommutCount_Flg=1;
//
//
//  if(((Lsector_Last-Lsector)==1)||((Lsector-Lsector_Last)==5))
//  {
//   Lrpm *= -1;
//   upLCommutCount =-upLCommutCount;
//  }
//  LPerCommutCount=0;
// }
// else if(LPerCommutCount>80000)
// { //��������
//  Lrpm = 0;
// }
// 
// if(Rsector_Last != Rsector)
// {
//  //�ҵ�����β�:
//  Rfirstin=1;
//  //printsector=1;
//
//  Last_RPerCommutCount = RPerCommutCount;  //�����ҵ��ǰ���������ڼ���ֵ
//  //Last_RTq = RTq;   //�����ҵ����������ǰ����ֵ
//  //�ҵ������:
//  if(RPerCommutCount==0)   //��0����
//  	RPerCommutCount=1;
//  Rrpm = (u16)(80000/RPerCommutCount);
//
//  if(RPerCommutCount>160)
//  	RPerCommutCount=0;
//  upRCommutCount=(s16)RPerCommutCount;
//  upLRCommutCount_Flg=1;
//
//  if(((Rsector-Rsector_Last)==1)||((Rsector_Last-Rsector)==5))
//  {
//   Rrpm *= -1;
//   upRCommutCount =-upRCommutCount;
//  }
// 
//  RPerCommutCount=0;
// }
// else if(RPerCommutCount>80000)
// { //��������
//  Rrpm = 0;
// }
// if(upLRCommutCount_Flg)
// {
// 	upLRCC *=7;
// 	upLRCC += (upLCommutCount+upRCommutCount);
//	upLRCC >>=3;
////	if(upLRCC>120)
////		Upcomp = (upLRCC-120)*3;
////	else if(upLRCC<-120)
////		Upcomp = (upLRCC+120)*3;
// }

 Lsector_Last = Lsector;
 Rsector_Last = Rsector;
 
 //��ȡ���ظ���ֵ
// if(LTqok)  //��������ֵ�����Ч
// if(RTqok)
//s16 LMotorCmd=0; RMotorCmd=0;
// writeCmd_EN=0;
 if(Anglek_FLAG)  //�����ظ���ֵ
 {
//  if(TupFlag>=3)	 //���Ե��	   //test0422
//  {
	  testRTq = RMotorCmd; //>>2;  //test0619
	  testLTq = LMotorCmd; //>>2;  //test0619
//  }
  Anglek_FLAG = 0; 
 }
 RTq = testRTq;
 LTq = testLTq;
 

  
 //�����������
 //5(101) 4(100) 6(110) 2(010) 3(011) 1(001)
 //A+B-  A+C-    B+C-  B+A-  C+A-  C+B-
  //�����䶯���� [-15,+15]
  if(RTq>=JitterTestvalue) //JITTER_P)
   RTq -= JitterTestvalue; //JITTER_P;
  else if(RTq<=-JitterTestvalue)  //JITTER_N)
   RTq += JitterTestvalue;   //JITTER_P;
  else
   RTq =0;
  if(LTq>= JitterTestvalue)  //JITTER_P)
   LTq -=JitterTestvalue;  //JITTER_P;
  else if(LTq<=-JitterTestvalue) //JITTER_N)
   LTq += JitterTestvalue;  //JITTER_P;
  else
   LTq =0;
  //���������������  TqDEADBAND_P TqDEADBAND_N
  //�����ҵ������������ͬ,��Ӧ�ֱ����������
  if(RTq>0)
   RTq += TqDEADBAND_P;
  else if(RTq<0)
   RTq += TqDEADBAND_N;
  else
  {
   RTq = 0;
  }
  if(LTq>0)
   LTq += TqDEADBAND_P;
  else if(LTq<0)
   LTq += TqDEADBAND_N;
  else
  {
   LTq = 0;
  }
  //��������ת������

  Last_LTq = LTq;
  Last_RTq = RTq;
  //��ֵ����
  if(RTq>TqMAX)  
   RTq=TqMAX;
  if(RTq<TqMIN)  
   RTq=TqMIN;
  if(LTq>TqMAX)  
   LTq=TqMAX;
  if(LTq<TqMIN)  
   LTq=TqMIN;
 

  //if((LTq>-190)&&(LTq<190))	  //��������ʵ��2013-06-20
  if((LTq>-300)&&(LTq<300))		 //�����������л����� 2013-06-29�޸�
  	CommutMode=0;
  else
  	CommutMode=1;  
  pwmH = DUTY50 + LTq;
  pwmL = DUTY50 - LTq;//
//  pwmS = DUTY50 + LpwmH_L;
//CommutMode��
//0: normal LR231546����
//1: LRT(231546)����
  switch(Lsector)
  {//���ֶ������ʱ��,����˳��546231
   //������������ת���ҵ����ת(�൱�������غ���),���ڻ�����ƽ��������,~010=101ԭ��
   //�޸�Ϊ������������ת���ҵ����ת(�൱��������ǰ��)2012-11-27
   //           hallֵ=~ʵ������ֵ: 
   case 0x0001: //0x0002: //0x0005: //A+B-    010 = ~101
   	 pwm_U = pwmH;
     pwm_V = pwmL;
     pwm_W = DUTY50;
   //CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_OFF;  
	}				   

   break;
 
   case 0x0002: //0x0003: //0x0004: //A+C-    100 = ~011
     pwm_U = pwmH;
     pwm_V = DUTY50;
     pwm_W = pwmL;
   //CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_ON;
		LV_SD_OFF;
		LW_SD_ON;  
	}
   break;
 
   case 0x0003: //0x0001: //0x0006: //B+C-    110 = ~001
     pwm_U = DUTY50;
     pwm_V = pwmH;
     pwm_W = pwmL;
   //CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_OFF;
		LV_SD_ON;
		LW_SD_ON;  
	}
   break;
 
   case 0x0004: //0x0005: //0x0002: //B+A-    010 = ~101
     pwm_U = pwmL;
     pwm_V = pwmH;
     pwm_W = DUTY50;
    //CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_OFF;  
	}
   break;
 
   case 0x0005: //0x0004: //0x0003: //C+A-   011 = ~100	   
     pwm_U = pwmL;
     pwm_V = DUTY50;
     pwm_W = pwmH;
	//CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_ON;
		LV_SD_OFF;
		LW_SD_ON;  
	}
   break;
 
   case 0x0006: //0x0001: //C+B-   001 = ~110
     pwm_U = DUTY50;
     pwm_V = pwmL;
     pwm_W = pwmH;
   //CommutMode:
    if(CommutMode==0)
    {
		LU_SD_ON;
		LV_SD_ON;
		LW_SD_ON;     
    }
	else
	{
		LU_SD_OFF;
		LV_SD_ON;
		LW_SD_ON;  
	}
   break;
 
   default:
     pwm_U = DUTY50;
     pwm_V = DUTY50;
     pwm_W = DUTY50;
   break; 
  }
//  LLast_pwm_U=pwm_U;
//  LLast_pwm_V=pwm_V;
//  LLast_pwm_W=pwm_W;
  TIM8->CCR1 = pwm_U; //pwm_W;
  TIM8->CCR2 = pwm_V;
  TIM8->CCR3 = pwm_W; //pwm_U;

  CPU_LED_ON;
 //�ҵ���������
//  if((RTq>-190)&&(RTq<190))	 //��������ʵ��2013-06-20
  if((RTq>-300)&&(RTq<300))		//�����������л����� 2013-06-29�޸�
  	CommutMode=0;
  else
  	CommutMode=1; 
  pwmH = DUTY50 + RTq;
  pwmL = DUTY50 - RTq;
  switch(Rsector)
  {//���ֶ������ʱ��,����˳��546231
   //������������ת���ҵ����ת(�൱�������غ���),���ڻ�����ƽ��������,~010=101ԭ��
   //�޸�Ϊ������������ת���ҵ����ת(�൱��������ǰ��)2012-11-27
   //           hallֵ=~ʵ������ֵ: 
   case 0x0001: //0x0002: //0x0005: //A+B-    010 = ~101
     pwm_U = pwmH;
     pwm_V = pwmL;
     pwm_W = DUTY50;
   //CommutMode:
    if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_OFF;
	}
   break;
 
   case 0x0002: //0x0003: //0x0004: //A+C-    100 = ~011   
     pwm_U = pwmH;
     pwm_V = DUTY50;
     pwm_W = pwmL;
	//CommutMode:
	if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_ON;
		RV_SD_OFF;
		RW_SD_ON;
	}
   break;
 
   case 0x0003: //0x0001: //0x0006: //B+C-    110 = ~001
     pwm_U = DUTY50;
     pwm_V = pwmH;
     pwm_W = pwmL;

	//CommutMode:
	if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_OFF;
		RV_SD_ON;
		RW_SD_ON;
	}
   break;
 
   case 0x0004: //0x0005: //0x0002: //B+A-    010 = ~101
     pwm_U = pwmL;
     pwm_V = pwmH;
     pwm_W = DUTY50;
	//CommutMode:
	if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_OFF;
	}
   break;
 
   case 0x0005: //0x0004: //0x0003: //C+A-   011 = ~100
     pwm_U = pwmL;
     pwm_V = DUTY50;
     pwm_W = pwmH;
	//CommutMode:
	if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_ON;
		RV_SD_OFF;
		RW_SD_ON;
	}
   break;
 
   case 0x0006: //0x0001: //C+B-   001 = ~110
     pwm_U = DUTY50;
     pwm_V = pwmL;
     pwm_W = pwmH;
	//CommutMode:
	if(CommutMode==0)
    {
		RU_SD_ON;
		RV_SD_ON;
		RW_SD_ON; 
    }
	else
	{
		RU_SD_OFF;
		RV_SD_ON;
		RW_SD_ON;
	}
   break;
 
   default:
     pwm_U = DUTY50;
     pwm_V = DUTY50;
     pwm_W = DUTY50;
   break; 
  }
  //
//  RLast_pwm_U=pwm_U;
//  RLast_pwm_V=pwm_V;
//  RLast_pwm_W=pwm_W;
  TIM1->CCR1 = pwm_U; //pwm_W;    //TIM1�ҵ��,ת��+=˳ʱ��
  TIM1->CCR2 = pwm_V;
  TIM1->CCR3 = pwm_W; //pwm_U;
  //CPU_LED_ON;
 //��������:
 if(LIbus_Protect_Flag|RIbus_Protect_Flag|MaxAnglekProtect)
 {
	 TIM8->CCR1 = DUTY50;     //TIM8����
	 TIM8->CCR2 = DUTY50;
	 TIM8->CCR3 = DUTY50;
	 
	 TIM1->CCR1 = DUTY50;     //TIM1�ҵ��
	 TIM1->CCR2 = DUTY50;
	 TIM1->CCR3 = DUTY50;
	 //��ʱ10ms�ر�SD
	RU_SD_OFF;
	RV_SD_OFF;
	RW_SD_OFF;
	LU_SD_OFF;
	LV_SD_OFF;
	LW_SD_OFF; 
 } 
 //uart_Tx();
 if((TimUpdateCount&0x1)==0) //2��pwm���ڷ���һ���ַ�
 { 
 	uart_Tx(); 
 }
 TimUpdateCount++;

 //��������ʱ:
 if(keycount<TimeOut_100ms)    //�̰�����ʱ
  keycount++;
 else if((keycount<TimeOut_2000ms)&&(keyState==3)) //��������ʱ
  keycount++;

// GPIO_ResetBits(GPIOE, GPIO_Pin_7); //�ж��¼�����ʱ�����
	No_Gyro_Data_Count++;
 CPU_LED_OFF;
}


void IBusOffsetInit(void)	 //2013-01-15
{
	static s32 itempL,itempR;
	u16 n;
	while(RIbus<1500);	   //�ȴ������������������
	while(RIbus<1500);
	IbusOffsetRdy=0;
	LIbus_Offset=1800;
	RIbus_Offset=1800;
	delay_us(100);
	LIbus_Protect_Flag=0;  //0615 ����ȶ�֮ǰ�ĵ�������ֵ
	RIbus_Protect_Flag=0; 
	for(n=0;n<256;n++)
	{
		itempL+=LIbus;
		itempR+=RIbus;
		delay_us(100);
	}
	itempL/=256;
	LIbus_Offset=(s16)itempL;
	LIbus_Offset-=7;
	itempR/=256;
	RIbus_Offset=(s16)itempR;
	//RIbus_Offset -=50;

	IbusOffsetRdy=1;
	delay_us(1000);
	LIbus_Protect_Flag=0;  //0615 ����ȶ�֮ǰ�ĵ�������ֵ
	RIbus_Protect_Flag=0; 
}

 
/*
��Ӧ�����ת���򣬻���λ���ź��빦�ʹܵ�ͨ��ϵ��
  HaHbHc ��ʱ��ת��  ˳ʱ��ת��
��5�� 101  A+B-   B+A-
��4�� 100  A+C-   C+A-
��6�� 110  B+C-   C+B-
��2�� 010  B+A-   A+B-
��3�� 011  C+A-   A+C-
��1�� 001  C+B-   B+C-
*/
 
 
