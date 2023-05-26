#include "key.h"
#include "usart.h"
#include "delay.h"
/******************************/
extern u8 mode;
//extern u8 k0, k1, k2, k3;
extern s8 menu_flag, // 菜单标志位
	Set_flag,		 // 阈值设置标志位
	flag;			 // 按键确定标志位
extern u8 relay_flag;
extern s8 param_t,	 // 温度阈值参数
	param_h;		 // 湿度阈值参数
extern s16 param_p;		 // 气体浓度阈值参数
extern float  param_z;              //甲醛浓度阈值参数
extern u8 cl,cl3;
/******************************/

static void EXTI_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           //配置中断分组
	/*配置   按键0 嵌套向量中断控制器NVIC*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;	   //*配置中断源选择
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;		   //*配置使能总中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //*配置抢占优先级0-1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		   //*配置子优先级 0-7
	NVIC_Init(&NVIC_InitStruct);

	/*配置中断源：  按键1    其他配置同上*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn; //*配置中断源选择
	NVIC_Init(&NVIC_InitStruct);
	/*配置中断源：  按键2    其他配置同上*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn; //*配置中断源选择
	NVIC_Init(&NVIC_InitStruct);
	/*配置中断源：  按键3    其他配置同上*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn; //*配置中断源选择
	NVIC_Init(&NVIC_InitStruct);
}

 void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;

	// 3、配置中断优先级
	EXTI_NVIC_Config(); // 只能被该函数所调用
	// 1、初始化GPIO
	RCC_APB2PeriphClockCmd(KEY0_INT_GPIO_CLK| KEY1_INT_GPIO_CLK | KEY2_INT_GPIO_CLK | KEY3_INT_GPIO_CLK |RCC_APB2Periph_AFIO, ENABLE);// 
	/*PB3、PB4，还有PA13-15这五个引脚不是普通的IO口，用作JTAG/SWD仿真器的调试接口,这些引脚只
	有重定义	功能后才能作为普通的IO口使用，所以我们需要对这些引脚进行配置的时候开启AFIO复用时钟。
	*/
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 用PB3，PB4，PA15做普通IO

	//PA15--Key0配置================================================================
	GPIO_InitStruct.GPIO_Pin = KEY0_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY0_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);  //输入线
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line15;                     //外部中断线选择
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //使能中断屏蔽寄存器
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;          //触发选择 -- 下降沿触发
	EXTI_Init(&EXTI_InitStruct);                         				 //调用函数将结构体成员写入相应寄存器	
	
	
	//PB3--Key1配置================================================================
	GPIO_InitStruct.GPIO_Pin = KEY1_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);   //输入线 -- 将相应的GPIO引脚连到相应的中断线上
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line3;                      //外部中断线选择
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //使能中断屏蔽寄存器
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //触发选择 -- 下降沿触发
	EXTI_Init(&EXTI_InitStruct);                         				 //调用函数将结构体成员写入相应寄存器	
	
	//PB4--Key2配置================================================================
	GPIO_InitStruct.GPIO_Pin = KEY2_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);   //输入线 -- 将相应的GPIO引脚连到相应的中断线上
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;                      //外部中断线选择
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //使能中断屏蔽寄存器
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //触发选择 -- 下降沿触发
	EXTI_Init(&EXTI_InitStruct);                         				 //调用函数将结构体成员写入相应寄存器		
	
	//PB5--Key3配置================================================================
	GPIO_InitStruct.GPIO_Pin = KEY3_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY3_INT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);   //输入线 -- 将相应的GPIO引脚连到相应的中断线上
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;                      //外部中断线选择
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                       //使能中断屏蔽寄存器
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;             //模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;         //触发选择 -- 下降沿触发
	EXTI_Init(&EXTI_InitStruct);                         				 //调用函数将结构体成员写入相应寄存器		
}
#if 1
u8 key_delay = 30;
//======================= 按键KEY4 -- PA15
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)//菜单界面
		{
			menu_flag ++;
			if(menu_flag>3)menu_flag = 1;
		}
		if (mode == 2)//阈值设置界面
		{
			if (flag == 0)//如果未按下确认键，表示选择模式
			{
				Set_flag++;
				if (Set_flag > 3)
					Set_flag = 3;
				if(Set_flag==3)
				{
					cl3=1;//清屏标志位
				}
			}
			else if (flag == 1)//如果按下确认键，表示增减模式
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
//======================= 按键KEY3 -- PB3
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)//菜单界面
		{
			menu_flag --;
			if(menu_flag<1)menu_flag = 3;
		}
		if (mode == 2)//阈值设置界面
		{
			if (flag == 0)//如果未按下确认键，表示选择模式
			{
				Set_flag--;
				if (Set_flag < 0)
					Set_flag = 0;
				if(Set_flag==2)
				{
					cl3=1;//清屏标志位
				}
			}
			else if (flag == 1)//如果按下确认键，表示增减模式
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
//======================= 按键KEY2 -- PB4
void EXTI4_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 1)
		{
			mode = 0; // 返回模式0
			cl = 1;
		}

		if (mode == 2)
		{
			flag = 1;
			mode = 0; // 返回模式0
			cl = 1;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}
//======================= 按键KEY1 -- PB5
void EXTI9_5_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		delay_ms(key_delay);
		if (mode == 0)
		{
			cl = 1; // 清屏
			if (menu_flag == 1)
				mode = 1; // 跳转到模式一：实时监测
			if (menu_flag == 2)
				mode = 2; // 跳转到模式二：阈值设置
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
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;

u8 KEY_Scan(u8 mode)//按键扫描
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(K1==0||K2==0||K3==0||K4==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(K1==0)return 1;
		else if(K2==0)return 2;
		else if(K3==0)return 3;
		else if(K4==0)return 4;
	}else if(K1==1&&K2==1&&K3==1&&K4==1)key_up=1; 	     
	return 0;// 无按键按下
}
void KEY_Detection(void)//按键检测
{
	switch(KEY_Scan(0))
	{
		case 1:
					if (mode == 0)
					{
						cl = 1; // 清屏
						if (menu_flag == 0)
							mode = 1; // 跳转到模式一：实时监测
						if (menu_flag == 1)
							mode = 2; // 跳转到模式二：阈值设置
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
						mode = 0; // 返回模式0
						cl = 1;
					}

					if (mode == 2)
					{
						flag = 1;
						mode = 0; // 返回模式0
						cl = 1;
					}			
		break;
		case 3:
					if (mode == 0)//菜单界面
						menu_flag = 0;
					if (mode == 2)//阈值设置界面
					{
						if (flag == 0)//如果未按下确认键，表示选择模式
						{
							Set_flag--;
							if (Set_flag < 0)
								Set_flag = 0;
							if(Set_flag==2)
							{
								cl3=1;//清屏标志位
							}
						}
						else if (flag == 1)//如果按下确认键，表示增减模式
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
					if (mode == 0)//菜单界面
						menu_flag = 1;
					if (mode == 2)//阈值设置界面
					{
						if (flag == 0)//如果未按下确认键，表示选择模式
						{
							Set_flag++;
							if (Set_flag > 3)
								Set_flag = 3;
							if(Set_flag==3)
							{
								cl3=1;//清屏标志位
							}
						}
						else if (flag == 1)//如果按下确认键，表示增减模式
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


