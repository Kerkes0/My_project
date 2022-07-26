/***************************** *************************************
 * Module		:	Mode.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2018/7
 *******************************************************************/
#include "Mode.h"
#include "User.h"
#include "LCD.h"
#include "ModeFunc.h"
#include "ErrorFunc.h"
#include "Adc.h"
#include "Uart.h"
#include "Key.h"
#include "string.h"
/******************************************************************
const
******************************************************************/

const u8 u8SpmTempTab[] = {78, 81, 85};

const u8 u8SpmDwRatioTab1[] = {83, 88, 95, 100};

/******************************************************************
variables in project
******************************************************************/
/*****************系统运行************************/
u8 u8fg_SysIsOn = 0;	   //开关机
u8 gu8SetMode = SPEED;	   //通常模式类型
u8 gu8SetMode_Bef = SPEED; //前一刻通常模式类型
u8 gu8SetMode_ago = SPEED; //前一次通常模式类型
u8 u8WashState = 0;		   //自吸类型
u8 u8OutCtrlState = 0;	   //外部控制类型

u8 gu8MotorStopTm = 0xff;

u8 gu8SetTimeTm = 0;
u8 gu8BakLgtDlyTm = 120; // yxl-5
u8 gu8SetTimeQueryTm = 0;
u8 gu8SpmTempDwTm = 0;

uint gu16AcVolt = 220;
uint gu16AcCurrent = 0;
uint gu16DcVolt = 310;
uint gu16PhaseCurrent = 0;
u8 gu8MotorTrip = 0;
uint gu16MotorWt = 0;
u8 gu8SpmTempAvg = 30;
uint gu16MotorRunSpd = 0;
uint gu16RunSpd = 0;
u32 gu16RunSpd_bef = 0;

uint gu16MotorSetSpd = 0; //发送给驱动板的转速
u8 bCompOnCmd = 0;		  //是否有停机故障

//--------------------------------
u8 fg_hand_close = 0; //手动关机标志

//--------------------------------转速模式
u8 u8ModeSpeed_SpeedSet = 36; //转速设定值（26-40）
//--------------------------------流量模式
u16 u16ModeGate_GateSet = 5; //流量设定值

u8 fg_ModeUnit_IsChange = 0; //流量单位是否切换了
//--------------------------------定时模式
u8 gu8SetTimeState = 0;
sint gu16SetNowTm;
uint gs16RunNowTm;
sint SetStartTm[4];		//设置用：定时各时间段的运行开始时间
sint SetEndTm[4];		//设置用：定时各时间段的运行开始时间
sint gs16RunStartTm[4]; //定时各时间段的运行开始时间
sint gs16RunEndTm[4];	//定时各时间段的运行结束时间

u8 u8ModeTime_SpeedSet[4];	   //定时各时间段的转速设定值（26-40）
u8 u8ModeTime_SpeedSet_Tem[4]; //设置用：各转速设定值（26-40）
u16 u8ModeTime_GateSet[4];	   //定时各时间段的流量设定值
u16 u8ModeTime_GateSet_Tem[4]; //设置用：各段流量设定值

u8 gu8SetTimeTmSn = 0;
u8 gu8SetTimeTmNum = 0;
u16 gu8SetTimeFlg = 0;
u8 u8ModeTimeRunSpeed = 0;	//定时模式当前时间段的转速值
uint u8ModeTimeRunGate = 0; //定时模式当前时间段的流量值
u8 gu8SetTimeQuerySn = 0;
u8 gu8TimeRunNum = 0;

u8 gu8BuzzDlyTm = 0; //蜂鸣器鸣叫持续时间
u8 gu8TimeFlg = 0;	 //定时模式时快增快减

u8 gu8SetFlg = 0; //故障不存储

//-------------------------------------预警
u8 gu8AcVoltLmtTm = 1; //是否有预警降速的检测间隔时间

//-------------------------------------驱动板
uint gu16UartRunSpd = 0; //驱动板的转速

//--------------------------------------自检
u8 u8SysTestTm = 0; //系统自检时的时间

//--------------------------------------自检时，对应按键灯的亮灭，0为灭
u8 KeyLed1 = 0;
u8 KeyLed2 = 0;
u8 KeyLed3 = 0;
u8 KeyLed4 = 0;
u8 KeyLed5 = 0;
u8 KeyLed6 = 0;

u8 u8FirstFunIn = 0; //恢复出厂设置后的自学习，会有降速判断

uint WaterGate_set = 0;		//流量模式的流量值（与单位有关）
u8 fg_time_spd_contorl = 0; // 0为转速，定时模式时是流量还是转速控制

u8 fg_slave_tempoverhigh = 0; //从机散热器过热E101
u8 fg_host_tempoverhigh = 0;  //主机散热器过热E101

///////////////////////默认是循环过滤模式（流量）
u8 gu8SetMode_eep = 0;	 // yxl-5//20211115/20211118
u8 fg_rest_con_wifi = 0; // 20220105+

//-----------------------------------------------开机上水
u8 fg_gu8SetMode_laststate_lowwater = 0;
u8 fg_auto_upwater = 0; //上水标志（需删）

u8 gu8upwateroverokTm = 0; //上水超过1min后，可以开始判断是否提前结束

u8 fg_TimeSetSpd = 0;		  //定时停机上水标志
u8 count_upwater_iserror = 0; //上水自吸失败的次数相关
u16 gu16upwateriserrorTm = 0; //上水自吸失败重新进入等待时间

//-------------------------------------------系统部分属性记录时间
TypeSysProTime SysProTime = {0, 0, 0, 0, 0}; //系统部分属性记录时间
u8 SysProState = SysProState_Init;			 //系统的执行状态，是进入初始化还是进入模式运行

//-------------------------------------------自学习

/**
 * @brief  欠压预警
 * @param[in]  {spd}转速
 * @return 欠压后降低的速度
 * @note   AC欠压限速；若无欠压，则返回原来数值，反之，并且大于限速度，则返回限速的数值
 */
uint AcVoltLmt(uint spd)
{
	static u32 gu16SysMaxSpd = 2600; // // u16LadderSpd[16];
	if (gu8AcVoltLmtTm == 0)
	{
		// AC电压正常时，每隔5秒检测一次，无限速（最大转速为100%），清除AC欠压故障
		if (gu16AcVolt > 199)
		{
			gu8AcVoltLmtTm = 10;				 // 500ms*10 = 5s，每隔2秒检测一次
			ClrBit(gu16TripFlg_Warn, Lslowvolt); //清除AC欠压故障AL02
			gu16SysMaxSpd = u16LadderSpd[16];	 //无限速
		}
		// AC电压异常时，每隔120秒检测一次，进行限速，报AC欠压故障
		else
		{
			gu8AcVoltLmtTm = 120;				 // 500ms*120 = 60s，每隔60秒检测一次
			SetBit(gu16TripFlg_Warn, Lslowvolt); //报AC欠压故障AL02
			//进行限速
			if (gu16AcVolt <= 180)				  // yxl-5
				gu16SysMaxSpd = u16LadderSpd[10]; // 1820（70%） //yyxxll
			else if (gu16AcVolt <= 190)
				gu16SysMaxSpd = u16LadderSpd[11]; // 1980（75%）  //yyxxll
			else
				gu16SysMaxSpd = u16LadderSpd[13]; // 2340（85%） //yyxxkk
		}
	}
	//限速后，若当前运行转速大于限速的速度，则当前转速强制降至限速速度
	if (gu16SysMaxSpd < spd)
		spd = gu16SysMaxSpd;

	return (spd);
}

/**
 * @brief  高温预警
 * @param[in]  {spd}转速
 * @return 高温预警后降低的速度
 * @note   高温限速；若无欠压，则返回原来数值，反之，并且大于限速度，则返回限速的数值
 */
uint SpmTempProtect(uint spd)
{
	u8 k;

	static u8 gu8SpmZeroTempAvg = 0;
	static uint sProtectSpd = 870; //高温降速
	static u8 gu8SpmTempDnSn = 0;

	// gu8SpmTempAvg=dgc;

	if (!bCompOnCmd)
	{
		//电机停机500ms*60=30s时
		if (gu8MotorStopTm > 60)
		{
			//温度低于81°C时
			if (gu8SpmTempAvg <= u8SpmTempTab[1]) //温度小于81°C时
			{
				sProtectSpd = spd;
				if (gu8SpmTempOVHCnt < 3)
				{
					fg_host_tempoverhigh = 0;
				}
			}
			//温度高于81°C时
			else
			{
				sProtectSpd = 0;
				fg_host_tempoverhigh = 1;
			}
		}
		else
		{
			sProtectSpd = spd;
		}
	}
	else
	{
		//温度高于85°C时
		if (gu8SpmTempAvg > u8SpmTempTab[2])
		{
			fg_host_tempoverhigh = 1; //散热器过热故障E101
			sProtectSpd = 0;		  //降低后的速度
			gu8SpmTempOVHCnt++;		  //高温停机次数
		}
		//温度高于81°C时
		else if (gu8SpmTempAvg > u8SpmTempTab[1])
		{

			SetBit(gu16TripFlg_Warn, LsHighTemp); //高温预警降速AL01
		}
		//温度低于等于78°C时
		else if (gu8SpmTempAvg <= u8SpmTempTab[0])
		{

			ClrBit(gu16TripFlg_Warn, LsHighTemp); //清除高温预警降速AL01
		}

		if (ValBit(gu16TripFlg_Warn, LsHighTemp))
		{
			if (gu8SpmTempDnSn == 0)
			{
				gu8SpmTempDwTm = 240; // yxl-5  2min
				gu8SpmZeroTempAvg = gu8SpmTempAvg;
				gu8SpmTempDnSn = 1;

				if (spd > u16LadderSpd[13]) // spd > 85%
				{
					if (spd > u16LadderSpd[15])
						sProtectSpd = u16LadderSpd[13];
					else if (spd > u16LadderSpd[14])
						sProtectSpd = u16LadderSpd[12];
					else if (spd > u16LadderSpd[13])
						sProtectSpd = u16LadderSpd[11];
				}
				else if (spd > u16LadderSpd[10]) // spd > 70%
				{
					if (spd > u16LadderSpd[12])
						sProtectSpd = u16LadderSpd[11];
					else if (spd > u16LadderSpd[11])
						sProtectSpd = u16LadderSpd[10];
					else if (spd > u16LadderSpd[10])
						sProtectSpd = u16LadderSpd[9];
				}
				else
				{
					sProtectSpd = spd;
				}
			}
			//降速时，每2min检测一次，判断是否继续降速
			if (gu8SpmTempDwTm == 0)
			{
				//温度对比之前上升
				if (gu8SpmTempAvg > gu8SpmZeroTempAvg)
				{
					gu8SpmTempDwTm = 240; // yxl-5  2min

					gu8SpmZeroTempAvg = gu8SpmTempAvg; // yyxxll  yxl-5  要和上一次做对比

					for (k = 1; k < 17; k++)
					{
						if (sProtectSpd == u16LadderSpd[k]) //修改不能连续降速
							sProtectSpd = u16LadderSpd[k - 1];
					}

					if (spd == u16LadderSpd[0])
						sProtectSpd = u16LadderSpd[0];
				}
				else
				{
					gu8SpmTempDwTm = 240; // 2min
				}
			}
			if (spd < sProtectSpd)
				sProtectSpd = spd;
			if (sProtectSpd < u16LadderSpd[0])
				sProtectSpd = u16LadderSpd[0];
		}
		else
		{
			sProtectSpd = spd;
			gu8SpmTempDnSn = 0;
		}
	}
	return (sProtectSpd);
}

//小时设置时，总分钟数超过1440则-1440，小于0则+1440，主要作用是使得小时数超过24后归0
static sint subMinTime(sint data)
{
	if (data > 1440)
		data -= 1440;
	if (data < 0)
		data += 1440;

	return (data);
}

//设置预约时间段的开始小时，它与其他小时分钟耦合判断
static void subStartTm(schar mode)
{
	u8 i = 0;

	while (i < gu8SetTimeTmNum)
	{
		if (SetStartTm[gu8SetTimeTmNum] < SetEndTm[i])
		{
			if (mode > 0)
				SetStartTm[gu8SetTimeTmNum] = SetEndTm[i];
			if (mode < 0)
				SetStartTm[gu8SetTimeTmNum] = 1440;
		}
		i++;
	}
	if (SetStartTm[gu8SetTimeTmNum] > SetEndTm[gu8SetTimeTmNum])
	{
		SetEndTm[gu8SetTimeTmNum] = SetStartTm[gu8SetTimeTmNum];
	}
}
//设置预约时间段的结束小时，它与其他小时分钟耦合判断
static void subEndTm(schar mode)
{
	u8 i = 0;

	while (i < gu8SetTimeTmNum)
	{
		if (SetStartTm[gu8SetTimeTmNum] < SetEndTm[i])
		{
			SetStartTm[gu8SetTimeTmNum] = SetEndTm[i];
		}
		i++;
	}

	if (SetStartTm[gu8SetTimeTmNum] > SetEndTm[gu8SetTimeTmNum])
	{
		if (mode > 0)
			SetEndTm[gu8SetTimeTmNum] = SetStartTm[gu8SetTimeTmNum];
		if (mode < 0)
			SetEndTm[gu8SetTimeTmNum] = 1440;
	}
	// SetEndTm[gu8SetTimeTmNum] = subMinTime(SetEndTm[gu8SetTimeTmNum]);
}

//点击上或下键，设置定时功能的时钟和预约时间段
static void subKeyUpDownSetTime(const char mode)
{
	sint Min, Hour;
	u8 unit = fg_ModeGateUnit;
	u8 GateDif;

	///============================//yyxxll
	//定时设置中时钟的小时设置
	if (gu8SetTimeState == 1)
	{
		gu16SetNowTm += (sint)(mode)*60;
		if (gu16SetNowTm > 1439)
			gu16SetNowTm -= 1440;
		if (gu16SetNowTm < 0)
			gu16SetNowTm += 1440;
	}
	//定时设置中时钟的分钟设置
	else if (gu8SetTimeState == 2)
	{
		Min = gu16SetNowTm % 60; //之前分钟
		gu16SetNowTm += mode;

		if (mode > 0)
		{
			if (Min + mode > 59)
				gu16SetNowTm -= 60;
		}
		if (mode < 0)
		{
			if (Min + mode < 0)
				gu16SetNowTm += 60;
		}

		// gu16SetNowTm = subMaxTimeM(gu16SetNowTm,mode);
	}
	//定时设置中预约时间段1~4的设置
	else
	{
		//定时设置中预约时间段：gu8SetTimeTmNum的开始小时设置（gu8SetTimeTmNum=0~4，其中0~3为时间段1~4，4为速率/流量设置）
		if (gu8SetTimeTmSn == 0)
		{
			SetStartTm[gu8SetTimeTmNum] += mode * 60;
			SetStartTm[gu8SetTimeTmNum] = subMinTime(SetStartTm[gu8SetTimeTmNum]);
			subStartTm(mode);
		}
		//定时设置中预约时间段：gu8SetTimeTmNum的开始分钟设置（gu8SetTimeTmNum=0~4，其中0~3为时间段1~4，4为速率/流量设置）
		else if (gu8SetTimeTmSn == 1)
		{
			Min = SetStartTm[gu8SetTimeTmNum] % 60;
			Hour = SetStartTm[gu8SetTimeTmNum] / 60;

			SetStartTm[gu8SetTimeTmNum] += mode;

			if (mode > 0)
			{
				if (Min + mode > 59)
					SetStartTm[gu8SetTimeTmNum] -= 60;
				if (Hour > 23)
					SetStartTm[gu8SetTimeTmNum] -= 1440;
			}
			if (mode < 0)
			{
				if (Min + mode < 0)
					SetStartTm[gu8SetTimeTmNum] += 60;
				if (Hour > 23)
					SetStartTm[gu8SetTimeTmNum] -= 60;
			}
			subStartTm(mode);
		}
		//定时设置中预约时间段：gu8SetTimeTmNum的结束小时设置（gu8SetTimeTmNum=0~4，其中0~3为时间段1~4，4为速率/流量设置）
		else if (gu8SetTimeTmSn == 2)
		{
			SetEndTm[gu8SetTimeTmNum] += mode * 60;
			SetEndTm[gu8SetTimeTmNum] = subMinTime(SetEndTm[gu8SetTimeTmNum]);
			subEndTm(mode);
		}
		//定时设置中预约时间段：gu8SetTimeTmNum的结束分钟设置（gu8SetTimeTmNum=0~4，其中0~3为时间段1~4，4为速率/流量设置）
		else if (gu8SetTimeTmSn == 3)
		{
			Min = SetEndTm[gu8SetTimeTmNum] % 60;
			Hour = SetEndTm[gu8SetTimeTmNum] / 60;

			SetEndTm[gu8SetTimeTmNum] += mode;

			if (mode > 0)
			{
				if (Min + mode > 59)
					SetEndTm[gu8SetTimeTmNum] -= 60;
				if (Hour > 23)
					SetEndTm[gu8SetTimeTmNum] -= 1440;
			}
			subEndTm(mode);
		}
		//定时设置中预约时间段：gu8SetTimeTmNum的速率/流量设置（gu8SetTimeTmNum=0~4，其中0~3为时间段1~4，4为速率/流量设置）
		else
		{
			//流量设置
			if (fg_time_spd_contorl == 1) // yxl-5   流量控制
			{
				switch (unit)
				{
				case Unit_m3h:
					GateDif = 1;
					break;
				case Unit_Imp:
					GateDif = 5;
					break;
				case Unit_Lmin:
					GateDif = 20;
					break;
				case Unit_Us:
					GateDif = 5;
					break;
				default:
					unit = Unit_m3h;
					GateDif = 1;
					break;
				}
				if (mode > 0)
					u8ModeTime_GateSet_Tem[gu8SetTimeTmNum] += GateDif;
				else
					u8ModeTime_GateSet_Tem[gu8SetTimeTmNum] -= GateDif;

				u8ModeTime_GateSet_Tem[gu8SetTimeTmNum] = FunModeGateLimit(u8ModeTime_GateSet_Tem[gu8SetTimeTmNum], unit);

				count_5s_dis_realwater = 0;
			}
			//转速设置
			else
			{
				u8 a = _abs(mode);
				if (mode > 0)
					u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] = u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] + a;
				else
					u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] = u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] - a;
				if (u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] > 40)
					u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] = 40;
				if (u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] < 26)
					u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] = 26;
			}
		}
	}
}
//同时按下上、下键，退会上一个时间设置
static void suKeyUp_DownSetTime(void)
{
	if (gu8SetTimeState > 1)
		gu8SetTimeState--;

	if (gu8SetTimeState > 3)
	{
		if (gu8SetTimeState < 8)
			gu8SetTimeTmNum = 0;
		else if (gu8SetTimeState < 13)
			gu8SetTimeTmNum = 1;
		else if (gu8SetTimeState < 18)
			gu8SetTimeTmNum = 2;
		else
			gu8SetTimeTmNum = 3;

		gu8SetTimeTmSn = (gu8SetTimeState - 3) % 5;
	}
}
//定时模式在设置时
void FunModeTimeSet(void)
{
	u8 i;
	//点击定时键，进入下个参数
	if (gu8KeyFlg == bTime)
	{
		gu8SetTimeTm = 20; //定时设置时间刷新

		if (++gu8SetTimeState >= 23)
		{
			gu8SetTimeState = 0;
			gu8SetTimeTm = 0;
			gu8SetTimeTmSn = 0;
		}

		if (gu8SetTimeState < 3)
		{
			SetBit(gu8SetTimeFlg, bNowTime); //在RTC芯片中保存时间
		}
		else
		{
			SetBit(gu8SetTimeFlg, bSetTime); //在E2p芯片中保存时间转速等
		}
		if (gu8SetTimeState > 3)
		{
			if (gu8SetTimeState < 8)
				gu8SetTimeTmNum = 0;
			else if (gu8SetTimeState < 13)
				gu8SetTimeTmNum = 1;
			else if (gu8SetTimeState < 18)
				gu8SetTimeTmNum = 2;
			else
				gu8SetTimeTmNum = 3;

			gu8SetTimeTmSn = (gu8SetTimeState - 3) % 5;
			//保证后一个开始时间不会小于前一个结束时间
			if ((SetStartTm[gu8SetTimeTmNum] != 0) AND(SetEndTm[gu8SetTimeTmNum] != 0))
			{
				i = 0;
				while (i < gu8SetTimeTmNum)
				{
					if (SetStartTm[gu8SetTimeTmNum] < SetEndTm[i])
						SetStartTm[gu8SetTimeTmNum] = SetEndTm[i];
					i++;
				}
				if (SetEndTm[gu8SetTimeTmNum] < SetStartTm[gu8SetTimeTmNum])
					SetEndTm[gu8SetTimeTmNum] = SetStartTm[gu8SetTimeTmNum];
			}
		}
		else
		{
			gu8SetTimeTmSn = 0;
			gu8SetTimeTmNum = 0;
		}
	}
	//点击上键
	else if (gu8KeyFlg == bUp)
	{
		subKeyUpDownSetTime(1);
		gu8SetTimeTm = 20; //定时设置时间刷新
	}
	//长按上键
	else if (gu8KeyFlg == bLongUp)
	{
		if (ValBit(gu8TimeFlg, bTime500ms))
			subKeyUpDownSetTime(2);
		gu8SetTimeTm = 20; //定时设置时间刷新
	}
	//点击下键
	else if (gu8KeyFlg == bDown)
	{
		subKeyUpDownSetTime(-1);
		gu8SetTimeTm = 20; //定时设置时间刷新
	}
	//长按下键
	else if (gu8KeyFlg == bLongDown)
	{
		if (ValBit(gu8TimeFlg, bTime500ms))
			subKeyUpDownSetTime(-2);
		gu8SetTimeTm = 20; //定时设置时间刷新
	}
	//长按上下键，退回上个参数
	else if (gu8KeyFlg == bLongUpDown)
	{
		if (ValBit(gu8TimeFlg, bTime2s))
		{
			suKeyUp_DownSetTime();
			subBuzCtrl(SHORTBE);
		}
		gu8SetTimeTm = 20; //定时设置时间刷新
	}
	//按control键，在第一个转速设置中切换转速或流量
	else if (gu8KeyFlg == bControl)
	{
		if ((gu8SetTimeTmNum == 0) && (gu8SetTimeTmSn == 4))
		{
			fg_time_spd_contorl ^= 1; //进行转速或流量切换
		}
	}
	//长按定时键，保存设置并退出设置
	else if (gu8KeyFlg == bLongTime)
	{
		gu8SetTimeTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime); //在RTC芯片中保存时间
		SetBit(gu8SetTimeFlg, bSetTime); //在E2p芯片中保存时间转速等
	}

	//设置倒数时间前0.5s提前结束并保存
	if (gu8SetTimeTm == 1)
	{
		gu8SetTimeTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime); //在RTC芯片中保存时间
		SetBit(gu8SetTimeFlg, bSetTime); //在E2p芯片中保存时间转速等
	}

	//后面的开始时间永远不小于前面某个结束时间、同一时间段内的结束时间不小于开始时间
	for (i = 1; i < 4; i++)
	{
		for (u8 j = 0; j < i; j++)
		{
			if (SetStartTm[i] < SetEndTm[j])
				SetStartTm[i] = SetEndTm[j];
		}
	}
	for (i = 0; i < 4; i++)
	{
		if (SetStartTm[i] > 1440)
			SetStartTm[i] = 1440;
		if (SetStartTm[i] < 0)
			SetStartTm[i] = 0;
		if (SetEndTm[i] > 1440)
			SetEndTm[i] = 1440;
		if (SetEndTm[i] < 0)
			SetEndTm[i] = 0;
		if (SetStartTm[i] > SetEndTm[i])
		{
			SetEndTm[i] = SetStartTm[i];
		}
		if ((SetStartTm[i] == 1440) && (SetEndTm[i] == 1440))
		{
			SetStartTm[i] = 0;
			SetEndTm[i] = 0;
		}
		if ((SetStartTm[i] == 0) && (SetEndTm[i] == 1440))
		{
			SetStartTm[i] = 0;
			SetEndTm[i] = 0;
		}
	}
	//保存时间段、转速
	if (ValBit(gu8SetTimeFlg, bSetTime))
	{
		ClrBit(gu8SetTimeFlg, bSetTime);
		gs16RunNowTm = gu16SetNowTm;
		for (i = 0; i < 4; i++)
		{
			gs16RunStartTm[i] = SetStartTm[i];
			gs16RunEndTm[i] = SetEndTm[i];
			u8ModeTime_SpeedSet[i] = u8ModeTime_SpeedSet_Tem[i];
			u8ModeTime_GateSet[i] = u8ModeTime_GateSet_Tem[i];
			// u8ModeTime_GateSet_Tem;
		}
	}
}

//设置完时间后，进入定时运行、等待或因无效时间而闪烁
static void subSetTimeCtrl(void)
{
	u8 i, k = 0;

	for (i = 0; i < 4; i++)
	{
		if (gs16RunStartTm[i] == gs16RunEndTm[i])
			k++;
	}
	ClrBit(gu8SetTimeFlg, bInvalid); //存在有效定时段时
	ClrBit(gu8SetTimeFlg, bRun);	 //清除某定时段运行
	ClrBit(gu8SetTimeFlg, bStay);	 //清除某定时段等待
	//全部定时段无效时
	if (k >= 4)
	{
		SetBit(gu8SetTimeFlg, bInvalid); //全部定时段无效时
		u8ModeTimeRunSpeed = 0;
		u8ModeTimeRunGate = 0;
		gu8TimeRunNum = 0;
	}
	//定时段存在有效时
	else
	{
		//定时运行时
		if ((gs16RunNowTm >= gs16RunStartTm[0]) AND(gs16RunNowTm < gs16RunEndTm[0]))
		{
			SetBit(gu8SetTimeFlg, bRun);
			u8ModeTimeRunSpeed = u8ModeTime_SpeedSet[0];
			u8ModeTimeRunGate = u8ModeTime_GateSet[0];
			gu8TimeRunNum = 0;
		}
		else if ((gs16RunNowTm >= gs16RunStartTm[1]) AND(gs16RunNowTm < gs16RunEndTm[1]))
		{
			SetBit(gu8SetTimeFlg, bRun);
			u8ModeTimeRunSpeed = u8ModeTime_SpeedSet[1];
			u8ModeTimeRunGate = u8ModeTime_GateSet[1];
			gu8TimeRunNum = 1;
		}
		else if ((gs16RunNowTm >= gs16RunStartTm[2]) AND(gs16RunNowTm < gs16RunEndTm[2]))
		{
			SetBit(gu8SetTimeFlg, bRun);
			u8ModeTimeRunSpeed = u8ModeTime_SpeedSet[2];
			u8ModeTimeRunGate = u8ModeTime_GateSet[2];
			gu8TimeRunNum = 2;
		}
		else if ((gs16RunNowTm >= gs16RunStartTm[3]) AND(gs16RunNowTm < gs16RunEndTm[3]))
		{
			SetBit(gu8SetTimeFlg, bRun);
			u8ModeTimeRunSpeed = u8ModeTime_SpeedSet[3];
			u8ModeTimeRunGate = u8ModeTime_GateSet[3];
			gu8TimeRunNum = 3;
		}
		//定时运行等待时
		else if (gs16RunNowTm < gs16RunStartTm[0] AND !(gs16RunStartTm[0] == gs16RunEndTm[0])) ///////20211126
		{
			SetBit(gu8SetTimeFlg, bStay);
			u8ModeTimeRunSpeed = 0;
			u8ModeTimeRunGate = 0;
			gu8TimeRunNum = 0;
		}
		else if (gs16RunNowTm < gs16RunStartTm[1] AND !(gs16RunStartTm[1] == gs16RunEndTm[1])) ///////20211126
		{
			SetBit(gu8SetTimeFlg, bStay);
			u8ModeTimeRunSpeed = 0;
			u8ModeTimeRunGate = 0;
			gu8TimeRunNum = 1;
		}
		else if (gs16RunNowTm < gs16RunStartTm[2] AND !(gs16RunStartTm[2] == gs16RunEndTm[2])) ///////20211126
		{
			SetBit(gu8SetTimeFlg, bStay);
			u8ModeTimeRunSpeed = 0;
			u8ModeTimeRunGate = 0;
			gu8TimeRunNum = 2;
		}
		else if (gs16RunNowTm < gs16RunStartTm[3] AND !(gs16RunStartTm[3] == gs16RunEndTm[3])) ///////20211126
		{
			SetBit(gu8SetTimeFlg, bStay);
			u8ModeTimeRunSpeed = 0;
			u8ModeTimeRunGate = 0;
			gu8TimeRunNum = 3;
		}
		else
		{
			SetBit(gu8SetTimeFlg, bStay);
			u8ModeTimeRunSpeed = 0;
			u8ModeTimeRunGate = 0;
			gu8TimeRunNum = 0;
		}
	}
}
//在定时功能且在运行状态时，按上、下键输入后动作
//（定时运行状态下，按上、下键键，查阅各Timer的设置。）
void subSetTimeQuery(void)
{
	if (gu8KeyFlg == bUp)
	{
		if (++gu8SetTimeQuerySn >= 4)
			gu8SetTimeQuerySn = 0;

		gu8SetTimeQueryTm = 20; //定时运行时查询查询时间刷新
	}

	if (gu8KeyFlg == bDown)
	{
		if (gu8SetTimeQuerySn)
			gu8SetTimeQuerySn--;
		else
			gu8SetTimeQuerySn = 3;

		gu8SetTimeQueryTm = 20; //定时运行时查询查询时间刷新
	}

	if (gu8SetTimeQueryTm == 0)
		gu8SetTimeQuerySn = gu8TimeRunNum; //退出查询
}

//按键触发功能后清除
static void subClrKeyBit(void)
{
	gu8KeyFlg = 0;
	gu8TimeFlg = 0;
}
//将显示屏速度传给电机速度（电机速度通过串口通信传给变频器）
void subSpdToMotor(const u32 Spd)
{
	//自检时无转速
	if (SysProState == SysProState_Test)
	{
		gu16MotorSetSpd = 0;
	}
	//老化时100%转速
	else if (SysProState == SysProState_Aging)
	{
		gu16MotorSetSpd = u16LadderSpd[16] / 2;
	}
	//关机时无转速
	else if (u8fg_SysIsOn == 0)
	{
		gu16MotorSetSpd = 0;
	}
	//开机
	else
	{
		if (((Spd == 0) OR(!bCompOnCmd)))
		{
			if ((gu16UartRunSpd > 1000) AND(gu8MotorTrip == 0))
			{
				if (gu16MotorSetSpd < 300)
					gu16MotorSetSpd = 0;
				else
					gu16MotorSetSpd -= 3;
			}
			else
				gu16MotorSetSpd = 0;

			if ((gu16TripFlg_Board == 0) && (gu16TripFlg_Driver == 0)) // yxl-5 低压保护和高温预警这两个故障不停机
			{

				gu16MotorSetSpd = Spd / 2;
			}
		}
		else
		{
			gu16MotorSetSpd = Spd / 2;
		}
	}
}

//自检函数
void subTestMode(void) F
{
	static u8 gu8FastTestSn = 8; //自检显示的数字
	gu8LockKeyTm = 120;			 //按键解锁持续时间
	//有故障时
	if ((gu16TripFlg_Board)OR(gu16TripFlg_Driver)) // yxl-5
	{
		ClrBit(gu16TripFlg_Driver, Inlowflowerr); //无E207干烧保护
		ClrBit(gu16TripFlg_Board, Owupwaterfail); //无E209自吸失败
		gu16TripFlg_Warn = 0;					  //无预警降速
		FunLcdError();							  //故障显示
	}
	//无故障时
	else
	{
		if (u8SysTestTm < 22) // yxl-5
		{
			KeyLed1 = 0;
			KeyLed2 = 0;
			KeyLed3 = 0;
			KeyLed4 = 0;
			KeyLed5 = 0;
			KeyLed6 = 0;
			gu8FastTestSn = 8;
			gu8BakLgtDlyTm = 120;
			FunLcdSysTest_0();
		}
		if (u8SysTestTm >= 22)
		{
			if (gu8KeyFlg == bPower)
			{
				if (KeyLed1 > 0)
					KeyLed1 = 0;
				else
					KeyLed1 = 1;

				gu8FastTestSn = 5;
				gu8BakLgtDlyTm = 4;
			}
			if (gu8KeyFlg == bUnlock)
			{
				if (KeyLed2 > 0)
					KeyLed2 = 0;
				else
					KeyLed2 = 1;

				gu8FastTestSn = 0;
				gu8BakLgtDlyTm = 4;
			}
			if (gu8KeyFlg == bUp)
			{
				if (KeyLed3 > 0)
					KeyLed3 = 0;
				else
					KeyLed3 = 1;

				gu8FastTestSn = 1;
				gu8BakLgtDlyTm = 4;
			}
			if (gu8KeyFlg == bControl)
			{
				if (KeyLed4 > 0)
					KeyLed4 = 0;
				else
					KeyLed4 = 1;

				gu8FastTestSn = 2;
				gu8BakLgtDlyTm = 4;
			}
			if (gu8KeyFlg == bDown)
			{
				if (KeyLed5 > 0)
					KeyLed5 = 0;
				else
					KeyLed5 = 1;

				gu8FastTestSn = 3;
				gu8BakLgtDlyTm = 4;
			}
			if (gu8KeyFlg == bTime)
			{
				if (KeyLed6 > 0)
					KeyLed6 = 0;
				else
					KeyLed6 = 1;

				gu8FastTestSn = 4;
				gu8BakLgtDlyTm = 4;
			}
			if (ValBit(u8OutCtrl_Din, bOutCtrl_Stop))
			{
				gu8FastTestSn = 6;
				gu8BakLgtDlyTm = 4;
			}
			else if (ValBit(u8OutCtrl_Din, bOutCtrl_1))
			{
				gu8FastTestSn = 7;
				gu8BakLgtDlyTm = 4;
			}
			else if (ValBit(u8OutCtrl_Din, bOutCtrl_2))
			{
				gu8FastTestSn = 8;
				gu8BakLgtDlyTm = 4;
			}
			else if (ValBit(u8OutCtrl_Din, bOutCtrl_3))
			{
				gu8FastTestSn = 9;
				gu8BakLgtDlyTm = 4;
			}
			FunLcdSysTest_1(gu8FastTestSn);
		}
		subClrKeyBit(); //清除按键触发
	}
}

void subSetMode(void)
{

	/**************************开关机状态*************************/
	//开机状态：按开关键关机、485关机
	if (u8fg_SysIsOn)
	{
		if ((gu8KeyFlg == bPower))
		{
			u8fg_SysIsOn = 0;  //关机
			fg_hand_close = 1; //  手动关机，不能485开机

			//-------+
			subBuzCtrl(LONGBE);	   //蜂鸣器特殊响（长）
			gu8SetTimeTm = 0;	   //定时设置模式的持续时间
			gu8SetTimeQueryTm = 0; //定时运行时的查询闪烁时间
		}
	}
	//关机状态：按开关键开机、485关机、参数设置
	else
	{
		FunModeShutDown();	  //关机时功能的进入设置
		FunLcdModeShutDown(); //关机时功能的显示
		/***转速****/
		gu16RunSpd = 0;
		/*****自吸相关******/
		u8WashState = 0; //退出自吸模式
		WashTm = 0;
		count_upwater_iserror = 0;				  //上水自吸失败次数清零
		ClrBit(gu16TripFlg_Board, Owupwaterfail); //关机清自吸失败故障E209
	}

	/**************************上水*************************/
	// TIME模式时的停机上水，（暂时放这边）
	if (gu8SetMode == TIME && ValBit(gu8SetTimeFlg, bStay))
	{
		fg_TimeSetSpd = 1;
	}
	else if ((gu8SetMode != TIME) || !ValBit(gu8SetTimeFlg, bStay))
	{
		fg_TimeSetSpd = 0;
	}
	//上水自吸进入
	if (u8fg_SysIsOn == 0							 //开关机
		|| (gu16TripFlg_Driver || gu16TripFlg_Board) //故障(高温降速和低压降速不会触发)
		|| (fg_TimeSetSpd == 1)						 //定时
	)
	{
		fg_gu8SetMode_laststate_lowwater = 0;
	}
	else
	{
		if (fg_gu8SetMode_laststate_lowwater == 0)
		{
			fg_gu8SetMode_laststate_lowwater = 1;
			if (u8SD_PowerOnTm)
			{
				FunModeToUpWash();
			}
		}
	}

	/**************************不在对应模式时的清零*************************/
	if (gu8SetMode != SPEED)
	{
		count_dis_WaterPress_value_tm = 0; //显示压力值的时间
	}
	if (gu8SetMode != CONTROL)
	{
		count_dis_WaterPress_value_tm = 0; //显示压力值的时间
	}
	if (gu8SetMode != TIME)
	{
		gu8SetTimeTm = 0;	   //定时设置模式时间清零
		gu8SetTimeState = 0;   //定时设置第一个参数
		gu8SetTimeQueryTm = 0; //定时运行时的查询闪烁时间
	}

	/**************************各模式运行和显示*************************/
	//开机时
	if (u8fg_SysIsOn)
	{
		//在自吸模式时
		if (u8WashState)
		{
			//在反冲洗自吸时，其转速、时间调节、退出
			if (u8WashState == WashState_Normal)
			{
				FunNormalWash_Set();
			}
			//在超频自吸时，其转速、时间调节、退出、自学习
			else if (u8WashState == WashState_Super)
			{
				FunSuperWash_Set();
			}
			//在上水自吸时
			else if (u8WashState == WashState_UpWater)
			{
				FunUpWash_Set();
			}
			//在自学习时
			else if (u8WashState == WashState_SelfStudy)
			{
				FunSelfStudy_Set();
			}
			//--------------------------LCD显示
			FunLcdModeWash();
		}
		//外部控制触发时
		else if (u8OutCtrlState)
		{
			if (fg_hand_close)
			{
				u8OutCtrlState = 0;
			}
			//转速改变时，亮屏幕
			if (gu16RunSpd_bef != gu16RunSpd)
			{
				gu8BakLgtDlyTm = 120;
			}
			//外部控制关机
			if (u8OutCtrlState & 0xf0) //前4位为1时表示关机
			{
				u8fg_SysIsOn = 0; //关机
			}
			//外部控制开机
			else
			{
				//数字量控制
				if (ValBit(u8OutCtrlState, Ctrl_Din))
				{
					FunModeOutCtrl_Din(); // Din控制时的功能实现
				}
				//模拟量控制
				else if (ValBit(u8OutCtrlState, Ctrl_Analog))
				{
					FunModeOutCtrl_Analog(); //模拟量控制时的功能实现
				}
				// 485控制
				else if (ValBit(u8OutCtrlState, Ctrl_485))
				{
					FunModeOutCtrl_485(); // 485控制时的功能实现
				}
				//--------------------------转速设置
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD显示
				FunLcdOutCtrl(gu16RunSpd);
			}
		}
		//普通模式时
		else if (gu8SetMode)
		{
			//转速模式时
			if (gu8SetMode == SPEED)
			{
				//---------------------------切换
				//点击定时键进入定时模式
				if (gu8KeyFlg == bTime)
				{
					gu8SetMode = TIME;
				}
				//按control键，进入流量控制模式
				else if (gu8KeyFlg == bControl) // YXL-5
				{
					gu8SetMode = CONTROL;
				}
				//点击解锁键进入反冲洗模式
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //进入反冲洗模式
				}
				//长按解锁、切换键进入超频自吸
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //进入超频自吸模式
				}

				//--------------------------设置
				if (gu8KeyFlg == bUp)
				{
					subModeUpDown(1);
				}
				else if (gu8KeyFlg == bDown)
				{
					subModeUpDown(-1);
				}
				else if (gu8KeyFlg == bLongControl)
				{
					count_dis_WaterPress_value_tm = 15; //显示压力值
				}

				//--------------------------转速设置
				if (u8ModeSpeed_SpeedSet > 40)
					u8ModeSpeed_SpeedSet = 40;
				if (u8ModeSpeed_SpeedSet < 26)
					u8ModeSpeed_SpeedSet = 26;
				gu16RunSpd = u16LadderSpd[u8ModeSpeed_SpeedSet - 24];
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD显示
				//无报警降速时
				if (gu16TripFlg_Warn == 0)
				{
					FunLcdModeSpeed(u8ModeSpeed_SpeedSet); //转速模式时的显示
				}
				//报警降速时
				else
				{
					FunLcdWarnSpeed(gu16RunSpd); //转速模式时，有警告降速时，预警代码交替的显示（只有预警时）
				}
			}
			//流量模式时
			else if (gu8SetMode == CONTROL)
			{
				//---------------------------切换
				//点击定时键进入定时模式
				if (gu8KeyFlg == bTime)
				{
					gu8SetMode = TIME;
				}
				//点击切换键进入转速模式
				else if (gu8KeyFlg == bControl) // YXL-5
				{
					gu8SetMode = SPEED;
				}
				//点击解锁键进入反冲洗模式
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //进入反冲洗模式
				}
				//长按解锁、切换键进入超频自吸
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //进入超频自吸模式
				}

				//--------------------------设置
				if (count_5s_dis_realwater >= 10) // yxl-5  可调
				{
					if (WaterGate_set > water_max_set)
						WaterGate_set = water_max_set;
					if (WaterGate_set < water_min_set)
						WaterGate_set = water_min_set;
				}
				if (gu8KeyFlg == bUp)
				{
					FunModeGateUpDown(1, fg_ModeGateUnit);
					count_5s_dis_realwater = 0;
				}
				else if (gu8KeyFlg == bDown)
				{
					FunModeGateUpDown(-1, fg_ModeGateUnit);
					count_5s_dis_realwater = 0;
				}
				else if (gu8KeyFlg == bLongControl)
				{
					count_dis_WaterPress_value_tm = 15; //显示压力值
				}
				else if (gu8KeyFlg == bLongControlUp)
				{
					FunModeGate_UnitSwitch(1);
				}
				else if (gu8KeyFlg == bLongControlDown)
				{
					FunModeGate_UnitSwitch(-1);
				}

				//--------------------------转速设置
				//在单位切换持续影响状态(一般不在)
				if (fg_ModeUnit_IsChange)
				{
					gu16RunSpd = gu16RunSpd_bef;
				}
				//不在单位切换持续影响状态
				else
				{
					gu16RunSpd = u16LadderSpd[16] * u16ModeGate_GateSet / FunModeGate_StudyMax(fg_ModeGateUnit);
				}
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD显示
				//无报警降速时
				if (gu16TripFlg_Warn == 0)
				{
					FunLcdModeGate(u16ModeGate_GateSet, fg_ModeGateUnit); //流量模式时的流量显示
				}
				//报警降速时
				else
				{
					FunLcdWarnGate(gu16RunSpd, fg_ModeGateUnit); //流量模式时，有警告降速时，预警代码交替的显示（只有预警时）
				}
			}
			//定时模式时
			else if (gu8SetMode == TIME)
			{
				//---------------------------切换
				//在定时运行时的长按定时键，返回原来模式
				if ((gu8KeyFlg == bLongTime) AND(gu8SetTimeTm == 0))
				{
					gu8SetMode = gu8SetMode_ago;
					for (u8 i = 0; i < 4; i++)
					{
						SetStartTm[i] = gs16RunStartTm[i];
						SetEndTm[i] = gs16RunEndTm[i];
						u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i];
						u8ModeTime_GateSet_Tem[i] = u8ModeTime_GateSet[i];
					}
				}
				//点击切换键，退回转速或流量模式
				else if (gu8KeyFlg == bControl)
				{
					if ((gu8SetTimeTmSn == 4) && (gu8SetTimeTmNum == 0) && (gu8SetTimeTm != 0))
					{
					}
					else
					{
						gu8SetMode = gu8SetMode_ago;
					}
				}
				//长按切换和上键，在定时为流量运行时，可切换流量单位
				else if (gu8KeyFlg == bLongControlUp)
				{
					if (fg_time_spd_contorl)
					{
						FunModeTime_UnitSwitch(1);
						gu8SetTimeTm = 20; //定时设置时间刷新
					}
				}
				//长按切换和下键，在定时为流量运行时，可切换流量单位
				else if (gu8KeyFlg == bLongControlDown)
				{
					if (fg_time_spd_contorl)
					{
						FunModeTime_UnitSwitch(-1);
						gu8SetTimeTm = 20; //定时设置时间刷新
					}
				}
				//点击解锁键进入反冲洗模式
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //进入反冲洗模式
				}
				//长按解锁、切换键进入超频自吸
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //进入超频自吸模式
				}

				//--------------------------设置
				//进入定时设置
				if (gu8SetTimeTm == 0 && gu8KeyFlg == bTime) //按下定时键
				{
					for (u8 i = 0; i < 4; i++)
					{
						SetStartTm[i] = gs16RunStartTm[i];
						SetEndTm[i] = gs16RunEndTm[i];
						u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i];
						u8ModeTime_GateSet_Tem[i] = u8ModeTime_GateSet[i];
					}
					gu16SetNowTm = gs16RunNowTm;

					gu8SetTimeState = 0; //定时设置第一个参数
					gu8SetTimeTm = 20;	 //进入定时设置后，其持续时间
				}
				//长按定时键退出定时设置（退出定时模式时也同样一样）
				else if (gu8KeyFlg == bLongTime)
				{
					gu8SetTimeTm = 0; //退出定时设置
				}

				//定时设置时，流量设定值暂且不与设定最小值比较设置
				if (gu8SetTimeTm)
				{
					gu8SetTimeQueryTm = 0;		//定时运行时查询查询时间刷新
					count_5s_dis_realwater = 0; //定时设置完了，再限定范围
				}

				//定时设置时
				if (gu8SetTimeTm)
				{
					FunModeTimeSet(); //定时设置时，设置和保存时钟时间、时间段、转速
				}
				//定时运行、等待或查询时
				else
				{
					subSetTimeCtrl();  //设置完时间后，进入定时运行或因无效时间而闪烁，相关转速运行
					subSetTimeQuery(); //定时运行状态下，按上、下键键，查阅各Timer的设置。
				}

				//--------------------------转速设置
				if (ValBit(gu8SetTimeFlg, bInvalid))
				{
					gu16RunSpd = u16LadderSpd[10]; // 定时都无效时，强制70%转速运行
				}
				else if (ValBit(gu8SetTimeFlg, bRun))
				{
					if (fg_time_spd_contorl == 0) //转速运行
					{
						gu16RunSpd = u16LadderSpd[u8ModeTimeRunSpeed - 24];
					}
					else //流量运行
					{
						if (fg_ModeUnit_IsChange)
						{
							gu16RunSpd = gu16RunSpd_bef;
						}
						//不在单位切换持续影响状态
						else
						{
							u16 studyMax = FunModeGate_StudyMax(fg_ModeTimeUnit);
							gu16RunSpd = u16LadderSpd[16] * u8ModeTimeRunGate / studyMax;
						}
					}
				}
				else if (ValBit(gu8SetTimeFlg, bStay))
				{
					gu16RunSpd = 0;
				}
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);

				//--------------------------LCD显示
				//无报警降速时
				if (gu16TripFlg_Warn == 0)
				{
					//定时设置时
					if (gu8SetTimeTm)
					{
						FunLcdModeTime_Set();
					}
					//定时运行时
					else
					{
						if (ValBit(gu8SetTimeFlg, bInvalid))
							FunLcdModeTime_Invalid();
						else if (ValBit(gu8SetTimeFlg, bRun))
							FunLcdModeTime_Run();
						else if (ValBit(gu8SetTimeFlg, bStay))
							FunLcdModeTime_Stay();
						else
							;
					}
				}
				//报警降速时
				else
				{
					gu8SetTimeTm = 0; //无法进入定时设置
					if (ValBit(gu8SetTimeFlg, bInvalid))
					{
						FunLcdWarnSpeed(gu16RunSpd);
					}

					else if (ValBit(gu8SetTimeFlg, bRun))
					{
						if (fg_time_spd_contorl == 0)
							FunLcdWarnSpeed(gu16RunSpd);
						else
							FunLcdWarnGate(gu16RunSpd, fg_ModeGateUnit);
					}
					else if (ValBit(gu8SetTimeFlg, bStay))
						if (fg_time_spd_contorl == 0)
							FunLcdWarnSpeed(0);
						else
							FunLcdWarnGate(0, fg_ModeGateUnit);
					else
						;
				}
			}
			//防错
			else
			{
				gu8SetMode = SPEED;
			}
		}
		//防错
		else
		{
			gu8SetMode = SPEED;
		}
	}
	//关机时
	else
	{
	}
	/**************************故障相关*************************/
	//出现故障时恢复
	subMotorTrip();
	//出现故障时
	if ((gu16TripFlg_Board) || (gu16TripFlg_Driver))
	{
		bCompOnCmd = 0;			   //不可以运行
		u8WashState = 0;		   //退出自吸
		gu16RunSpd = 0;			   //转速为0，停机
		memset(gu8DsipNum, 0, 16); //其他清除显示，准备显示故障
		//开机时显示
		if (u8fg_SysIsOn)
		{
			FunLcdError(); //故障显示
		}
	}
	//无故障时
	else
	{
		bCompOnCmd = 1;		//可以运行
		gu8TripQueryTm = 0; //故障闪烁显示时间
	}

	/**************************系统内其他功能*************************/
	FunReEnterSelfStudy(); //自学习被打断后需要重新进入
	FunLcdOther();		   // WIFI、自学习时单位闪烁、
	FunKeyLockLight();	   //按键锁定和解锁时亮的个数
	FunLcdbrightness();	   //背光和按键亮度
	FunModeRemeber();	   //设备模式记忆
	FunErrorRemeber();	   //系统故障记忆
	subClrKeyBit();		   //清除按键触发
	gu16RunSpd_bef = gu16RunSpd;
	subSpdToMotor(gu16RunSpd); //将显示屏转速存入驱动板串口发送缓冲
}

/******************************************************************
Name		:FunSysProRun
Input		:
Output	:
Comments:
******************************************************************/
//系统执行模式：老化、自检、通常
u8 fg_atext485Error = 1;
void FunSysProRun(void)
{
	//进入自检
	if (SysProState == SysProState_Test)
	{
		//判断是否出现外部485通讯故障（E206）
		if (gu8MonitComErrTm >= 30 && fg_atext485Error)
		{
			SetBit(gu16TripFlg_Board, Ow485error); //出现外部485通讯故障（E206）//屏蔽自检模式的外部485通讯故障（E206）时将此行注销
		}
		else
		{
			ClrBit(gu16TripFlg_Board, Ow485error); //没有出现外部485通讯故障（E206）
		}

		subTestMode(); //自检时按键触发和液晶显示

		FunLcdbrightness(); //背光和按键亮度
		subMotorTrip();		//故障恢复相关
		subSpdToMotor(0);	//自检时速度0
	}
	//进入老化
	else if (SysProState == SysProState_Aging)
	{
		FunLcdSysAging();				 //系统的老化显示
		subSpdToMotor(u16LadderSpd[16]); //老化时速度100%
	}
	//正常进入
	else if (SysProState == SysProState_Nol)
	{
		subSetMode(); //正常时的功能运行
	}
	//防错用
	else
	{
		subSetMode(); //正常时的功能运行
	}
}

//系统接通电源后前8s初始化
void FunSysProInit(void)
{
	static u8 count_TestPress = 0;		   //进入自检的按键次数
	ClrBit(gu16TripFlg_Board, Ow485error); //正常进入不报E206
	if (SysProTime.PowerTm <= 20)
	{
		//------------------点击3次解锁键，进入自检
		if (gu8KeyFlg == bUnlock && SysProTime.PowerTm < 10)
		{
			gu8KeyFlg = 0;
			count_TestPress++;
		}
		else if (SysProTime.PowerTm >= 10)
		{
			count_TestPress = 0;
		}
		if (count_TestPress >= 3)
		{
			subBuzCtrl(TESTBE);
			u8SysTestTm = 0;				//自检时间开始
			SysProState = SysProState_Test; //进入自检模式
		}
		//------------------接收到驱动板老化标志
		if (fg_ageing_ok)
		{
			SysProState = SysProState_Aging; //进入老化模式
		}
		//------------------正常时
		if (SysProTime.PowerTm < 6)
		{
			//		FunSysParamInit();	  //初始化变量（一般是接通电源后）

			FunLcdSysUpPowerInit_0(); //全显示
			fg_light_pwm = 0;		  //全亮
		}
		else if (SysProTime.PowerTm < 12)
		{
			FunLcdSysUpPowerInit_1(); //显示机型码、拨码、压力值
			gu8DsipNum[16] = 0xff;	  //按键灯全显
			fg_light_pwm = 0;		  //全亮
		}
		else if (SysProTime.PowerTm <= 16)
		{
			FunForVersionToSet();	  //根据拨码器的拨码选择版本，设置该版本的默认数值范围
			FunLcdSysUpPowerInit_1(); //显示机型码、拨码、压力值
			gu8DsipNum[16] = 0xff;	  //按键灯全显
			fg_light_pwm = 1;		  //半亮
		}
		else
		{
			SysProState = SysProState_Nol; //进入通常模式
		}
	}
	//防错用
	else
	{
		SysProState = SysProState_Nol; //进入通常模式
	}
	subClrKeyBit(); //清除按键触发
}

/******************************************************************
							The End
******************************************************************/
