#include "time.h"
#include "key.h"
u8 time=0;//��ʱ������
u8 count=0;//��ʱ������
u8 time3=0;
u8 count3=0;

// �ж����ȼ�����
static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
	/*TIM2*/
    // �����ж���Ϊ0
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ                                                   
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ ;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
//	/*TIM3*/
//	   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;	
//		// ���������ȼ�Ϊ 0
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
//	  // ������ռ���ȼ�Ϊ3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
}

static void BASIC_TIM_Config(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
	
		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Period = BASIC_TIM_Period;	

	  // ʱ��Ԥ��Ƶ��Ϊ
    TIM_TimeBaseStructure.TIM_Prescaler= BASIC_TIM_Prescaler;
	
//		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//		
//		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		
//		// �ظ���������ֵ��������ʱ��û�У����ù�
//		TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // ��ʼ����ʱ��
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);
		
		// ����������жϱ�־λ
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
	  
		// �����������ж�
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
		
		// ʹ�ܼ�����
//    TIM_Cmd(BASIC_TIM, ENABLE);//���ʺϵĵط�ʹ�ܶ�ʱ��
}


//static void BASIC_TIM3_Config(void);
void BASIC_TIM_Init(void)
{
	BASIC_TIM_NVIC_Config();
	BASIC_TIM_Config();
//	BASIC_TIM3_Config();
}


void BASIC_TIM_IRQHandler(void)
{
    if ( TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET )
    {
        time++;
			if(time==200)
			{
				time = 0;
				count++;
			}				
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
    }
}



//static void BASIC_TIM3_Config(void)
//{
//		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//		
//		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
//    BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM3, ENABLE);
//	
//		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
//    TIM_TimeBaseStructure.TIM_Period = BASIC_TIM_Period;	

//	  // ʱ��Ԥ��Ƶ��Ϊ
//    TIM_TimeBaseStructure.TIM_Prescaler= BASIC_TIM_Prescaler;
//	
////		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
////    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
////		
////		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
////    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
////		
////		// �ظ���������ֵ��������ʱ��û�У����ù�
////		TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	
//	  // ��ʼ����ʱ��
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//		
//		// ����������жϱ�־λ
//    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
//	  
//		// �����������ж�
//    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
//		
//		// ʹ�ܼ�����
////    TIM_Cmd(TIM3, ENABLE);//���ʺϵĵط�ʹ�ܶ�ʱ��
//}


//void TIM3_IRQHandler(void)
//{
//    if ( TIM_GetITStatus( TIM3, TIM_IT_Update) != RESET )
//    {
//        time3++;
//			if(time3==50)
//			{
//				time3 = 0;
//				count3++;
//			}				
//        TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
//    }
//}
