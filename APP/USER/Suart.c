#include "Suart.h"

struct
{
	u8 li : 1;		//����ʹ�ܱ�־λ
	u8 rx : 1;		//����ʹ�ܱ�־λ
	u8 rxstart : 1; //���տ�ʼ��־λ
	u8 tx : 1;		//����ʹ�ܱ�־λ
	u8 txstart : 1; //���Ϳ�ʼ��־λ
	u8 rxfinish;	//������ɱ�־λ
	u8 len;			//���ͽ������ݳ���
} suart1_state = {0};
#define SUART1_BUF_SIZE 32
u8 suart1_buf[SUART1_BUF_SIZE] = {0};
uint8_t (*suart1_rx_process)(uint8_t data) = 0;

void suart1_init(void)
{
	//--����ʱ�ӳ�ʼ��
	// RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); // yxl-5

	//--����IO���ó�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	//���ͽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // yxl-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; // yxl-5  ���5V   ��©���
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_SetBits(GPIOB, GPIO_Pin_10);//20220523+����Ĭ���Ǹߵ�ƽ
	GPIO_Init(GPIOB, &GPIO_InitStructure); // YXL-5
	
	//���ս�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // YXL-5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // YXL-5


	//--��ʱ��ʱ�ӳ�ʼ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	//--��ʱ����ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 208 - 1;	  //����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //��ʱ�ж�ʹ��
	// TIM_Cmd(TIM6, ENABLE);

	//--SYSCFGʱ��ʹ�ܲ�ӳ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11); // YXL-5

	//--�ⲿ�жϳ�ʼ��
	EXTI_InitTypeDef EXTI_InitStructure;
	//  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;				// YXL-5
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ؼ��

	// EXTI_InitStructure.EXTI_LineCmd = DISABLE;//YXL-5
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; // YXL-5

	EXTI_Init(&EXTI_InitStructure);

	//--NVIC�ж����ȼ�����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn; // TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;		//���ȼ�1��
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn; //�ⲿ�ж�0_1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;		//���ȼ�1��
	NVIC_Init(&NVIC_InitStructure);

	suart1_listen(1);
}

// void test_exti(){
//  EXTI_InitTypeDef   EXTI_InitStructure;
//  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //�ж�ģʽ
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ؼ��
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//}

void suart1_rx_onoff(u8 mode)
{
	if (mode)
	{
		suart1_state.rx = 1;			//���մ�
		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //��ʱ���жϹر�
										//	EXTI->PR |= EXTI_Line14;	//�¼���־���
										//	EXTI->IMR|=EXTI_Line14;	//�ⲿ�жϴ�
		EXTI->PR |= EXTI_Line11;		//�¼���־���//YXL-5
		EXTI->IMR |= EXTI_Line11;		//�ⲿ�жϴ�

		//		test_exti();
		suart1_state.rxstart = 1; // rx��ر�������
	}
	else
	{
		suart1_state.rx = 0;	   //���չر�
								   //	EXTI->IMR&=~EXTI_Line14;	//�ⲿ�жϹر�
		EXTI->IMR &= ~EXTI_Line11; //�ⲿ�жϹر�//yxl-5

		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //��ʱ���жϹر�
	}
}

void suart1_SetRxProcess(uint8_t (*fun)(uint8_t))
{
	suart1_rx_process = fun;
}

//��ʱ�������ݴ���
void suart1_rx(void)
{
	static u8 bitcnt; //��Ҫ���յ�bitλ��,������ʼλ��ֹͣλ��
	static u8 rxbyte; //�������ݻ������

	if (suart1_state.rxstart)
	{ //��һ�ν��գ���ʼ����̬����
		suart1_state.rxstart = 0;
		bitcnt = 9;
		suart1_state.len = 0;
	}

	switch (bitcnt)
	{
	case 9: //��ʼλ
		if (SUART1_RX_IO())
		{
			//��ʼλ��ƽ����ȷ
			TIM6->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��

			//	EXTI->PR |= EXTI_Line14;	//�¼���־���
			//	EXTI->IMR|=EXTI_Line14;	//�ⲿ�жϴ�

			EXTI->PR |= EXTI_Line11;  //�¼���־���//yxl-5
			EXTI->IMR |= EXTI_Line11; //�ⲿ�жϴ�

			//			test_exti();
		}
		else
		{
			// T01CR1_STA=0;	//�رն�ʱ��
			TIM6->ARR = 417; //��ʱ0.417msһ��
			// T01CR1_STA=1;	//�򿪶�ʱ��
			// EXTI->IMR&=~EXTI_Line14;	//�ⲿ�жϹر�

			bitcnt = 0;
			rxbyte = 0;
		}
		break;
	default: //����λ
		if (SUART1_RX_IO())
			rxbyte |= 1 << bitcnt;
		bitcnt++;
		break;
	case 8: //ֹͣλ
		if (SUART1_RX_IO())
		{
			if (suart1_rx_process == 0)
				break;

			//���մ�����
			switch (suart1_rx_process(rxbyte))
			{
			case 0:								//����������һ���ֽ�����
				TIM6->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��

				//	EXTI->PR |=EXTI_Line14;	//�¼���־���
				//	EXTI->IMR|=EXTI_Line14;	//�ⲿ�жϴ�

				EXTI->PR |= EXTI_Line11;  //�¼���־���//yxl-5
				EXTI->IMR |= EXTI_Line11; //�ⲿ�жϴ�

				//					test_exti();
				bitcnt++;
				break;
			case 1: //һ֡���ݽ������
				suart1_rx_onoff(0);
				break;
			}
		}
		else
			bitcnt++; //��ʱ����,ֱ�Ӷ�����֡
		break;
	}
}
//��ʱ�������ݴ���
void suart1_tx(void)
{
	static u8 ibyte;  //��Ҫ���͵��ֽ�λ��
	static u8 bitcnt; //��Ҫ���͵�bitλ��,������ʼλ��ֹͣλ��
	static u8 txbyte; //�������ݻ������

	if (suart1_state.txstart)
	{ //��һ�η��ͣ���ʼ����̬����
		suart1_state.txstart = 0;
		ibyte = 0;
		bitcnt = 10;

		suart1_state.tx = 1;			//����ʹ��
		TIM6->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
		TIM6->ARR = 417;				//��ʱ2msһ��
		TIM6->CR1 |= TIM_CR1_CEN;		//�򿪶�ʱ��
	}

	switch (bitcnt)
	{
	case 10:			 //��ʼλ
		SUART1_TX_IO(0); //��ʼλ
		bitcnt = 0;
		txbyte = suart1_buf[ibyte];
		break;
	default: //����λ
		SUART1_TX_IO((txbyte)&0x01);
		txbyte >>= 1;
		bitcnt++;
		break;
	case 8:				 //ֹͣλ1
		SUART1_TX_IO(1); //ֹͣλ
		bitcnt++;
		break;
	case 9: //ֹͣλ2
		SUART1_TX_IO(1);
		ibyte++;
		if (ibyte >= suart1_state.len)
		{									//�ѷ������
			suart1_state.tx = 0;			//�رշ���
			TIM6->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
			if (suart1_state.li)
				suart1_rx_onoff(1);
			break;
		}
		bitcnt++;
		break;
	}
}

//��������
void suart1_send(const u8 *data, u8 len)
{
	u8 i;
	if (len > SUART1_BUF_SIZE) //���ݳ��ȳ�����������С
		return;
	if (suart1_state.tx) //�Ѿ��ڷ���״̬
		return;
	if (suart1_state.rx)	//�Ѿ��ڽ���״̬
		suart1_rx_onoff(0); //�رս���

	suart1_state.rxfinish = 0; //���ս���
	suart1_state.len = len;
	for (i = 0; i < len; i++)
		suart1_buf[i] = data[i];

	suart1_state.txstart = 1;
	suart1_tx();
}

//��������
u8 suart1_recv(u8 data[], u8 len)
{
	u8 i;
	u8 ret = 0;
	if (len < suart1_state.len) //���յ������ݳ��ȱ�Ŀ������������
		return 0;

	if (suart1_state.rxfinish)
	{ //���յ���������
		for (i = 0; i < suart1_state.len; i++)
			data[i] = suart1_buf[i];
		ret = suart1_state.rxfinish;
		suart1_state.rxfinish = 0;

		suart1_rx_onoff(1); //��������
		return ret;
	}

	return ret;
}

//�������ݳ���
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

//���ڼ�������
void suart1_listen(u8 en)
{
	suart1_state.li = en ? 1 : 0; ////����ʹ�ܱ�־λ����
	if (suart1_state.tx)		  //����״̬��
		return;
	suart1_rx_onoff(suart1_state.li); //���տ�������
}


//�ⲿ�жϣ��±��ز���
void EXTI4_15_IRQHandler(void)
{
	//��������Ĵ���ͨ��
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{ // yxl-5
		EXTI->IMR &= ~EXTI_Line11; //�ر��ⲿ�ж�//yxl-5
		EXTI->PR |= EXTI_Line11;   //�¼���־���//yxl-5

		TIM6->ARR = 208;		  //��ʱ0.208ms
		TIM6->CR1 |= TIM_CR1_CEN; //�򿪶�ʱ��
	}
	//485ͨ��
	else if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{						
		EXTI->IMR &= ~EXTI_Line10; //�ر��ⲿ�ж�//yxl-5
		EXTI->PR |= EXTI_Line10;   //�¼���־���//yxl-5

		TIM2->ARR = 52;		  //Լ9600�����ʵ�2����Ϊ��ȡ����ʱ���ڵ�ƽ�м�
		TIM2->CR1 |= TIM_CR1_CEN; //�򿪶�ʱ��
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{ //�ж��Ƿ���Ӧ���¼�
		// TIM6->SR=(u16)~TIM_IT_CC1;	//�¼���־���
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if (suart1_state.rx) //��ģʽ״̬
			suart1_rx();
		else if (suart1_state.tx)
		{ //дģʽ״̬
			suart1_tx();
		}
	}
}
