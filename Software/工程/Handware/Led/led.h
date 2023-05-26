#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LED0_GPIO_PIN         GPIO_Pin_4
#define LED0_GPIO_PORT        GPIOA
#define LED0_GPIO_CLK        	RCC_APB2Periph_GPIOA

#define LED1_GPIO_PIN         GPIO_Pin_2
#define LED1_GPIO_PORT        GPIOD
#define LED1_GPIO_CLK        	RCC_APB2Periph_GPIOD

#define BUZZER_GPIO_CLK      RCC_APB2Periph_GPIOA
#define BUZZER_GPIO_PIN      GPIO_Pin_1
#define BUZZER_GPIO_PORT     GPIOA

#define Relay_GPIO_CLK      RCC_APB2Periph_GPIOA
#define Relay_GPIO_PIN      GPIO_Pin_5
#define Relay_GPIO_PORT     GPIOA

#define ON       1
#define OFF      0
//‘\’C语言里面叫续行符，后面不能有任何东西
#define LED0(a)      if(a)\
													GPIO_ResetBits(LED0_GPIO_PORT,LED0_GPIO_PIN);\
										else  GPIO_SetBits(LED0_GPIO_PORT,LED0_GPIO_PIN);
#define LED1(a)      if(a)\
													GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
										else  GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);

#define Relay(a)      if(a)\
													GPIO_SetBits(Relay_GPIO_PORT,Relay_GPIO_PIN);\
										else  GPIO_ResetBits(Relay_GPIO_PORT,Relay_GPIO_PIN);

#define LED0_TOGGLE      {LED0_GPIO_PORT->ODR^=LED0_GPIO_PIN;}
#define LED1_TOGGLE      {LED1_GPIO_PORT->ODR^=LED1_GPIO_PIN;}
#define BUZZER_TOGGLE    {BUZZER_GPIO_PORT->ODR^=BUZZER_GPIO_PIN;}
													
void Led_GPIO_Config(void);
void Buzzer_GPIO_Config(void);
void Relay_GPIO_Config(void);
void Alarm_Moudle_OPEN(void);
void Alarm_Moudle_CLOSE(void);
	
#endif

