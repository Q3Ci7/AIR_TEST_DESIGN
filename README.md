 <div align=center>#这是一个空气质量检测系统设计</div>

 <div align=center><img width="500" height="500" src="https://github.com/Q3Ci7/-/assets/89730447/c8ff24fe-8a1d-4b26-b83c-440076ad7f68"/></div>

  系统上电后首先初始化使用到的外设以及各传感器模块，配置引脚端口，同时检测NB通信模块以及物联网卡是否到位并在屏幕上显示初始化状态。

 <div align=center><img width="300" height="300" src="https://github.com/Q3Ci7/-/assets/89730447/02e06956-162c-44f6-9278-d0f60a5876f8"/></div>
 
  初始化完成建立好通信连接后系统进入菜单界面，菜单界面包含3个选项分别为实时检测、阈值设置和模式，通过按键进行选择。

<div align=center><img width="300" height="300" src="https://github.com/Q3Ci7/-/assets/89730447/ce1701c5-8c12-401e-a20b-2bcb2678b25a"/></div>

  选择实时检测界面，按确定键进入可以观察到实时的温度、湿度、气体浓度、甲醛浓度的具体数值。

<div align=center><img width="300" height="300" src="https://github.com/Q3Ci7/-/assets/89730447/fc0d42dc-a9f5-49d2-aecc-280dc9cbcda8"/></div>

  选择阈值设置界面，界面显示为温度阈值、湿度阈值、气体浓度阈值、甲醛浓度阈值，选择对应的阈值参数按下确认键表示选中，上下键调整阈值，再次按下确认键表示设定该阈值退出设置。当有检测值超出其对应的阈值时，系统声光报警提醒。

<div align=center><img width="400" height="300" src="https://github.com/Q3Ci7/-/assets/89730447/5ceda6e3-62b3-4454-8848-0d144512be69"/></div>
 
  选择模式选项，通过按下确定键可以切换模式为自动模式或手动模式，在自动模式下，当有检测值超出其对应的阈值时会自动打开继电器，在手动模式下可以通过手机APP控制继电器开关与否，继电器可以外接加温、加湿、排气扇等设备，具体所接设备可以依据使用情景和需求而定。

 <div align=center><img width="900" height="300" src="https://github.com/Q3Ci7/-/assets/89730447/3f0f4efe-89e4-461f-aebf-fecdca905a65"/></div>

  综上，经过实验后，结果表示该系统能够较为准确得测量温度、湿度、气体浓度、甲醛浓度的含量值，在超过阈值后能及时报警，能及时将数据及异常报警状态反馈给云端上位机。经实践测量后发现，该系统的达到了预期的效果。虽然还存在一些需要改进的地方，但总体来说，该系统已经初步具备了实际应用的能力。
