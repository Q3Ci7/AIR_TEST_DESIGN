#ifndef __M26_H
#define __M26_H	
#include "stm32f10x.h"
void Uart1_SendStr(char*SendBuf);//����1��ӡ����;
void Clear_Buffer(void);//��ջ���
void AT_Init(void);//M26��ʼ��
void Send_Str(char*data);//��������
void MQTT_Init(void);
void SendMessage(u8 tt,u8 hh,float pp,float zz,u8 alarm,u8 param_t,u8 param_h,u16 param_p,float param_z);


void Recv_Data(void);
#endif  

