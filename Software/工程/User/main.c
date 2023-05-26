#include "sys.h"
#include "json.h"
/**************************************************************************/
extern char USART2_RX_BUF[100];
extern	u16 USART2_RX_STA; 

extern u8 count;//��ʱ������
//extern u8 count3;//��ʱ������

bool swch_flag = 0;//���ر�־λ
u8 relay_flag = 0;//ģʽ��־λ
u8 string[10] = {0};      //�ַ����������

extern u8 flag_usart3,    //����3�жϱ�־λ
buff_usart3[9];  //����3���ջ����������

//-------------------���ø���־λ-------------
u8 mode = 0;       //ģʽѡ���־λ          
s8 menu_flag = 1,  //�˵�ѡ���־λ          
    Set_flag = 0,  //��ֵѡ���־λ          
    flag = 1;      //����ȷ����־λ         
u8 cl = 0,cl3 = 0; //������־λ							 
//--------------------------------------------

//-------------------���ò���-----------------
u8    temp, 			 //�¶�                    
	    humi;        //ʪ��                   
float ppm;         //����Ũ��                
extern u8 alarm;   //����
float p_CO;        //COŨ�Ȳ���             
float p_ze08; //��ȩŨ��(ppm)               
s16 param_t = 40,  //�¶���ֵ����				
    param_h = 90,  //ʪ����ֵ����						 
		param_p = 300; //����Ũ����ֵ����        
float  param_z = 1;    //��ȩŨ����ֵ����       
//--------------------------------------------

/**************************************************************************/

void TimeSow(void);
void ValueSet(void);
void ModeSet(void);
void Menushow(void);
void DATA_GET(void);
void Compare_param(void);
void OPEN_Show(void);
void Send_data(void);
void Swch_ctrl(void);

int main(void)
{
	delay_init();         //��ʱ������ʼ��
	ADCInit();            //ADC��ʼ��
	Led_GPIO_Config();    //LED IO������
	Buzzer_GPIO_Config(); //������ IO������
	Relay_GPIO_Config();  //�̵��� IO������	
	BASIC_TIM_Init();     //��ʱ����ʼ��

	OLED_Init();          //OLED��ʼ��
	OLED_DisplayTurn(0);  //��Ļ��ת180��
	OLED_ColorTurn(0);    //���Ժ���

	EXTI_Key_Config();    //������ʼ��
	NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�	 
	uart1_init(9600);     //����1��ʼ����������PC���д�ӡģ�鷵������   PA9��PA10
	uart2_init(9600);     //��ʼ����BC26���Ӵ���  PA2��PA3
	uart3_init(9600);
	OPEN_Show();          //��������
	TIM_Cmd(BASIC_TIM, ENABLE);//ʹ�ܼ�����
//	TIM_Cmd(TIM3, ENABLE);
	while (1)
	{
//***********************************************************			
		Recv_Data();//ʹ��strstr�����жϴ��ص�����
		DATA_GET();
		if (mode == 0)
		{
				Menushow();
		}
		else if (mode == 1)
		{
				TimeSow();
		}
		else if (mode == 2)
		{
				ValueSet();
		}
		Compare_param();		
		Swch_ctrl();
		Send_data();
//***********************************************************		
	}
}

void OPEN_Show(void)
{
	//--------�����������ϵͳ--------
	OLED_ShowChinese(0, 0, 0, 16, 1);
	OLED_ShowChinese(16, 0, 1, 16, 1);
	OLED_ShowChinese(32, 0, 2, 16, 1);
	OLED_ShowChinese(48, 0, 3, 16, 1);
	OLED_ShowChinese(64, 0, 4, 16, 1);
	OLED_ShowChinese(80, 0, 5, 16, 1);
	OLED_ShowChinese(96, 0, 6, 16, 1);
	OLED_ShowChinese(112, 0, 7, 16, 1);
	//--------------------------------
	OLED_Refresh();
	OLED_ShowString(0, 20, "AT init ...", 16, 1);
	OLED_Refresh();
	AT_Init();   //ģ���ʼ��
	OLED_ShowString(0, 20, "AT init OK ", 16, 1);
	OLED_Refresh();
	OLED_ShowString(0, 40, "MQTT init ...", 16, 1);
	OLED_Refresh();
	MQTT_Init(); //MQTT��ʼ��
	OLED_ShowString(0, 40, "MQTT init OK ", 16, 1);
	OLED_Refresh();
	delay_ms(500);
	OLED_Clear();
	OLED_Refresh();
}


void Menushow(void) 
{
    if (cl == 1)
    {
        OLED_Clear(); // ����
        cl = 0;
    }
	
    /*�˵�����*/
    OLED_ShowChinese(48, 0, 19, 16, 1); // ��
    OLED_ShowChinese(64, 0, 20, 16, 1); // ��

    OLED_ShowChinese(0, 16, 21, 16, 1); // ʵʱ���
    OLED_ShowChinese(16, 16, 22, 16, 1);
    OLED_ShowChinese(32, 16, 23, 16, 1);
    OLED_ShowChinese(48, 16, 24, 16, 1);

    OLED_ShowChinese(0, 32, 15, 16, 1); // ��ֵ����
    OLED_ShowChinese(16, 32, 16, 16, 1);
    OLED_ShowChinese(32, 32, 17, 16, 1);
    OLED_ShowChinese(48, 32, 18, 16, 1);
		
    OLED_ShowChinese(0, 48, 25, 16, 1); // ģʽ
    OLED_ShowChinese(16, 48, 26, 16, 1);
		OLED_ShowString(32, 48, ":", 16, 1);
		OLED_ShowChinese(103, 48, 29, 16, 1);
		
    OLED_Refresh();
    if (menu_flag == 1)
    {      
        OLED_ShowString(119, 16, "<", 16, 1);
				OLED_ShowString(119, 32, " ", 16, 1);
				OLED_ShowString(119, 48, " ", 16, 1);
        OLED_Refresh();
    }
    else if (menu_flag == 2)
    {
        OLED_ShowString(119, 16, " ", 16, 1);
        OLED_ShowString(119, 32, "<", 16, 1);
				OLED_ShowString(119, 48, " ", 16, 1);
        OLED_Refresh();
    }
    else if (menu_flag == 3)
    {
        OLED_ShowString(119, 16, " ", 16, 1);
        OLED_ShowString(119, 32, " ", 16, 1);
				OLED_ShowString(119, 48, "<", 16, 1);	
        OLED_Refresh();			
    }
				if(relay_flag == 0)
				{
				OLED_ShowChinese(83, 48, 27, 16, 1);//��   	
				OLED_Refresh();			
				}
				if(relay_flag == 1)
				{
				OLED_ShowChinese(83, 48, 28, 16, 1);//��   	
				OLED_Refresh();			
				}		
}


void TimeSow(void) 
{
    if (cl == 1)
    {
        OLED_Clear(); // ����
        cl = 0;
    }
    OLED_ShowChinese(32, 0, 21, 16, 1); // ʵʱ���
    OLED_ShowChinese(48, 0, 22, 16, 1);
    OLED_ShowChinese(64, 0, 23, 16, 1);
    OLED_ShowChinese(80, 0, 24, 16, 1);
    //--------�¶�:ʪ��:����Ũ��:--------
    OLED_ShowString(0, 17, "Temp:", 12, 1); //%
    OLED_ShowString(43, 17, "`C;",12, 1); //

    OLED_ShowString(72, 17, "Humi:", 12, 1); //%
    OLED_ShowString(121, 17, "%", 12, 1); //%

    OLED_ShowString(0, 30, "GCC:", 12, 1); //%

    OLED_ShowString(0, 43, "CH2O:", 12, 1); //%
		
//		OLED_ShowString(0, 49, "CO:", 12, 1); //%
    

    sprintf((char *)string, "%d", temp); // ��������ת��Ϊ�ַ�
    OLED_ShowString(30, 17, string, 12, 1);
//		OLED_ShowNum(30,17,temp/10,2,12,1);

    sprintf((char *)string, "%d", humi); // ��������ת��Ϊ�ַ�
    OLED_ShowString(102, 17, string, 12, 1);

    sprintf((char *)string, "%0.2f PPM        ", ppm);
    OLED_ShowString(25, 30, string, 12, 1);

    sprintf((char *)string, "%0.4f PPM        ", p_ze08);
    OLED_ShowString(30, 43, string, 12, 1);
		
//		sprintf((char *)string, "%0.2f PPM        ", p_CO);
//    OLED_ShowString(30, 49, string, 12, 1);
    OLED_Refresh();
}

void ValueSet(void)
{
	if (cl == 1)
	{
		OLED_Clear(); // ����
		cl = 0;
	}	
	if(cl3==1)
	{
		OLED_Clear();
		cl3=0;
	}
	/*��ֵ����*/
	OLED_ShowChinese(32, 0, 15, 16, 1);
	OLED_ShowChinese(48, 0, 16, 16, 1);
	OLED_ShowChinese(64, 0, 17, 16, 1);
	OLED_ShowChinese(80, 0, 18, 16, 1);	

	if(Set_flag!=3)
	{
		OLED_ShowString(0, 18, "Temp:", 16, 1);
		OLED_ShowString(0, 34, "Humi:", 16, 1);
		OLED_ShowString(0, 50, "GCC:", 16, 1);
		OLED_Refresh();

		if (Set_flag == 0)
		{
			OLED_ShowString(119, 48, " ", 16, 1);
			OLED_ShowString(119, 32, " ", 16, 1);
			OLED_ShowString(119, 16, "<", 16, 1);
		}
		else if (Set_flag == 1)
		{
			OLED_ShowString(119, 16, " ", 16, 1);
			OLED_ShowString(119, 48, " ", 16, 1);
			OLED_ShowString(119, 32, "<", 16, 1);
		}
		else if (Set_flag == 2)
		{
			OLED_ShowString(119, 32, " ", 16, 1);
			OLED_ShowString(119, 16, " ", 16, 1);
			OLED_ShowString(119, 48, "<", 16, 1);
		}
		sprintf((char *)string, "%2d", param_t); // ��������ת��Ϊ�ַ�
		OLED_ShowString(102, 16, string, 16, 1);

		sprintf((char *)string, "%2d", param_h); // ��������ת��Ϊ�ַ�
		OLED_ShowString(102, 32, string, 16, 1);

		sprintf((char *)string, "%3d", param_p); // ��������ת��Ϊ�ַ�
		OLED_ShowString(94, 48, string, 16, 1);
		OLED_Refresh();			
	}

	else if(Set_flag == 3)//��ҳ
	{
		OLED_ShowString(0, 18, "CH2O:", 16, 1);			 
		OLED_ShowString(119, 16, "<", 16, 1);
		sprintf((char *)string, "%0.2f", param_z);
		OLED_ShowString(87, 16, string, 16, 1);
		OLED_Refresh();
	}
}
void DATA_GET(void) 
{
		ppm = Read_Mq135_Data();//��ȡ����Ũ��ֵ
		//	p_CO = Read_Mq7_Data();	
		DHT11_Read_Data(&temp, &humi);//�ɼ�ʪ��
		//  delay_ms(50);
		if(flag_usart3 == 1)//���ڽ����ж�3��־λ ����ZE08����CH2O����������
		{
					p_ze08 = (buff_usart3[4]*256+buff_usart3[5])/1000.0;//��λ����ڵ�Ũ��ֵ
		//					p_ze08 = (buff_usart3[4]*256 + buff_usart3[5])/66.64 * 0.08; 
		//				USART1_printf("ppm:%f\r\n",p_ze08);//�����ǲ������ ʹ�ô���1����					
					memset(buff_usart3,'\0',255);
					flag_usart3 = 0;//�������2�����ж�
		}			
}

void Compare_param(void)
{
	if((temp > param_t)||(humi > param_h)||(ppm > param_p)||(p_ze08 > param_z)) 
	{
		if(count%5==0 )//����Ƶ��
		{
			Alarm_Moudle_OPEN();
		}		
	}
	else
	{
		Alarm_Moudle_CLOSE();
	}
}

void Send_data(void)
{
	//**************��ʱ��������**************		
		if(count==8)
		{
			count=0;
			TIM_Cmd(BASIC_TIM,DISABLE);
			SendMessage(temp,humi,ppm,p_ze08,alarm,param_t,param_h,param_p,param_z);
			delay_ms(100);
			TIM_Cmd(BASIC_TIM,ENABLE);
		}	
}

void Swch_ctrl(void)
{
		if(relay_flag==1)
		{
		LED1(1);
			if(swch_flag==1)
			{
				Relay(1);
			}
			else if(swch_flag==0)
			{
				Relay(0);
			}
		}
		else if(relay_flag==0)
		{
			LED1(0);		
		}			
}
