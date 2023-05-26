#include "sys.h"
#include "json.h"
/**************************************************************************/
extern char USART2_RX_BUF[100];
extern	u16 USART2_RX_STA; 

extern u8 count;//定时器计数
//extern u8 count3;//定时器计数

bool swch_flag = 0;//开关标志位
u8 relay_flag = 0;//模式标志位
u8 string[10] = {0};      //字符串数组变量

extern u8 flag_usart3,    //串口3中断标志位
buff_usart3[9];  //串口3接收缓冲数组变量

//-------------------配置各标志位-------------
u8 mode = 0;       //模式选择标志位          
s8 menu_flag = 1,  //菜单选择标志位          
    Set_flag = 0,  //阈值选择标志位          
    flag = 1;      //按键确定标志位         
u8 cl = 0,cl3 = 0; //清屏标志位							 
//--------------------------------------------

//-------------------配置参数-----------------
u8    temp, 			 //温度                    
	    humi;        //湿度                   
float ppm;         //气体浓度                
extern u8 alarm;   //警报
float p_CO;        //CO浓度参数             
float p_ze08; //甲醛浓度(ppm)               
s16 param_t = 40,  //温度阈值参数				
    param_h = 90,  //湿度阈值参数						 
		param_p = 300; //气体浓度阈值参数        
float  param_z = 1;    //甲醛浓度阈值参数       
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
	delay_init();         //延时函数初始化
	ADCInit();            //ADC初始化
	Led_GPIO_Config();    //LED IO口配置
	Buzzer_GPIO_Config(); //蜂鸣器 IO口配置
	Relay_GPIO_Config();  //继电器 IO口配置	
	BASIC_TIM_Init();     //定时器初始化

	OLED_Init();          //OLED初始化
	OLED_DisplayTurn(0);  //屏幕旋转180度
	OLED_ColorTurn(0);    //反显函数

	EXTI_Key_Config();    //按键初始化
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	 
	uart1_init(9600);     //串口1初始化，可连接PC进行打印模块返回数据   PA9、PA10
	uart2_init(9600);     //初始化和BC26连接串口  PA2、PA3
	uart3_init(9600);
	OPEN_Show();          //开机动画
	TIM_Cmd(BASIC_TIM, ENABLE);//使能计数器
//	TIM_Cmd(TIM3, ENABLE);
	while (1)
	{
//***********************************************************			
		Recv_Data();//使用strstr函数判断传回的数据
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
	//--------空气质量检测系统--------
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
	AT_Init();   //模块初始化
	OLED_ShowString(0, 20, "AT init OK ", 16, 1);
	OLED_Refresh();
	OLED_ShowString(0, 40, "MQTT init ...", 16, 1);
	OLED_Refresh();
	MQTT_Init(); //MQTT初始化
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
        OLED_Clear(); // 清屏
        cl = 0;
    }
	
    /*菜单界面*/
    OLED_ShowChinese(48, 0, 19, 16, 1); // 菜
    OLED_ShowChinese(64, 0, 20, 16, 1); // 单

    OLED_ShowChinese(0, 16, 21, 16, 1); // 实时检测
    OLED_ShowChinese(16, 16, 22, 16, 1);
    OLED_ShowChinese(32, 16, 23, 16, 1);
    OLED_ShowChinese(48, 16, 24, 16, 1);

    OLED_ShowChinese(0, 32, 15, 16, 1); // 阈值设置
    OLED_ShowChinese(16, 32, 16, 16, 1);
    OLED_ShowChinese(32, 32, 17, 16, 1);
    OLED_ShowChinese(48, 32, 18, 16, 1);
		
    OLED_ShowChinese(0, 48, 25, 16, 1); // 模式
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
				OLED_ShowChinese(83, 48, 27, 16, 1);//自   	
				OLED_Refresh();			
				}
				if(relay_flag == 1)
				{
				OLED_ShowChinese(83, 48, 28, 16, 1);//手   	
				OLED_Refresh();			
				}		
}


void TimeSow(void) 
{
    if (cl == 1)
    {
        OLED_Clear(); // 清屏
        cl = 0;
    }
    OLED_ShowChinese(32, 0, 21, 16, 1); // 实时检测
    OLED_ShowChinese(48, 0, 22, 16, 1);
    OLED_ShowChinese(64, 0, 23, 16, 1);
    OLED_ShowChinese(80, 0, 24, 16, 1);
    //--------温度:湿度:气体浓度:--------
    OLED_ShowString(0, 17, "Temp:", 12, 1); //%
    OLED_ShowString(43, 17, "`C;",12, 1); //

    OLED_ShowString(72, 17, "Humi:", 12, 1); //%
    OLED_ShowString(121, 17, "%", 12, 1); //%

    OLED_ShowString(0, 30, "GCC:", 12, 1); //%

    OLED_ShowString(0, 43, "CH2O:", 12, 1); //%
		
//		OLED_ShowString(0, 49, "CO:", 12, 1); //%
    

    sprintf((char *)string, "%d", temp); // 将浮点数转换为字符
    OLED_ShowString(30, 17, string, 12, 1);
//		OLED_ShowNum(30,17,temp/10,2,12,1);

    sprintf((char *)string, "%d", humi); // 将浮点数转换为字符
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
		OLED_Clear(); // 清屏
		cl = 0;
	}	
	if(cl3==1)
	{
		OLED_Clear();
		cl3=0;
	}
	/*阈值设置*/
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
		sprintf((char *)string, "%2d", param_t); // 将浮点数转换为字符
		OLED_ShowString(102, 16, string, 16, 1);

		sprintf((char *)string, "%2d", param_h); // 将浮点数转换为字符
		OLED_ShowString(102, 32, string, 16, 1);

		sprintf((char *)string, "%3d", param_p); // 将浮点数转换为字符
		OLED_ShowString(94, 48, string, 16, 1);
		OLED_Refresh();			
	}

	else if(Set_flag == 3)//翻页
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
		ppm = Read_Mq135_Data();//获取气体浓度值
		//	p_CO = Read_Mq7_Data();	
		DHT11_Read_Data(&temp, &humi);//采集湿度
		//  delay_ms(50);
		if(flag_usart3 == 1)//串口接收中断3标志位 接受ZE08——CH2O传来的数据
		{
					p_ze08 = (buff_usart3[4]*256+buff_usart3[5])/1000.0;//单位面积内的浓度值
		//					p_ze08 = (buff_usart3[4]*256 + buff_usart3[5])/66.64 * 0.08; 
		//				USART1_printf("ppm:%f\r\n",p_ze08);//这里是测试语句 使用串口1发送					
					memset(buff_usart3,'\0',255);
					flag_usart3 = 0;//清除串口2接受中断
		}			
}

void Compare_param(void)
{
	if((temp > param_t)||(humi > param_h)||(ppm > param_p)||(p_ze08 > param_z)) 
	{
		if(count%5==0 )//警报频率
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
	//**************定时发送数据**************		
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
