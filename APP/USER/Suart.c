#include "Suart.h"

struct
{
	u8 li : 1;		//监听使能标志位
	u8 rx : 1;		//接收使能标志位
	u8 rxstart : 1; //接收开始标志位
	u8 tx : 1;		//发送使能标志位
	u8 txstart : 1; //发送开始标志位
	u8 rxfinish;	//接收完成标志位
	u8 len;			//发送接收数据长度
} suart1_state = {0};
#define SUART1_BUF_SIZE 32
u8 suart1_buf[SUART1_BUF_SIZE] = {0};
uint8_t (*suart1_rx_process)(uint8_t data) = 0;

void suart1_init(void)
{
	//--引脚时钟初始化
	// RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); // yxl-5

	//--引脚IO配置初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	//发送脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // yxl-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; // yxl-5  外接5V   开漏输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_SetBits(GPIOB, GPIO_Pin_10);//20220523+串口默认是高电平
	GPIO_Init(GPIOB, &GPIO_InitStructure); // YXL-5
	
	//接收脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // YXL-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // YXL-5


	//--定时器时钟初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	//--定时器初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 208 - 1;	  //重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //定时中断使能
	// TIM_Cmd(TIM6, ENABLE);

	//--SYSCFG时钟使能并映射配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11); // YXL-5

	//--外部中断初始化
	EXTI_InitTypeDef EXTI_InitStructure;
	//  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;				// YXL-5
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿检测

	// EXTI_InitStructure.EXTI_LineCmd = DISABLE;//YXL-5
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; // YXL-5

	EXTI_Init(&EXTI_InitStructure);

	//--NVIC中断优先级设置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn; // TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;		//优先级1级
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn; //外部中断0_1中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;		//优先级1级
	NVIC_Init(&NVIC_InitStructure);

	suart1_listen(1);
}

// void test_exti(){
//  EXTI_InitTypeDef   EXTI_InitStructure;
//  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //中断模式
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿检测
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//}

void suart1_rx_onoff(u8 mode)
{
	if (mode)
	{
		suart1_state.rx = 1;			//接收打开
		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //定时器中断关闭
										//	EXTI->PR |= EXTI_Line14;	//事件标志清除
										//	EXTI->IMR|=EXTI_Line14;	//外部中断打开
		EXTI->PR |= EXTI_Line11;		//事件标志清除//YXL-5
		EXTI->IMR |= EXTI_Line11;		//外部中断打开

		//		test_exti();
		suart1_state.rxstart = 1; // rx相关变量设置
	}
	else
	{
		suart1_state.rx = 0;	   //接收关闭
								   //	EXTI->IMR&=~EXTI_Line14;	//外部中断关闭
		EXTI->IMR &= ~EXTI_Line11; //外部中断关闭//yxl-5

		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //定时器中断关闭
	}
}

void suart1_SetRxProcess(uint8_t (*fun)(uint8_t))
{
	suart1_rx_process = fun;
}

//定时接收数据处理
void suart1_rx(void)
{
	static u8 bitcnt; //需要接收的bit位标,包括开始位和停止位等
	static u8 rxbyte; //接收数据缓存变量

	if (suart1_state.rxstart)
	{ //第一次接收，初始化静态变量
		suart1_state.rxstart = 0;
		bitcnt = 9;
		suart1_state.len = 0;
	}

	switch (bitcnt)
	{
	case 9: //开始位
		if (SUART1_RX_IO())
		{
			//开始位电平不正确
			TIM6->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器

			//	EXTI->PR |= EXTI_Line14;	//事件标志清除
			//	EXTI->IMR|=EXTI_Line14;	//外部中断打开

			EXTI->PR |= EXTI_Line11;  //事件标志清除//yxl-5
			EXTI->IMR |= EXTI_Line11; //外部中断打开

			//			test_exti();
		}
		else
		{
			// T01CR1_STA=0;	//关闭定时器
			TIM6->ARR = 417; //定时0.417ms一次
			// T01CR1_STA=1;	//打开定时器
			// EXTI->IMR&=~EXTI_Line14;	//外部中断关闭

			bitcnt = 0;
			rxbyte = 0;
		}
		break;
	default: //数据位
		if (SUART1_RX_IO())
			rxbyte |= 1 << bitcnt;
		bitcnt++;
		break;
	case 8: //停止位
		if (SUART1_RX_IO())
		{
			if (suart1_rx_process == 0)
				break;

			//接收处理函数
			switch (suart1_rx_process(rxbyte))
			{
			case 0:								//继续接收下一个字节数据
				TIM6->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器

				//	EXTI->PR |=EXTI_Line14;	//事件标志清除
				//	EXTI->IMR|=EXTI_Line14;	//外部中断打开

				EXTI->PR |= EXTI_Line11;  //事件标志清除//yxl-5
				EXTI->IMR |= EXTI_Line11; //外部中断打开

				//					test_exti();
				bitcnt++;
				break;
			case 1: //一帧数据接收完成
				suart1_rx_onoff(0);
				break;
			}
		}
		else
			bitcnt++; //超时处理,直接丢弃该帧
		break;
	}
}
//定时发送数据处理
void suart1_tx(void)
{
	static u8 ibyte;  //需要发送的字节位标
	static u8 bitcnt; //需要发送的bit位标,包括开始位和停止位等
	static u8 txbyte; //发送数据缓存变量

	if (suart1_state.txstart)
	{ //第一次发送，初始化静态变量
		suart1_state.txstart = 0;
		ibyte = 0;
		bitcnt = 10;

		suart1_state.tx = 1;			//发送使能
		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
		TIM6->ARR = 417;				//定时2ms一次
		TIM6->CR1 |= TIM_CR1_CEN;		//打开定时器
	}

	switch (bitcnt)
	{
	case 10:			 //开始位
		SUART1_TX_IO(0); //开始位
		bitcnt = 0;
		txbyte = suart1_buf[ibyte];
		break;
	default: //数据位
		SUART1_TX_IO((txbyte)&0x01);
		txbyte >>= 1;
		bitcnt++;
		break;
	case 8:				 //停止位1
		SUART1_TX_IO(1); //停止位
		bitcnt++;
		break;
	case 9: //停止位2
		SUART1_TX_IO(1);
		ibyte++;
		if (ibyte >= suart1_state.len)
		{									//已发送完成
			suart1_state.tx = 0;			//关闭发送
			TIM6->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
			if (suart1_state.li)
				suart1_rx_onoff(1);
			break;
		}
		bitcnt++;
		break;
	}
}

//发送数据
void suart1_send(const u8 *data, u8 len)
{
	u8 i;
	if (len > SUART1_BUF_SIZE) //数据长度超出缓冲区大小
		return;
	if (suart1_state.tx) //已经在发送状态
		return;
	if (suart1_state.rx)	//已经在接收状态
		suart1_rx_onoff(0); //关闭接收

	suart1_state.rxfinish = 0; //接收结束
	suart1_state.len = len;
	for (i = 0; i < len; i++)
		suart1_buf[i] = data[i];

	suart1_state.txstart = 1;
	suart1_tx();
}

//接收数据
u8 suart1_recv(u8 data[], u8 len)
{
	u8 i;
	u8 ret = 0;
	if (len < suart1_state.len) //接收到的数据长度比目标数据容量大
		return 0;

	if (suart1_state.rxfinish)
	{ //接收到完整数据
		for (i = 0; i < suart1_state.len; i++)
			data[i] = suart1_buf[i];
		ret = suart1_state.rxfinish;
		suart1_state.rxfinish = 0;

		suart1_rx_onoff(1); //继续接收
		return ret;
	}

	return ret;
}

//接收数据长度
u8 suart1_recv_len(void)
{
	if (suart1_state.rxfinish)
		return suart1_state.len;
	else
		return 0;
}

uint8_t suart1_TxState(void)
{
	return suart1_state.tx;
}

//串口监听设置
void suart1_listen(u8 en)
{
	suart1_state.li = en ? 1 : 0; ////监听使能标志位设置
	if (suart1_state.tx)		  //发送状态中
		return;
	suart1_rx_onoff(suart1_state.li); //接收开关设置
}


//外部中断，下边沿捕获
void EXTI4_15_IRQHandler(void)
{
	//与驱动板的串口通信
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{ // yxl-5
		EXTI->IMR &= ~EXTI_Line11; //关闭外部中断//yxl-5
		EXTI->PR |= EXTI_Line11;   //事件标志清除//yxl-5

		TIM6->ARR = 208;		  //定时0.208ms
		TIM6->CR1 |= TIM_CR1_CEN; //打开定时器
	}
	//485通信
	else if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{						
		EXTI->IMR &= ~EXTI_Line10; //关闭外部中断//yxl-5
		EXTI->PR |= EXTI_Line10;   //事件标志清除//yxl-5

		TIM2->ARR = 52;		  //约9600波特率的2倍，为读取数据时卡在电平中间
		TIM2->CR1 |= TIM_CR1_CEN; //打开定时器
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{ //判断是否相应的事件
		// TIM6->SR=(u16)~TIM_IT_CC1;	//事件标志清除
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if (suart1_state.rx) //读模式状态
			suart1_rx();
		else if (suart1_state.tx)
		{ //写模式状态
			suart1_tx();
		}
	}
}
