#include "Suart.h"
#include "Uart485.h"

#define UART485_RECV_SIZE 30
struct
{
	u8 rx : 1;		//����ʹ�ܱ�־λ
	u8 rxstart : 1; //���տ�ʼ��־λ
	u8 tx : 1;		//����ʹ�ܱ�־λ
	u8 txstart : 1; //���Ϳ�ʼ��־λ
	u8 rxhead;		//�������ݻ����д��-ͷ
	u8 rxtail;		//�������ݻ���Ķ�ȡ-β
} uart485_state = {0};

struct
{
	u32 rxstarterror; //���տ�ʼ����ȷ����
	u32 rxenderror;	  //���ս�������ȷ����
	u32 rxtableover;  //���ջ����������
} uart485_error = {0};//485�ڷ��ͽ��չ����е�һ�´����¼

u8 uart485_send_buf = 0;//���ͻ���
u8 uart485_recv_buf[UART485_RECV_SIZE] = {0};//���ջ���
u8 gu8uart485corrTm = 0; // 485�������жϹر�ʱ��

/**
 * @brief  485ͨ�ų�ʼ��
 * @param  Null
 * @return Null
 * @note   485��IO���շ��ͽ����á�TIM2��ʱ����ʼ�������ս�PA1�жϳ�ʼ����485оƬ���ƽ�PA11��ʼ��
**/
void Uart485_init(void)
{
	//--����ʱ�ӳ�ʼ��
	// RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // yxl-5

	//--����IO���ó�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;

	// 485���ͽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // yxl-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //���5V ��©���
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // YXL-5

	// 485���ս�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // YXL-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // YXL-5

	//--��ʱ��ʱ�ӳ�ʼ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//--��ʱ����ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	// TIM_TimeBaseStructure.TIM_Period = 208 - 1;	  //����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //��ʱ�ж�ʹ��
	// TIM_Cmd(TIM2, ENABLE);

	//--SYSCFGʱ��ʹ�ܲ�ӳ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//����PA10Ϊ�ⲿ�жϽ�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10); // YXL-5
	//--�ⲿ�жϳ�ʼ��
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;				// YXL-5
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ؼ��
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				// YXL-5
	EXTI_Init(&EXTI_InitStructure);

	//--NVIC�ж����ȼ�����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; // TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //���ȼ�1��
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn; //�ⲿ�ж�0_1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;		//���ȼ�1��
	NVIC_Init(&NVIC_InitStructure);

	// 485оƬ�Ľ��շ����л���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		  //ѡ��Ҫ�õ�GPIO����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  //��������Ϊ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  //��������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  //��������Ϊ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //���������ٶ�Ϊ50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  //���ÿ⺯������ʼ��GPIO

	GPIO_ResetBits(GPIOA, GPIO_Pin_11);

	uart485_rx_onoff(1); //���մ򿪣��رն�ʱ�������ж�
}


/**
 * @brief  ��������
 * @param  {data} Ҫ���͵����� 
 * @return Null
 * @note   ��Ҫ���͵��ֽ����뷢�ͻ��棬���ڶ�ʱ���н��з���
**/
void Uart485_send(const u8 data)
{
	if (uart485_state.tx) //�Ѿ��ڷ���״̬
	{
		return;
	}
	if (uart485_state.rx) //�Ѿ��ڽ���״̬
	{
		uart485_rx_onoff(0); //���չرգ��򿪶�ʱ�����ر��жϺͽ���
	}
	
	uart485_send_buf = data;   //�������������뷢�ͻ�����
	uart485_state.txstart = 1; //��ʼ����
	uart485_tx();			   //��ʼ����
}


/**
 * @brief  �жϴ�ʱ�Ƿ���Է�������
 * @param  Null
 * @return 1�����ԣ� 0��������
 * @note   ��ϵͳ�Ѿ��ڷ����ֽ�ʱ���˴η��͵������ȵȴ����ȷ�����������·���
**/
u8 Uart485_sendok(void)
{
	if (uart485_state.tx) //�Ѿ��ڷ���״̬
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief  ��ȡ���ջ����ڵ�����
 * @param  Null
 * @return ���ν��ջ����β������
 * @note   ����˳���ȡ���ν��ջ����е�����
**/
u8 Uart485_recv(void)
{
	u8 data;
	if (uart485_state.rxtail != uart485_state.rxhead) //�������ݻ������ݻ���ʱ
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
 * @brief  ���ջ������Ƿ������ݣ����򷵻�1
 * @param  Null
 * @return 0�������ݣ� 1��������
 * @note   �����ջ���������ʱ������Ҫǰ��Uart.c�ļ���485�������ݳ�����д���
**/
u8 Uart485_handler(void)
{
	if (uart485_state.rxtail != uart485_state.rxhead) //�������ݻ������ݻ���ʱ
	{
		return 1;
	}
	return 0;
}
/**
 * @brief  485�������жϹر�ʱ�ľ���
 * @param  Null
 * @return Null
 * @note   485�������жϹر�ʱ�ľ���
**/
void Uart485_correction(void)
{
	if (EXTI->IMR & EXTI_Line10) //�ж�����ʱ
	{
		gu8uart485corrTm = 0;
	}
	if (gu8uart485corrTm >= 6) // 0.5s*6=3s
	{
		uart485_rx_onoff(1);
	}
}

/**
 * @brief  	�Ƿ��485����
 * @param	{mode} ���տ��أ�1Ϊ����0Ϊ��
 * @return	Null
 * @note   	�ú�����Ҫ���ڽ��ա����ͽ�������õ�
**/
void uart485_rx_onoff(u8 mode)
{
	if (mode) //�رն�ʱ�������ж�
	{
		uart485_state.rx = 1;			//���մ�
		TIM2->CR1 &= (u16)~TIM_CR1_CEN; //��ʱ���жϹر�
		EXTI->PR |= EXTI_Line10;		//�¼���־���//YXL-5
		EXTI->IMR |= EXTI_Line10;		//�ⲿ�жϴ�
		uart485_state.rxstart = 1;		//��һ�ν��գ���ʼ����̬����
	}
	else
	{
		uart485_state.rx = 0;	   //���չر�
		EXTI->IMR &= ~EXTI_Line10; //�ⲿ�жϹر�
		TIM2->CR1 |= TIM_CR1_CEN;  //�򿪶�ʱ��
	}
}


//----------------------------�ڲ�
//��ʱ�������ݴ���
void uart485_rx(void)
{
	static u8 bitcnt; //��Ҫ���յ�bitλ��,������ʼλ��ֹͣλ��
	static u8 rxbyte; //�������ݻ������

	if (uart485_state.rxstart)
	{ //��һ�ν��գ���ʼ����̬����
		uart485_state.rxstart = 0;
		bitcnt = 0;
	}

	switch (bitcnt)
	{
	case 0:					 //��ʼλ
		if (UART485_RX_IO()) //��ʼλ��ƽ����ȷ
		{
			uart485_rx_onoff(1);		  //���մ򿪣��رն�ʱ�������ж�
			uart485_error.rxstarterror++; //���տ�ʼ����ȷ����
		}
		else //��ʼλ��ƽ��ȷ
		{
			TIM2->ARR = 104; //������9600
			bitcnt++;
			rxbyte = 0;
		}
		break;
	default: //����λ
		if (1 <= bitcnt && bitcnt <= 8)
		{
			if (UART485_RX_IO())
				rxbyte |= 1 << (bitcnt - 1);
			bitcnt++;
		}
		break;
	case 9:					 //ֹͣλ
		if (UART485_RX_IO()) //ֹͣλ��ȷ
		{
			uart485_recv_buf[uart485_state.rxhead] = rxbyte;
			if (++uart485_state.rxhead >= UART485_RECV_SIZE)
			{
				uart485_state.rxhead = 0;
			}
			if (uart485_state.rxhead == uart485_state.rxtail) //����Ľ������ݹ���ʱ
			{
				uart485_error.rxtableover++; //���������������
			}
		}
		else //��ʱ����,ֱ�Ӷ�����֡
		{
			uart485_error.rxenderror++; //���ս�������ȷ����
			bitcnt++;
		}
		uart485_rx_onoff(1);	   //���մ򿪣��رն�ʱ�������ж�
		uart485_state.rxstart = 1; //���½���
		break;
	}
}

//��ʱ�������ݴ���
void uart485_tx(void)
{
	static u8 bitcnt; //��Ҫ���͵�bitλ��,������ʼλ��ֹͣλ��
	static u8 txbyte; //�������ݻ������

	if (uart485_state.txstart)
	{ //��һ�η��ͣ���ʼ����̬����
		uart485_state.txstart = 0;
		bitcnt = 0;
		txbyte = uart485_send_buf;

		uart485_state.tx = 1;			//����ʹ��
		TIM2->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
		TIM2->ARR = 104;				//������9600
		TIM2->CR1 |= TIM_CR1_CEN;		//�򿪶�ʱ��
	}

	switch (bitcnt)
	{
	case 0:				  //��ʼλ
		UART485_TX_IO(0); //��ʼλ
		bitcnt++;

		break;
	default: //����λ
		if (1 <= bitcnt && bitcnt <= 8)
		{
			UART485_TX_IO((txbyte)&0x01);
			txbyte >>= 1;
			bitcnt++;
		}
		break;
	case 9:				  //ֹͣλ1
		UART485_TX_IO(1); //ֹͣλ
		bitcnt++;
		break;
	case 10: //ֹͣλ2
		UART485_TX_IO(1);

		//�ѷ������
		uart485_state.tx = 0; //�رշ���
		uart485_rx_onoff(1);  //���մ򿪣��رն�ʱ�������ж�
		break;
	}
}

//��ʱ��TIM2�ж�

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ //�ж��Ƿ���Ӧ���¼�
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		if (uart485_state.rx) //��ģʽ״̬
		{
			uart485_rx();
		}
		else if (uart485_state.tx) //дģʽ״̬
		{
			uart485_tx();
		}
		else //����������½������
		{
			uart485_rx_onoff(1);
		}
	}
}

/*-------------------�Ѿ���Suart.c�ļ�����IO���жϣ����轫������������Ǳ�
//�ⲿ�жϣ��±��ز���
void EXTI4_15_IRQHandler(void)
{
	// if(EXTI_GetITStatus(EXTI_Line14) != RESET){
	if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		EXTI->IMR &= ~EXTI_Line10; //�ر��ⲿ�ж�//yxl-5
		EXTI->PR |= EXTI_Line10;   //�¼���־���//yxl-5

		TIM2->ARR = 104>>1;		  //Լ9600�����ʵ�2����Ϊ��ȡ����ʱ���ڵ�ƽ�м�
		TIM2->CR1 |= TIM_CR1_CEN; //�򿪶�ʱ��
	}
}
*/
