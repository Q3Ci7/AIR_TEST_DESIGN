#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_tim.h"
#include "json.h"

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
    USART2->DR = (u8) ch;
    return ch;
}
#endif

//**************************************************************************************

//char RxCounter,RxBuffer[100];     //接收缓冲,最大USART_REC_LEN个字节.
//char RxCounter1,RxBuffer1[100];     //接收缓冲,最大USART_REC_LEN个字节.


#if EN_USART1
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

char USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART1_RX_STA=0;       //接收状态标记
//u8 buf1_size = 0;//串口数据接收数量标记
//bool data_change = 0;//串口接收完成/变化标志位u8


void Uart1_SendStr(char*SendBuf)//串口1打印数据
{
    while(*SendBuf)
    {
        while((USART1->SR&0X40)==0);//等待发送完成
        USART1->DR = (u8) *SendBuf;
        SendBuf++;
    }
}

void uart1_init(u32 bound) 
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	USART_DeInit(USART1);  //复位串口1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
	USART_Cmd(USART1, ENABLE);                    //使能串口
}
//串口1中断服务程序
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				else USART1_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
} 

void USART1_printf (char *fmt, ...){ 
	char buffer[USART1_REC_LEN+1];  // 数据长度
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART1_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART1_REC_LEN) && (i < strlen(buffer))){
        USART_SendData(USART1, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

#endif


#if EN_USART2
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

char USART2_RX_BUF[USART2_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
u8 RxCounter;
u16 USART2_RX_STA=0;       //接收状态标记
bool data_change = 0;//串口接收完成/变化标志位

void uart2_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);   //USART2
	USART_DeInit(USART2);                                   //复位串口2

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);             //初始化PA2

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA3

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口

//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口
}

//串口2中断服务程序，模块返回数据
void USART2_IRQHandler(void)                	
{
	u8 Res;
//	u8 clear;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收模块返回的数据
    {
				Res=USART_ReceiveData(USART2);//接收模块的数据;
        USART2_RX_BUF[RxCounter++]=USART_ReceiveData(USART2);//接收模块的数据
        if(RxCounter > USART2_REC_LEN)//长度自行设定
            RxCounter=0;			
				USART_SendData(USART1,Res);//串口1打印调试，可注释
				USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
//*****************************空闲中断置位用于数据解析**************************		 
//	if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE)!=Bit_RESET)//空闲中断发生
//	{			
//		flag_IDLE = 1;
////		RxCounter = 0;
//		clear = USART2->SR;
//		clear = USART2->DR;	
//	}
//********************************************************************************			
}


//void USART2_printf (char *fmt, ...){ 
//	char buffer[USART2_REC_LEN+1];  // 数据长度
//	u8 i = 0;	
//	va_list arg_ptr;
//	va_start(arg_ptr, fmt);  
//	vsnprintf(buffer, USART2_REC_LEN+1, fmt, arg_ptr);
//	while ((i < USART2_REC_LEN) && (i < strlen(buffer))){
//        USART_SendData(USART2, (u8) buffer[i++]);
//        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
//	}
//	va_end(arg_ptr);
//}
#endif

#if EN_USART3

//**************************************************************************************
uint8_t SendBuff[SENDBUFF_SIZE];
u8 buff_usart3[9];
u8 flag_usart3=0;
//**************************************************************************************

void uart3_init(u32 bound)
{
	//GPIO端口设置
//	DMA_InitTypeDef DMA_InitStruct;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);   //USART3
	RCC_AHBPeriphClockCmd(USART_TX_DMA_CLK,ENABLE);
	USART_DeInit(USART3);                                   //复位串口3

	//USART2_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);             //初始化PB10

	//USART2_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化PB11

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART3, ENABLE);                    //使能串口
		
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADRESS;
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SendBuff;
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_BufferSize = SENDBUFF_SIZE;
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;  //发送一次
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;    //M2M模式使能
//	
//	DMA_Init(USART_TX_DMA_Channel,&DMA_InitStruct);
//	DMA_ClearFlag(USART_TX_DMA_FLAG_TC);       //清除标志位
//	DMA_Cmd(USART_TX_DMA_Channel,ENABLE);		
}

extern unsigned char FucCheckSum(unsigned char *i,unsigned char ln)
{
	unsigned char j,tempq=0;
	i+=1;
	for(j=0;j<(ln-2);j++)
	{
		tempq+=*i;
		i++;
	}
	tempq=(~tempq)+1;
	return(tempq);
}

//void USART3_IRQHandler(void)
//{
//	static int i = 0;//数组下标
//	static int flag_data = 0;
//	unsigned char res;
//	if(USART_GetITStatus(USART3,USART_IT_RXNE) == 1)
//	{
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//		res = USART_ReceiveData(USART3);
//		if(res == 0xFF)//接收到了0xFF之后表明数据安全
//		{
//			flag_data = 1;
//		}
//		if(res != 0xFF && flag_data == 1)//接收其它的数据
//		{
//			buff_usart3[i] = res;
//			i++;
//			if(i == 8)
//			{
//				i = 0;
//				flag_data = 0;//重新要开始接收0xFF
//				flag_usart3 = 1;//数据接收完毕
//			}
//		}
//	}
//}


//串口2中断
void USART3_IRQHandler(void)
{ 
	static int i = 0;                                     
 
//	static int flag_data = 0;
//	unsigned char res;
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == 1)
	{ 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); 	
		buff_usart3[i++] = USART_ReceiveData(USART3);	
		 if(!(buff_usart3[0] == 0xFF))
         {
              i=0;
			  buff_usart3[0]=0;               
         }
		if(i==9)
				{
					if(buff_usart3[8]==FucCheckSum(buff_usart3,9))
					{						
						flag_usart3 = 1; 		
					}
					i=0;
				}			 
	}
}
 

#endif
