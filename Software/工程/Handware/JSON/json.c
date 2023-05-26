#include "json.h"
#include <jansson.h>
#include "usart.h"

//s16 param_t = 25,  //温度阈值参数						 |
//    param_h = 70,  //湿度阈值参数						 |
//		param_p = 300; //气体浓度阈值参数  


//bool data_sys;//允许JSON解析函数运行标志位 0不允许 1允许
extern bool swch_flag;

//开关灯JSON函数{"status":true}
//调试问题，使用后串口printf无法使用，建议使用USART1_printf函数实现发送
uint8_t Jansson1_Analysis(char *text)
{
	json_error_t error;
	json_t *root;
	json_t *status;
	u8 param_t;
	u8 led;
	root = json_loads((const char*)text, 0, &error); 
	
//	if(json_is_object(root))
//	{
//		/* bool 类型的解析 */
//		status = json_object_get(root, "swch");
//		if(json_is_true(status))//真
//		{
//			printf("真");
//			swch_flag = 1;			
//		}
//		else if(json_is_false(status))//假
//		{
//			printf("假");
//			swch_flag = 0;		
//		}	
//	}

		if(json_is_object(root))
		{
		led = json_integer_value(json_object_get(root, "swch"));
			if(led == 1)
			{
				printf("led:%d\r\n",led);
				swch_flag = 1;		
			}
			else if(led == 0)
			{
				printf("led:%d\r\n",led);
				swch_flag = 0;
			}
//		USART2_printf("led: %d\r\n", led);
			
		param_t = json_integer_value(json_object_get(root, "param_t"));			
		}

	
//	else
//	{
//		USART2_printf("root format error:%d-%s\r\n", error.line, error.text);
//		return 1;
//	}
	json_decref(root);//释放JSON空间
	return 0;
}

/*
// string 类型的解析
name = (char *)json_string_value(json_object_get(root, "name"));
// int 类型的解析
age = json_integer_value(json_object_get(root, "age"));
// double 类型的解析
score = json_real_value(json_object_get(root, "score"));
// bool 类型的解析
status = json_object_get(root, "status");
if(json_is_true(status))
else if(json_is_false(status))
*/



