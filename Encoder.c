/*encoder config*/


/*
main()��������2����������encoder�������
TIM4_Configuration(); //
//extINT_init();
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/

u8 prnt_opt=0x00;

void test_2DataOutput(s16, s16);
void test_1DataOutput(s16);



//Tim4 ������ģʽ
void Tim4_RCC_Configuration()
{
	//Step1.����TIM����Ӧ�˿�ʱ��
	//����GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD  ,ENABLE);

	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	 	//����AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);	 
}


void Tim4_GPIO_Configuration()
{//����TIM4��ch1,ch2 GPIO����Ӧ���ã�ΪAF���
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA.7/������ΪTIM8_CH1N
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

		//CPU_LED==PD4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					    //ָʾ�����¼��ж�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 		//�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void TIM_4_Configuration()
{ 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
	TIM_ICInitTypeDef TIM_ICInitStructure; 
	TIM_ICStructInit(& TIM_ICInitStructure); 
	//TIM_ICStructInit(&TIM_ICInitStructure); 
	//TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_PWMI;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1|TIM_Channel_2; 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
	TIM_ICInitStructure.TIM_ICFilter = 0x0; 
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	
	
	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  // Time base configuration 
	
	
	
	
	 
	//TIM_ETRConfig(TIM3,TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
	//TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2 );
	
	//TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_External1);
	//TIM_ETRClockMode1Config(TIM4, TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_Inverted, 0x0);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM4, 0);
	    
	
	TIM_Cmd(TIM4, ENABLE);
}

/*����1*/
void TIM4_Configuration(void)
{
  	 Tim4_RCC_Configuration();
	 Tim4_GPIO_Configuration();
	 TIM_4_Configuration();
}


//Tim4 4��Ƶģʽ
/*
void Tim4_RCC_Configuration()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD  ,ENABLE);

	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	 	//����AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);	 
}


void Tim4_GPIO_Configuration()
{//����TIM1��TIM8.GPIO����Ӧ���ã�ΪAF���
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA.7/������ΪTIM8_CH1N
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

}

//Step3. TIM1��TIM8ģ���ʼ��
void TIM_4_Configuration()
{ 
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
TIM_ICInitTypeDef TIM_ICInitStructure; 
TIM_ICStructInit(& TIM_ICInitStructure); 
//TIM_ICStructInit(&TIM_ICInitStructure); 
//TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_PWMI;
TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; 
TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
TIM_ICInitStructure.TIM_ICFilter = 0x0; 
TIM_ICInit(TIM4, &TIM_ICInitStructure);



TIM_TimeBaseStructure.TIM_Prescaler = 0;

TIM_TimeBaseStructure.TIM_Period = 0XFFFF; 

TIM_TimeBaseStructure.TIM_ClockDivision = 0; 

TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 

TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  // Time base configuration 




 
//TIM_ETRConfig(TIM3,TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2 );
TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_External1);
//TIM_ETRClockMode1Config(TIM4, TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_Inverted, 0x0);
TIM_SetCounter(TIM4, 0);
    

TIM_Cmd(TIM4, ENABLE);


}

void TIM4_Configuration()
{
  	 Tim4_RCC_Configuration();
	 Tim4_GPIO_Configuration();
	 TIM_4_Configuration();
}
*/


/*externalINT */

/**********************************************************************
* ��    �ƣ�RCC_Configuration()
* ��    �ܣ�����ʱ��
* ��ڲ����� 
* ���ڲ�����
-----------------------------------------------------------------------
* ˵����ʹ�ÿ⺯��
***********************************************************************/
void extINT_RCC_Configuration(void)
{

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|
  RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO , ENABLE); //��GPIOA����ӳ���ṩʱ�ӣ�ע�⣺һ��Ҫ����RCC_APB2Periph_AFIO

}


/**********************************************************************
* ��    �ƣ�GPIO_Configuration()
* ��    �ܣ������������
* ��ڲ����� 
* ���ڲ�����
-----------------------------------------------------------------------
* ˵����ʹ�ÿ⺯��������IO��
***********************************************************************/
 void extINT_GPIO_Configuration(void)
 {

  GPIO_InitTypeDef  GPIO_InitStructure;
  /*����D10Ϊ����*/

///////������� ����ת��INDEX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;			   //ת�����2��Ƶ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;			  //ת��INDEX�ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				   //ת�����ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//|GPIO_Pin_12 ;	 //���ֱ����� ����ת��INDEX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		   
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				    //���ֱ����� ����ת��INDEX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		   
	GPIO_Init(GPIOB, &GPIO_InitStructure);




 }

/**********************************************************************
* ��    ��:EXTI_Configuration()
* ��    ��:
* ��ڲ�����
* ���ڲ�����
-----------------------------------------------------------------------
* ˵���� 
	PD8  -- left index - ���ӵ��ⲿ�ж�ͨ��8
	PB10 --
	PB11 --
***********************************************************************/
void extINT_EXTI_Configuration(void)
{
  EXTI_InitTypeDef  EXTI_InitStructure;


  /* Configure EXTI Line3 to generate an interrupt on falling edge */  
//  EXTI_InitStructure.EXTI_Line = EXTI_Line2;                   		//�ⲿ�ж�ͨ��2
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;   //�����½��ش���
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
//  EXTI_Init(&EXTI_InitStructure);
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource2);  		//��PG2���ӵ��ⲿ�ж�ͨ��2
//  EXTI_ClearITPendingBit(EXTI_Line2);
//
//  EXTI_InitStructure.EXTI_Line = EXTI_Line3;                   		//�ⲿ�ж�ͨ��3
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;    		//�½��ش���
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
//  EXTI_Init(&EXTI_InitStructure);
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);  		//��PC3���ӵ��ⲿ�ж�ͨ��3
//  EXTI_ClearITPendingBit(EXTI_Line3);


  EXTI_InitStructure.EXTI_Line = EXTI_Line8;                   		//�ⲿ�ж�ͨ��8
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;    		//�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
  EXTI_Init(&EXTI_InitStructure);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource8);  		//��PD8���ӵ��ⲿ�ж�ͨ��8
  EXTI_ClearITPendingBit(EXTI_Line8);


//  EXTI_InitStructure.EXTI_Line = EXTI_Line12;                   	//�ⲿ�ж�ͨ��12
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;    //�����½��ش���
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
//  EXTI_Init(&EXTI_InitStructure);
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);  	//��PD12���ӵ��ⲿ�ж�ͨ��12
//  EXTI_ClearITPendingBit(EXTI_Line12);


  EXTI_InitStructure.EXTI_Line = EXTI_Line11;                   	//�ⲿ�ж�ͨ��11
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;   //�����½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
  EXTI_Init(&EXTI_InitStructure);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11); 		//��PB11���ӵ��ⲿ�ж�ͨ��11
  EXTI_ClearITPendingBit(EXTI_Line11);

  EXTI_InitStructure.EXTI_Line = EXTI_Line10;                   	//�ⲿ�ж�ͨ��10
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;      	//�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;        
  EXTI_Init(&EXTI_InitStructure);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);  	//��PB10���ӵ��ⲿ�ж�ͨ��10
  EXTI_ClearITPendingBit(EXTI_Line10);



}

/**********************************************************************
* ��    ��:NVIC_Configuration()
* ��    ��:
* ��ڲ�����
* ���ڲ�����
-----------------------------------------------------------------------
* ˵���� ���ñ�����index�����ж�
***********************************************************************/
 void extINT_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 

//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);           //��ռʽ���ȼ�������Ϊ����ռ���ȼ�
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;    		//ָ���ж�Դ
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //ָ����Ӧ���ȼ���2
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        	//ʹ���ⲿ�ж�ͨ��2
//  NVIC_Init(&NVIC_InitStructure);
//
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);           //��ռʽ���ȼ�������Ϊ����ռ���ȼ�
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;    		//ָ���ж�Դ
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //ָ����Ӧ���ȼ���1
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        	 //ʹ���ⲿ�ж�ͨ��3
//  NVIC_Init(&NVIC_InitStructure);

//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);           //��ռʽ���ȼ�������Ϊ����ռ���ȼ�
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;    	//ָ���ж�Դ
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //ָ����Ӧ���ȼ���2
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        	 //ʹ���ⲿ�ж�ͨ��15_10
//  NVIC_Init(&NVIC_InitStructure);
//
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);           //��ռʽ���ȼ�������Ϊ����ռ���ȼ�
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;    	//ָ���ж�Դ
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //ָ����Ӧ���ȼ���1
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        	 //ʹ���ⲿ�ж�ͨ��9_5
//  NVIC_Init(&NVIC_InitStructure);

// 	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel =EXTI15_10_IRQn;    			//�����¼�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 		//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  			//�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); 

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;    			//�����¼�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 		//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  			//�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);


} 
///**********************************************************************
//* ��    �ƣ�extINT_init(void) 
//* ��    �ܣ��ⲿ�жϳ�ʼ������
//* ��ڲ����� 
//* ���ڲ�����
//-----------------------------------------------------------------------
//* ˵����
//***********************************************************************/
void extINT_init(void) 
{
	 extINT_RCC_Configuration();  //����ʱ��                      
	 extINT_GPIO_Configuration(); //����IO��
	 extINT_EXTI_Configuration(); //�ⲿ�ж�����
	 extINT_NVIC_Configuration(); //�ж�����
}




//����INDEX�жϴ�����

void EXTI9_5_IRQHandler(void)							//����INDEX
{
	u8 opt;
	opt=prnt_opt;
	if(EXTI_GetITStatus(EXTI_Line8) != RESET) 			 
    {
		 if(opt==0x05)
		 	test_1DataOutput(TIM4->CNT);
	     TIM_GenerateEvent(TIM4, TIM_EventSource_Update ); 
	     EXTI_ClearFlag(EXTI_Line8);          
	     EXTI_ClearITPendingBit(EXTI_Line8);
    }
}

void EXTI15_10_IRQHandler(void)					
 {
	 if(EXTI_GetITStatus(EXTI_Line11) != RESET) 	  //�ҵ�������ź�
    {
     //����жϴ������
     EXTI_ClearFlag(EXTI_Line11);         
     EXTI_ClearITPendingBit(EXTI_Line11);
     }

	 if(EXTI_GetITStatus(EXTI_Line10) != RESET) 	  //�ҵ��INDEX
    {
     //����жϴ������
     EXTI_ClearFlag(EXTI_Line10);          //����жϱ�־�����룩
     EXTI_ClearITPendingBit(EXTI_Line10);
     }

 }







