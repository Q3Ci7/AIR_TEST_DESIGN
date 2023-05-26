#ifndef __JSON_H
#define __JSON_H 	
#include "stm32f10x.h"
#include "stdbool.h"	


extern bool data_sys;//允许JSON解析函数运行标志位 0不允许 1允许
uint8_t Jansson1_Analysis(char *text);
#endif
