#ifndef _MQ_H
#define _MQ_H
#include "stm32f10x.h"

/*GPIO�궨��*/
#define GPIO_ADC_PORT   GPIOB
#define GPIO_ADC_Pin    GPIO_Pin_0
#define GPIO_ADC_CLK    RCC_APB2Periph_GPIOB

//#define GPIO1_ADC_PORT   GPIOB
//#define GPIO1_ADC_Pin    GPIO_Pin_1
//#define GPIO1_ADC_CLK    RCC_APB2Periph_GPIOB

//#define GPIO2_ADC_PORT   GPIOA
//#define GPIO2_ADC_Pin    GPIO_Pin_7
//#define GPIO2_ADC_CLK    RCC_APB2Periph_GPIOA
/*ADC���ѡ�� 1/2*/
#define ADC_x           ADC1
#define ADC_CLK         RCC_APB2Periph_ADC1
/*ADCͨ��ѡ�� 1~16*/
#define ADC_Channelx    ADC_Channel_8

#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1
#define    ADC_DMA_CHANNEL               DMA1_Channel1//ADC1������ͨ����ӦDMA1��ͨ��1

// ת��ͨ������
#define    NUMCHANEL										 3

void ADCInit(void);
float Read_Mq135_Data(void);
float Read_Mq7_Data(void);
float MQ2_GetPPM(void);
#endif
