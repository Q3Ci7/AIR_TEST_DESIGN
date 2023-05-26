#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include <stdarg.h>
#define  SENDBUFF_SIZE            5000	

//��ʹ��ĳ������ʱҪ��ֹ�˴��ڣ��Լ��ٱ�����
#define EN_USART1 			1		//ʹ�ܣ�1��/��ֹ��0������1
#define EN_USART2 			1		//ʹ�ܣ�1��/��ֹ��0������2
#define EN_USART3 			1		//ʹ�ܣ�1��/��ֹ��0������3

void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);

#define USART1_REC_LEN  			200  	//�����������ֽ��� 200
#define USART2_REC_LEN  			200  	//�����������ֽ��� 200

#define USART_DR_ADRESS            (USART3_BASE+0x04)    //����Ĵ�����ַ
#define USART_TX_DMA_CLK           RCC_AHBPeriph_DMA1
#define USART_TX_DMA_Channel       DMA1_Channel3
#define USART_TX_DMA_FLAG_TC       DMA1_FLAG_TC3

void USART1_printf (char *fmt, ...);
void USART2_printf (char *fmt, ...);//����2��ר��printf����
#endif


