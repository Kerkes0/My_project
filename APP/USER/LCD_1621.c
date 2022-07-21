#include "LCD_1621.h"
//#include "string.h"

#define BPWM_OK 0 //背光是否是单口pwm控制，0为不是

//关于液晶屏芯片的一些设置
#define Normal 0xe3
#define RC256K 0x18
#define SysEn 0x01
#define Bais3_4 0x29
#define LcdOn 0x03
#define DisInt 0x80

//时钟线
#define GPIO_Tm1621Wr_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Wr_PORT GPIOB
#define GPIO_Tm1621Wr_PIN GPIO_Pin_7
#define set_1621clk() GPIO_WriteBit(GPIO_Tm1621Wr_PORT, GPIO_Tm1621Wr_PIN, Bit_SET);
#define clr_1621clk() GPIO_WriteBit(GPIO_Tm1621Wr_PORT, GPIO_Tm1621Wr_PIN, Bit_RESET);
//数据线
#define GPIO_Tm1621Data_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Data_PORT GPIOB
#define GPIO_Tm1621Data_PIN GPIO_Pin_5
#define set_1621data() GPIO_WriteBit(GPIO_Tm1621Data_PORT, GPIO_Tm1621Data_PIN, Bit_SET);
#define clr_1621data() GPIO_WriteBit(GPIO_Tm1621Data_PORT, GPIO_Tm1621Data_PIN, Bit_RESET);
//开关总线
#define GPIO_Tm1621Cs_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Cs_PORT GPIOB
#define GPIO_Tm1621Cs_PIN GPIO_Pin_6
#define set_1621cs() GPIO_WriteBit(GPIO_Tm1621Cs_PORT, GPIO_Tm1621Cs_PIN, Bit_SET);
#define clr_1621cs() GPIO_WriteBit(GPIO_Tm1621Cs_PORT, GPIO_Tm1621Cs_PIN, Bit_RESET);

// lcd的引脚初始化设置
void LCD1621_GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//--引脚时钟使能
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOF , ENABLE);

	//芯片引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //定义输出最大速率
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	 //定义管脚的模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Tm1621Data_PIN | GPIO_Tm1621Wr_PIN | GPIO_Tm1621Cs_PIN; //选中串口默认输出管脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#if BPWM_OK
// lcd：定时器tim3和其pwm的设置
void TIM3_ConfigInit(void)
{
	//背光引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // io口复用
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1); // io口复用为AF1
	//定时器设置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //开启tim3时钟

	TIM_TimeBaseInitStruct.TIM_Period = 250 - 1;				 //自动重载计数周期值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 48 - 1;				 //预分屏系数1us
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;				 // TIM_CKD_DIV1;//时钟分频系数0
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //设置为向上计数方式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  // TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性:TIM输出比较极性
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_SetCompare1(TIM3, 250);						  //背光灯亮度调节
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); // CH1预装载使能
	TIM_CtrlPWMOutputs(TIM3, ENABLE);				  //使能pwm
													  // TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_Cmd(TIM3, ENABLE);							  //使能TIM3定时器
}
#else
void TIM3_ConfigInit(void)
{
	//背光引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//--定时器时钟初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//--定时器初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //定时中断使能
	//--NVIC中断优先级设置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //优先级1级
	NVIC_Init(&NVIC_InitStructure);

	TIM3->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
	TIM3->ARR = 250;
	TIM3->CR1 |= TIM_CR1_CEN; //打开定时器
}
u16 TIM3_PWMCompare = 50; //模拟pwm的脉宽
void TIM3_IRQHandler(void)
{
	static u8 timselect = 1;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{ //判断是否相应的事件
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if (timselect) //低电平输出
		{
			timselect = 0;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
			TIM3->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
			TIM3->ARR = 250 - TIM3_PWMCompare;
			TIM3->CR1 |= TIM_CR1_CEN; //打开定时器
		}
		else
		{
			timselect = 1;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
			TIM3->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
			TIM3->ARR = TIM3_PWMCompare;	//脉宽
			TIM3->CR1 |= TIM_CR1_CEN;		//打开定时器
		}
	}
}
void TIM3_SetPWM(u16 Compare)
{
	if (Compare == 0)
	{
		TIM3->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
	}
	else if (Compare >= 250)
	{
		TIM3->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
	}
	else
	{
		TIM3_PWMCompare = Compare;
		TIM3->CR1 |= TIM_CR1_CEN;		//打开定时器
	}
}

#endif

/******************************************
将Data写进（Write）给1621
******************************************/
void DispTranBit_ForCam(uint8_t Data, uint8_t Length)
{
	while (Length)
	{
		if (Data & 0x80)
		{
			set_1621data();
		}
		else
		{
			clr_1621data();
		}
		clr_1621clk();
		Data <<= 1;
		delay_us(3); // 20210207
		set_1621clk();
		delay_us(3); // 20210207
		Length--;
	}
}
// lcd的芯片初始化设置
void LCD1621_SelfSetInit(void)
{

	clr_1621cs();
	DispTranBit_ForCam(0x80, 3);
	DispTranBit_ForCam(Normal, 9);
	DispTranBit_ForCam(RC256K, 9);
	DispTranBit_ForCam(SysEn, 9);
	DispTranBit_ForCam(Bais3_4, 9);
	DispTranBit_ForCam(LcdOn, 9);
	DispTranBit_ForCam(DisInt, 9);
	set_1621cs();
}

///////////////////////////////
//////////////将数据中按键灯点亮转换为lcd液晶点亮的格式
//////////////////20211015/20211108

/////////原：GFED CBA_
///第0种-转：_CGB DEFA
///第1种-转：BGC_ AFED
u8 DCR_Data_func(u8 data, u8 sel)
{
	u8 changedata = 0x00;
	if (sel == 0)
	{
		changedata |= ((data << 5) & (1 << 7)); // x000 0000
		changedata |= ((data >> 1) & (1 << 6)); // 0x00 0000
		changedata |= ((data << 2) & (1 << 5)); // 00x0 0000
		changedata |= ((data << 4) & (1 << 4)); // 000x 0000

		changedata |= ((data << 2) & (1 << 3)); // 0000 x000
		changedata |= ((data >> 4) & (1 << 2)); // 0000 0x00
		changedata |= ((data >> 4) & (1 << 1)); // 0000 00x0
		changedata |= ((data >> 4) & (1 << 0)); // 0000 000x

		return changedata;
	}
	else if (sel == 1)
	{
		changedata |= ((data << 7) & (1 << 7)); // x000 0000
		changedata |= ((data << 3) & (1 << 6)); // 0x00 0000
		changedata |= ((data >> 2) & (1 << 5)); // 00x0 0000
		changedata |= ((data << 2) & (1 << 4)); // 000x 0000

		changedata |= ((data >> 1) & (1 << 3)); // 0000 x000
		changedata |= ((data >> 3) & (1 << 2)); // 0000 0x00
		changedata |= ((data >> 5) & (1 << 1)); // 0000 00x0
		changedata |= ((data >> 1) & (1 << 0)); // 0000 000x
		return changedata;
	}
	return changedata;
}
//////////////////20211015/20211108

void SendDataToHt1621_Sub(uint8_t *Prt)
{
	u8 len = 16;
	//////////数据转换规则：hc595转为led1621
	u8 Prtcen[16];
	u8 *Prtcen2;
	Prtcen[0] = DCR_Data_func((Prt[14] & 0xfe) | (Prt[12] & 0x01), 0); // D122->4
	Prtcen[1] = DCR_Data_func((Prt[13] & 0xfe) | (Prt[11] & 0x01), 0); // D114->3
	Prtcen[2] = DCR_Data_func((Prt[12] & 0xfe) | (Prt[13] & 0x01), 0); // D122->COL2
	Prtcen[3] = DCR_Data_func((Prt[11] & 0xfe) | (Prt[9] & 0x01), 0);  // D122->2
	Prtcen[4] = DCR_Data_func((Prt[10] & 0xfe) | (Prt[10] & 0x01), 0); // D122->S9
	Prtcen[5] = DCR_Data_func((Prt[9] & 0xfe) | (Prt[8] & 0x01), 0);   // D122->1
	Prtcen[6] = DCR_Data_func((Prt[8] & 0xfe) | (Prt[7] & 0x01), 0);   // D122->COL1
	Prtcen[7] = DCR_Data_func(Prt[7] & 0xfe, 0);					   // NULL->NULL
	////上下两部分在TH1621B中存放的数据顺序结构不同，在这里0-7进行第0数据转换，8-16进行第1数据转换
	Prtcen[8] = DCR_Data_func((Prt[3] & 0xfe) | (Prt[1] & 0x01), 1);  // D10->W
	Prtcen[9] = DCR_Data_func((Prt[2] & 0xfe) | (Prt[6] & 0x01), 1);  // D36->S6
	Prtcen[10] = DCR_Data_func((Prt[1] & 0xfe) | (Prt[2] & 0x01), 1); // D19->S4
	Prtcen[11] = DCR_Data_func(Prt[0], 1);							  // D9->S1
	Prtcen[12] = DCR_Data_func((Prt[6] & 0xfe) | (Prt[5] & 0x01), 1); // D51->S2
	Prtcen[13] = DCR_Data_func((Prt[5] & 0xfe) | (Prt[3] & 0x01), 1); // D18->S5
	Prtcen[14] = DCR_Data_func((Prt[4] & 0xfe) | (Prt[4] & 0x01), 1); // D35->S3
	Prtcen[15] = DCR_Data_func(Prt[15] & 0xfe, 1);					  // NULL->NULL

	Prtcen2 = &Prtcen[0];

	clr_1621cs();
	DispTranBit_ForCam(0xaa, 3);
	DispTranBit_ForCam(0x00, 6);
	while (len--)
	{
		DispTranBit_ForCam(*Prtcen2++, 8);
	}
	set_1621cs();
	delay_us(3);
}
//////////////////////20211015/20211108

void McuLCDInit(void)
{
	LCD1621_GpioInit();
	TIM3_ConfigInit();
	LCD1621_SelfSetInit();
}
