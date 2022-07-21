#include "User.h"
#include "Adc.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "LCD.h"
#define ADfindouble 2

int32_t Temp_Value = 0; //������У׼��ADֵ
uint32_t WaterPress_value = 0;//������ѹ��ֵ

uint32_t u16Analog_AD = 0; //ģ�������Ƶ�ģ����


uint32_t WaterGate = 0;//��*******��


uchar water_max_set = 0;	// yxl-5
uchar water_min_set = 0;	// yxl-5



u16 temp_cail_add = 0;	  //������У׼ֵ����
u16 temp_cail_subtra = 0; //������У׼ֵ����

u16 Temp_Value_curr_zf10_test = 0; //�Լ�ʱ��ģ����Ƶ�adֵ
u16 Temp_value_test = 0;		   //�Լ�ʱ��ѹ����������adֵ

//                      1     2    3    4    5    6    7    8    9   10    11  12   13   14    15  16   17    18   19   20  21    22  23   24    25 26  27  28  29  30   31  32  33  34  35
uchar sp_2600_con_press[] = {55, 59, 62, 64, 67, 70, 72, 75, 78, 80, 83, 86, 88, 91, 94, 96, 99, 102, 104, 107, 109, 112, 115, 117, 120, 123, 126, 129, 132, 135, 138, 170, 170, 170, 170, 170}; // 36
u16 sp_2600_con_flow[] = {
	2702,
	2625,
	2574,
	2539,
	2489,
	2453,
	2399,
	2340,
	2278,
	2225,
	2166,
	2129,
	2067,
	2033,
	1971,
	1921,
	1830,
	1761,
	1670,
	1601,
	1512,
	1466,
	1320,
	1217,
	1121,
	998,
	860,
	740,
	567,
	394,
	100,
	0,
	0,
	0,
	0,
	0,
};








uchar fg_flash_rest_con = 0; // 0Ϊ������׼����ѧϰʱ��˸��־

void McuAdcInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* Configure PA.01/02/03/04/05/06 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1); //��ʼ��ADC1
	ADC_StructInit(&ADC_InitStructure);
	/* Configure the ADC1 in Single mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						//ѡ��ת���ķֱ���
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//��������ת��ģʽ;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //ѡ���ⲿ��������Ե�����ó�����Ĵ�������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;		// ADC ת�������ź�ѡ��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						// ADC ���ݼĴ��������ʽ
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;				//ɨ��ͨ���ķ���
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ����ADCʱ��Ϊ4��Ƶ����48/4=12M */
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4); //��Ҫ����14MHz
	/* ����ADCͨ������ʱ�� */				// ADC_Channel_4Ϊͨ��4���������ʾPA4Ϊת��ͨ��
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_55_5Cycles);

	//?��ͨ��6�Ĳ���ʱ������

	//ע��F0��ADC��ʹ��֮ǰ��ҪУ׼?
	ADC_GetCalibrationFactor(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_ADRDY); // For HK32F0xx case
										 /* ����ADC������ת�� */
	ADC_Cmd(ADC1, ENABLE);
	/* Wait the ADRDY flag */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
		; //�ȴ�ADC׼����
		  /* ADC1 regular Software Start Conv */
		  // ADC_StartOfConversion(ADC1);

	//----------------------------------------- ��������⿪��
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_8); //Ĭ�Ͽ���
}

u16 Get_average(uint32_t channel)
{
	uint32_t buf = 0;
	uint8_t i;
	ADC1->CHSELR = channel;
	ADC_StartOfConversion(ADC1); //��ʼת��
	for (i = 0; i < 3; i++)
	{
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
			;								 //�ȴ�ת�����
		buf += ADC_GetConversionValue(ADC1); //�����ڼĴ������ת���������ӵ�buf�����У�׼����ƽ��ֵ
		ADC1->ISR = (uint32_t)ADC_FLAG_OVR;	 //���־λ
	}
	ADC_StopOfConversion(ADC1); //ֹͣת��
	return buf / 3;				//����ƽ��ֵ
}

//ѭ���ɼ�������IO�ڣ������1msʱ����
u32 u32PSensor_GetTab[20];
u8 PSensor_GetOK = 0;
u32 atextad = 0xffffffff;
void FunPSensor_CycleGet(void)
{
	static u8 GetCount = 0;
	u32 adtable;
	adtable = Get_average(ADC_Channel_6);
	if (atextad == 0xffffffff)
	{
	}
	else
	{
		adtable = atextad; //�����ã��ǵ�����
	}
	if (GetCount < 20)
	{
		u32PSensor_GetTab[GetCount++] = adtable;
	}
	else
	{
		GetCount = 0;
		PSensor_GetOK = 1;
	}
}

//��ȡ20��������ADֵ�������˲��������С��20ms��ʱ���ڣ�
u16 fg_AD_hanok_time = 0; // 20msһ�ζ�ȡADֵ�ɹ���ʱ���ǩ����
u32 AD_fin = 760, AD_fin_buf = 760, AD_fin_xxbuf = 760 << ADfindouble;
u32 AD_use_table[10];
static void FunPSensor_GetAD(void)
{
	static u8 AD_use_sec = 0;
	u32 ad_all = 0;
	u32 ad_max0 = 0, ad_max1 = 0;
	u32 ad_min0 = 4096, ad_min1 = 4096;

	u32 ad_use_max = 0, ad_use_min = 4096;
	if (PSensor_GetOK)
	{
		PSensor_GetOK = 0;
		for (u8 i = 0; i < 20; i++)
		{
			if (u32PSensor_GetTab[i] >= ad_max0 && u32PSensor_GetTab[i] >= ad_max1)
			{
				ad_max1 = ad_max0;
				ad_max0 = u32PSensor_GetTab[i];
			}
			else if (u32PSensor_GetTab[i] < ad_max0 && u32PSensor_GetTab[i] >= ad_max1)
			{
				ad_max1 = u32PSensor_GetTab[i];
			}
			if (u32PSensor_GetTab[i] <= ad_min0 && u32PSensor_GetTab[i] <= ad_min1)
			{
				ad_min1 = ad_min0;
				ad_min0 = u32PSensor_GetTab[i];
			}
			else if (u32PSensor_GetTab[i] > ad_min0 && u32PSensor_GetTab[i] <= ad_min1)
			{
				ad_min1 = u32PSensor_GetTab[i];
			}
			ad_all += u32PSensor_GetTab[i];
		}
		ad_all = ad_all - (ad_max0 + ad_max1 + ad_min0 + ad_min1);
		/************************************/
		AD_use_table[AD_use_sec] = ad_all >> 4;
		AD_use_sec++;
		ad_all = 0;
	}
	if (AD_use_sec == 10)
	{
		AD_use_sec = 0;
		for (u8 i = 0; i < 10; i++)
		{
			if (AD_use_table[i] > ad_use_max)
			{
				ad_use_max = AD_use_table[i];
			}
			if (AD_use_table[i] < ad_use_min)
			{
				ad_use_min = AD_use_table[i];
			}
			ad_all += AD_use_table[i];
		}
		ad_all = ad_all - (ad_use_max + ad_use_min);
		AD_fin_buf = ad_all >> 3;													   //ƽ���˲����ѹ��������adֵ
		Temp_value_test = AD_fin_buf;												   //�Լ�ʱ��ѹ����������adֵ������
		AD_fin_xxbuf = (((15 * AD_fin_xxbuf) + (AD_fin_buf << ADfindouble) + 8) >> 4); //��ͨ�˲�(15 �� 1 �� 16)
		AD_fin = AD_fin_xxbuf >> ADfindouble;										   //ƽ���͵�ͨ�˲����ѹ��������adֵ
		// AD_hanok = 1;
		//  20msһ�ζ�ȡADֵ�ɹ���ʱ���ǩ����//20220506
		if (++fg_AD_hanok_time >= 60000)
			fg_AD_hanok_time = 0;
	}
}

//�ָ���ʼ���ú�Ļ�ȡADУ׼ֵ,�Լ���ˮѹ
u8 getadbaseTm = 0;//��ȡADУ׼ֵ����ʱ��
void FunPSensor_GetADBase(void)
{
	// if(SysProState == SysProState_Init)
	if (fg_air_cali) // ��������ѹ��У׼
	{
		if (getadbaseTm >= 14) //��С��16
		{
			getadbaseTm = 0;

			fg_air_cali = 0;
			if (AD_fin_buf >= 760)
			{
				temp_cail_add = AD_fin_buf - 760;
				if (temp_cail_add > 150)
					temp_cail_add = 150; // yxl-e207
			}
			else
			{
				temp_cail_subtra = 760 - AD_fin_buf;
				if (temp_cail_subtra > 150)
					temp_cail_subtra = 150; // yxl-e207
			}
		}
	}
}

// 6sһ�λ�ȡˮѹ
u8 getwaterpressTm = 0;			 // 6s��ȡһ��AD��ת��Ϊѹ��ֵ
u8 fg_waterPress_value_time = 0; // 6sһ�ζ�ȡAD�ɹ���ʱ���ǩ
static void FunPSensor_GetPress(void)
{
	static u16 fg_waterpress_time = 0;
	if (getwaterpressTm >= 12)
	{
		getwaterpressTm = 0;
		if (fg_AD_hanok_time != fg_waterpress_time)
		{
			fg_AD_hanok_time = fg_waterpress_time;
			//==============================================================
			Temp_Value = AD_fin;
			if (temp_cail_add > 0)
			{
				Temp_Value -= temp_cail_add; // yxl-5  ��ȥУ׼����
			}
			else if (temp_cail_subtra > 0)
			{
				Temp_Value += temp_cail_subtra; // yxl-5 ����У׼����
			}
			//==============================================================
			if ((Temp_Value > 4000) || (Temp_Value < 50)) //����������  yxl-e207
			{
				WaterPress_value = 0;
			}
			else if (Temp_Value >= 760)
			{
				WaterPress_value = Temp_Value - 759;
				WaterPress_value = WaterPress_value * 3000;
				WaterPress_value = WaterPress_value / 3038;
				WaterPress_value = WaterPress_value;
			}
			else
			{
				WaterPress_value = 0; // yxl  -5   ��Ӵ���֤
			}
			// 6sһ�ζ�ȡADֵ�ɹ���ʱ���ǩ����//20220506
			if (++fg_waterPress_value_time >= 200)
				fg_waterPress_value_time = 0;
		}
	}
}

//�жϴ������Ƿ����
u8 gu8SensorIserror500msTm = 0;//��������500msʱ��
u8 count_Sensor_1min_tm = 0;
static void FunPSensor_IsError(void)
{
	static u8 fg_PSensorOpen = 1;
	static u8 count_SensorIserror_6s = 0;
	static u8 count_Sensor_iserror_tm = 0;
	static u8 count_Sensor_noerror_tm = 0;
	if (gu8SensorIserror500msTm >= 1)
	{
		gu8SensorIserror500msTm = 0;

		if (PAin(8)) //����������ʱ
		{
			if (++count_SensorIserror_6s >= 4)
			{
				count_SensorIserror_6s = 0;
				if ((AD_fin_buf > 4000) || (AD_fin_buf < 50)) //���ڷ�Χ��������ʱ
				{
					count_Sensor_noerror_tm = 0;
					if (++count_Sensor_iserror_tm >= 3) // 3�γ�����Χ����Ϊ�й���
					{
						count_Sensor_iserror_tm = 0;
						GPIO_ResetBits(GPIOA, GPIO_Pin_8); //�رմ�����
						SetBit(gu16TripFlg_Board, Ownowaterpr);
						fg_rest_con_ok = 1; //������ѧϰ
					}
				}
				else //�ڷ�Χ��������ʱ
				{
					count_Sensor_iserror_tm = 0;
					if (++count_Sensor_noerror_tm >= 3) // 3���ڷ�Χ����Ϊ�޹���
					{
						count_Sensor_noerror_tm = 0;
						ClrBit(gu16TripFlg_Board, Ownowaterpr);
					}
				}
			}
		}
		else //�������ر�ʱ
		{
			if (u8fg_SysIsOn)
			{
				SetBit(gu16TripFlg_Board, Ownowaterpr);
			}
			count_Sensor_iserror_tm = 0;
			count_Sensor_noerror_tm = 0;
			if (++count_Sensor_1min_tm > 120) // 1���ӿ�һ�����¼��
			{
				count_Sensor_1min_tm = 0;
				GPIO_SetBits(GPIOA, GPIO_Pin_8);
			}
		}
		//�رմ���������
		if (u8fg_SD_PSensorOpen == 0)
		{
			fg_PSensorOpen = 0;				   //�������ر���
			GPIO_ResetBits(GPIOA, GPIO_Pin_8); //�رմ�����
			ClrBit(gu16TripFlg_Board, Ownowaterpr);
		}
		//��������������
		else
		{
			if (fg_PSensorOpen == 0)
			{
				fg_PSensorOpen = 1;				 //������������
				GPIO_SetBits(GPIOA, GPIO_Pin_8); //����������
			}
		}
	}
	if (SysProState == SysProState_Aging)
	{
		ClrBit(gu16TripFlg_Board, Ownowaterpr);
	}
}

//����������:��ȡADֵ��ˮѹֵ���жϴ���������
void FunPSonser(void)
{
	FunPSensor_GetAD();	   //��ȡ20��������ADֵ�������˲��������С��20ms��ʱ���ڣ�
	FunPSensor_GetPress(); // 6sһ�λ�ȡˮѹ
	FunPSensor_IsError();  //�жϴ������Ƿ����
}

u8 count_UpWater_1min_tm = 0;
u8 gu8AutoUpWater500msTm = 0;//��ˮ������500msʱ��
u16 count_6min_lowwater = 0;
u16 count_02min_lowwater = 0;


u32 sum_waterpress_lowwater = 0;
u32 waterpress_lowwater_max = 0;
u32 waterpress_lowwater_min = 4095;

u8 fg_waterPress_lowpower = 0;
u32 AutoUpWater_Base = 0;
//��ȡ��׼����ж��Ƿ���ˮ��ѧϰ
void FunAutoUpWater(void)
{
	static u32 AutoUpWater_Deviation = 0;
	static u32 gu16RunSpd_lowwater_buf = 0;
	if (gu8AutoUpWater500msTm >= 1) // 500msһ��
	{
		gu8AutoUpWater500msTm = 0;
		//-----------------------------------------��ȡ��׼��
		//��������ʱ
		if ((u8fg_SysIsOn == 0)				  //�ػ�ʱ
			|| (gu16TripFlg_Driver)			  //�й���ʱ
			|| (gu16TripFlg_Board)			  //�й���ʱ
			|| (gu16TripFlg_Warn)			  //��Ԥ��ʱ
			|| (u8WashState)				  //�Ѿ�������ʱ
			|| (ValBit(gu8SetTimeFlg, bStay)) //�ڶ�ʱģʽ�ȴ�����ʱ
			|| (gu8BakLgtDlyTm)				  //��Ļ����ȫ��ʱ��ȫ����Ӱ�촫����������
		)
		{
			gu16RunSpd_lowwater_buf = 0;

			fg_waterPress_lowpower = 0;
			AutoUpWater_Base = 0;

			count_02min_lowwater = 0;
			sum_waterpress_lowwater = 0;
			waterpress_lowwater_max = 0;
			waterpress_lowwater_min = 4095;
		}
		else
		{
			//ת�ٵ��ں�
			if (gu16RunSpd_lowwater_buf != gu16RunSpd)
			{
				gu16RunSpd_lowwater_buf = gu16RunSpd;

				AutoUpWater_Base = 0;
				fg_waterPress_lowpower = 0;

				count_02min_lowwater = 0;
				sum_waterpress_lowwater = 0;
				waterpress_lowwater_max = 0;
				waterpress_lowwater_min = 4095;
			}
			//ת�ٴ���0ʱ����δ��ȡ��׼��ʱ
			else if (gu16RunSpd && fg_waterPress_lowpower == 0)
			{
				count_02min_lowwater++;
				//ǰ�������ڲ�Ҫ��ÿ6sһ�Ρ���10��
				if ((count_02min_lowwater >= 240) && ((count_02min_lowwater % 12) == 0) && (count_02min_lowwater < 360))
				{
					if (waterpress_lowwater_max < Temp_Value)
						waterpress_lowwater_max = Temp_Value;
					if (waterpress_lowwater_min > Temp_Value)
						waterpress_lowwater_min = Temp_Value;
					sum_waterpress_lowwater += Temp_Value;
				}
				if (count_02min_lowwater >= 360)
				{
					count_02min_lowwater = 0;

					sum_waterpress_lowwater = sum_waterpress_lowwater - waterpress_lowwater_max;
					sum_waterpress_lowwater = sum_waterpress_lowwater - waterpress_lowwater_min;

					AutoUpWater_Base = (sum_waterpress_lowwater >> 3);
					fg_waterPress_lowpower = 1; //ѧϰ�ɹ�
				}
			}
		}
		//-----------------------------------------��ȡ��׼��

		//-----------------------------------------�ж��Ƿ���Ҫ������ˮ��ѧϰ
		//δ��ȡ��׼��ʱ
		if (fg_waterPress_lowpower == 0)
		{
			count_UpWater_1min_tm = 0; //����ʱ��
			count_6min_lowwater = 0;
		}
		//�ѻ�ȡ��׼���
		else
		{
			//��ʼ��ʱʱ��
			if (count_UpWater_1min_tm < 0xff)
			{
				count_UpWater_1min_tm++;
			}

			//����ת���������Ʒ�Χ
			if (gu16RunSpd > 2000)
				AutoUpWater_Deviation = 60;
			else if (gu16RunSpd < 1200)
				AutoUpWater_Deviation = 35;
			else
				AutoUpWater_Deviation = 35 + ((gu16RunSpd - 1200) >> 5);

			// 1min��ʼ�ж��Ƿ���������ˮ����
			if (count_UpWater_1min_tm >= 120) // 1min
			{
				//������׼ֵ���޶���Χ������Ҫ������ˮ����
				if (((Temp_Value < AutoUpWater_Base - AutoUpWater_Deviation && AutoUpWater_Base > AutoUpWater_Deviation) //
					 || (Temp_Value > AutoUpWater_Base + AutoUpWater_Deviation)											 //
					 )																									 //
					&& (Temp_Value > 50 && Temp_Value < 4000)															 //
				)
				{
					count_6min_lowwater++;

					if (count_6min_lowwater >= 720) // ����6���Ӳ��ڷ�Χ�ڣ��򴥷���ˮ��ѧϰ
					{
						count_6min_lowwater = 0;
						FunModeToUpWash();	//������ˮ����ģʽ
						fg_rest_con_ok = 1; //׼��������ѧϰ
					}
				}
				else
				{
					if (count_6min_lowwater > 0)
					{
						count_6min_lowwater--;
					}
					if ((Temp_Value <= 50) || (Temp_Value >= 4000))
					{
						count_6min_lowwater = 0;
					}
				}
			}
		}
		//һ���Ӻ����жϲ���������Χ�ڣ���ʼ��˸
		if (count_6min_lowwater > 120) // yxl-e207  һ���ӹ�ȥ����˸
		{
			fg_flash_rest_con = 1; //��˸
		}
		else
		{
			fg_flash_rest_con = 0;
		}
	}
	//-----------------------------------------�ж��Ƿ���Ҫ������ˮ��ѧϰ
}



//---------------------------------------------------�ⲿģ��������
//ģ���������
u16 atextconad = 0xffff;
//ѭ���ɼ�������IO�ڣ����������ʱ����
u16 u16Analog_GetTab[8];
u8 Analog_GetOK = 0;
void FunAnalog_CycleGet(void)
{
	static u8 GetCount = 0;
	u16 adtable;
	adtable = Get_average(ADC_Channel_4); //ѡ��PA4Ϊת��ͨ�����������ѹ���룩
	if (atextconad == 0xffff)
	{
	}
	else
	{
		adtable = atextconad;
	}

	if (GetCount < 8)
	{
		u16Analog_GetTab[GetCount++] = adtable;
	}
	else
	{
		GetCount = 0;
		Analog_GetOK = 1;
	}
}
//ģ������ȡ
void FunAnalogCtrl(void)
{
	u16 Analog_ADTab;
	static u16 Analog_ADTab_bef = 0;
	if (Analog_GetOK)
	{
		for (u8 i = 0; i < 8; i++)
		{
			Analog_ADTab += u16Analog_GetTab[i];
		}
		Analog_ADTab = Analog_ADTab >> 8;		  //�������͵�ƽ��ֵ���൱����10*2*10ms�ڶ�ȡ10*3��ģ������ƽ��ֵ
		Temp_Value_curr_zf10_test = Analog_ADTab; // yxl-5  ����

		//һ�����ƽ����ȶ�������ϳ��ȶ����߱仯
		if (Analog_ADTab > 40)
		{
			if ((Analog_ADTab_bef > Analog_ADTab + 40) || (Analog_ADTab_bef < Analog_ADTab - 40))
			{
				Analog_ADTab_bef = Analog_ADTab;
				u16Analog_AD = Analog_ADTab;
			}
		}
		else
		{
			u16Analog_AD = 0;
		}

		//�жϵõ���ģ������������ر��ⲿģ���������
		//ģ����������Ч
		if ((u16Analog_AD < 496))
		{
			ClrBit(u8OutCtrlState, Ctrl_Analog);	//�˳��ⲿģ���������
			ClrBit(u8OutCtrlState, Ctrl_AnalogOff); //ģ�������ƿ���
		}
		//ģ�������ƹػ�
		else if (u16Analog_AD < 1117)
		{
			SetBit(u8OutCtrlState, Ctrl_Analog);	//�����ⲿģ���������
			SetBit(u8OutCtrlState, Ctrl_AnalogOff); //ģ�������ƹػ�
		}
		//ģ��������ת��
		else
		{
			SetBit(u8OutCtrlState, Ctrl_Analog);	//�����ⲿģ���������
			ClrBit(u8OutCtrlState, Ctrl_AnalogOff); //ģ�������ƿ���
		}
	}
}
