#include "led.h"
#include "stdbool.h"
#include "time.h"
u8 alarm;
extern u8 relay_flag;
void Led_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
		
	RCC_APB2PeriphClockCmd(LED0_GPIO_CLK|LED1_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LED0_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED1_GPIO_PIN;
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
//**************配置led灯初始状态***********//
//配置LED0初始态
#if 1
	GPIO_SetBits(LED0_GPIO_PORT,LED0_GPIO_PIN);
#else 
	GPIO_ResetBits(LED0_GPIO_PORT,LED0_GPIO_PIN);
#endif
//配置LED1初始态
#if 1
	GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
#else
	GPIO_ResetBits(LED0_GPIO_PORT,LED0_GPIO_PIN);
#endif
}


void Buzzer_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;		
	RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = BUZZER_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);
	GPIO_ResetBits(BUZZER_GPIO_PORT,BUZZER_GPIO_PIN);
}

void Relay_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;		
	RCC_APB2PeriphClockCmd(Relay_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = Relay_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Relay_GPIO_PORT, &GPIO_InitStruct);
	GPIO_ResetBits(Relay_GPIO_PORT,Relay_GPIO_PIN);
}



void Alarm_Moudle_OPEN(void)
{
//	LED0_TOGGLE;
	LED0(1);
	if(relay_flag==0){Relay(1);}//自动模式
//	BUZZER_TOGGLE;
	GPIO_SetBits(BUZZER_GPIO_PORT,BUZZER_GPIO_PIN);
	alarm = 1;
}

void Alarm_Moudle_CLOSE(void)
{
	LED0(0);
	if(relay_flag==0){Relay(0);}//自动模式
	GPIO_ResetBits(BUZZER_GPIO_PORT,BUZZER_GPIO_PIN);
	alarm = 0;
//	relay_flag = 0;
}
