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
	USART_InitTypeDef USART_InitStructure; //���崮�ڳ�ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd(USART_GPIO_RCC, ENABLE);		   //ʹ��GPIOA��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //ʹ��USART��ʱ��

	GPIO_PinAFConfig(USART_GPIO_PORT, GPIO_PinSource2, GPIO_AF_1); //����PA9�ɵڶ���������	TX
	GPIO_PinAFConfig(USART_GPIO_PORT, GPIO_PinSource3, GPIO_AF_1); //����PA10�ɵڶ���������  RX

	/*USART2_TX ->PA2     USART2_RX ->PA3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //ѡ�д���Ĭ������ܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		   //��������������
														   // 20220105+
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //����Ϊ����������ģ��Ҳ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����ܽ�9��ģʽ
	GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);  //���ú������ѽṹ�����������г�ʼ��
	/*����ͨѶ��������*/
	USART_InitStructure.USART_BaudRate = 9600;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//У��λ ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//ʹ�ܽ��պͷ�������

	USART_Init(USART2, &USART_InitStructure);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);

	/* USART2��NVIC�ж����� */
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
////////////////20211222

/*---------------20220507+485���±�д��
void pro_485uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //���崮�ڳ�ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd(USART_GPIO_RCC, ENABLE); //ʹ��GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(USART_RCC, ENABLE);	   //ʹ��USART��ʱ��

	GPIO_PinAFConfig(USART_GPIO_PORT, USART_TX_GPIO_PinSource, GPIO_AF_1); //����PA9�ɵڶ���������	TX
	GPIO_PinAFConfig(USART_GPIO_PORT, USART_RX_GPIO_PinSource, GPIO_AF_1); //����PA10�ɵڶ���������  RX

	//USART1_TX ->PA9  USART1_RX ->PA10
	GPIO_InitStructure.GPIO_Pin = USART_TX | USART_RX; //ѡ�д���Ĭ������ܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	   //����ܽ�9��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //��������������
	GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);  //���ú������ѽṹ�����������г�ʼ��

	//����ͨѶ��������
	USART_InitStructure.USART_BaudRate = 9600;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//У��λ ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//ʹ�ܽ��պͷ�������

	USART_Init(USART, &USART_InitStructure);

	USART_ClearFlag(USART, USART_FLAG_TC);
	USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART, ENABLE);

	//USART1��NVIC�ж�����
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 2; //��Ӧ���ȼ�Ϊ2
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		  //ѡ��Ҫ�õ�GPIO����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //��������Ϊ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //��������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //��������Ϊ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //���������ٶ�Ϊ50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  //���ÿ⺯������ʼ��GPIO

	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}
*/

static void McuGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//--����ʱ��ʹ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF, ENABLE); // YXL-5

	//--LED������
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
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_1); // 20211022�޸�

	///////////////������������иı�
	//////////////////////////20211016
	//--��������
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/////////////////////////////
	//--��������
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

	//--������

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_0);

	//--ʱ��оƬ
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

	//--E2PROMоƬ
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
	//--���뿪��//yxl-5

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  //ѡ��Ҫ�õ�GPIO����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //��������Ϊ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //��������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //��������Ϊ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //���������ٶ�Ϊ50MHZ
	GPIO_Init(GPIOF, &GPIO_InitStructure);			  //���ÿ⺯������ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		  //ѡ��Ҫ�õ�GPIO����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //��������Ϊ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //��������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //��������Ϊ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //���������ٶ�Ϊ50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  //���ÿ⺯������ʼ��GPIO

	GPIO_SetBits(GPIOF, GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);

	///////��ֵ����Ķ˿ڸ���165оƬ��
	///////////////20211015
	////--�ⲿ����-������
	//	GPIO_StructInit(&GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//--�ⲿ����-ģ���� //yxl-5

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

	//--�ⲿ����-RS485

	//--ѹ��������

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
	//û����ϵͳ�Դ���pwm��������IC74HC595������ģ�⣬������Ϊ1/5����
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 20211124
	TIM_OC2Init(TIM15, &TIM_OCInitStructure);				 // 20211022�޸�

	TIM_SetCompare2(TIM15, 0);
	TIM_OC2PreloadConfig(TIM15, TIM_OCPreload_Enable); // 20211122

	TIM_CtrlPWMOutputs(TIM15, ENABLE);
	TIM_Cmd(TIM15, ENABLE);
	TIM15->BDTR |= 1 << 15;
	////////////////////////////////20211125
	//--������
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //��Ƶֵ
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

//����
static void McuUartInit(void)
{

	//--����ͨѶ
	suart1_init();

	//--�ⲿ����-RS485
	// USART_InitTypeDef USART_InitStructure;
	// NVIC_InitTypeDef NVIC_InitStruct;

	Uart485_init(); // 485ͨ�ų�ʼ��
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
		u8ModeTime_SpeedSet[0] = 10; // yyxxll  Ĭ��ֵ70//u8ModeTime_SpeedSet[0] = 6; //yyxxll  Ĭ��ֵ70  yxl-5
	else
		u8ModeTime_GateSet_Tem[0] = 20;



	for (i = 1; i < 4; i++)
	{
		gs16RunStartTm[i] = 0;
		gs16RunEndTm[i] = 0;


		if (fg_time_spd_contorl == 0)
			u8ModeTime_SpeedSet[i] = 10; // yyxxll  Ĭ��ֵ70//u8ModeTime_SpeedSet[0] = 6; //yyxxll  Ĭ��ֵ70  yxl-5

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

	u8SD_ParamSet_OCtrlSpd1 = 40; //���ֿ���DIN1���ٶ�
	u8SD_ParamSet_OCtrlSpd2 = 36; //���ֿ���DIN2���ٶ�
	u8SD_ParamSet_OCtrlSpd3 = 28; //���ֿ���DIN3���ٶ�

//	gu8RomSelfSpd = 29;

	WashTmSet = 180; //��ʼ��ϴ����ʱ��
	WashTmSet_Normal = 180; //��ʼ����ϴ����ʱ��


	u8FirstFunIn = 0;//�ָ��������ú����ѧϰ�����н����ж�

	u8SD_ParamSet_HandWashSpd = 4; // yxl-5

	u8ModeSpeed_SpeedSet = 36; // OEM������ֱ��Ϊ80%����ֵ

	u8ModeTimeRunGate = 20; // yxl-5

	fg_time_spd_contorl = 0; // yxl-5

	WaterGate_set = 20;

	water_max_set = 25;
	water_min_set = 5;

	u8fg_SD_ParamSet_AnalogSel = 0;//0Ϊ������ģ�������Ƶ�����ѹ�л���־λ

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

	////--����ʱ��ʹ��
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

	///////////�����Ǻ�����LED1621�ĳ�ʼ��
	///////////20211019
	McuLCDInit();
	// LCD1621_GpioInit();
	// LCD1621_TIM3_Init();
	////////////////20211019


	// WDTC=0x35;			//Clear watch dog timer
	gu16KeyPressedTm = 0;
	gu8PowerOnTm = 0;
	gu8LcdInitTm = 15; // yxl-5  ��ʾ������
	gu8BuzzDlyTm = SHORTBE;
	gu8RTCWaitTm = 7;

	for (i = 0; i < 100; i++)
		__NOP(); // 20211222

	pro_wifiuart_init();  // 20211222
	wifi_protocol_init(); // yxl-wifi
}
