#include "M26.h"
#include "usart.h"
#include "stdlib.h"
#include "string.h"
#include "wdg.h"
#include "delay.h"
#include "oled.h"
#include "stdbool.h"	

#define ProductKey     "a14Uk3lGTjE"             //产品KEY
#define DeviceName          "air_01"      //
#define DeviceSecret    "c59bfec1a12104637a77cd96216f8807"  //
#define PubTopic         "/sys/a14Uk3lGTjE/air_01/thing/event/property/post"//发布主题
#define SubTopic       	 "/sys/a14Uk3lGTjE/air_01/thing/service/property/set"

//#define ProductKey     "id5zsYRSDUB"             //产品KEY
//#define DeviceName          "air_01"      //
//#define DeviceSecret    "1c6a78d86cf785a20de93bbac042246e"  //
//#define PubTopic         "/sys/id5zsYRSDUB/air_01/thing/event/property/post"//发布主题
//#define SubTopic       	 "/sys/id5zsYRSDUB/air_01/thing/service/property/set"

extern char USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u8 RxCounter;
u16 dltime = 400;//延时时间
char *strx=0;
extern bool swch_flag;
extern u8 relay_flag;
void Clear_Buffer(void)//清空缓存
{
	u8 i;
//	Uart1_SendStr(USART2_RX_BUF);//串口1监控数据
	for(i=0; i<RxCounter; i++)
		USART2_RX_BUF[i]=0;//缓存
	RxCounter=0;
}

void  AT_Init(void)
{
    Clear_Buffer();
    printf("AT\r\n");
    delay_ms(dltime);
    printf("AT\r\n");
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
			Clear_Buffer();
			printf("AT\r\n");
			delay_ms(dltime);
			strx=strstr((const char*)USART2_RX_BUF,(const char*)"OK");//返回OK
    }
//    printf("ATE0\r\n"); //关闭回显
//    delay_ms(dltime);
    Clear_Buffer();
    printf("AT+CSQ\r\n"); //检查CSQ
    delay_ms(dltime);
    printf("AT+CPIN?\r\n");//检查SIM卡是否在位
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"+CPIN: READY");//查看是否返回ready
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CPIN?\r\n");
        delay_ms(dltime);
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"+CPIN: READY");//检查SIM卡是否在位，等待卡在位，如果卡识别不到，剩余的工作就没法做了
    }
    Clear_Buffer();
    printf("AT+QMTCLOSE=0\r\n");//断开MQTT连接
    delay_ms(dltime);
}


void  MQTT_Init(void)
{
//-----------------------------------------------------------------------------------------------------
    Clear_Buffer();
    //查看是否获取到IP
    printf("AT+CGPADDR=1\r\n");
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"+CGPADDR:");
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGPADDR=1\r\n");
        delay_ms(dltime);
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"+CGPADDR:");
    }
//-----------------------------------------------------------------------------------------------------
    Clear_Buffer();
    //设置阿里云设备信息
    printf("AT+QMTCFG=ALIAUTH,0,\"%s\",\"%s\",\"%s\"\r\n",ProductKey,DeviceName,DeviceSecret);
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"OK");
    while(strx==NULL)
    {
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"OK");
    }
//-----------------------------------------------------------------------------------------------------
    Clear_Buffer();
    //连接阿里云服务器端
    printf("AT+QMTOPEN=0,\"139.196.135.135\",1883\r\n");
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTOPEN: 0,0");
    while(strx==NULL)
    {
        delay_ms(dltime);
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTOPEN: 0,0");
    }
//-----------------------------------------------------------------------------------------------------
    Clear_Buffer();
    //对接阿里云物联网平台创建的设备
    printf("AT+QMTCONN=0,\"air_01\"\r\n");
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTCONN: 0,0,0");//看下返回状态
    while(strx==NULL)
    {
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTCONN: 0,0,0");//看下返回状态
    }
//-----------------------------------------------------------------------------------------------------
    Clear_Buffer();
    //订阅主题
    printf("AT+QMTSUB=0,1,\"%s\",0\r\n",SubTopic);
//		printf("AT+QMTSUB=0,1,\"/id5zsYRSDUB/air_01/user/get\",0\r\n");
    delay_ms(dltime);
    strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTSUB: 0,1,0");//订阅成功
    while(strx==NULL)
    {
        strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTSUB: 0,1,0");//订阅成功
    }
    Clear_Buffer();
}

void SendMessage(u8 tt,u8 hh,float pp,float zz,u8 alarm,u8 param_t,u8 param_h,u16 param_p,float param_z)
{
	printf("AT+QMTPUB=0,1,1,0,\"%s\",\"{\"sys\":{\"ack\":0},\"params\":{mtemp:%d,mhumi:%d,mppm:%0.2f,mze08:%0.4f,alarm:%d,param_t:%d,param_h:%d,param_p:%d,ppm_z:%0.2f}}\"\r\n",PubTopic,tt,hh,pp,zz,alarm,param_t,param_h,param_p,param_z);//发布消息
	strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTPUB: 0,1,0");//发布成功
//	while(strx==NULL)
//	{
//        delay_ms(dltime);
//			strx=strstr((const char*)USART2_RX_BUF,(const char*)"+QMTPUB: 0,1,0");//发布成功
//	}
    Clear_Buffer();
}

void Recv_Data(void)
{
	strx=strstr((const char*)USART2_RX_BUF,(const char*)"\"swch\":1");
	if(strx)
	{
		TIM_Cmd(BASIC_TIM,DISABLE);
		swch_flag = 1;
		TIM_Cmd(BASIC_TIM,ENABLE);
	}
	strx=strstr((const char*)USART2_RX_BUF,(const char*)"\"swch\":0");
	if(strx)
	{
		TIM_Cmd(BASIC_TIM,DISABLE);
		swch_flag = 0;
		TIM_Cmd(BASIC_TIM,ENABLE);
	}
//	strx=strstr((const char*)USART2_RX_BUF,(const char*)"\"relay_flag\":1");
//	if(strx)
//	{
//		TIM_Cmd(BASIC_TIM,DISABLE);
//		relay_flag = 1;
//		TIM_Cmd(BASIC_TIM,ENABLE);
//	}	
//	strx=strstr((const char*)USART2_RX_BUF,(const char*)"\"relay_flag\":0");
//	if(strx)
//	{
//		TIM_Cmd(BASIC_TIM,DISABLE);
//		relay_flag = 0;
//		TIM_Cmd(BASIC_TIM,ENABLE);
//	}	
	Clear_Buffer();
}
