#include "key.h"
#include "usart.h"
#include "delay.h"
/******************************/
extern u8 mode;
//extern u8 k0, k1, k2, k3;
extern s8 menu_flag, // �˵���־λ
	Set_flag,		 // ��ֵ���ñ�־λ
	flag;			 // ����ȷ����־λ
extern u8 relay_flag;
extern s8 param_t,	 // �¶���ֵ����
	param_h;		 // ʪ����ֵ����
extern s16 param_p;		 // ����Ũ����ֵ����
extern float  param_z;              //��ȩŨ����ֵ����
extern u8 cl,cl3;
/******************************/

static void EXTI_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           //�����жϷ���
	/*����   ����0 Ƕ�������жϿ�����NVIC*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;	   //*�����ж�Դѡ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;		   //*����ʹ�����ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //*������ռ���ȼ�0-1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		   //*���������ȼ� 0-7
	NVIC_Init(&NVIC_InitStruct);

	/*�����ж�Դ��  ����1    ��������ͬ��*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn; //*�����ж�Դѡ��
	NVIC_Init(&NVIC_InitStruct);
	/*�����ж�Դ��  ����2    ��������ͬ��*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn; //*�����ж�Դѡ��
	NVIC_Init(&NVIC_InitStruct);
	/*�����ж�Դ��  ����3    ��������ͬ��*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn; //*�����ж�Դѡ��
	NVIC_Init(&NVIC_InitStruct);
}

 void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;

	// 3�������ж����ȼ�
	EXTI_NVIC_Config(); // ֻ�ܱ��ú���������
	// 1����ʼ��GPIO
	RCC_APB2PeriphClockCmd(KEY0_INT_GPIO_CLK| KEY1_INT_GPIO_CLK | KEY2_INT_GPIO_CLK | KEY3_INT_GPIO_CLK |RCC_APB2Periph_AFIO, ENABLE);// 
	/*PB3��PB4������PA13-15��������Ų�����ͨ��IO�ڣ�����JTAG/SWD�������ĵ��Խӿ�,��Щ����ֻ
	���ض���	���ܺ������Ϊ��ͨ��IO��ʹ�ã�����������Ҫ����Щ���Ž������õ�ʱ����AFIO����ʱ�ӡ�
	*/
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // ��PB3��PB4��PA15����ͨIO

	//PA15--Key0����================================================================
	GPIO_InitStruct.GPIO_Pin = KEY0_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY0_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);  //������
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line15;                     //�ⲿ�ж���ѡ��
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //ʹ���ж����μĴ���
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;          //����ѡ�� -- �½��ش���
	EXTI_Init(&EXTI_InitStruct);                         				 //���ú������ṹ���Աд����Ӧ�Ĵ���	
	
	
	//PB3--Key1����================================================================
	GPIO_InitStruct.GPIO_Pin = KEY1_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);   //������ -- ����Ӧ��GPIO����������Ӧ���ж�����
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line3;                      //�ⲿ�ж���ѡ��
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //ʹ���ж����μĴ���
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //����ѡ�� -- �½��ش���
	EXTI_Init(&EXTI_InitStruct);                         				 //���ú������ṹ���Աд����Ӧ�Ĵ���	
	
	//PB4--Key2����================================================================
	GPIO_InitStruct.GPIO_Pin = KEY2_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);   //������ -- ����Ӧ��GPIO����������Ӧ���ж�����
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;                      //�ⲿ�ж���ѡ��
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //ʹ���ж����μĴ���
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //����ѡ�� -- �½��ش���
	EXTI_Init(&EXTI_InitStruct);                         				 //���ú������ṹ���Աд����Ӧ�Ĵ���		
	
	//PB5--Key3����================================================================
	GPIO_InitStruct.GPIO_Pin = KEY3_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY3_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);   //������ -- ����Ӧ��GPIO����������Ӧ���ж�����
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;                      //�ⲿ�ж���ѡ��
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //ʹ���ж����μĴ���
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //����ѡ�� -- �½��ش���
	EXTI_Init(&EXTI_InitStruct);                         				 //���ú������ṹ���Աд����Ӧ�Ĵ���		
}
#if 1
u8 key_delay = 30;
//======================= ����KEY4 -- PA15
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)//�˵�����
		{
			menu_flag ++;
			if(menu_flag>3)menu_flag = 1;
		}
		if (mode == 2)//��ֵ���ý���
		{
			if (flag == 0)//���δ����ȷ�ϼ�����ʾѡ��ģʽ
			{
				Set_flag++;
				if (Set_flag > 3)
					Set_flag = 3;
				if(Set_flag==3)
				{
					cl3=1;//������־λ
				}
			}
			else if (flag == 1)//�������ȷ�ϼ�����ʾ����ģʽ
			{
				if (Set_flag == 0)
				{
					param_t--;
					if (param_t < 0)
						param_t = 0;
				}
				if (Set_flag == 1)
				{
					param_h--;
					if (param_h < 0)
						param_h = 0;
				}
				if (Set_flag == 2)
				{
					param_p--;
					if (param_p < 0)
						param_p = 0;
				}
				if (Set_flag == 3)
				{
					param_z = param_z-0.01;
					if (param_z < 0)
						param_z = 0;
				}
			}
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line15);
}
//======================= ����KEY3 -- PB3
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)//�˵�����
		{
			menu_flag --;
			if(menu_flag<1)menu_flag = 3;
		}
		if (mode == 2)//��ֵ���ý���
		{
			if (flag == 0)//���δ����ȷ�ϼ�����ʾѡ��ģʽ
			{
				Set_flag--;
				if (Set_flag < 0)
					Set_flag = 0;
				if(Set_flag==2)
				{
					cl3=1;//������־λ
				}
			}
			else if (flag == 1)//�������ȷ�ϼ�����ʾ����ģʽ
			{
				if (Set_flag == 0)
				{
					param_t++;
					if (param_t > 99)
						param_t = 99;
				}
				if (Set_flag == 1)
				{
					param_h++;
					if (param_h > 99)
						param_h = 99;
				}
				if (Set_flag == 2)
				{
					param_p++;
					if (param_p > 999)
						param_p = 999;
				}
				if (Set_flag == 3)
				{
					param_z = param_z+0.01;
					if (param_z > 3)
						param_z = 3;					
				}
			}
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}
//======================= ����KEY2 -- PB4
void EXTI4_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 1)
		{
			mode = 0; // ����ģʽ0
			cl = 1;
		}

		if (mode == 2)
		{
			flag = 1;
			mode = 0; // ����ģʽ0
			cl = 1;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}
//======================= ����KEY1 -- PB5
void EXTI9_5_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)
		{
			cl = 1; // ����
			if (menu_flag == 1)
				mode = 1; // ��ת��ģʽһ��ʵʱ���
			if (menu_flag == 2)
				mode = 2; // ��ת��ģʽ������ֵ����
			if (menu_flag == 3)
			{
				if(relay_flag == 0) relay_flag=1;
				else if(relay_flag == 1) relay_flag=0;				
			}		
		}

		if (mode == 2)
		{
			if (flag == 0)
				flag = 1;
			else if (flag == 1)
				flag = 0;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line5);
}
#else
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;

u8 KEY_Scan(u8 mode)//����ɨ��
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(K1==0||K2==0||K3==0||K4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(K1==0)return 1;
		else if(K2==0)return 2;
		else if(K3==0)return 3;
		else if(K4==0)return 4;
	}else if(K1==1&&K2==1&&K3==1&&K4==1)key_up=1; 	     
	return 0;// �ް�������
}
void KEY_Detection(void)//�������
{
	switch(KEY_Scan(0))
	{
		case 1:
					if (mode == 0)
					{
						cl = 1; // ����
						if (menu_flag == 0)
							mode = 1; // ��ת��ģʽһ��ʵʱ���
						if (menu_flag == 1)
							mode = 2; // ��ת��ģʽ������ֵ����
					}
					if (mode == 2)
					{
						if (flag == 0)
							flag = 1;
						else if (flag == 1)
							flag = 0;
					}
		break;
		
		case 2:
					if (mode == 1)
					{
						mode = 0; // ����ģʽ0
						cl = 1;
					}

					if (mode == 2)
					{
						flag = 1;
						mode = 0; // ����ģʽ0
						cl = 1;
					}			
		break;
		case 3:
					if (mode == 0)//�˵�����
						menu_flag = 0;
					if (mode == 2)//��ֵ���ý���
					{
						if (flag == 0)//���δ����ȷ�ϼ�����ʾѡ��ģʽ
						{
							Set_flag--;
							if (Set_flag < 0)
								Set_flag = 0;
							if(Set_flag==2)
							{
								cl3=1;//������־λ
							}
						}
						else if (flag == 1)//�������ȷ�ϼ�����ʾ����ģʽ
						{
							if (Set_flag == 0)
							{
								param_t++;
								if (param_t > 99)
									param_t = 99;
							}
							if (Set_flag == 1)
							{
								param_h++;
								if (param_h > 99)
									param_h = 99;
							}
							if (Set_flag == 2)
							{
								param_p++;
								if (param_p > 999)
									param_p = 999;
							}
							if (Set_flag == 3)
							{
								param_z = param_z+0.1;
								if (param_z > 3)
									param_z = 3;					
							}
						}
					}			
		break;
		
		case 4:
					if (mode == 0)//�˵�����
						menu_flag = 1;
					if (mode == 2)//��ֵ���ý���
					{
						if (flag == 0)//���δ����ȷ�ϼ�����ʾѡ��ģʽ
						{
							Set_flag++;
							if (Set_flag > 3)
								Set_flag = 3;
							if(Set_flag==3)
							{
								cl3=1;//������־λ
							}
						}
						else if (flag == 1)//�������ȷ�ϼ�����ʾ����ģʽ
						{
							if (Set_flag == 0)
							{
								param_t--;
								if (param_t < 0)
									param_t = 0;
							}
							if (Set_flag == 1)
							{
								param_h--;
								if (param_h < 0)
									param_h = 0;
							}
							if (Set_flag == 2)
							{
								param_p--;
								if (param_p < 0)
									param_p = 0;
							}
							if (Set_flag == 3)
							{
								param_z = param_z-0.1;
								if (param_z < 0)
									param_z = 0;
							}
						}
					}			
		break;
			default:
				delay_ms(10);		
	}
}
#endif


