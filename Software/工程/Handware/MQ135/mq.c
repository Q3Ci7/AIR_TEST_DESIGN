#include "mq.h"
#include "math.h"
#include "delay.h"
u16 ADC_Convertedvalue[NUMCHANEL];

#define CAL_PPM  10  // 校准环境中PPM值
#define RL	10  // RL阻值
static float R01=16,R02=16; // 元件在洁净空气中的阻值

void ADC_Config(void)
{
	GPIO_InitTypeDef GPIOInitStruct;//GPIO结构体变量
	ADC_InitTypeDef ADCInitStruct;//ADC结构体变量
	DMA_InitTypeDef DMAInitStruct;//DMA结构体变量
	
	RCC_APB2PeriphClockCmd(GPIO_ADC_CLK|ADC_CLK,ENABLE);//使能GPIO、ADC时钟
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);//使能DMA时钟
/*--------------------------------------------------------------------------*/	
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIOInitStruct.GPIO_Pin = GPIO_ADC_Pin;
	GPIO_Init(GPIO_ADC_PORT, &GPIOInitStruct);//GPIO初始化函数
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
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;//使能连续转换模式
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;//转换结果靠右对齐
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不选择，外部触发
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;//独立工作模式
	ADCInitStruct.ADC_NbrOfChannel = 1;//要转换的通道数目
	ADCInitStruct.ADC_ScanConvMode = ENABLE;//单通道不需要扫描
	
	ADC_Init(ADC_x, &ADCInitStruct);//ADC初始化函数
	ADC_Cmd(ADC_x,ENABLE);//使能ADC
/*--------------------------------------------------------------------------*/	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//配置ADC时钟CLK2的6分频，即14Mhz	
	ADC_RegularChannelConfig(ADC_x,ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);//配置ADC通道转换顺序，采样周期（55.5个时钟周期）
//	ADC_RegularChannelConfig(ADC_x,ADC_Channel_9, 2, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADC_x,ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
	
	ADC_ResetCalibration(ADC_x);//重置所选ADC校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC_x));//等待复位完成（获取所选ADC复位校准寄存器状态。）
	ADC_SoftwareStartConvCmd(ADC_x,ENABLE);//软件触发ADC转换
	
}




void ADCInit(void)
{
	ADC_Config();
	ADC_DMACmd(ADC_x, ENABLE);//使能ADC DMA 请求
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
// 获取MQ135传感器的值
float Read_Mq135_Data(void)
{
	float vol; // 电压参数
	float ppm;
	vol = (float)ADC_Convertedvalue[0] / 4096 * 3.3;                      // 电压值
	ppm = pow((3.4880 * 10 * vol) / (5 - vol), (1.0 / 0.3203)) / 1000; // 气体浓度值	
	return ppm;
}

//========================================================================================================
// 获取MQ7传感器的值
float Read_Mq7_Data(void)
{		
    float Vrl = (float)ADC_Convertedvalue[1] / 4096 * 3.3;                      // 电压值
    float RS1 = (3.3 - Vrl) / Vrl * 10;
//		float R0 = RS / pow(10 / 98.322, 1 / -1.458);// 传感器校准函数
    float ppm = 98.322 * pow(RS1/R01, -1.458);
    return  ppm;
}

//========================================================================================================
// 获取MQ2传感器的值
float MQ2_GetPPM(void)
{	
   float Vrl = (float)ADC_Convertedvalue[2] / 4096 * 3.3;                      // 电压值
	 Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
    float RS2 = (3.3f - Vrl) / Vrl * 5; 
    float ppm = 613.9f * pow(RS2/R02, -2.074f);
    return ppm;
}
