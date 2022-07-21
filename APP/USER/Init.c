/******************************************************************
 * Module		:	Init.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2018/7
 *******************************************************************/
#include "User.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Suart.h"
#include "init.h"
#include "Uart485.h"

static void McuGPIOInit(void);
static void McuTimeInit(void);
static void McuPPGInit(void);
static void McuUartInit(void);
// static void McuADCInit(void);
static void McuRamInit(void);

void FunMcuInit(void);

#define BaudRate 9600
#define USART USART1
#define USART_RCC RCC_APB2Periph_USART1
#define USART_GPIO_RCC RCC_AHBPeriph_GPIOA
#define USART_TX_GPIO_PinSource GPIO_PinSource9
#define USART_RX_GPIO_PinSource GPIO_PinSource10
#define USART_TX GPIO_Pin_9	 // out
#define USART_RX GPIO_Pin_10 // in
#define USART_GPIO_PORT GPIOA

////////////////20211222
void pro_wifiuart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //定义串口初始化结构体
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd(USART_GPIO_RCC, ENABLE);		   //使能GPIOA的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //使能USART的时钟

	GPIO_PinAFConfig(USART_GPIO_PORT, GPIO_PinSource2, GPIO_AF_1); //配置PA9成第二功能引脚	TX
	GPIO_PinAFConfig(USART_GPIO_PORT, GPIO_PinSource3, GPIO_AF_1); //配置PA10成第二功能引脚  RX

	/*USART2_TX ->PA2     USART2_RX ->PA3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //选中串口默认输出管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		   //定义输出最大速率
														   // 20220105+
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //配置为上拉，不接模块也是上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //定义管脚9的模式
	GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);  //调用函数，把结构体参数输入进行初始化
	/*串口通讯参数设置*/
	USART_InitStructure.USART_BaudRate = 9600;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//校验位 无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//使能接收和发送引脚

	USART_Init(USART2, &USART_InitStructure);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);

	/* USART2的NVIC中断配置 */
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
////////////////20211222

/*---------------20220507+485重新编写了
void pro_485uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //定义串口初始化结构体
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd(USART_GPIO_RCC, ENABLE); //使能GPIOA的时钟
	RCC_APB2PeriphClockCmd(USART_RCC, ENABLE);	   //使能USART的时钟

	GPIO_PinAFConfig(USART_GPIO_PORT, USART_TX_GPIO_PinSource, GPIO_AF_1); //配置PA9成第二功能引脚	TX
	GPIO_PinAFConfig(USART_GPIO_PORT, USART_RX_GPIO_PinSource, GPIO_AF_1); //配置PA10成第二功能引脚  RX

	//USART1_TX ->PA9  USART1_RX ->PA10
	GPIO_InitStructure.GPIO_Pin = USART_TX | USART_RX; //选中串口默认输出管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	   //定义管脚9的模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //定义输出最大速率
	GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);  //调用函数，把结构体参数输入进行初始化

	//串口通讯参数设置
	USART_InitStructure.USART_BaudRate = 9600;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//校验位 无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//使能接收和发送引脚

	USART_Init(USART, &USART_InitStructure);

	USART_ClearFlag(USART, USART_FLAG_TC);
	USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART, ENABLE);

	//USART1的NVIC中断配置
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 2; //响应优先级为2
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		  //选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //设置引脚为普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //设置引脚为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //设置引脚为上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置引脚速度为50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  //调用库函数，初始化GPIO

	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}
*/

static void McuGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//--引脚时钟使能
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF, ENABLE); // YXL-5

	//--LED灯驱动
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_1); // 20211022修复

	///////////////按键的输入口有改变
	//////////////////////////20211016
	//--按键输入
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/////////////////////////////
	//--按键输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/////////////////////20211016

	//--蜂鸣器

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_0);

	//--时钟芯片
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PAout(8, 1);
	PAout(9, 1);
	// PAout(GPIO_Pin_8|GPIO_Pin_9,1);

	//--E2PROM芯片
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PAout(5, 1);
	PBout(2, 1);
	//--拨码开关//yxl-5

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  //选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //设置引脚为普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //设置引脚为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //设置引脚为上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置引脚速度为50MHZ
	GPIO_Init(GPIOF, &GPIO_InitStructure);			  //调用库函数，初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		  //选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //设置引脚为普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //设置引脚为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //设置引脚为上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置引脚速度为50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  //调用库函数，初始化GPIO

	GPIO_SetBits(GPIOF, GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);

	///////数值输入的端口改在165芯片上
	///////////////20211015
	////--外部控制-数字量
	//	GPIO_StructInit(&GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//--外部控制-模拟量 //yxl-5

	//	GPIO_StructInit(&GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// GPIO_SetBits(GPIOB,GPIO_Pin_10);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_12);

	//--外部控制-RS485

	//--压力传感器

	//--WIFI
}

static void McuTimeInit(void)
{
}

// PWM
static void McuPPGInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);

	//--LED
	////////////////////////////////20211125
	//没有用系统自带的pwm，而是再IC74HC595中自行模拟，脉冲宽度为1/5周期
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 20211124
	TIM_OC2Init(TIM15, &TIM_OCInitStructure);				 // 20211022修复

	TIM_SetCompare2(TIM15, 0);
	TIM_OC2PreloadConfig(TIM15, TIM_OCPreload_Enable); // 20211122

	TIM_CtrlPWMOutputs(TIM15, ENABLE);
	TIM_Cmd(TIM15, ENABLE);
	TIM15->BDTR |= 1 << 15;
	////////////////////////////////20211125
	//--蜂鸣器
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);

	TIM_SetCompare1(TIM14, 0);
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM14, ENABLE);
	TIM_Cmd(TIM14, ENABLE);
}

//串口
static void McuUartInit(void)
{

	//--主板通讯
	suart1_init();

	//--外部控制-RS485
	// USART_InitTypeDef USART_InitStructure;
	// NVIC_InitTypeDef NVIC_InitStruct;

	Uart485_init(); // 485通信初始化
	// pro_485uart_init(); // YXL-5
}

// ADC
// static void McuADCInit(void)
//{

//}

// RAM
static void McuRamInit(void)
{
	uchar i;




	gs16RunStartTm[0] = 7 * 60;
	gs16RunEndTm[0] = 21 * 60;

	/// u8ModeTime_SpeedSet[0] = 4;//yyxxll

	if (fg_time_spd_contorl == 0)
		u8ModeTime_SpeedSet[0] = 10; // yyxxll  默认值70//u8ModeTime_SpeedSet[0] = 6; //yyxxll  默认值70  yxl-5
	else
		u8ModeTime_GateSet_Tem[0] = 20;



	for (i = 1; i < 4; i++)
	{
		gs16RunStartTm[i] = 0;
		gs16RunEndTm[i] = 0;


		if (fg_time_spd_contorl == 0)
			u8ModeTime_SpeedSet[i] = 10; // yyxxll  默认值70//u8ModeTime_SpeedSet[0] = 6; //yyxxll  默认值70  yxl-5

		else
			u8ModeTime_GateSet_Tem[i] = 20;


	}

	for (i = 0; i < 4; i++)
	{
		SetStartTm[i] = gs16RunStartTm[i];
		SetEndTm[i] = gs16RunEndTm[i];
		u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i];

	}

	gu8InCtrlFlg = 0;
	u8DinLFTCnt[0] = 0;
	u8DinLFTCnt[1] = 0;
	u8DinLFTCnt[2] = 0;
	u8DinLFTCnt[3] = 0;
	gu8SetMode = 0;





	gu16RomMinSpd = u16LadderSpd[0]; //	gu16RomMinSpd = 1200;  //yxl-5

	u8SD_ParamSet_OCtrlSpd1 = 40; //数字控制DIN1的速度
	u8SD_ParamSet_OCtrlSpd2 = 36; //数字控制DIN2的速度
	u8SD_ParamSet_OCtrlSpd3 = 28; //数字控制DIN3的速度

//	gu8RomSelfSpd = 29;

	WashTmSet = 180; //初始冲洗设置时间
	WashTmSet_Normal = 180; //初始反冲洗设置时间


	u8FirstFunIn = 0;//恢复出厂设置后的自学习，会有降速判断

	u8SD_ParamSet_HandWashSpd = 4; // yxl-5

	u8ModeSpeed_SpeedSet = 36; // OEM的流量直接为80%流量值

	u8ModeTimeRunGate = 20; // yxl-5

	fg_time_spd_contorl = 0; // yxl-5

	WaterGate_set = 20;

	water_max_set = 25;
	water_min_set = 5;

	u8fg_SD_ParamSet_AnalogSel = 0;//0为电流，模拟量控制电流电压切换标志位

	fg_hand_close = 0;

	water_uint_switch = 0; // 20211222

//	for (i = 0; i < 45; i++)
//	{

//		WaterPress_value_con[i] = 0;
//		fg_waterPress_value_con[i] = 0;
//	}
}

void FunMcuInit(void)
{
	uchar i;
	//
	//		GPIO_InitTypeDef GPIO_InitStructure;

	////--引脚时钟使能
	//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF | RCC_AHBPeriph_GPIOC, ENABLE);//YXL-5
	//

	McuGPIOInit();
	McuTimeInit();
	McuPPGInit();
	McuUartInit();
	// InitIrqLevels();
	//	EI();
	for (i = 0; i < 100; i++)
		__NOP(); // 20211222

	McuRamInit();
	McuAdcInit();

	///////////以下是函数是LED1621的初始化
	///////////20211019
	McuLCDInit();
	// LCD1621_GpioInit();
	// LCD1621_TIM3_Init();
	////////////////20211019


	// WDTC=0x35;			//Clear watch dog timer
	gu16KeyPressedTm = 0;
	gu8PowerOnTm = 0;
	gu8LcdInitTm = 15; // yxl-5  显示机型码
	gu8BuzzDlyTm = SHORTBE;
	gu8RTCWaitTm = 7;

	for (i = 0; i < 100; i++)
		__NOP(); // 20211222

	pro_wifiuart_init();  // 20211222
	wifi_protocol_init(); // yxl-wifi
}
