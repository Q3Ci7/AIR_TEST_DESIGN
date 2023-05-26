#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "led.h"
/*
**********************
按键KEY
**********************
*/

//下
#define KEY0_INT_GPIO_PIN        GPIO_Pin_15
#define KEY0_INT_GPIO_PORT        GPIOA
#define KEY0_INT_GPIO_CLK        	RCC_APB2Periph_GPIOA
//上
#define KEY1_INT_GPIO_PIN         GPIO_Pin_3
#define KEY1_INT_GPIO_PORT        GPIOB
#define KEY1_INT_GPIO_CLK        	RCC_APB2Periph_GPIOB
//返回
#define KEY2_INT_GPIO_PIN         GPIO_Pin_4
#define KEY2_INT_GPIO_PORT        GPIOB
#define KEY2_INT_GPIO_CLK        	RCC_APB2Periph_GPIOB
//确定
#define KEY3_INT_GPIO_PIN         GPIO_Pin_5
#define KEY3_INT_GPIO_PORT        GPIOB
#define KEY3_INT_GPIO_CLK        	RCC_APB2Periph_GPIOB

void EXTI_Key_Config(void);
void KEY_Detection(void);
u8 KEY_Scan(u8 mode);
#define K4  GPIO_ReadInputDataBit(KEY0_INT_GPIO_PORT,KEY0_INT_GPIO_PIN)
#define K3  GPIO_ReadInputDataBit(KEY1_INT_GPIO_PORT,KEY1_INT_GPIO_PIN)
#define K2  GPIO_ReadInputDataBit(KEY2_INT_GPIO_PORT,KEY2_INT_GPIO_PIN)
#define K1  GPIO_ReadInputDataBit(KEY3_INT_GPIO_PORT,KEY3_INT_GPIO_PIN)
#endif

