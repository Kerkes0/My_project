#include "User.h"
#include "Adc.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "LCD.h"
#define ADfindouble 2

int32_t Temp_Value = 0; //传感器校准过AD值
uint32_t WaterPress_value = 0;//传感器压力值

uint32_t u16Analog_AD = 0; //模拟量控制的模拟量


uint32_t WaterGate = 0;//（*******）


uchar water_max_set = 0;	// yxl-5
uchar water_min_set = 0;	// yxl-5



u16 temp_cail_add = 0;	  //传感器校准值：正
u16 temp_cail_subtra = 0; //传感器校准值：负

u16 Temp_Value_curr_zf10_test = 0; //自检时，模拟控制的ad值
u16 Temp_value_test = 0;		   //自检时，压力传感器的ad值

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








uchar fg_flash_rest_con = 0; // 0为不闪，准备自学习时闪烁标志

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

	ADC_DeInit(ADC1); //初始化ADC1
	ADC_StructInit(&ADC_InitStructure);
	/* Configure the ADC1 in Single mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						//选择转换的分辨率
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//开启连续转换模式;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //选择外部触发器边缘并启用常规组的触发器。
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;		// ADC 转换触发信号选择
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						// ADC 数据寄存器对齐格式
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;				//扫描通道的方向
	ADC_Init(ADC1, &ADC_InitStructure);
	/* 配置ADC时钟为4分频，即48/4=12M */
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4); //不要超过14MHz
	/* 配置ADC通道采用时间 */				// ADC_Channel_4为通道4，在这里表示PA4为转换通道
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_55_5Cycles);

	//?无通道6的采样时间配置

	//注意F0的ADC在使用之前需要校准?
	ADC_GetCalibrationFactor(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_ADRDY); // For HK32F0xx case
										 /* 开启ADC，进行转换 */
	ADC_Cmd(ADC1, ENABLE);
	/* Wait the ADRDY flag */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
		; //等待ADC准备好
		  /* ADC1 regular Software Start Conv */
		  // ADC_StartOfConversion(ADC1);

	//----------------------------------------- 传感器检测开关
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_8); //默认开启
}

u16 Get_average(uint32_t channel)
{
	uint32_t buf = 0;
	uint8_t i;
	ADC1->CHSELR = channel;
	ADC_StartOfConversion(ADC1); //开始转换
	for (i = 0; i < 3; i++)
	{
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
			;								 //等待转换完成
		buf += ADC_GetConversionValue(ADC1); //将存在寄存器里的转换数字量加到buf变量中，准备求平均值
		ADC1->ISR = (uint32_t)ADC_FLAG_OVR;	 //清标志位
	}
	ADC_StopOfConversion(ADC1); //停止转换
	return buf / 3;				//返回平均值
}

//循环采集传感器IO口，需放在1ms时基中
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
		adtable = atextad; //测试用，记得屏蔽
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

//获取20个传感器AD值并进行滤波（需放在小于20ms的时基内）
u16 fg_AD_hanok_time = 0; // 20ms一次读取AD值成功的时间标签更新
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
		AD_fin_buf = ad_all >> 3;													   //平均滤波后的压力传感器ad值
		Temp_value_test = AD_fin_buf;												   //自检时，压力传感器的ad值，跑马
		AD_fin_xxbuf = (((15 * AD_fin_xxbuf) + (AD_fin_buf << ADfindouble) + 8) >> 4); //低通滤波(15 、 1 、 16)
		AD_fin = AD_fin_xxbuf >> ADfindouble;										   //平均和低通滤波后的压力传感器ad值
		// AD_hanok = 1;
		//  20ms一次读取AD值成功的时间标签更新//20220506
		if (++fg_AD_hanok_time >= 60000)
			fg_AD_hanok_time = 0;
	}
}

//恢复初始设置后的获取AD校准值,以计算水压
u8 getadbaseTm = 0;//获取AD校准值所需时间
void FunPSensor_GetADBase(void)
{
	// if(SysProState == SysProState_Init)
	if (fg_air_cali) // 触发大气压力校准
	{
		if (getadbaseTm >= 14) //需小于16
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

// 6s一次获取水压
u8 getwaterpressTm = 0;			 // 6s读取一次AD并转换为压力值
u8 fg_waterPress_value_time = 0; // 6s一次读取AD成功的时间标签
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
				Temp_Value -= temp_cail_add; // yxl-5  减去校准数据
			}
			else if (temp_cail_subtra > 0)
			{
				Temp_Value += temp_cail_subtra; // yxl-5 加上校准数据
			}
			//==============================================================
			if ((Temp_Value > 4000) || (Temp_Value < 50)) //传感器故障  yxl-e207
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
				WaterPress_value = 0; // yxl  -5   添加待验证
			}
			// 6s一次读取AD值成功的时间标签更新//20220506
			if (++fg_waterPress_value_time >= 200)
				fg_waterPress_value_time = 0;
		}
	}
}

//判断传感器是否故障
u8 gu8SensorIserror500msTm = 0;//传感器的500ms时基
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

		if (PAin(8)) //传感器工作时
		{
			if (++count_SensorIserror_6s >= 4)
			{
				count_SensorIserror_6s = 0;
				if ((AD_fin_buf > 4000) || (AD_fin_buf < 50)) //不在范围，有问题时
				{
					count_Sensor_noerror_tm = 0;
					if (++count_Sensor_iserror_tm >= 3) // 3次超出范围则认为有故障
					{
						count_Sensor_iserror_tm = 0;
						GPIO_ResetBits(GPIOA, GPIO_Pin_8); //关闭传感器
						SetBit(gu16TripFlg_Board, Ownowaterpr);
						fg_rest_con_ok = 1; //触发自学习
					}
				}
				else //在范围，无问题时
				{
					count_Sensor_iserror_tm = 0;
					if (++count_Sensor_noerror_tm >= 3) // 3次在范围则认为无故障
					{
						count_Sensor_noerror_tm = 0;
						ClrBit(gu16TripFlg_Board, Ownowaterpr);
					}
				}
			}
		}
		else //传感器关闭时
		{
			if (u8fg_SysIsOn)
			{
				SetBit(gu16TripFlg_Board, Ownowaterpr);
			}
			count_Sensor_iserror_tm = 0;
			count_Sensor_noerror_tm = 0;
			if (++count_Sensor_1min_tm > 120) // 1分钟开一次重新监测
			{
				count_Sensor_1min_tm = 0;
				GPIO_SetBits(GPIOA, GPIO_Pin_8);
			}
		}
		//关闭传感器故障
		if (u8fg_SD_PSensorOpen == 0)
		{
			fg_PSensorOpen = 0;				   //传感器关闭中
			GPIO_ResetBits(GPIOA, GPIO_Pin_8); //关闭传感器
			ClrBit(gu16TripFlg_Board, Ownowaterpr);
		}
		//开启传感器故障
		else
		{
			if (fg_PSensorOpen == 0)
			{
				fg_PSensorOpen = 1;				 //传感器开启中
				GPIO_SetBits(GPIOA, GPIO_Pin_8); //开启传感器
			}
		}
	}
	if (SysProState == SysProState_Aging)
	{
		ClrBit(gu16TripFlg_Board, Ownowaterpr);
	}
}

//传感器工作:获取AD值、水压值、判断传感器故障
void FunPSonser(void)
{
	FunPSensor_GetAD();	   //获取20个传感器AD值并进行滤波（需放在小于20ms的时基内）
	FunPSensor_GetPress(); // 6s一次获取水压
	FunPSensor_IsError();  //判断传感器是否故障
}

u8 count_UpWater_1min_tm = 0;
u8 gu8AutoUpWater500msTm = 0;//上水自吸的500ms时基
u16 count_6min_lowwater = 0;
u16 count_02min_lowwater = 0;


u32 sum_waterpress_lowwater = 0;
u32 waterpress_lowwater_max = 0;
u32 waterpress_lowwater_min = 4095;

u8 fg_waterPress_lowpower = 0;
u32 AutoUpWater_Base = 0;
//获取基准点和判断是否上水自学习
void FunAutoUpWater(void)
{
	static u32 AutoUpWater_Deviation = 0;
	static u32 gu16RunSpd_lowwater_buf = 0;
	if (gu8AutoUpWater500msTm >= 1) // 500ms一次
	{
		gu8AutoUpWater500msTm = 0;
		//-----------------------------------------获取基准点
		//不可运行时
		if ((u8fg_SysIsOn == 0)				  //关机时
			|| (gu16TripFlg_Driver)			  //有故障时
			|| (gu16TripFlg_Board)			  //有故障时
			|| (gu16TripFlg_Warn)			  //有预警时
			|| (u8WashState)				  //已经在自吸时
			|| (ValBit(gu8SetTimeFlg, bStay)) //在定时模式等待运行时
			|| (gu8BakLgtDlyTm)				  //屏幕亮度全亮时（全亮会影响传感器读数）
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
			//转速调节后
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
			//转速大于0时，且未获取基准点时
			else if (gu16RunSpd && fg_waterPress_lowpower == 0)
			{
				count_02min_lowwater++;
				//前两分钟内不要、每6s一次、共10次
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
					fg_waterPress_lowpower = 1; //学习成功
				}
			}
		}
		//-----------------------------------------获取基准点

		//-----------------------------------------判断是否需要重新上水自学习
		//未获取基准点时
		if (fg_waterPress_lowpower == 0)
		{
			count_UpWater_1min_tm = 0; //计数时间
			count_6min_lowwater = 0;
		}
		//已获取基准点后
		else
		{
			//开始计时时间
			if (count_UpWater_1min_tm < 0xff)
			{
				count_UpWater_1min_tm++;
			}

			//根据转速设置限制范围
			if (gu16RunSpd > 2000)
				AutoUpWater_Deviation = 60;
			else if (gu16RunSpd < 1200)
				AutoUpWater_Deviation = 35;
			else
				AutoUpWater_Deviation = 35 + ((gu16RunSpd - 1200) >> 5);

			// 1min后开始判断是否需重新上水自吸
			if (count_UpWater_1min_tm >= 120) // 1min
			{
				//超过基准值的限定范围后，则需要重新上水自吸
				if (((Temp_Value < AutoUpWater_Base - AutoUpWater_Deviation && AutoUpWater_Base > AutoUpWater_Deviation) //
					 || (Temp_Value > AutoUpWater_Base + AutoUpWater_Deviation)											 //
					 )																									 //
					&& (Temp_Value > 50 && Temp_Value < 4000)															 //
				)
				{
					count_6min_lowwater++;

					if (count_6min_lowwater >= 720) // 连续6分钟不在范围内，则触发上水自学习
					{
						count_6min_lowwater = 0;
						FunModeToUpWash();	//进入上水自吸模式
						fg_rest_con_ok = 1; //准备进入自学习
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
		//一分钟后仍判断不在正常范围内，则开始闪烁
		if (count_6min_lowwater > 120) // yxl-e207  一分钟过去再闪烁
		{
			fg_flash_rest_con = 1; //闪烁
		}
		else
		{
			fg_flash_rest_con = 0;
		}
	}
	//-----------------------------------------判断是否需要重新上水自学习
}



//---------------------------------------------------外部模拟量控制
//模拟输入控制
u16 atextconad = 0xffff;
//循环采集传感器IO口，需放在任意时基中
u16 u16Analog_GetTab[8];
u8 Analog_GetOK = 0;
void FunAnalog_CycleGet(void)
{
	static u8 GetCount = 0;
	u16 adtable;
	adtable = Get_average(ADC_Channel_4); //选择PA4为转换通道（电流或电压输入）
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
//模拟量读取
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
		Analog_ADTab = Analog_ADTab >> 8;		  //求得上面和的平均值，相当与在10*2*10ms内读取10*3次模拟量的平均值
		Temp_Value_curr_zf10_test = Analog_ADTab; // yxl-5  跑马

		//一种类似将不稳定折线拟合成稳定折线变化
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

		//判断得到的模拟量，开启或关闭外部模拟输入控制
		//模拟量控制无效
		if ((u16Analog_AD < 496))
		{
			ClrBit(u8OutCtrlState, Ctrl_Analog);	//退出外部模拟输入控制
			ClrBit(u8OutCtrlState, Ctrl_AnalogOff); //模拟量控制开机
		}
		//模拟量控制关机
		else if (u16Analog_AD < 1117)
		{
			SetBit(u8OutCtrlState, Ctrl_Analog);	//开启外部模拟输入控制
			SetBit(u8OutCtrlState, Ctrl_AnalogOff); //模拟量控制关机
		}
		//模拟量控制转速
		else
		{
			SetBit(u8OutCtrlState, Ctrl_Analog);	//开启外部模拟输入控制
			ClrBit(u8OutCtrlState, Ctrl_AnalogOff); //模拟量控制开机
		}
	}
}
