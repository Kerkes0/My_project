#include "ModeFunc.h"
#include "main.h"
#include "Mode.h"
#include "User.h"
#include "LcdFunc.h"
#include "Key.h"
#include "Adc.h"
#include "string.h"

/***********关机进入功能显示**********************************/
//查看版本
u8 u8SD_SoftWareVerTm = 0; //软件版本的显示时间
//参数设置
u8 u8SD_ParamSetTm = 0;			   //参数设置的显示和设置时间
u8 u8SD_ParamSetOrder = 0;		   //参数设置第几项
u8 u8SD_ParamSet_OCtrlSpd1 = 40;   // Din控制时的转速设置，Din1默认100%
u8 u8SD_ParamSet_OCtrlSpd2 = 36;   // Din控制时的转速设置，Din1默认80%
u8 u8SD_ParamSet_OCtrlSpd3 = 28;   // Din控制时的转速设置，Din1默认40%
u8 u8SD_ParamSet_HandWashSpd = 0;  // 手动自吸时的转速设置
u8 u8fg_SD_ParamSet_AnalogSel = 0; // 0为电流，模拟量控制电流电压切换标志位
u8 u8SD_PowerOnTm = 25;			   // 25为开启，开机是否进入25min上水
//恢复出厂设置（无水初始化）
u8 u8SD_ReStoreDisWaterTm = 0;	 //恢复出厂设置（无水初始化）完成后，CA01的显示时间
u8 u8SD_IntoReStoreDisWater = 0; // 1为进入，恢复出厂设置（无水初始化）
u8 fg_air_cali = 0;				 // 1为触发，是否触发大气压力校准
//传感器是否开启
u8 u8SD_PSensorDisTm = 0;	//传感器是否开启标志的显示时间
u8 u8fg_SD_PSensorOpen = 1; // 1为开启，开启传感器故障
//查看AD校准值
u8 u8SD_PressAdDisTm = 0; //显示压力AD校准值时间

//-------------------------------------------------------
//查看实际压力值
u8 count_dis_WaterPress_value_tm = 0; //显示实际压力时间

//----------------------------------------------自吸
u8 gu8Wash = 0;				 //
uint WashTmSet = 180;		 //自吸设置时间
uint WashTmSet_Normal = 180; //反冲洗自吸的时间设置，记忆用
uint WashTm = 0;			 //自吸的倒数时间

uchar fg_rest_con_ok = 0; //自学习标志
//----------------------------------------------流量
u8 fg_ModeGateUnit = 0; //流量模式的流量单位
u8 fg_ModeTimeUnit = 0; //定时模式的流量单位
TypeGateRange ModeGate_m3h;
TypeGateRange ModeGate_Imp;
TypeGateRange ModeGate_Lmin;
TypeGateRange ModeGate_US;

u8 fg_auto_upwater_end = 0; //上水结束

/**
 * @brief  参数设置时，上、下键输入后动作
 * @param[in] {mode：1/-1} 向上或向下
 * @return Null
 * @note   mode为1表示当前设置的参数增加，反之为-1时表示减少
 */
void subSetRomUpDown(schar mode)
{

	gu16RomMinSpd = u16LadderSpd[0]; // 850固定死不可调整
	/////////第一个参数设置
	if (u8SD_ParamSetOrder == 0)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd1 = u8SD_ParamSet_OCtrlSpd1 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd1 = u8SD_ParamSet_OCtrlSpd1 - 1;
	}
	/////////第二个参数设置
	else if (u8SD_ParamSetOrder == 1)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd2 = u8SD_ParamSet_OCtrlSpd2 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd2 = u8SD_ParamSet_OCtrlSpd2 - 1;
	}
	/////////第三个参数设置
	else if (u8SD_ParamSetOrder == 2)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd3 = u8SD_ParamSet_OCtrlSpd3 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd3 = u8SD_ParamSet_OCtrlSpd3 - 1;
	}
	/////////第四个参数设置
	else if (u8SD_ParamSetOrder == 3) // yxl-5
	{
		if (mode > 0)
			u8SD_ParamSet_HandWashSpd = u8SD_ParamSet_HandWashSpd + 1;
		if (u8SD_ParamSet_HandWashSpd > 0)
		{
			if (mode < 0)
				u8SD_ParamSet_HandWashSpd = u8SD_ParamSet_HandWashSpd - 1;
		}
		if (u8SD_ParamSet_HandWashSpd > 4)
			u8SD_ParamSet_HandWashSpd = 4;
		if (u8SD_ParamSet_HandWashSpd < 1)
			u8SD_ParamSet_HandWashSpd = 0;
	}
	/////////第五个参数设置
	else if (u8SD_ParamSetOrder == 4) // yxl-5
	{
		if (mode > 0)
			u8fg_SD_ParamSet_AnalogSel = 1; // 1为电压，模拟量控制电流电压切换标志位
		if (mode < 0)
			u8fg_SD_ParamSet_AnalogSel = 0; // 0为电流，模拟量控制电流电压切换标志位
	}
	/////////第六个参数设置
	else if (u8SD_ParamSetOrder == 5) //开机是否进入上水
	{
		if (mode > 0)
			u8SD_PowerOnTm = 25;
		if (mode < 0)
			u8SD_PowerOnTm = 0;
	}

	/////////第一个参数设置范围：30%-100%
	if (u8SD_ParamSet_OCtrlSpd1 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd1 = 26; // yxl-30%								 // yyxxll
	if (u8SD_ParamSet_OCtrlSpd1 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd1 = 40; // 100%
	/////////第二个参数设置范围：30%-100%
	if (u8SD_ParamSet_OCtrlSpd2 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd2 = 26; // yxl-30%
	if (u8SD_ParamSet_OCtrlSpd2 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd2 = 40; // yyxxll
	/////////第三个参数设置范围：30%-100%
	if (u8SD_ParamSet_OCtrlSpd3 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd3 = 26; // yxl-30%
	if (u8SD_ParamSet_OCtrlSpd3 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd3 = 40; // yyxxll
}

/*********************系统内其他功能***********************/
//初始化变量（恢复出厂设置后）
void FunSysParamInit(void)
{
	SysProState = SysProState_Init;
	u8fg_E2_SysFirstIn = 1;						//接通电源或初始化重新进入
	u8SysExcuteOrder = 0;						//系统函数执行顺序，从0开始
												//	count_TestPress = 0;						//进入自检的按键次数
	memset(&T_base, 0, sizeof(T_base));			//系统时基
	memset(&SysProTime, 0, sizeof(SysProTime)); //系统部分属性记录时间

	/************流量**********************/
	// m3/h
	ModeGate_m3h.TabMax = 20;
	ModeGate_m3h.TabMin = 5;
	ModeGate_m3h.StudyMax = 50;
	ModeGate_m3h.StudyMin = 5;
	ModeGate_m3h.Set = 15;
	ModeGate_m3h.Real = 15;
	// IMP gpm
	ModeGate_Imp.TabMax = 70;
	ModeGate_Imp.TabMin = 15;
	ModeGate_Imp.StudyMax = 185;
	ModeGate_Imp.StudyMin = 15;
	ModeGate_Imp.Set = 45;
	ModeGate_Imp.Real = 45;
	// L/min
	ModeGate_Lmin.TabMax = 320;
	ModeGate_Lmin.TabMin = 80;
	ModeGate_Lmin.StudyMax = 840;
	ModeGate_Lmin.StudyMin = 80;
	ModeGate_Lmin.Set = 220;
	ModeGate_Lmin.Real = 220;
	// US gpm
	ModeGate_US.TabMax = 85;
	ModeGate_US.TabMin = 20;
	ModeGate_US.StudyMax = 220;
	ModeGate_US.StudyMin = 20;
	ModeGate_US.Set = 60;
	ModeGate_US.Real = 60;

	/************故障和预警**********************/
	gu16TripFlg_Board = 0;
	gu16TripFlg_Driver = 0;
	gu16TripFlg_Warn = 0;
}

//初始化设置（恢复出厂设置）
void sub1stRam(void)
{
	u8 i;

	u8ModeSpeed_SpeedSet = 36; // OEM的流量直接为80%流量值

	//-----------------------------------------------定时初始化
	gu16SetNowTm = 0; //定时设置的时钟时间

	gs16RunStartTm[0] = 7 * 60; //定时设置的预约时间段第1段的开始时间
	gs16RunEndTm[0] = 21 * 60;	//定时设置的预约时间段第1段的结束时间
	if (fg_time_spd_contorl == 0)
		u8ModeTime_SpeedSet[0] = 10; //定时设置的预约时间段第1段转速默认值70
	else
		u8ModeTime_GateSet_Tem[0] = 20; //定时设置的预约时间段第1段流量默认值

	for (i = 1; i < 4; i++) // yyxxll
	{

		gs16RunStartTm[i] = 0; //定时设置的预约时间段第2、3、4段的开始时间
		gs16RunEndTm[i] = 0;   //定时设置的预约时间段第2、3、4段的结束时间
		if (fg_time_spd_contorl == 0)
			u8ModeTime_SpeedSet[i] = 10; //定时设置的预约时间段第2、3、4段转速默认值70
		else
			u8ModeTime_GateSet_Tem[i] = 20; //定时设置的预约时间段第1段流量默认值20
	}

	for (i = 0; i < 4; i++)
	{
		SetStartTm[i] = gs16RunStartTm[i];					 //定时设置的预约时间段的开始时间
		SetEndTm[i] = gs16RunEndTm[i];						 //定时设置的预约时间段的结束时间
		u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i]; //定时设置的预约时间段的转速默认值70
															 // gu8SetTmGrd[i] = gu8RunTmGrd[i];					 //？
	}
	//-----------------------------------------------定时初始化

	gu16RomMinSpd = u16LadderSpd[0]; // gu16RomMinSpd = 1200; yxl-5

	//	gu8RomSelfSpd = 29;

	WashTmSet = 180;		//冲洗设置时间
	WashTmSet_Normal = 180; //反冲洗冲洗设置时间

	u8FirstFunIn = 0; //恢复出厂设置后的自学习，会有降速判断

	u8ModeTimeRunGate = 20; // yxl-5

	fg_time_spd_contorl = 0; // 0为转速，定时转速或流量控制标志位

	WaterGate_set = 20; //当前流量
	water_max_set = 25; //流量设置最大值
	water_min_set = 5;	//流量设置最小值

	fg_hand_close = 0; // 0为否，是否为手动开机标志位

	fg_rest_con_ok = 1; //自学习标志

	//--------------------------------参数设置
	u8SD_ParamSet_OCtrlSpd1 = 40;	//第1项//数字控制DIN1的速度
	u8SD_ParamSet_OCtrlSpd2 = 36;	//第2项//数字控制DIN2的速度
	u8SD_ParamSet_OCtrlSpd3 = 28;	//第3项//数字控制DIN3的速度
	u8SD_ParamSet_HandWashSpd = 4;	//第4项//反冲洗自吸速度
	u8fg_SD_ParamSet_AnalogSel = 0; //第5项//0为电流，模拟量控制电流电压切换标志位
	u8SD_PowerOnTm = 25;			//第6项//默认开启且为25minn，开机是否进入上水

	//--------------------------------参数设置

	//	for (i = 0; i < 45; i++)
	//	{
	//		WaterPress_value_con[i] = 0;
	//		fg_waterPress_value_con[i] = 0;
	//	}

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////恢复出厂设置，不同机型参数不一致
	////////////////////////////////////////////////////////////////////

	////定时设置中，流量控制相关按后面拨码器设置版本进行恢复
	if ((switch_key & 0x06) == 0) // IP20
	{
		water_max_set = 20;
		water_min_set = 5;
		WaterGate_set = 15;
		u8ModeTime_GateSet_Tem[0] = 15;
		u8ModeTime_GateSet_Tem[1] = 15;
		u8ModeTime_GateSet_Tem[2] = 15;
		u8ModeTime_GateSet_Tem[3] = 15;
	}
	else if ((switch_key & 0x06) == 2) // IP25
	{
		water_max_set = 25;
		water_min_set = 5;
		WaterGate_set = 20;
		u8ModeTime_GateSet_Tem[0] = 20;
		u8ModeTime_GateSet_Tem[1] = 20;
		u8ModeTime_GateSet_Tem[2] = 20;
		u8ModeTime_GateSet_Tem[3] = 20;
	}
	else if ((switch_key & 0x06) == 4) // IP30
	{
		water_max_set = 30;
		water_min_set = 5;
		WaterGate_set = 25;
		u8ModeTime_GateSet_Tem[0] = 25;
		u8ModeTime_GateSet_Tem[1] = 25;
		u8ModeTime_GateSet_Tem[2] = 25;
		u8ModeTime_GateSet_Tem[3] = 25;
	}
	// 20211223
	else //默认IP20
	{
		water_max_set = 20;
		water_min_set = 5;
		WaterGate_set = 15;
		u8ModeTime_GateSet_Tem[0] = 15;
		u8ModeTime_GateSet_Tem[1] = 15;
		u8ModeTime_GateSet_Tem[2] = 15;
		u8ModeTime_GateSet_Tem[3] = 15;
	}
	// 20211223

	water_uint_switch = 0; // 20211222

	gu8SetMode_eep = 0; // yxl  恢复默认流量模式
}
//清除关机功能中的持续时间
static void subModeSDClearTime(void)
{
	u8SD_SoftWareVerTm = 0;		//软件版本的显示时间
	u8SD_ParamSetTm = 0;		//参数设置的显示和设置时间
	u8SD_ReStoreDisWaterTm = 0; //无水初始化后CA01的显示时间
	u8SD_PSensorDisTm = 0;		//传感器是否开启标志的显示时间
	u8SD_PressAdDisTm = 0;		// AD校准值的显示时间
}

//设备关机的功能操作
u8 fg_InParamSet = 0; // 0为不进入，是否进入参数设置
void FunModeShutDown(void)
{
	//开机
	if (gu8KeyFlg == bPower)
	{
		u8fg_SysIsOn = 1;
		fg_hand_close = 0;	  //不在手动关机时
		fg_InParamSet = 0;	  //退出关机功能
		subModeSDClearTime(); //清除关机功能中的持续时间
	}

	//恢复出厂设置
	if (gu8KeyFlg == bLongTimeUp)
	{
		subModeSDClearTime();			//清除关机功能中的持续时间
		sub1stRam();					//恢复出厂设置
		SysProState = SysProState_Init; //系统进入初始化状态
		SysProTime.PowerTm = 0;			//接通电源时间重新计时

		gu8LcdInitTm = 15; //液晶7.5s初始化
		gu8RTCWaitTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime);
	}
	//软件版本号
	else if (gu8KeyFlg == bLongTimeDown)
	{
		subModeSDClearTime();	 //清除关机功能中的持续时间
		u8SD_SoftWareVerTm = 12; //版本号显示时间12*500ms=6s
	}
	//参数设置
	else if (gu8KeyFlg == bLongUpDown)
	{
		if (fg_InParamSet == 0)
		{
			gu8KeyFlg = 0;			//特殊：需手动清按键
			subModeSDClearTime();	//清除关机功能中的持续时间
			u8SD_ParamSetTm = 30;	//参数设置持续时间
			fg_InParamSet = 1;		//进入参数设置
			u8SD_ParamSetOrder = 0; //从第1个参数开始设置
		}
	}
	//进行无水初始化（长按MODE、CONTROL、TIME键，需在恢复出厂设置之后）
	else if (u8SD_IntoReStoreDisWater == 1) // yxl-5
	{
		subModeSDClearTime();		  //清除关机功能中的持续时间
		u8SD_IntoReStoreDisWater = 0; //手动清标志
		u8SD_ReStoreDisWaterTm = 15;  //显示500ms*15=7.5s时间的//无水初始化后CA01的显示时间
		temp_cail_add = 0;			  //校准数据归零
		temp_cail_subtra = 0;		  //校准数据归零
		fg_air_cali = 1;			  //大气校准中重新获得校准数据
		u8fg_SD_PSensorOpen = 1;	  //开启传感器故障        //只有出厂才保证是有效
	}
	//传感器是否开启标志（长按解锁和定时键）
	else if (gu8KeyFlg == bLongUnlockControl)
	{
		subModeSDClearTime(); //清除关机功能中的持续时间
		//开就关、关就开
		if (u8fg_SD_PSensorOpen)
		{
			u8fg_SD_PSensorOpen = 0; //关闭传感器
		}
		else
		{
			u8fg_SD_PSensorOpen = 1; //打开传感器
		}
		fg_rest_con_ok = 1;		//进入自学习
		u8SD_PSensorDisTm = 15; //显示500ms*15=7.5s时间的无水保护是否开启
	}
	// AD校准值显示
	else if (gu8KeyFlg == bLongUnlockTime)
	{
		subModeSDClearTime();	//清除关机功能中的持续时间
		u8SD_PressAdDisTm = 10; //显示压力AD校准值时间
	}

	//-----------参数设置
	if (fg_InParamSet) //关机时的参数设置模式
	{
		if (gu8KeyFlg == bUp)
		{
			subSetRomUpDown(1);
			u8SD_ParamSetTm = 30;
		}
		else if (gu8KeyFlg == bDown)
		{
			subSetRomUpDown(-1);
			u8SD_ParamSetTm = 30;
		}
		else if (gu8KeyFlg == bControl)
		{
			u8SD_ParamSetTm = 30; //参数设置持续时间
			if (++u8SD_ParamSetOrder > 5)
			{
				fg_InParamSet = 0;
				u8SD_ParamSetTm = 0;
			}
		}
		else if (gu8KeyFlg == bLongUpDown)
		{
			u8SD_ParamSetTm = 30; //参数设置持续时间
			if (++u8SD_ParamSetOrder > 5)
			{
				fg_InParamSet = 0;
				u8SD_ParamSetTm = 0;
			}
		}

		if (u8SD_ParamSetTm EQU 0)
		{
			fg_InParamSet = 0;
		}
	}
}
//设备模式记忆
void FunModeRemeber(void)
{
	static u8 Mode_bef = SPEED;
	//设备模式记录
	gu8SetMode_Bef = gu8SetMode;
	if (Mode_bef != gu8SetMode && gu8SetMode > 0 && gu8SetMode < 4)
	{
		gu8SetMode_ago = Mode_bef; //记忆前一刻运行模式
		Mode_bef = gu8SetMode;
	}
}

/*********************外部控制***********************/
// Din控制时的功能实现
void FunModeOutCtrl_Din(void)
{
	if (ValBit(u8OutCtrl_Din, bOutCtrl_Stop))
	{
		SetBit(u8OutCtrlState, Ctrl_DinOff); //关机
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_1))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_2))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_3))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
}
//模拟量控制时的功能实现
void FunModeOutCtrl_Analog(void)
{
	if ((u16Analog_AD < 1303))
		gu16RunSpd = u16LadderSpd[2]; // 1.05
	else if ((u16Analog_AD < 1489))
		gu16RunSpd = u16LadderSpd[3]; // 1.2
	else if ((u16Analog_AD < 1675))
		gu16RunSpd = u16LadderSpd[4]; // 1.35
	else if ((u16Analog_AD < 1861))
		gu16RunSpd = u16LadderSpd[5]; // 1.5
	else if ((u16Analog_AD < 2048))
		gu16RunSpd = u16LadderSpd[6]; // 1.65
	else if ((u16Analog_AD < 2234))
		gu16RunSpd = u16LadderSpd[7]; // 1.8
	else if ((u16Analog_AD < 2420))
		gu16RunSpd = u16LadderSpd[8]; // 1.95
	else if ((u16Analog_AD < 2606))
		gu16RunSpd = u16LadderSpd[9]; // 2.1
	else if ((u16Analog_AD < 2792))
		gu16RunSpd = u16LadderSpd[10]; // 2.25
	else if ((u16Analog_AD < 2978))
		gu16RunSpd = u16LadderSpd[11]; // 2.4
	else if ((u16Analog_AD < 3165))
		gu16RunSpd = u16LadderSpd[12]; // 2.55
	else if ((u16Analog_AD < 3351))
		gu16RunSpd = u16LadderSpd[13]; // 2.7
	else if ((u16Analog_AD < 3537))
		gu16RunSpd = u16LadderSpd[14]; // 2.85
	else if ((u16Analog_AD < 3723))
		gu16RunSpd = u16LadderSpd[15]; // 3
	else
		gu16RunSpd = u16LadderSpd[16];
}
// 485控制时的功能实现
void FunModeOutCtrl_485(void)
{
	if (gu16UartSpd)
	{
		for (u8 k = 0; k < 17; k++) // YXL-5
		{
			if ((gu16UartSpd >= (20 + 5 * k)) AND(gu16UartSpd < (25 + 5 * k)))
			{
				gu16RunSpd = u16LadderSpd[k];
			}
		}
	}
	// 485关机
	else
	{
		u8fg_SysIsOn = 0;
	}
}

/*********************自学习与流量范围相关**************/
//限制流量设置值在范围内
u16 FunModeGateLimit(const u16 gate, const u8 unit)
{
	u16 TabMax, TabMin, StudyMax, StudyMin;
	u16 GateSet;
	switch (unit)
	{
	case Unit_m3h:
		TabMax = ModeGate_m3h.TabMax;
		TabMin = ModeGate_m3h.TabMin;
		StudyMax = ModeGate_m3h.StudyMax;
		StudyMin = ModeGate_m3h.StudyMin;
		break;
	case Unit_Imp:
		TabMax = ModeGate_Imp.TabMax;
		TabMin = ModeGate_Imp.TabMin;
		StudyMax = ModeGate_Imp.StudyMax;
		StudyMin = ModeGate_Imp.StudyMin;
		break;
	case Unit_Lmin:
		TabMax = ModeGate_Lmin.TabMax;
		TabMin = ModeGate_Lmin.TabMin;
		StudyMax = ModeGate_Lmin.StudyMax;
		StudyMin = ModeGate_Lmin.StudyMin;
		break;
	case Unit_Us:
		TabMax = ModeGate_US.TabMax;
		TabMin = ModeGate_US.TabMin;
		StudyMax = ModeGate_US.StudyMax;
		StudyMin = ModeGate_US.StudyMin;
		break;
	default:
		TabMax = ModeGate_m3h.TabMax;
		TabMin = ModeGate_m3h.TabMin;
		StudyMax = ModeGate_m3h.StudyMax;
		StudyMin = ModeGate_m3h.StudyMin;
		break;
	}
	if (GateSet > TabMax)
		GateSet = TabMax;
	if (GateSet < TabMin)
		GateSet = TabMin;
	if (GateSet > StudyMax)
		GateSet = StudyMax;
	if (GateSet < StudyMin)
		GateSet = StudyMin;
	return GateSet;
}
//获取自学习流量范围最大值
u16 FunModeGate_StudyMax(const u8 unit)
{
	switch (unit)
	{
	case Unit_m3h:
		return ModeGate_m3h.StudyMax;

	case Unit_Imp:
		return ModeGate_Imp.StudyMax;

	case Unit_Lmin:
		return ModeGate_Lmin.StudyMax;

	case Unit_Us:
		return ModeGate_US.StudyMax;

	default:

		break;
	}
	return ModeGate_m3h.StudyMax;
}
//转速转换为对应单位流量
u16 FunMode_SpeedToGate(const u32 speed, const u8 unit)
{
	u16 GateSet;
	u16 StudyMax;
	StudyMax = FunModeGate_StudyMax(unit);
	GateSet = StudyMax * speed / u16LadderSpd[16];
	GateSet = FunModeGateLimit(GateSet, unit);
	return GateSet;
}
//流量设置量转换为转速
u32 FunMode_GateToSpeed(const u16 GateSet, const u8 unit)
{
	u32 speed;
	u16 StudyMax;
	StudyMax = FunModeGate_StudyMax(unit);
	speed = u16LadderSpd[16] * GateSet / StudyMax;
	return speed;
}
//转速转换为流量后的取整
u16 FunMode_UnitRound(const u16 gate, const u8 unit)
{
	u16 gate_set = gate;
	switch (unit)
	{
	case Unit_m3h:
		gate_set = gate;
		break;
	case Unit_Imp:
		gate_set = 5 * (gate / 5 + ((gate % 5 > 2) ? 1 : 0));
		break;
	case Unit_Lmin:
		gate_set = 20 * (gate / 20 + ((gate % 20 > 10) ? 1 : 0));
		break;
	case Unit_Us:
		gate_set = 5 * (gate / 5 + ((gate % 5 > 2) ? 1 : 0));
		break;
	default:
		gate_set = gate;
		break;
	}
	return gate_set;
}

//由m3/h单位转换为其他单位
u16 FunModeGate_m3hToOhter(const u16 GateSet, const u8 unit)
{
	u16 UnitSet = 5;
	u16 tab;
	switch (unit)
	{
	case Unit_m3h:
		UnitSet = GateSet;
		break;
	case Unit_Imp:
		tab = GateSet * 11 / 3;
		UnitSet = 5 * (tab / 5 + ((tab % 5 >= 2) ? 1 : 0)); //四舍五入
		break;
	case Unit_Lmin:
		tab = GateSet * 50 / 3;
		UnitSet = 20 * (tab / 20 + ((tab % 20 >= 10) ? 1 : 0)); //四舍五入
		break;
	case Unit_Us:
		tab = GateSet * 22 / 5;
		UnitSet = 5 * (tab / 5 + ((tab % 5 >= 2) ? 1 : 0)); //四舍五入
		break;
	default:
		break;
	}
	return UnitSet;
}

//转速转换为转速百分比量
u8 FunMode_SpeedToPercent(u32 speed)
{
	u8 spdset;
	for (u8 i = 0; i < 17; i++)
	{
		if (speed > (u16LadderSpd[i] - u16LadderSpd_rang))
		{
			spdset = i + 24;
		}
	}
	if (speed > u16LadderSpd[16])
		spdset = 40;
	if (speed && speed < u16LadderSpd[2])
		spdset = 26;
	if (speed == 0)
		spdset = 0;
	return spdset;
}
/*********************普通模式***********************/
//转速模式时，运转速率百分比设置（使用上下键调节）
void subModeUpDown(schar i)
{
	if (i >= 0)
		u8ModeSpeed_SpeedSet += 1;
	else
		u8ModeSpeed_SpeedSet -= 1;

	if (u8ModeSpeed_SpeedSet > 40)
		u8ModeSpeed_SpeedSet = 40;
	if (u8ModeSpeed_SpeedSet < 24 + 2)
		u8ModeSpeed_SpeedSet = 24 + 2; // yxl-30%
}

//流量模式时，运转流量速率设置（使用上下键调节）
void FunModeGateUpDown(const char i, const u8 unit)
{
	u8 GateDif;
	u16 *GateSet; //, *GateReal;
	switch (unit)
	{
	case Unit_m3h:
		GateDif = 1;
		GateSet = &ModeGate_m3h.Set;
		break;
	case Unit_Imp:
		GateDif = 5;
		GateSet = &ModeGate_Imp.Set;
		break;
	case Unit_Lmin:
		GateDif = 20;
		GateSet = &ModeGate_Lmin.Set;
		break;
	case Unit_Us:
		GateDif = 5;
		GateSet = &ModeGate_US.Set;
		break;
	default:
		GateDif = 1;
		GateSet = &ModeGate_m3h.Set;
		break;
	}
	if (i >= 0)
	{
		u16ModeGate_GateSet += GateDif;
		*GateSet += GateDif;
	}
	else
	{
		u16ModeGate_GateSet -= GateDif;
		*GateSet -= GateDif;
	}
	u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, unit);
	*GateSet = FunModeGateLimit(*GateSet, unit);
}
//流量模式时，运行流量单位切换
void FunModeGate_UnitSwitch(const char i)
{
	u8 a, fg_ModeGateUnit_bef;
	u32 speed;

	//	fg_ModeUnit_IsChange = 1; // 1为在，单位切换影响标志
	fg_ModeGateUnit_bef = fg_ModeGateUnit;
	if (i >= 0)
	{
		fg_ModeGateUnit += i;
		if (fg_ModeGateUnit >= 4)
			fg_ModeGateUnit = fg_ModeGateUnit - 4;
	}
	else
	{
		a = _abs(i);
		if (fg_ModeGateUnit < a)
			fg_ModeGateUnit = 4 + fg_ModeGateUnit - a;
		else
			fg_ModeGateUnit -= a;
	}
	//将流量值转换为选择的单位的值
	speed = FunMode_GateToSpeed(u16ModeGate_GateSet, fg_ModeGateUnit_bef);
	u16ModeGate_GateSet = FunMode_SpeedToGate(speed, fg_ModeGateUnit);
	u16ModeGate_GateSet = FunMode_UnitRound(u16ModeGate_GateSet, fg_ModeGateUnit);//取整
	u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, fg_ModeGateUnit);//限制
}

//定时模式
//定时模式时，运行流量单位切换
void FunModeTime_UnitSwitch(const char i)
{
	u8 a, fg_ModeTimeUnit_bef;

	//	fg_ModeUnit_IsChange = 1; // 1为在，单位切换影响标志
	fg_ModeTimeUnit_bef = fg_ModeTimeUnit;
	if (i >= 0)
	{
		fg_ModeTimeUnit += i;
		if (fg_ModeTimeUnit >= 4)
			fg_ModeTimeUnit = fg_ModeTimeUnit - 4;
	}
	else
	{
		a = _abs(i);
		if (fg_ModeTimeUnit < a)
			fg_ModeTimeUnit = 4 + fg_ModeTimeUnit - a;
		else
			fg_ModeTimeUnit -= a;
	}
	//定时模式设置时
	if (gu8SetTimeTm)
	{
		for (u8 speed, gate, i = 0; i < 4; i++)
		{
			gate = u8ModeTime_GateSet_Tem[i];
			speed = FunMode_GateToSpeed(gate, fg_ModeTimeUnit_bef);
			gate = FunMode_SpeedToGate(speed, fg_ModeTimeUnit);
			gate = FunMode_UnitRound(gate, fg_ModeTimeUnit);//取整
			gate = FunModeGateLimit(gate, fg_ModeTimeUnit);//限制
			u8ModeTime_GateSet_Tem[i] = gate;
		}
	}
	//定时模式运行时
	else
	{
		for (u8 speed, gate, i = 0; i < 4; i++)
		{
			gate = u8ModeTime_GateSet[i];
			speed = FunMode_GateToSpeed(gate, fg_ModeTimeUnit_bef);
			gate = FunMode_SpeedToGate(speed, fg_ModeTimeUnit);
			gate = FunMode_UnitRound(gate, fg_ModeTimeUnit);
			gate = FunModeGateLimit(gate, fg_ModeTimeUnit);
			u8ModeTime_GateSet[i] = gate;
		}
	}
}

/*********************自吸***********************/
//进入反冲洗模式
void FunModeToNormalWash(void)
{
	if (u8WashState == 0) //不是自学习
	{
		u8WashState = WashState_Normal; //进入反冲洗自吸
		gu8SetWastTm = 10;				//反冲洗设置时间在显示屏的闪烁时间
		WashTmSet = WashTmSet_Normal;	//反冲洗自吸设置时间给予显示屏的冲洗设置时间
		WashTm = WashTmSet;
	}
}
//在反冲洗自吸时，其转速、时间调节、退出
void FunNormalWash_Set(void)
{
	//转速设置
	if ((gu8SetWastTm == 0) || (WashTmSet != 0))
	{
		if (u8SD_ParamSet_HandWashSpd == 0)
			gu16RunSpd = u16LadderSpd[12]; // yxl-5
		else if (u8SD_ParamSet_HandWashSpd == 1)
			gu16RunSpd = u16LadderSpd[13];
		else if (u8SD_ParamSet_HandWashSpd == 2)
			gu16RunSpd = u16LadderSpd[14];
		else if (u8SD_ParamSet_HandWashSpd == 3)
			gu16RunSpd = u16LadderSpd[15];
		else if (u8SD_ParamSet_HandWashSpd == 4)
			gu16RunSpd = u16LadderSpd[16];
		else
			gu16RunSpd = u16LadderSpd[16];
	}
	//时间调节
	if (gu8KeyFlg == bUp)
	{
		gu8SetWastTm = WASH_TWINLETIME; //自吸设置时间改变时在显示屏的闪烁时间
		WashTmSet_Normal += 30;
		if (WashTmSet_Normal > 1500) // 20211210+
		{
			WashTmSet_Normal = 1500;
		}
		WashTmSet = WashTmSet_Normal;
	}
	else if (gu8KeyFlg == bDown)
	{
		gu8SetWastTm = WASH_TWINLETIME; //自吸设置时间改变时在显示屏的闪烁时间
		if (WashTmSet_Normal > 30)
		{
			WashTmSet_Normal -= 30;
		}
		else if (WashTmSet_Normal <= 30) // 20220530+
		{
			WashTmSet_Normal = 0; // yxl-5
		}
		WashTmSet = WashTmSet_Normal;
	}
	else if ((u8fg_SysIsOn == 0)											  //关机
			 || (gu8KeyFlg == bLongWash)									  //长按解锁键
			 || (gu16TripFlg_Driver || gu16TripFlg_Board || gu16TripFlg_Warn) //故障时
			 || (u8OutCtrlState > 0)										  //外部控制
			 || (WashTm == 0)												  //时间结束
	)
	{
		u8WashState = 0; //退出自吸
	}
}
//进入超频自吸模式
void FunModeToSuperWash(void)
{
	if (u8WashState == 0) //不是自学习
	{
		u8WashState = WashState_Super; //进入超频自吸模式
		WashTmSet = 600;
		WashTm = WashTmSet;
		gu8SetWastTm = 10;	//反冲洗设置时间在显示屏的闪烁时间
		fg_rest_con_ok = 1; //需要自学习
	}
}
//在超频自吸时，其转速、时间调节、退出
void FunSuperWash_Set(void)
{
	//转速设置
	gu16RunSpd = 2850;
	//时间调节
	if (gu8KeyFlg == bUp)
	{
		gu8SetWastTm = WASH_TWINLETIME; //自吸设置时间改变时在显示屏的闪烁时间
		WashTmSet += 30;
		if (WashTmSet > 1500)
		{
			WashTmSet = 1500;
		}
	}
	else if (gu8KeyFlg == bDown)
	{
		gu8SetWastTm = WASH_TWINLETIME; //自吸设置时间改变时在显示屏的闪烁时间
		WashTmSet -= 30;
		if (WashTmSet < 600)
		{
			WashTmSet = 600; // yxl-5
		}
	}
	if ((gu16TripFlg_Driver || gu16TripFlg_Board || gu16TripFlg_Warn)) //故障时
	{
		u8WashState = 0; //退出自吸
	}
	else if ((gu8KeyFlg == bLongWash)	//长按
			 || (u8fg_SysIsOn == 0)		//关机
			 || (u8OutCtrlState > 0)	//外部控制
			 || (fg_rest_con_wifi == 1) // wifi//20220105+
	)
	{
		u8WashState = WashState_SelfStudy; //退出自吸,进入自学习
		fg_rest_con_wifi = 0;
	}
	else if (WashTm == 180) //倒计时120s
	{
		u8WashState = WashState_SuperStudy; //退出自吸,进入自学习
		FunModeToSelfStudy();
	}
}
//进入上水自吸模式
void FunModeToUpWash(void)
{
	if (u8WashState == 0 && u8SD_PowerOnTm)
	{
		u8WashState = WashState_UpWater; //进入超频自吸模式
		WashTmSet = u8SD_PowerOnTm * 60;
		WashTm = WashTmSet;
		gu8SetWastTm = WASH_TWINLETIME; // 自吸设置时间在显示屏的闪烁时间
		gu8upwateroverokTm = 0;			// 60/2s=30s判断时间开始计时
	}
}
//在上水自吸时，其退出
void FunUpWash_Set(void)
{
	//转速设置
	gu16RunSpd = 2850;

	//上水结束
	if (WashTm == 0)
	{
		fg_auto_upwater_end = 1; //上水结束
	}
	//上水结束后
	if (fg_auto_upwater_end == 1)
	{
		fg_auto_upwater_end = 0;
		//抽不上水，报自吸失败故障E209
		if (gu16MotorWt < 500 && (u8fg_SysIsOn) && (gu16TripFlg_Driver == 0 && gu16TripFlg_Board == 0))
		{
			SetBit(gu16TripFlg_Board, Owupwaterfail); //自吸失败E209
			count_upwater_iserror++;				  //自吸失败次数（两次以上（包括两次）锁定）
			gu16upwateriserrorTm = 240;				  //第一次上水自吸失败后，等0.5s*240=2min后重新进入上水
		}
		//抽得上水，清除E209
		else if (gu16MotorWt >= 500)
		{
			u8WashState = 0;
			ClrBit(gu16TripFlg_Board, Owupwaterfail); //清除自吸失败E209
			count_upwater_iserror = 0;				  //清除自吸失败次数（两次锁定）
		}
	}
	//功率>=500，上水时间>60s,
	if ((gu16MotorWt >= 500) && (WashTm > 60)) // yxl-e207
	{
		if (gu8upwateroverokTm > 60) // 0.5S*60=30S
		{
			WashTm = 60;
		}
	}
	else
	{
		gu8upwateroverokTm = 0;
	}
}
//进入自学习模式
void FunModeToSelfStudy(void)
{
	if (u8WashState == WashState_SuperStudy)
	{
		u8WashState = WashState_SelfStudy; //进入超频自吸模式
	}
	else
	{
		u8WashState = WashState_SelfStudy; //进入超频自吸模式
		WashTmSet = 180;
		WashTm = WashTmSet;
		gu8SetWastTm = 10; // 自吸设置时间在显示屏的闪烁时间
	}
}
//自学习被打断后需要重新进入
void FunReEnterSelfStudy(void)
{
	if (u8fg_SysIsOn == 1 && u8WashState == 0)
	{
		if (fg_rest_con_ok == 1)
		{
			FunModeToSelfStudy();
		}
	}
}
	 u8 fg_SelfStudy_GetAd = 1;
	u8 count_SelfStudy = 0;
	 struct
	{
		u32 GetTab;
		u32 GetTab_max;
		u32 GetTab_min;
		u32 GetTab_Sum;

	} SelfStudy_Gate;
//在自学习时，设备流量范围学习
void FunSelfStudy_Set(void)
{
	u8 s;

	//转速设置
	gu16RunSpd = u16LadderSpd[16];

	//功能实现
	// 66s前先等待稳定并进行初始化
	if (WashTm >= 66) // 6s/次，共10次，倒数12s最后一次，66=12+6*（10-1）
	{
		fg_SelfStudy_GetAd = 1;
		count_SelfStudy = 0;
		SelfStudy_Gate.GetTab = 0;
		SelfStudy_Gate.GetTab_max = 0;
		SelfStudy_Gate.GetTab_min = 4096;
		SelfStudy_Gate.GetTab_Sum = 0;
	}
	// 66s后开始读取压力进行学习
	else
	{
		if (WashTm % 6)
		{
			fg_SelfStudy_GetAd = 1;
		}
		if (WashTm % 6 == 0 && fg_SelfStudy_GetAd == 1)
		{
			fg_SelfStudy_GetAd = 0;
			count_SelfStudy++;
			SelfStudy_Gate.GetTab = WaterGate_cail_ot_preset; //只留一个转速2600
			for (u8 k = 0; k < 35; k++)
			{
				if ((WaterPress_value >= sp_2600_con_press[k] * 10) && (WaterPress_value < sp_2600_con_press[k + 1] * 10))
				{
					SelfStudy_Gate.GetTab = sp_2600_con_flow[k];
				}
			}

			if (SelfStudy_Gate.GetTab_max < SelfStudy_Gate.GetTab)
				SelfStudy_Gate.GetTab_max = SelfStudy_Gate.GetTab;
			if (SelfStudy_Gate.GetTab_min > SelfStudy_Gate.GetTab)
				SelfStudy_Gate.GetTab_min = SelfStudy_Gate.GetTab;

			SelfStudy_Gate.GetTab_Sum += SelfStudy_Gate.GetTab;
		}
		if (count_SelfStudy >= 10)
		{
			count_SelfStudy = 0;

			SelfStudy_Gate.GetTab_Sum -= SelfStudy_Gate.GetTab_max;
			SelfStudy_Gate.GetTab_Sum -= SelfStudy_Gate.GetTab_min;
			water_max_set = (SelfStudy_Gate.GetTab_Sum >> 3) / 100;
			s = (SelfStudy_Gate.GetTab_Sum >> 3) % 100;
			if (s > 50)
				water_max_set += 1;

			water_min_set = water_max_set * u16LadderSpd[0] / u16LadderSpd[16];

			//流量转换为转速的比例
			if (water_max_set < 5)
				water_max_set = 5;
			if (water_max_set > 50)
				water_max_set = 50; //可以超过25
			if (water_min_set < 3)
				water_min_set = 3;
			if (water_min_set > 10)
				water_min_set = 10;

			//流量转换为转速的比例
			ModeGate_m3h.StudyMax = water_max_set;
			ModeGate_m3h.StudyMin = water_min_set;
			if (ModeGate_m3h.StudyMax < 5)
				ModeGate_m3h.StudyMax = 5;
			if (ModeGate_m3h.StudyMax > 50)
				ModeGate_m3h.StudyMax = 50; //可以超过25
			if (ModeGate_m3h.StudyMin < 3)
				ModeGate_m3h.StudyMin = 3;
			if (ModeGate_m3h.StudyMin > 10)
				ModeGate_m3h.StudyMin = 10;
			ModeGate_Imp.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Imp);
			ModeGate_Imp.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Imp);
			ModeGate_Lmin.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Lmin);
			ModeGate_Lmin.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Lmin);
			ModeGate_US.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Us);
			ModeGate_US.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Us);

			//恢复出厂设置后的第一次自学习会进行流量降速
			if (ValBit(u8FirstFunIn, LowGate))
			{
				ClrBit(u8FirstFunIn, LowGate);
				if (WaterGate_set >= water_max_set) //流量降速到80%
				{
					WaterGate_set = water_max_set * 4 / 5;
					if (WaterGate_set < 5)
						WaterGate_set = 5;
				}
			}
		}
	}

	if (WashTm == 0)
	{
		u8WashState = 0;
		fg_rest_con_ok = 0;
	}
}

//在平稳运行时，设备判断是否需掉水自吸
