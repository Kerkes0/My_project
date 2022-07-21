#include "Suart.h"
#include "Uart485.h"

#define UART485_RECV_SIZE 30
struct
{
	u8 rx : 1;		//接收使能标志位
	u8 rxstart : 1; //接收开始标志位
	u8 tx : 1;		//发送使能标志位
	u8 txstart : 1; //发送开始标志位
	u8 rxhead;		//接收数据缓存的写入-头
	u8 rxtail;		//接收数据缓存的读取-尾
} uart485_state = {0};

struct
{
	u32 rxstarterror; //接收开始不正确次数
	u32 rxenderror;	  //接收结束不正确次数
	u32 rxtableover;  //接收缓存溢出次数
} uart485_error = {0};//485在发送接收过程中的一下错误记录

u8 uart485_send_buf = 0;//发送缓存
u8 uart485_recv_buf[UART485_RECV_SIZE] = {0};//接收缓存
u8 gu8uart485corrTm = 0; // 485非正常中断关闭时间

/**
 * @brief  485通信初始化
 * @param  Null
 * @return Null
 * @note   485的IO接收发送脚设置、TIM2定时器初始化、接收脚PA1中断初始化、485芯片控制脚PA11初始化
**/
void Uart485_init(void)
{
	//--引脚时钟初始化
	// RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // yxl-5

	//--引脚IO配置初始化
	GPIO_InitTypeDef GPIO_InitStructure;

	// 485发送脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // yxl-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //外接5V 开漏输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // YXL-5

	// 485接收脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // YXL-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // YXL-5

	//--定时器时钟初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//--定时器初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	// TIM_TimeBaseStructure.TIM_Period = 208 - 1;	  //重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //定时中断使能
	// TIM_Cmd(TIM2, ENABLE);

	//--SYSCFG时钟使能并映射配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//设置PA10为外部中断脚
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10); // YXL-5
	//--外部中断初始化
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;				// YXL-5
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿检测
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				// YXL-5
	EXTI_Init(&EXTI_InitStructure);

	//--NVIC中断优先级设置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; // TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //优先级1级
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn; //外部中断0_1中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;		//优先级1级
	NVIC_Init(&NVIC_InitStructure);

	// 485芯片的接收发送切换脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		  //选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //设置引脚为普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //设置引脚为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //设置引脚为上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置引脚速度为50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  //调用库函数，初始化GPIO

	GPIO_ResetBits(GPIOA, GPIO_Pin_11);

	uart485_rx_onoff(1); //接收打开，关闭定时器，打开中断
}


/**
 * @brief  发送数据
 * @param  {data} 要发送的数据 
 * @return Null
 * @note   将要发送的字节塞入发送缓存，并在定时器中进行发送
**/
void Uart485_send(const u8 data)
{
	if (uart485_state.tx) //已经在发送状态
	{
		return;
	}
	if (uart485_state.rx) //已经在接收状态
	{
		uart485_rx_onoff(0); //接收关闭，打开定时器、关闭中断和接收
	}
	
	uart485_send_buf = data;   //将发送数据塞入发送缓存中
	uart485_state.txstart = 1; //开始发送
	uart485_tx();			   //开始发送
}


/**
 * @brief  判断此时是否可以发送数据
 * @param  Null
 * @return 1：可以； 0：不可以
 * @note   当系统已经在发送字节时，此次发送的数据先等待，等发送完后再重新发送
**/
u8 Uart485_sendok(void)
{
	if (uart485_state.tx) //已经在发送状态
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief  读取接收缓存内的数据
 * @param  Null
 * @return 环形接收缓存的尾部数据
 * @note   依次顺序读取环形接收缓存中的数据
**/
u8 Uart485_recv(void)
{
	u8 data;
	if (uart485_state.rxtail != uart485_state.rxhead) //接收数据缓存内容还有时
	{
		data = uart485_recv_buf[uart485_state.rxtail];
		if (++uart485_state.rxtail >= UART485_RECV_SIZE)
		{
			uart485_state.rxtail = 0;
		}
	}
	return data;
}

/**
 * @brief  接收缓存内是否有内容，有则返回1
 * @param  Null
 * @return 0：无内容； 1：有内容
 * @note   当接收缓存有内容时，即需要前往Uart.c文件中485接收数据程序进行处理
**/
u8 Uart485_handler(void)
{
	if (uart485_state.rxtail != uart485_state.rxhead) //接收数据缓存内容还有时
	{
		return 1;
	}
	return 0;
}
/**
 * @brief  485非正常中断关闭时的纠错
 * @param  Null
 * @return Null
 * @note   485非正常中断关闭时的纠错
**/
void Uart485_correction(void)
{
	if (EXTI->IMR & EXTI_Line10) //中断正常时
	{
		gu8uart485corrTm = 0;
	}
	if (gu8uart485corrTm >= 6) // 0.5s*6=3s
	{
		uart485_rx_onoff(1);
	}
}

/**
 * @brief  	是否打开485接收
 * @param	{mode} 接收开关：1为开，0为关
 * @return	Null
 * @note   	该函数主要是在接收、发送结束后调用的
**/
void uart485_rx_onoff(u8 mode)
{
	if (mode) //关闭定时器、打开中断
	{
		uart485_state.rx = 1;			//接收打开
		TIM2->CR1 &= (u16)~TIM_CR1_CEN; //定时器中断关闭
		EXTI->PR |= EXTI_Line10;		//事件标志清除//YXL-5
		EXTI->IMR |= EXTI_Line10;		//外部中断打开
		uart485_state.rxstart = 1;		//第一次接收，初始化静态变量
	}
	else
	{
		uart485_state.rx = 0;	   //接收关闭
		EXTI->IMR &= ~EXTI_Line10; //外部中断关闭
		TIM2->CR1 |= TIM_CR1_CEN;  //打开定时器
	}
}


//----------------------------内部
//定时接收数据处理
void uart485_rx(void)
{
	static u8 bitcnt; //需要接收的bit位标,包括开始位和停止位等
	static u8 rxbyte; //接收数据缓存变量

	if (uart485_state.rxstart)
	{ //第一次接收，初始化静态变量
		uart485_state.rxstart = 0;
		bitcnt = 0;
	}

	switch (bitcnt)
	{
	case 0:					 //开始位
		if (UART485_RX_IO()) //开始位电平不正确
		{
			uart485_rx_onoff(1);		  //接收打开，关闭定时器，打开中断
			uart485_error.rxstarterror++; //接收开始不正确次数
		}
		else //开始位电平正确
		{
			TIM2->ARR = 104; //波特率9600
			bitcnt++;
			rxbyte = 0;
		}
		break;
	default: //数据位
		if (1 <= bitcnt && bitcnt <= 8)
		{
			if (UART485_RX_IO())
				rxbyte |= 1 << (bitcnt - 1);
			bitcnt++;
		}
		break;
	case 9:					 //停止位
		if (UART485_RX_IO()) //停止位正确
		{
			uart485_recv_buf[uart485_state.rxhead] = rxbyte;
			if (++uart485_state.rxhead >= UART485_RECV_SIZE)
			{
				uart485_state.rxhead = 0;
			}
			if (uart485_state.rxhead == uart485_state.rxtail) //缓存的接收数据过多时
			{
				uart485_error.rxtableover++; //结束缓存溢出次数
			}
		}
		else //超时处理,直接丢弃该帧
		{
			uart485_error.rxenderror++; //接收结束不正确次数
			bitcnt++;
		}
		uart485_rx_onoff(1);	   //接收打开，关闭定时器，打开中断
		uart485_state.rxstart = 1; //重新接收
		break;
	}
}

//定时发送数据处理
void uart485_tx(void)
{
	static u8 bitcnt; //需要发送的bit位标,包括开始位和停止位等
	static u8 txbyte; //发送数据缓存变量

	if (uart485_state.txstart)
	{ //第一次发送，初始化静态变量
		uart485_state.txstart = 0;
		bitcnt = 0;
		txbyte = uart485_send_buf;

		uart485_state.tx = 1;			//发送使能
		TIM2->CR1 &= (u16)~TIM_CR1_CEN; //关闭定时器
		TIM2->ARR = 104;				//波特率9600
		TIM2->CR1 |= TIM_CR1_CEN;		//打开定时器
	}

	switch (bitcnt)
	{
	case 0:				  //开始位
		UART485_TX_IO(0); //开始位
		bitcnt++;

		break;
	default: //数据位
		if (1 <= bitcnt && bitcnt <= 8)
		{
			UART485_TX_IO((txbyte)&0x01);
			txbyte >>= 1;
			bitcnt++;
		}
		break;
	case 9:				  //停止位1
		UART485_TX_IO(1); //停止位
		bitcnt++;
		break;
	case 10: //停止位2
		UART485_TX_IO(1);

		//已发送完成
		uart485_state.tx = 0; //关闭发送
		uart485_rx_onoff(1);  //接收打开，关闭定时器，打开中断
		break;
	}
}

//定时器TIM2中断

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ //判断是否相应的事件
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		if (uart485_state.rx) //读模式状态
		{
			uart485_rx();
		}
		else if (uart485_state.tx) //写模式状态
		{
			uart485_tx();
		}
		else //其他情况重新进入接收
		{
			uart485_rx_onoff(1);
		}
	}
}

/*-------------------已经在Suart.c文件中有IO口中断，故需将下面代码移至那边
//外部中断，下边沿捕获
void EXTI4_15_IRQHandler(void)
{
	// if(EXTI_GetITStatus(EXTI_Line14) != RESET){
	if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		EXTI->IMR &= ~EXTI_Line10; //关闭外部中断//yxl-5
		EXTI->PR |= EXTI_Line10;   //事件标志清除//yxl-5

		TIM2->ARR = 104>>1;		  //约9600波特率的2倍，为读取数据时卡在电平中间
		TIM2->CR1 |= TIM_CR1_CEN; //打开定时器
	}
}
*/
