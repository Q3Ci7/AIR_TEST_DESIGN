#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_tim.h"
#include "json.h"

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART2->SR&0X40)==0);//ѭ������,ֱ���������
    USART2->DR = (u8) ch;
    return ch;
}
#endif

//**************************************************************************************

//char RxCounter,RxBuffer[100];     //���ջ���,���USART_REC_LEN���ֽ�.
//char RxCounter1,RxBuffer1[100];     //���ջ���,���USART_REC_LEN���ֽ�.


#if EN_USART1
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ

char USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART1_RX_STA=0;       //����״̬���
//u8 buf1_size = 0;//�������ݽ����������
//bool data_change = 0;//���ڽ������/�仯��־λu8


void Uart1_SendStr(char*SendBuf)//����1��ӡ����
{
    while(*SendBuf)
    {
        while((USART1->SR&0X40)==0);//�ȴ��������
        USART1->DR = (u8) *SendBuf;
        SendBuf++;
    }
}

void uart1_init(u32 bound) 
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	USART_DeInit(USART1);  //��λ����1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

	//Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
}
//����1�жϷ������
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
			{
			if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
				else USART1_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
} 

void USART1_printf (char *fmt, ...){ 
	char buffer[USART1_REC_LEN+1];  // ���ݳ���
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
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ

char USART2_RX_BUF[USART2_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
u8 RxCounter;
u16 USART2_RX_STA=0;       //����״̬���
bool data_change = 0;//���ڽ������/�仯��־λ

void uart2_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ�ܣ�GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);   //USART2
	USART_DeInit(USART2);                                   //��λ����2

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);             //��ʼ��PA2

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ��PA3

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������

//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//���������ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���
}

//����2�жϷ������ģ�鷵������
void USART2_IRQHandler(void)                	
{
	u8 Res;
//	u8 clear;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //����ģ�鷵�ص�����
    {
				Res=USART_ReceiveData(USART2);//����ģ�������;
        USART2_RX_BUF[RxCounter++]=USART_ReceiveData(USART2);//����ģ�������
        if(RxCounter > USART2_REC_LEN)//���������趨
            RxCounter=0;			
				USART_SendData(USART1,Res);//����1��ӡ���ԣ���ע��
				USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
//*****************************�����ж���λ�������ݽ���**************************		 
//	if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE)!=Bit_RESET)//�����жϷ���
//	{			
//		flag_IDLE = 1;
////		RxCounter = 0;
//		clear = USART2->SR;
//		clear = USART2->DR;	
//	}
//********************************************************************************			
}


//void USART2_printf (char *fmt, ...){ 
//	char buffer[USART2_REC_LEN+1];  // ���ݳ���
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
	//GPIO�˿�����
//	DMA_InitTypeDef DMA_InitStruct;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ�ܣ�GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);   //USART3
	RCC_AHBPeriphClockCmd(USART_TX_DMA_CLK,ENABLE);
	USART_DeInit(USART3);                                   //��λ����3

	//USART2_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);             //��ʼ��PB10

	//USART2_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);               //��ʼ��PB11

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); //��ʼ������

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
		
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADRESS;
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SendBuff;
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_BufferSize = SENDBUFF_SIZE;
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;  //����һ��
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;    //M2Mģʽʹ��
//	
//	DMA_Init(USART_TX_DMA_Channel,&DMA_InitStruct);
//	DMA_ClearFlag(USART_TX_DMA_FLAG_TC);       //�����־λ
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
//	static int i = 0;//�����±�
//	static int flag_data = 0;
//	unsigned char res;
//	if(USART_GetITStatus(USART3,USART_IT_RXNE) == 1)
//	{
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//		res = USART_ReceiveData(USART3);
//		if(res == 0xFF)//���յ���0xFF֮��������ݰ�ȫ
//		{
//			flag_data = 1;
//		}
//		if(res != 0xFF && flag_data == 1)//��������������
//		{
//			buff_usart3[i] = res;
//			i++;
//			if(i == 8)
//			{
//				i = 0;
//				flag_data = 0;//����Ҫ��ʼ����0xFF
//				flag_usart3 = 1;//���ݽ������
//			}
//		}
//	}
//}


//����2�ж�
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
