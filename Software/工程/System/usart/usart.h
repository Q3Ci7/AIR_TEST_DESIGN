#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include <stdarg.h>
#define  SENDBUFF_SIZE            5000	

//不使用某个串口时要禁止此串口，以减少编译量
#define EN_USART1 			1		//使能（1）/禁止（0）串口1
#define EN_USART2 			1		//使能（1）/禁止（0）串口2
#define EN_USART3 			1		//使能（1）/禁止（0）串口3

void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);

#define USART1_REC_LEN  			200  	//定义最大接收字节数 200
#define USART2_REC_LEN  			200  	//定义最大接收字节数 200

#define USART_DR_ADRESS            (USART3_BASE+0x04)    //外设寄存器地址
#define USART_TX_DMA_CLK           RCC_AHBPeriph_DMA1
#define USART_TX_DMA_Channel       DMA1_Channel3
#define USART_TX_DMA_FLAG_TC       DMA1_FLAG_TC3

void USART1_printf (char *fmt, ...);
void USART2_printf (char *fmt, ...);//串口2的专用printf函数
#endif


