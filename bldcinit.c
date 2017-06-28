/*bldcinit.c*/
//xx0410
/*********************************************************************************************************
*
* File                : bldcinit.c
* Hardware Environment: kech ver 1.0
* Build Environment   : RealView MDK-ARM  Version: 4.22
* Version             : V1.0
* By                  : lihewen
*
*                                  (c) Copyright 2012-, lihewen
*                                          All Rights Reserved
*
*********************************************************************************************************/
/******************************************************************************************************** 
STM32_TIM_ADC_Configuration()
������TIM1��TIM8,ADC1��ADC2��ADC3��ʼ������
��;�������������PWM��ADC����ָ���˿ڵ������ѹ
ʹ�������� TIM,ADC��ʼ�����ã�
TIM1�ܽ���ӳ�䣬TIM1_CH1,TIM1_CH1N,TIM1_CH2,TIM1_CH2N,TIM1_CH3,TIM1_CH3N�������
    TIM8_CH1,TIM8_CH1N,TIM8_CH2,TIM8_CH2N,TIM8_CH3,TIM8_CH3N�������
TIM1--GPIOE(Right Notor): PE8:PE9 = UL:UH, PE10:PE11= VL:VH, PE12:PE13 = WL:WH;   
TIM8--GPIOx(Lift Notor):  UL:VL:WL = PA7:PB0:PB1; UH:VH:WH = PC6:PC7:PC8;
����ʱ������Ϊ�㣬����TIM_BDTRInitStructure.TIM_DeadTime = 0x00; ��������
ADC1��ADC2������ͬ�������ͬ��ע��ģʽ��������ͨ��ת�����������������ע����ͨ��ת����TIM1_TRGO  Update�¼���������
(1)ADC1 ����1�����ͨ��  ADC_Channel_14 
(2) ADC2 ����1�����ͨ��  ADC_Channel_15  
(3)������ת�����������������ͬ������ADC1��ADC2
ADC1ע��ͨ������ADC123_IN0 ��ADC2ע��ͨ������ADC123_IN3  ������U��V �����ͬʱ������ͨ���л��� TIM1�����¼��ж��
               �ж��¼�����ע��������������ȴ�������ɣ��л�����ͨ��
ADC3 �����ڶ���״̬���������������ADC3һ�δ���ת��5��ͨ��ADC3, ADC_Channel_5��ADC3, ADC_Channel_7��
         ADC3, ADC_Channel_10��ADC3, ADC_Channel_11��ADC3, ADC_Channel_13
ADC1��ADC3������DMA���䣬ת�������ݴ�ADC_DR�Ĵ������䵽�û�ָ����Ŀ�ĵ�ַ��ADC1��ADC2����DMA 
            ADC2����ͨ������ΪADC_DR���16λ����16ΪΪADC1���� 
ADC1��ADC2������ͨ��ת�������ڵ�ַΪ0X20000100 SRAM���16λΪADC2���ݣ���16λΪADC1����
ADC1��ADC2ע����ͨ��ת�������ڷֱ���ADC1->JDR1��ADC2->JDR1�С��������ҵ�����ݷ�ʱ����ADC1->JDR1��ADC2->JDR1
ADC3 ������ͨ��ת����������ʼ��ַΪ0X20000130 SRAM�� ����Ϊ32λ����12λ��Ч һ��5�����ݣ�5*4��ÿһ������4���ֽ�32λ��

˵��:
ADC1��ADC2ע����ͨ��ת��,ע����ͨ��ת����TIM1_TRGO  Update�¼�����ADC1��ADC2 ͬʱ���� һ��PWM��������һ�Ρ�
    ��һ�����ڲ����������ڶ������ڲ����ҵ�����������ֲ����������������
ADC1  LEFT_U_I_ADC  ADC123_IN0 PA0
ADC2  LEFT_V_I_ADC  ADC123_IN0 PA3
ADC1  RIGHT_U_I_ADC ADC123_IN0 PA1
ADC2  RIGHT_V_I_ADC ADC123_IN0 PA2
ADC1��ADC2������ͨ��ת�� ����������ƣ�һ������ͬʱת��ADC1��ADC2 ���ú���ADC_SoftwareStartConvCmd(ADC1, ENABLE);//����ADC1 ADC2������ת��
ADC1 ��ĸ�ߵ������   ADC12_IN14  PC4
ADC2 ��ĸ�ߵ������   ADC12_IN15  PC5
ADC3 ������ͨ��ת��  ����������ƣ�һ������ת��5�� ���ú���ADC_SoftwareStartConvCmd(ADC3, ENABLE);//����ADC3������ת��
�����¶Ȳ���   ADC3_IN5  PF7
�ҵ���¶Ȳ���   ADC3_IN7  PF9
3.3V��Դ��ѹ��� ADC123_IN10  PC0
ĸ�ߵ�ѹ���     ADC123_IN11  PC1
����ADC          ADC123_IN13  PC3
*******************************************************************************/
#include "stm32f10x.h"        //���ͷ�ļ�����STM32F10x������Χ�Ĵ�����λ���ڴ�ӳ��Ķ���
#include "MC_Globals.h"
//u8 a=0;
u32 ADC12_Buffer; //__attribute__((at(0X2000f10e0)))= {0};//ADC1-2,DMA���� ADC12_Buffer[15:0]=ADC1, ADC12_Buffer[31:16]=ADC2
//
u32 ADC3_Buffer[5]; // __attribute__((at(0X2000f130)))= {0};
//ADC12_Buffer[15:0]=ADC1, --PC4��ĸ�ߵ���
//ADC12_Buffer[31:16]=ADC2,--PC5��ĸ�ߵ���
//u32 *pADC12_Buffer = ADC12_Buffer; //= (u16)&ADC12_Buffer;
//u32 x,y; //x = L/R U��Ia. y = L/R V��Ib.
u16 TimUpdateCount;
u8 Gyro_Data_ready;
/*#define Deadtime Value   */ //(u16)((unsigned long long)CKTIM/2 *(unsigned long long)DEADTIME_NS/1000000000uL)
//#define DEADTIME    ((u16) 30) //define deadtime =800ns   //DTG[7:0]x27.778 ns. @TIM_CKD_DIV2,(tDTS = 2 �� tCK_INT); DTG[7:5]=0xx => DT=DTG[7:0]x tdtg with tdtg=tDTS.  
         //DEADTIME*(1/(72(MHz)/2))= DEADTIME_NS  => DEADTIME = (DEADTIME_NS(ns)//1000) * (72(MHz)/2)
//#define DEADTIME    ((u16) 36)  //define deadtime =1000ns
//#define DEADTIME    ((u16) 43)  //define deadtime =1200ns
//#define DEADTIME    ((u16) 54)  //define deadtime =1500ns  //���ڵ���ʱ��ѡ��Щ
//extern void SVPWM_CalcDutyCycles(void); //xx0410
//extern void uart_RxTx(void);
//extern void uart_Tx(void);
//extern void uart_Rx(void);
/*GPIO�ܽŵ�����*/
//ѡ��ADC��ͨ��0  1  2  3  0  1  3  4  5  7  9���ֱ��Ӧ�Ĺܽ�ΪPA0  PA1  PA2  PA3   PC0  PC1  PC3  PC4  PC5  PF7  PF9
//(PA0 : PA1 : PA2 : PA3 = LEFT_U_I_ADC : RIGHT_U_I_ADC : RIGHT_V_I_ADC : LEFT_V_I_ADC) lihewen 2012-09-17
//ADC1ע��: ��/�ҵ��Ia���(L:R = PA0:PA1 = CH0:CH1)   //lihewen 2012-09-20
//ADC2ע��: ��/�ҵ��Ib���(L:R = PA3:PA2 = CH3:CH2)
//ADC1 ��ĸ�ߵ������   ADC12_IN14  PC4
//ADC2 ��ĸ�ߵ������   ADC12_IN15  PC5
//�����¶Ȳ���   ADC3_IN5  PF7
//�ҵ���¶Ȳ���   ADC3_IN7  PF9
//3.3V��Դ��ѹ��� ADC123_IN10  PC0
//ĸ�ߵ�ѹ���     ADC123_IN11  PC1
//����ADC          ADC123_IN13  PC3
//void Tim_ADC_GPIO_Configuration(void)	 //xx0410
//{
// GPIO_InitTypeDef  GPIO_InitStructure;
////ADC_GPIO_Configuration 
// //PA0/1/2/3 ��Ϊģ��ͨ����������                         
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;     //ģ����������
// GPIO_Init(GPIOA, &GPIO_InitStructure);
// //PC0/1/3/4/5 ��Ϊģ��ͨ����������                         
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;     //ģ����������
// GPIO_Init(GPIOC, &GPIO_InitStructure);
// //joystick: encת��
//// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_6 ;      //ת��2PULSE����TURN_ENCODER_2PULSE
//// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     
//// GPIO_Init(GPIOG, &GPIO_InitStructure);
////
//// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;     //ת��INDEX�ź� TURN_ENCODER_INDEX
//// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     
//// GPIO_Init(GPIOC, &GPIO_InitStructure);
////
//// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;     //ת��DIRECT�ź� TURN_ENCODER_DIRECT
//// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     
//// GPIO_Init(GPIOD, &GPIO_InitStructure);
//
// //PF/7/9 ��Ϊģ��ͨ����������                         
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_9;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;     //ģ����������
// GPIO_Init(GPIOF, &GPIO_InitStructure);
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;      //�ҵ��Ӳ����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //������������
// GPIO_Init(GPIOF, &GPIO_InitStructure);
// //GPIOF,GPIO_Pin_10--�ҽ�̤���� 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      //��Footswitch
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;     //������������
// GPIO_Init(GPIOF, &GPIO_InitStructure);
// 
////TIM_GPIO_Configuration
// //����TIM1��TIM8.GPIO����Ӧ���ã�ΪAF���
// //(TIM8 = Lift Motor PWM; UL:VL:WL = PA7:PB0:PB1; UH:VH:WH = PC6:PC7:PC8)
// //PA.7/������ΪTIM8_CH1N   
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //���츴�����ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOA, &GPIO_InitStructure);
// 
// //PB.0/PB.1������ΪTIM8_CH2N��TIM8_CH3N�����
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //���츴�����ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
// GPIO_Init(GPIOB, &GPIO_InitStructure);
// 
// //PC.6/7/8������ΪTIM8_CH1��TIM8_CH2��TIM8_CH3�����
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7|GPIO_Pin_8;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //���츴�����ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure);
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //����Ӳ����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        //��������ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure);
// 
// //PE.8/9/10/11/12/13������ΪTIM1_CH1��TIM1_CH2��TIM1_CH3//TIM1_CH1N��TIM1_CH2N��TIM1_CH3N�����
// //(TIM1--GPIOE(Right Notor): PE8:PE9 = UL:UH, PE10:PE11= VL:VH, PE12:PE13 = WL:WH;)
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_9 | GPIO_Pin_10|GPIO_Pin_11 |
//               GPIO_Pin_12 | GPIO_Pin_13;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //���츴�����ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOE, &GPIO_InitStructure);
// //PE.1=Beep;PE.2=PWM_RIGHT_SD; PE.7=LED IND����Ϊ����� 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_2|GPIO_Pin_1; //PWM_SD;//ָʾ�����¼��ж�
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������ģʽ
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOE, &GPIO_InitStructure);
// 
// BEEP_OFF;
//}
 

/*����ϵͳʱ��,ʹ�ܸ�����ʱ��*/
//void Tim_ADC_RCC_Configuration(void) //xx0410
//{
//// SystemInit(); 
// RCC_PCLK2Config(RCC_HCLK_Div1);
//     /*ʹ�ܸ�������ʱ��*/
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE|
//    RCC_APB2Periph_GPIOF |RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2|RCC_APB2Periph_ADC3 , ENABLE );   //ʹ��ADC1ͨ��ʱ�ӣ������ܽ�ʱ��
// /* Configure ADCCLK such as ADCCLK = PCLK2/6 */ 
// RCC_ADCCLKConfig(RCC_PCLK2_Div8);          //72M/6=12,ADC���ʱ�Ӳ��ܳ���14M
//// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    //ʹ��DMA1ʱ��
// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);    //ʹ��DMA2ʱ��
// //����AFIO
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
// GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);     //TIM1�ܽ���ӳ��
// //����TIM1��TIM8
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
//  
//}
 
/*����ADC1*/
void ADC_Configuration(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
 /* ADC1 configuration ------------------------------------------------------*/
 ADC_InitStructure.ADC_Mode = ADC_Mode_RegInjecSimult ;   //ADC����ģʽ:ADC1��ADC2������ͬ�������ͬ��ע��ģʽ
 ADC_InitStructure.ADC_ScanConvMode =ENABLE;      //ģ��ת��������ɨ��ģʽ
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    //ģ��ת������������ת��ģʽ
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;//ת��������������ⲿ��������
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //ADC�����Ҷ���
 ADC_InitStructure.ADC_NbrOfChannel = 1;       //˳����й���ת����ADCͨ������Ŀ
 ADC_Init(ADC1, &ADC_InitStructure);        //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
 //ADC1����ͨ������ 
 /* ADC1 regular channel configuration */ 
 ADC_DiscModeChannelCountConfig(ADC1, 1);       //���ģʽ������ͨ����������ֵΪ1
 //��ĸ�ߵ������ ADC1, ADC_Channel_14
 ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_1Cycles5 );  //_1Cycles5,_7Cycles5,_13Cycles5,_28Cycles5...
 ADC_DiscModeCmd(ADC1, ENABLE);          //ʹ�ܼ��ģʽ
 ADC_ExternalTrigConvCmd(ADC1, ENABLE);       //ʹ�ܹ������ⲿ����
 //ADC1ע��ͨ������ 
 ADC_InjectedSequencerLengthConfig(ADC1, 1);       //ע����ͨ����ת�����г���1
 ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);  
 ADC_InjectedDiscModeCmd(ADC1, ENABLE);         //ע������ģʽ
 ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO); //ѡ��ʱ��1�Ĵ��������Ϊע��ת���ⲿ����
 ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE); //ʹ���ⲿ����
 
/* ADC2 configuration ------------------------------------------------------*/
 ADC_InitStructure.ADC_Mode = ADC_Mode_RegInjecSimult;   //ADC����ģʽ:ADC1��ADC2������ͬ�������ͬ��ע��ģʽ
 ADC_InitStructure.ADC_ScanConvMode =ENABLE;      //ģ��ת��������ɨ��ģʽ
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    //ģ��ת������������ת��ģʽ
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //ADC�����Ҷ���
 ADC_InitStructure.ADC_NbrOfChannel = 1;       //˳����й���ת����ADCͨ������Ŀ
 ADC_Init(ADC2, &ADC_InitStructure);        //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
 //ADC2����ͨ������
 ADC_DiscModeChannelCountConfig(ADC2, 1);       //���ģʽ������ͨ����������ֵΪ1
 //��ĸ�ߵ������ ADC2, ADC_Channel_15
 ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 1, ADC_SampleTime_1Cycles5 );  //_1Cycles5,_7Cycles5,_13Cycles5,_28Cycles5...
 ADC_ExternalTrigConvCmd(ADC1, ENABLE);
 ADC_DiscModeCmd(ADC2, ENABLE);          //ʹ�ܼ��ģʽ
 //ADC2ע��ͨ������
 ADC_InjectedSequencerLengthConfig(ADC2, 1);       //ע����ͨ����ת�����г���4 
 ADC_InjectedChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);  //ͬһͨ���ɼ��Ĵ�
 ADC_InjectedDiscModeCmd(ADC2, ENABLE);         //ע������ģʽ
 ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_None); //ע��ת��������������ⲿ��������
 ADC_ExternalTrigInjectedConvCmd(ADC2, ENABLE);      //ʹ���ⲿ����

 /* ADC3 configuration ------------------------------------------------------*/
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;    //����ģʽ
 ADC_InitStructure.ADC_ScanConvMode =ENABLE;      //ģ��ת��������ɨ��ģʽ
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    //ģ��ת������������ת��ģʽ
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;//ת��������������ⲿ��������
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //ADC�����Ҷ���
 ADC_InitStructure.ADC_NbrOfChannel = 5;       //˳����й���ת����ADCͨ������Ŀ
 ADC_Init(ADC3, &ADC_InitStructure);        //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
 //ADC3����ͨ������ 
 ADC_DiscModeChannelCountConfig(ADC3, 5);       //���ģʽ������ͨ����������ֵΪ1
 ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_1Cycles5 );   //_1Cycles5,_7Cycles5,_13Cycles5,_28Cycles5...
 ADC_RegularChannelConfig(ADC3, ADC_Channel_7, 2, ADC_SampleTime_1Cycles5 );
 ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 3, ADC_SampleTime_1Cycles5 );
 ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 4, ADC_SampleTime_1Cycles5 );
 ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 5, ADC_SampleTime_1Cycles5 );  //����ADC
 ADC_DiscModeCmd(ADC3, ENABLE);          //ʹ�ܼ��ģʽ
 ADC_ExternalTrigConvCmd(ADC3, ENABLE);        //ʹ�ܹ������ⲿ����
 // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
    ADC_DMACmd(ADC1, ENABLE);
 ADC_DMACmd(ADC3, ENABLE);
 
 /* Enable ADC1/ADC2 */
  ADC_Cmd(ADC1, ENABLE);              //ʹ��ָ����ADC1
  ADC_Cmd(ADC2, ENABLE);              //ʹ��ָ����ADC2
  ADC_Cmd(ADC3, ENABLE);              //ʹ��ָ����ADC3
  /* Enable ADC1 reset calibaration register */   
 ADC_ResetCalibration(ADC1);            //��λָ����ADC1��У׼�Ĵ���
 /* Check the end of ADC1 reset calibration register */
 while(ADC_GetResetCalibrationStatus(ADC1));      //��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
 
 /* Start ADC1 calibaration */
 ADC_StartCalibration(ADC1);          //��ʼָ��ADC1��У׼״̬
 /* Check the end of ADC1 calibration */
 while(ADC_GetCalibrationStatus(ADC1));       //��ȡָ��ADC1��У׼����,����״̬��ȴ�
 
 
 /* Enable ADC2 reset calibaration register */   
 ADC_ResetCalibration(ADC2);            //��λָ����ADC2��У׼�Ĵ���
 /* Check the end of ADC2 reset calibration register */
 while(ADC_GetResetCalibrationStatus(ADC2));      //��ȡADC2��λУ׼�Ĵ�����״̬,����״̬��ȴ�
 
 /* Start ADC2 calibaration */
 ADC_StartCalibration(ADC2);          //��ʼָ��ADC2��У׼״̬
 /* Check the end of ADC2 calibration */
 while(ADC_GetCalibrationStatus(ADC2));       //��ȡָ��ADC2��У׼����,����״̬��ȴ�

 /* Enable ADC23 reset calibaration register */   
 ADC_ResetCalibration(ADC3);            //��λָ����ADC3��У׼�Ĵ���
 /* Check the end of ADC3 reset calibration register */
 while(ADC_GetResetCalibrationStatus(ADC3));      //��ȡADC3��λУ׼�Ĵ�����״̬,����״̬��ȴ�
 
 /* Start ADC3 calibaration */
 ADC_StartCalibration(ADC3);          //��ʼָ��ADC3��У׼״̬
 /* Check the end of ADC3 calibration */
 while(ADC_GetCalibrationStatus(ADC3));       //��ȡָ��ADC3��У׼����,����״̬��ȴ�
}
 
/*����DMA*/
void DMA_Configuration(void)
{
 
 /* ADC1  DMA1 Channel Config */
 DMA_InitTypeDef DMA_InitStructure;
// DMA_DeInit(DMA1_Channel1);            //��DMA1��ͨ��1�Ĵ�������Ϊȱʡֵ
// DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA����ADC����ַ
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC12_Buffer;  //0x20000100;    //DMA�ڴ����ַ
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;     //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
// DMA_InitStructure.DMA_BufferSize = 1;         //DMAͨ����DMA����Ĵ�С
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�Ĵ�������
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    //�ڴ��ַ�Ĵ�������
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;  //���ݿ��Ϊ32λ
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; //���ݿ��Ϊ32λ
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;      //������ѭ������ģʽ
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;    //DMAͨ�� xӵ�и����ȼ� 
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;       //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);       //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
 /* ADC3  DMA1 Channel Config */
 DMA_DeInit(DMA2_Channel5);            //��DMA2��ͨ��5�Ĵ�������Ϊȱʡֵ
 DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC3->DR;  //DMA����ADC����ַ
 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC3_Buffer; //0x20000130;    //DMA�ڴ����ַ
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;     //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
 DMA_InitStructure.DMA_BufferSize = 5;         //DMAͨ����DMA����Ĵ�С
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    //�ڴ��ַ�Ĵ�������
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;  //���ݿ��Ϊ32λ
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; //���ݿ��Ϊ32λ
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;      //������ѭ������ģʽ
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;    //DMAͨ�� xӵ�и����ȼ� 
 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;      //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
 DMA_Init(DMA2_Channel5, &DMA_InitStructure);            //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
}

//TIM����
 
void TIM_NVIC_Configuration()
{ 
 NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 NVIC_InitStructure.NVIC_IRQChannel =TIM1_UP_IRQn ;       //�����¼�
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //��ռ���ȼ�0��
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;     //�����ȼ�1��
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //IRQͨ����ʹ��
 NVIC_Init(&NVIC_InitStructure);          //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
 

//Step3. TIM1��TIM8ģ���ʼ��
void TIM_Configuration()
{
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
 TIM_DeInit(TIM1); //lihewen 2012-09-21
 TIM_TimeBaseStructInit(&TIM_BaseInitStructure); //lihewen 2012-09-21
    //TIM1,8ʱ����������/* Time Base configuration */
    //Ƶ��=TIM1_CLK/(ARR+1)
    TIM_BaseInitStructure.TIM_Period = PWM_PERIOD;        //�Զ�װ�ذ�����ֵ��2250*2/72000000��=0.000062.5s=16KHZ
    TIM_BaseInitStructure.TIM_Prescaler = 0x0;        //ʱ��Ϊ72MHZ
    TIM_BaseInitStructure.TIM_ClockDivision = 0x0; ////����ʱ���Ƶ
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;//TIM �������ģʽ3����ģʽ
    TIM_BaseInitStructure.TIM_RepetitionCounter = 0x1;    //�ظ�������ֵΪ0ʱ����2���ж�
 //TIM1ʱ������
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);     //���ó�ʼ��������ʼ��TIM1
 TIM_ARRPreloadConfig(TIM1, ENABLE);        //����ARR��Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã�
 //TIM8ʱ������ 
 TIM_TimeBaseInit(TIM8, &TIM_BaseInitStructure);     //���ó�ʼ��������ʼ��TIM8
 TIM_ARRPreloadConfig(TIM8, ENABLE);        //����ARR��Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã�

 TIM_OCStructInit(&TIM_OCInitStructure); //lihewen 2012-09-21
 /* Channel 1, 2,3 in PWM mode */
    //TIM1_OCģ���������
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    //TIM �����ȵ���ģʽ 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM ����Ƚ�ʹ��
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //TIM ��������Ƚ�ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ;  //TIM ����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High ; //TIM ��������Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD >> 1;   //1125;   //��װ�벶��Ƚ�����ֵ���˼Ĵ�������PWMռ�ձ�25%��         
 TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Reset; //lihewen 2012-09-21
 TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;  //lihewen 2012-09-21
 //TIM1_OCģ������
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
 TIM_OC2Init(TIM1, &TIM_OCInitStructure);
 TIM_OC3Init(TIM1, &TIM_OCInitStructure); 
    
 //TIM8_OCģ������
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
 TIM_OC2Init(TIM8, &TIM_OCInitStructure);
 TIM_OC3Init(TIM8, &TIM_OCInitStructure);

 /* Enables the Preload on CCx Register */
    //TIM1_OCxģ����������CCRx�Ĵ�����Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã�
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    //TIM8_OCxģ����������CCRx�Ĵ�����Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã�
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
 TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

    //��������
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;  //ʹ��TIM1 OSSR״̬ 
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;  //ʹ��TIM1 OSSI״̬
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;  //�����κ�λ
    TIM_BDTRInitStructure.TIM_DeadTime  = DEADTIME;      //�������������С0-0x7f,����ʵ��0����1777ns ����ȡ0x80-0xc0����ʵ��1777ns����3555ns
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;   //ʧ��TIM1ɲ������
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//TIM1ɲ������ܽż��Ե�
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//�Զ��������ʹ��?2012-09-17
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
 //����ͬ������TIM1��TIM8
 /* Select the Master Slave Mode: TIM1 */
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);  //����TIM1Ϊ��ģʽ
 TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Enable );    //ʹ�ܱ�ѡΪ�������(TRGO) 
 TIM_SelectSlaveMode(TIM1,   TIM_SlaveMode_Trigger  );
 TIM_SelectInputTrigger(TIM1, TIM_TS_TI1F_ED); 
 TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
 /* Slave Mode selection: TIM8 */
 TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Trigger  );    //����TIM8Ϊ��ģʽ                
    TIM_SelectInputTrigger(TIM8, TIM_TS_ITR0);      //ѡ��TIM �ڲ����� 0
 //�������һ�������¼���ʹԤװ�ؼĴ���װ�ص�Ӱ�ӼĴ���
 TIM_GenerateEvent(TIM1, TIM_EventSource_Update );     
 TIM_GenerateEvent(TIM8, TIM_EventSource_Update ); 
 //�ж�����
 TIM_ClearFlag(TIM1, TIM_FLAG_Update);        //�����жϱ�־
 TIM_ITConfig(TIM1, TIM_IT_Update , ENABLE );     //ʹ�ܸ����ж�
// TIM_ClearFlag(TIM8, TIM_FLAG_Update);        //�����жϱ�־
// TIM_ITConfig(TIM8, TIM_IT_Update , ENABLE );     //ʹ�ܸ����ж�
 TIM1->CCR1 = 1125;
 TIM1->CCR2 = 1125;
 TIM1->CCR3 = 1125;
 TIM8->CCR1 = 1125;
 TIM8->CCR2 = 1125;
 TIM8->CCR3 = 1125;
    TIM_Cmd(TIM1, ENABLE);            //TIM1����
    TIM_Cmd(TIM8, ENABLE);           //TIM8����
}

void STM32_TIM_ADC_Configuration(void)
{
// Tim_ADC_RCC_Configuration();	 //xx0410
// Tim_ADC_GPIO_Configuration(); //xx0410
 ADC_Configuration();			 
 DMA_Configuration();			 
// TIM_NVIC_Configuration();	 //xx0410
 TIM_Configuration();
    /* Start ADC1 Software Conversion */ 
// DMA_Cmd(DMA1_Channel1, ENABLE);          //����DMAͨ��
 DMA_Cmd(DMA2_Channel5, ENABLE);          //����DMAͨ��
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 ADC_SoftwareStartConvCmd(ADC2, ENABLE);
 ADC_SoftwareStartConvCmd(ADC3, ENABLE);
// ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);    //2012-11-27
 TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update ); //?2012-09-17
}




