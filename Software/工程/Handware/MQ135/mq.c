#include "mq.h"
#include "math.h"
#include "delay.h"
u16 ADC_Convertedvalue[NUMCHANEL];

#define CAL_PPM  10  // У׼������PPMֵ
#define RL	10  // RL��ֵ
static float R01=16,R02=16; // Ԫ���ڽྻ�����е���ֵ

void ADC_Config(void)
{
	GPIO_InitTypeDef GPIOInitStruct;//GPIO�ṹ�����
	ADC_InitTypeDef ADCInitStruct;//ADC�ṹ�����
	DMA_InitTypeDef DMAInitStruct;//DMA�ṹ�����
	
	RCC_APB2PeriphClockCmd(GPIO_ADC_CLK|ADC_CLK,ENABLE);//ʹ��GPIO��ADCʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);//ʹ��DMAʱ��
/*--------------------------------------------------------------------------*/	
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIOInitStruct.GPIO_Pin = GPIO_ADC_Pin;
	GPIO_Init(GPIO_ADC_PORT, &GPIOInitStruct);//GPIO��ʼ������
/*--------------------------------------------------------------------------*/	
	
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMAInitStruct.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADC_x->DR ) );
	DMAInitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_Convertedvalue;
	DMAInitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMAInitStruct.DMA_BufferSize = NUMCHANEL;
	DMAInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMAInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; 	
	DMAInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMAInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMAInitStruct.DMA_Mode = DMA_Mode_Circular;	
	DMAInitStruct.DMA_Priority = DMA_Priority_High;
	DMAInitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMAInitStruct);
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
/*--------------------------------------------------------------------------*/	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;//ʹ������ת��ģʽ
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;//ת��������Ҷ���
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ѡ���ⲿ����
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;//��������ģʽ
	ADCInitStruct.ADC_NbrOfChannel = 1;//Ҫת����ͨ����Ŀ
	ADCInitStruct.ADC_ScanConvMode = ENABLE;//��ͨ������Ҫɨ��
	
	ADC_Init(ADC_x, &ADCInitStruct);//ADC��ʼ������
	ADC_Cmd(ADC_x,ENABLE);//ʹ��ADC
/*--------------------------------------------------------------------------*/	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADCʱ��CLK2��6��Ƶ����14Mhz	
	ADC_RegularChannelConfig(ADC_x,ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);//����ADCͨ��ת��˳�򣬲������ڣ�55.5��ʱ�����ڣ�
//	ADC_RegularChannelConfig(ADC_x,ADC_Channel_9, 2, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADC_x,ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
	
	ADC_ResetCalibration(ADC_x);//������ѡADCУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC_x));//�ȴ���λ��ɣ���ȡ��ѡADC��λУ׼�Ĵ���״̬����
	ADC_SoftwareStartConvCmd(ADC_x,ENABLE);//�������ADCת��
	
}




void ADCInit(void)
{
	ADC_Config();
	ADC_DMACmd(ADC_x, ENABLE);//ʹ��ADC DMA ����
//	delay_ms(1000);
//	float Vrl1 = 3.3f * ADC_Convertedvalue[1]/ 4095.f;
//	delay_ms(1000);
//	float Vrl2 = 3.3f * ADC_Convertedvalue[2]/ 4095.f;
//	delay_ms(1000);
//  float RS1 = (3.3f - Vrl1) / Vrl1 * 10;
//	float RS2 = (3.3f - Vrl2) / Vrl2 * 5; 
//	R01 = RS1 / pow(10 / 98.322, 1 / -1.458f);
//	R02 = RS2 / pow(20 / 613.9f, 1 / -2.074f);	
}
//=======================================================================================================
// ��ȡMQ135��������ֵ
float Read_Mq135_Data(void)
{
	float vol; // ��ѹ����
	float ppm;
	vol = (float)ADC_Convertedvalue[0] / 4096 * 3.3;                      // ��ѹֵ
	ppm = pow((3.4880 * 10 * vol) / (5 - vol), (1.0 / 0.3203)) / 1000; // ����Ũ��ֵ	
	return ppm;
}

//========================================================================================================
// ��ȡMQ7��������ֵ
float Read_Mq7_Data(void)
{		
    float Vrl = (float)ADC_Convertedvalue[1] / 4096 * 3.3;                      // ��ѹֵ
    float RS1 = (3.3 - Vrl) / Vrl * 10;
//		float R0 = RS / pow(10 / 98.322, 1 / -1.458);// ������У׼����
    float ppm = 98.322 * pow(RS1/R01, -1.458);
    return  ppm;
}

//========================================================================================================
// ��ȡMQ2��������ֵ
float MQ2_GetPPM(void)
{	
   float Vrl = (float)ADC_Convertedvalue[2] / 4096 * 3.3;                      // ��ѹֵ
	 Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
    float RS2 = (3.3f - Vrl) / Vrl * 5; 
    float ppm = 613.9f * pow(RS2/R02, -2.074f);
    return ppm;
}
