#include "json.h"
#include <jansson.h>
#include "usart.h"

//s16 param_t = 25,  //�¶���ֵ����						 |
//    param_h = 70,  //ʪ����ֵ����						 |
//		param_p = 300; //����Ũ����ֵ����  


//bool data_sys;//����JSON�����������б�־λ 0������ 1����
extern bool swch_flag;

//���ص�JSON����{"status":true}
//�������⣬ʹ�ú󴮿�printf�޷�ʹ�ã�����ʹ��USART1_printf����ʵ�ַ���
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
//		/* bool ���͵Ľ��� */
//		status = json_object_get(root, "swch");
//		if(json_is_true(status))//��
//		{
//			printf("��");
//			swch_flag = 1;			
//		}
//		else if(json_is_false(status))//��
//		{
//			printf("��");
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
	json_decref(root);//�ͷ�JSON�ռ�
	return 0;
}

/*
// string ���͵Ľ���
name = (char *)json_string_value(json_object_get(root, "name"));
// int ���͵Ľ���
age = json_integer_value(json_object_get(root, "age"));
// double ���͵Ľ���
score = json_real_value(json_object_get(root, "score"));
// bool ���͵Ľ���
status = json_object_get(root, "status");
if(json_is_true(status))
else if(json_is_false(status))
*/



