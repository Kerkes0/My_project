#include "LCD_1621.h"
//#include "string.h"

#define BPWM_OK 0 //�����Ƿ��ǵ���pwm���ƣ�0Ϊ����

//����Һ����оƬ��һЩ����
#define Normal 0xe3
#define RC256K 0x18
#define SysEn 0x01
#define Bais3_4 0x29
#define LcdOn 0x03
#define DisInt 0x80

//ʱ����
#define GPIO_Tm1621Wr_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Wr_PORT GPIOB
#define GPIO_Tm1621Wr_PIN GPIO_Pin_7
#define set_1621clk() GPIO_WriteBit(GPIO_Tm1621Wr_PORT, GPIO_Tm1621Wr_PIN, Bit_SET);
#define clr_1621clk() GPIO_WriteBit(GPIO_Tm1621Wr_PORT, GPIO_Tm1621Wr_PIN, Bit_RESET);
//������
#define GPIO_Tm1621Data_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Data_PORT GPIOB
#define GPIO_Tm1621Data_PIN GPIO_Pin_5
#define set_1621data() GPIO_WriteBit(GPIO_Tm1621Data_PORT, GPIO_Tm1621Data_PIN, Bit_SET);
#define clr_1621data() GPIO_WriteBit(GPIO_Tm1621Data_PORT, GPIO_Tm1621Data_PIN, Bit_RESET);
//��������
#define GPIO_Tm1621Cs_CLK RCC_APB2Periph_GPIOB
#define GPIO_Tm1621Cs_PORT GPIOB
#define GPIO_Tm1621Cs_PIN GPIO_Pin_6
#define set_1621cs() GPIO_WriteBit(GPIO_Tm1621Cs_PORT, GPIO_Tm1621Cs_PIN, Bit_SET);
#define clr_1621cs() GPIO_WriteBit(GPIO_Tm1621Cs_PORT, GPIO_Tm1621Cs_PIN, Bit_RESET);

// lcd�����ų�ʼ������
void LCD1621_GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//--����ʱ��ʹ��
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOF , ENABLE);

	//оƬ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //��������������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	 //����ܽŵ�ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Tm1621Data_PIN | GPIO_Tm1621Wr_PIN | GPIO_Tm1621Cs_PIN; //ѡ�д���Ĭ������ܽ�
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#if BPWM_OK
// lcd����ʱ��tim3����pwm������
void TIM3_ConfigInit(void)
{
	//��������
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // io�ڸ���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1); // io�ڸ���ΪAF1
	//��ʱ������
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //����tim3ʱ��

	TIM_TimeBaseInitStruct.TIM_Period = 250 - 1;				 //�Զ����ؼ�������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 48 - 1;				 //Ԥ����ϵ��1us
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;				 // TIM_CKD_DIV1;//ʱ�ӷ�Ƶϵ��0
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //����Ϊ���ϼ�����ʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  // TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //�������:TIM����Ƚϼ���
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_SetCompare1(TIM3, 250);						  //��������ȵ���
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); // CH1Ԥװ��ʹ��
	TIM_CtrlPWMOutputs(TIM3, ENABLE);				  //ʹ��pwm
													  // TIM_ARRPreloadConfig(TIM3, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM3, ENABLE);							  //ʹ��TIM3��ʱ��
}
#else
void TIM3_ConfigInit(void)
{
	//��������
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//--��ʱ��ʱ�ӳ�ʼ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//--��ʱ����ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;	  //����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1; //��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //��ʱ�ж�ʹ��
	//--NVIC�ж����ȼ�����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //���ȼ�1��
	NVIC_Init(&NVIC_InitStructure);

	TIM3->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
	TIM3->ARR = 250;
	TIM3->CR1 |= TIM_CR1_CEN; //�򿪶�ʱ��
}
u16 TIM3_PWMCompare = 50; //ģ��pwm������
void TIM3_IRQHandler(void)
{
	static u8 timselect = 1;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{ //�ж��Ƿ���Ӧ���¼�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if (timselect) //�͵�ƽ���
		{
			timselect = 0;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
			TIM3->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
			TIM3->ARR = 250 - TIM3_PWMCompare;
			TIM3->CR1 |= TIM_CR1_CEN; //�򿪶�ʱ��
		}
		else
		{
			timselect = 1;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
			TIM3->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
			TIM3->ARR = TIM3_PWMCompare;	//����
			TIM3->CR1 |= TIM_CR1_CEN;		//�򿪶�ʱ��
		}
	}
}
void TIM3_SetPWM(u16 Compare)
{
	if (Compare == 0)
	{
		TIM3->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
	}
	else if (Compare >= 250)
	{
		TIM3->CR1 &= (u16)~TIM_CR1_CEN; //�رն�ʱ��
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
	}
	else
	{
		TIM3_PWMCompare = Compare;
		TIM3->CR1 |= TIM_CR1_CEN;		//�򿪶�ʱ��
	}
}

#endif

/******************************************
��Dataд����Write����1621
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
// lcd��оƬ��ʼ������
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
//////////////�������а����Ƶ���ת��ΪlcdҺ�������ĸ�ʽ
//////////////////20211015/20211108

/////////ԭ��GFED CBA_
///��0��-ת��_CGB DEFA
///��1��-ת��BGC_ AFED
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
	//////////����ת������hc595תΪled1621
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
	////������������TH1621B�д�ŵ�����˳��ṹ��ͬ��������0-7���е�0����ת����8-16���е�1����ת��
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
