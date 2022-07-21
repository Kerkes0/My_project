/******************************************************************
 * Module		:	LCD.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2019/4
 *******************************************************************/
#include "LCD.h"
#include "User.h"
#include "string.h"
#include "IC74HC595.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Adc.h"
#include "LcdFunc.h"
#include "Key.h"
/****命令宏定义****/
u8 TwinkleTm = 0; // 0.5s闪烁时间

/******命令宏定义******/

uchar gu8SetWastTm; //反冲洗设置时间在显示屏的闪烁时间

u32 u16LadderSpd_rang = 65;
uint u16LadderSpd[] = {
	850,  // 0//yxl-30%
	850,  // 1
	850,  // 2  30%
	910,  // 3  35%
	1040, // 4  40%
	1170, // 5  45%
	1300, // 6  50%
	1430, // 7  55%
	1560, // 8  60%
	1690, // 9  65%
	1820, // 10 70%
	1950, // 11 75%
	2080, // 12 80%
	2210, // 13 85%
	2340, // 14	90%
	2470, // 15 95%//yxl  功率不稳定
	2600  // 16 100%

};

/******************************************************************
variables in project
******************************************************************/
uint gu16TripFlg_Board = 0; //驱动故障
uint gu16TripFlg_Board_Bef = 0;
uint gu16TripFlg_Driver = 0; //显示板故障
uint gu16TripFlg_Driver_bef = 0;
uint gu16TripFlg_Warn = 0; //降速故障
uint gu16TripFlg_Warn_bef = 0;

uchar gu8DsipNum[17]; //显存
uchar gu8SetTimeFlashCnt = 0;

uchar gu8LcdInitTm = 15; //显示机型码

uchar gu8TripQueryTm = 0;

void subDispRpm(uint data, uchar mode);

IC74HC595tag displayIc = {
	.clk = {GPIOB, GPIO_Pin_13},
	.sck = {GPIOB, GPIO_Pin_14},
	.sda = {GPIOB, GPIO_Pin_12},
	.en = {GPIOB, GPIO_Pin_15} // yxl-5
};

/*************系统接通电源后初始化显示**********************/
//全显示
void FunLcdSysUpPowerInit_0(void)
{
	memset(gu8DsipNum, 0xFF, 17);
}
//显示机型码、拨码、压力值
void FunLcdSysUpPowerInit_1(void)
{

	memset(gu8DsipNum, 0, 17);
	//机型码
	if (Modcode[0] == 'A')
		gu8DsipNum[15] = u8LcdDispTab1[10];
	else if (Modcode[0] == 'H')
		gu8DsipNum[15] = u8LcdDispTab1[16];
	else
		gu8DsipNum[15] = u8LcdDispTab1[22];

	if (Modcode[1] == 'c')
		gu8DsipNum[4] = u8LcdDispTab1[12];
	else if (Modcode[1] == 'd')
		gu8DsipNum[4] = u8LcdDispTab1[13];
	else
		gu8DsipNum[4] = u8LcdDispTab1[22];

	if (Modcode[2] == '0')
		gu8DsipNum[5] = u8LcdDispTab1[0];
	else if (Modcode[2] == '1')
		gu8DsipNum[5] = u8LcdDispTab1[1];
	else
		gu8DsipNum[5] = u8LcdDispTab1[22];

	if (Modcode[3] == '7')
		gu8DsipNum[6] = u8LcdDispTab1[7];
	else if (Modcode[3] == '1')
		gu8DsipNum[6] = u8LcdDispTab1[1];
	else if (Modcode[3] == '5')
		gu8DsipNum[6] = u8LcdDispTab1[5];
	else if (Modcode[3] == '8')
		gu8DsipNum[6] = u8LcdDispTab1[8];
	else
		gu8DsipNum[6] = u8LcdDispTab1[22];

	//拨码
	if ((switch_key & 0x01) == 0x01)
		gu8DsipNum[7] = u8LcdDispTab1[1];
	else
		gu8DsipNum[7] = u8LcdDispTab1[0];
	if ((switch_key & 0x02) == 0x02)
		gu8DsipNum[8] = u8LcdDispTab1[1];
	else
		gu8DsipNum[8] = u8LcdDispTab1[0];
	if ((switch_key & 0x04) == 0x04)
		gu8DsipNum[9] = u8LcdDispTab1[1];
	else
		gu8DsipNum[9] = u8LcdDispTab1[0];
	if ((switch_key & 0x08) == 0x08)
		gu8DsipNum[10] = u8LcdDispTab1[1];
	else
		gu8DsipNum[10] = u8LcdDispTab1[0];
	if ((switch_key & 0x10) == 0x10)
		gu8DsipNum[11] = u8LcdDispTab1[1];
	else
		gu8DsipNum[11] = u8LcdDispTab1[0];
	if ((switch_key & 0x20) == 0x20)
		gu8DsipNum[12] = u8LcdDispTab1[1];
	else
		gu8DsipNum[12] = u8LcdDispTab1[0];
	if ((switch_key & 0x40) == 0x40)
		gu8DsipNum[13] = u8LcdDispTab1[1];
	else
		gu8DsipNum[13] = u8LcdDispTab1[0];
	if ((switch_key & 0x80) == 0x80)
		gu8DsipNum[14] = u8LcdDispTab1[1];
	else
		gu8DsipNum[14] = u8LcdDispTab1[0];

	//当前压力值
	static u16 Temp_value_sensor_lcdinit = 0; //显示压力值
	static u8 fg_FirstIn = 1;
	if (fg_FirstIn)
	{
		fg_FirstIn = 0;
		Temp_value_sensor_lcdinit = AD_fin_buf;
	}
	subDispWt(Temp_value_sensor_lcdinit, 1);
}

/*************系统发送故障**************************/
u32 gu16TripNum = 0;
//显示故障时间，3s切换下一个
void subDispTrip(void)
{
	static uchar u8LcdTripSn = 0;

	switch (u8LcdTripSn)
	{
	case 0: //输入电压异常E001
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Involterror))
		{
			gu16TripNum = 001;
			gu8TripQueryTm = 60;
			break;
		}
	case 1: //输出电流超过限值E002
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Incurroverh))
		{
			gu16TripNum = 002;
			gu8TripQueryTm = 60;
			break;
		}
	case 2: //散热器过热E101
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, IpmTempoverh))
		{
			gu16TripNum = 101;
			gu8TripQueryTm = 60;
			break;
		}
	case 3: //散热器传感器故障E102
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Ipmnotemp))
		{
			gu16TripNum = 102;
			gu8TripQueryTm = 60;
			break;
		}
	case 4: //主控驱动故障E103
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Insysdriverr))
		{
			gu16TripNum = 103;
			gu8TripQueryTm = 60;
			break;
		}
	case 5: //电机缺相保护E104
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotmisitem))
		{
			gu16TripNum = 104;
			gu8TripQueryTm = 60;
			break;
		}
	case 6: //交流电流采样电路故障E105
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Incurrerror))
		{
			gu16TripNum = 105;
			gu8TripQueryTm = 60;
			break;
		}
	case 7: // DC电压异常E106
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Indccurrer))
		{
			gu16TripNum = 106;
			gu8TripQueryTm = 60;
			break;
		}
	case 8: // PFC保护E107
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inpfcprot))
		{
			gu16TripNum = 107;
			gu8TripQueryTm = 60;
			break;
		}
	case 9: //电机功率超载E108
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotoverload))
		{
			gu16TripNum = 108;
			gu8TripQueryTm = 60;
			break;
		}
	case 10: //电机电流检测电路故障E201
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotcurrerr))
		{
			gu16TripNum = 201;
			gu8TripQueryTm = 60;
			break;
		}
	case 11: //主控制EEPROM读写故障E202
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Insyseepeer))
		{
			gu16TripNum = 202;
			gu8TripQueryTm = 60;
			break;
		}
	case 12: // RTC时钟读写故障E203
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Owrtcerror))
		{
			gu16TripNum = 203;
			gu8TripQueryTm = 60;
			break;
		}
	case 13: //键盘EEPROM读写故障E204
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Oweeperror))
		{
			gu16TripNum = 204;
			gu8TripQueryTm = 60;
			break;
		}
	case 14: //键盘与主控制通信故障E205
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inuarterr))
		{
			gu16TripNum = 205;
			gu8TripQueryTm = 60;
			break;
		}
	case 15: //外部485通讯故障E206
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Ow485error))
		{
			gu16TripNum = 206;
			gu8TripQueryTm = 60;
			break;
		}
	case 16: //干烧保护E207
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inlowflowerr))
		{
			gu16TripNum = 207; // yxl-e207
			gu8TripQueryTm = 60;
			break;
		}
	case 17: //传感器故障E208
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Ownowaterpr))
		{
			gu16TripNum = 208;
			gu8TripQueryTm = 60;
			break;
		}
	case 18: //自吸失败E209
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Owupwaterfail))
		{
			gu16TripNum = 209; // yxl-e207
			gu8TripQueryTm = 60;
			break;
		}
	case 19: // 高温预警降速AL01
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Warn, LsHighTemp)) // 高温预警降速
		{
			gu16TripNum = 901;
			gu8TripQueryTm = 60;
			break;
		}
	case 20: // AC欠压降速AL02
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Warn, Lslowvolt))
		{
			gu16TripNum = 902;
			gu8TripQueryTm = 60;
			break;
		}
	case 21: //防冻保护AL03
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Warn, Wnfrostpro))
		{
			gu16TripNum = 903;
			gu8TripQueryTm = 60;
			break;
		}
	default:
		u8LcdTripSn = 0;
		break;
	}
}
//故障显示(3s一个)
void subDispTripFlash(void)
{
	subDispRpm(gu16TripNum, 0);
	if (gu16TripNum < 300)
	{
		gu8DsipNum[15] = u8LcdDispTab1[14]; // E
	}
	else
	{
		gu8DsipNum[15] = u8LcdDispTab1[10]; // A
		gu8DsipNum[4] = u8LcdDispTab1[17];	// L
	}
}
//停机时故障的显示(包括预警代码)
void FunLcdError(void)
{
	// 3秒一个故障代码，闪烁显示3次
	if ((gu16TripFlg_Board)OR(gu16TripFlg_Driver)) // yxl-5
	{
		if (gu8TripQueryTm EQU 0)
			subDispTrip();
		else if ((gu8TripQueryTm % 20) > 10)
			subDispTripFlash();
	}
}

//转速模式时，有警告降速时，预警代码交替的显示（只有预警时）
void FunLcdWarnSpeed(const u32 spd)
{
	u32 data;
	static u8 Warn_TwinkleTm_bef = 0xff;
	static u8 count_WarnTwinkle = 0;
	u8 spdset;
	//开始报警时，进行初始化
	if (gu16TripFlg_Warn_bef == 0 && gu16TripFlg_Warn)
	{
		count_WarnTwinkle = 0;
	}
	//每0.5s，次数+1
	if (Warn_TwinkleTm_bef != TwinkleTm)
	{
		Warn_TwinkleTm_bef = TwinkleTm;
		count_WarnTwinkle++;
		if (count_WarnTwinkle > 18)
			count_WarnTwinkle = 0;
	}

	spdset = FunMode_SpeedToPercent(spd);
	if (TwinkleTm % 2)
	{
		//显示转速
		if (count_WarnTwinkle <= 6)
		{
			if (spdset >= 26)
			{
				data = 30 + 5 * (spdset - 26);
				if (data > 100)
					data = 100;
				if (data < 30)
					data = 30;
			}
			else if (spdset == 0)
			{
				data = 0;
			}
			subDispRpm(data, 1);
			gu8DsipNum[5] |= One_SegFig; //%单位
		}
		//显示AL01
		if (count_WarnTwinkle > 6 && count_WarnTwinkle <= 12)
		{
			if (ValBit(gu16TripFlg_Warn, LsHighTemp))
			{
				memset(gu8DsipNum, 0x00, 16);
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[1];	// 1
			}
			else
			{
				count_WarnTwinkle = 13;
			}
		}
		//显示AL02
		if (count_WarnTwinkle > 12 && count_WarnTwinkle <= 18)
		{
			if (ValBit(gu16TripFlg_Warn, Lslowvolt))
			{
				memset(gu8DsipNum, 0x00, 16);
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[2];	// 2
			}
			else
			{
				count_WarnTwinkle = 19;
			}
		}
		//显示AL03
		if (count_WarnTwinkle > 18 && count_WarnTwinkle <= 24)
		{
			if (ValBit(gu16TripFlg_Warn, Wnfrostpro))
			{
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[3];	// 2
			}
			else
			{
				count_WarnTwinkle = 0;
			}
		}
	}
	//显示压力值
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//正常显示功率
	else
	{
		subDispWt(gu16MotorWt, 1);	 //功率
		gu8DsipNum[1] |= One_SegFig; //功率W图标
	}
}
//流量模式时，有警告降速时，预警代码交替的显示（只有预警时）
void FunLcdWarnGate(const u32 spd, const u8 unit)
{
	static u8 Warn_TwinkleTm_bef = 0xff;
	static u8 count_WarnTwinkle = 0;
	u32 gate = 0;
	//开始报警时，进行初始化
	if (gu16TripFlg_Warn_bef == 0 && gu16TripFlg_Warn)
	{
		count_WarnTwinkle = 0;
	}
	//每0.5s，次数+1
	if (Warn_TwinkleTm_bef != TwinkleTm)
	{
		Warn_TwinkleTm_bef = TwinkleTm;
		count_WarnTwinkle++;
		if (count_WarnTwinkle > 18)
			count_WarnTwinkle = 0;
	}

	gate = FunMode_SpeedToGate(spd, unit);

	if (TwinkleTm % 2)
	{
		//显示转速
		if (count_WarnTwinkle <= 6)
		{
			subDispRpm(gate, 1); //流量
			switch (unit)		 //单位
			{
			case Unit_m3h:
				gu8DsipNum[4] |= One_SegFig; // m3/h单位
				break;
			case Unit_Imp:
				gu8DsipNum[6] |= One_SegFig; // IMP gpm单位
				break;
			case Unit_Lmin:
				gu8DsipNum[2] |= One_SegFig; // L/min单位
				break;
			case Unit_Us:
				gu8DsipNum[3] |= One_SegFig; // US gpm单位
				break;
			default:
				break;
			}
		}
		//显示AL01
		if (count_WarnTwinkle > 6 && count_WarnTwinkle <= 12)
		{
			if (ValBit(gu16TripFlg_Warn, LsHighTemp))
			{
				memset(gu8DsipNum, 0x00, 16);
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[1];	// 1
			}
			else
			{
				count_WarnTwinkle = 13;
			}
		}
		//显示AL02
		if (count_WarnTwinkle > 12 && count_WarnTwinkle <= 18)
		{
			if (ValBit(gu16TripFlg_Warn, Lslowvolt))
			{
				memset(gu8DsipNum, 0x00, 16);
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[2];	// 2
			}
			else
			{
				count_WarnTwinkle = 19;
			}
		}
		//显示AL03
		if (count_WarnTwinkle > 18 && count_WarnTwinkle <= 24)
		{
			if (ValBit(gu16TripFlg_Warn, Wnfrostpro))
			{
				gu8DsipNum[15] = u8LcdDispTab1[10]; // A
				gu8DsipNum[4] = u8LcdDispTab1[17];	// L
				gu8DsipNum[5] = u8LcdDispTab1[0];	// 0
				gu8DsipNum[6] = u8LcdDispTab1[3];	// 2
			}
			else
			{
				count_WarnTwinkle = 0;
			}
		}
	}
	//显示压力值
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//正常显示功率
	else
	{
		subDispWt(gu16MotorWt, 1);	 //功率
		gu8DsipNum[1] |= One_SegFig; //功率W图标
	}
}

/***************************************/
//关机功能的使用时的显示
void FunLcdModeShutDown(void)
{
	u32 data;
	//软件版本的显示
	if (u8SD_SoftWareVerTm)
	{
		data = gstCodeTime.Day * 100;
		data += (gstCodeTime.Ver & 0x00ff);
		subDispRpm(data, 0); //
	}
	//参数设置的显示
	else if (u8SD_ParamSetTm)
	{
		subDispWt(u8SD_ParamSetOrder + 1, 1); //功率上的第几项目显示
		if (gu8SetTimeFlashCnt % 2)
		{
			if (u8SD_ParamSetOrder EQU 0) //参数1
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd1 - 26);
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 1) //参数2
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd2 - 26);
				subDispRpm(data, 1);
			}

			if (u8SD_ParamSetOrder EQU 2) //参数3
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd3 - 26);
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 3) // 参数4
			{
				data = 80 + 5 * u8SD_ParamSet_HandWashSpd;
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 4) //参数5
			{
				if (u8fg_SD_ParamSet_AnalogSel == 1)
					subDispRpm(1, 1);
				if (u8fg_SD_ParamSet_AnalogSel == 0)
					subDispRpm(0, 1);
			}
			if (u8SD_ParamSetOrder EQU 5) //参数6
			{
				subDispRpm(u8SD_PowerOnTm, 1);
			}
		}
		// 0、1、2、3显示%单位，4、5、6、7不显示
		if (u8SD_ParamSetOrder <= 3)
		{
			gu8DsipNum[5] |= One_SegFig; //单位%显示
		}
		else
		{
			gu8DsipNum[5] &= ~One_SegFig; //不显示&单位
		}
	}
	//气压校准显示：CAL1
	else if (u8SD_ReStoreDisWaterTm)
	{
		gu8DsipNum[6] = u8LcdDispTab1[1];
		gu8DsipNum[5] = u8LcdDispTab1[17];
		gu8DsipNum[4] = u8LcdDispTab1[10];
		gu8DsipNum[15] = u8LcdDispTab1[12];
	}
	//传感器是否开启标志：A--0/1
	else if (u8SD_PSensorDisTm)
	{
		gu8DsipNum[6] = u8LcdDispTab1[u8fg_SD_PSensorOpen];
		gu8DsipNum[5] = u8LcdDispTab1[22];
		gu8DsipNum[4] = u8LcdDispTab1[22];
		gu8DsipNum[15] = u8LcdDispTab1[10];
	}
	// AD校准值显示
	else if (u8SD_PressAdDisTm)
	{
		if (temp_cail_add > 0)
		{
			data = temp_cail_add;
			gu8DsipNum[4] |= One_SegFig; //流量单位
		}
		else if (temp_cail_subtra > 0)
		{
			data = temp_cail_subtra;
			gu8DsipNum[5] |= One_SegFig; //%单位
		}
		else
		{
			data = 0;
			gu8DsipNum[4] &= ~One_SegFig; //
			gu8DsipNum[5] &= ~One_SegFig; //
		}
		subDispRpm(data, 1);
	}
}

/***************************************/
//自吸时的显示
void FunLcdModeWash(void)
{
	subDispWt(gu16MotorWt, 1);	 //功率
	gu8DsipNum[1] |= One_SegFig; //功率W图标
	u32 data;
	// 5s闪烁时间
	if (gu8SetWastTm > 0)
	{
		WashTm = WashTmSet;
		if (TwinkleTm % 2)
		{
			data = WashTmSet;
			subDispRpm(data, 1); //倒数时间
		}
	}
	//闪烁完后开始倒数
	else
	{
		data = WashTm;
		subDispRpm(data, 1); //倒数时间
	}
}

/***************************************/
//外部控制时的显示
void FunLcdOutCtrl(const u32 spd)
{
	subDispWt(gu16MotorWt, 1);	 //功率
	gu8DsipNum[1] |= One_SegFig; //功率W图标
	u32 data = 30;
	if (spd > u16LadderSpd[16])
		data = 100;
	//将转速的数值转换为%单位的数值
	for (u8 j = 0; j < 17; j++)
	{
		if (spd == u16LadderSpd[j])
		{
			data = 20 + 5 * j;
			if (data < 30)
				data = 30; // 30%
		}
	}
	subDispRpm(data, 1);
	gu8DsipNum[5] |= One_SegFig; //%单位
}

/***************************************/
//转速模式时的显示
void FunLcdModeSpeed(const u8 spdset)
{
	u32 data = 30;
	//显示压力值
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//正常显示功率
	else
	{
		subDispWt(gu16MotorWt, 1);	 //功率
		gu8DsipNum[1] |= One_SegFig; //功率W图标
	}
	if (spdset >= 26)
	{
		data = 30 + 5 * (spdset - 26);
		if (data > 100)
			data = 100;
		if (data < 30)
			data = 30;
	}
	else if (spdset == 0)
	{
		data = 0;
	}

	subDispRpm(data, 1);
	gu8DsipNum[5] |= One_SegFig; //%单位
}

//流量模式时的显示
void FunLcdModeGate(const u16 gate, const u8 fg_unit)
{
	//显示压力值
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//正常显示功率
	else
	{
		subDispWt(gu16MotorWt, 1);	 //功率
		gu8DsipNum[1] |= One_SegFig; //功率W图标
	}

	subDispRpm(gate, 1); //流量

	switch (fg_unit)
	{
	case Unit_m3h:
		gu8DsipNum[4] |= One_SegFig; // m3/h单位
		break;
	case Unit_Imp:
		gu8DsipNum[6] |= One_SegFig; // IMP gpm单位
		break;
	case Unit_Lmin:
		gu8DsipNum[2] |= One_SegFig; // L/min单位
		break;
	case Unit_Us:
		gu8DsipNum[3] |= One_SegFig; // US gpm单位
		break;
	default:
		break;
	}
}

//定时模式时的显示：设置时间
void FunLcdModeTime_Set(void)
{
	u32 data;
	//时钟设置显示
	if (gu8SetTimeState < 3)
	{
		//时钟小时闪烁
		if (gu8SetTimeState == 1)
		{
			if (TwinkleTm % 2) //闪烁显示
			{
				subDispNowHour(gu16SetNowTm / 60, 1); //时钟小时
			}
			subDispNowMin(gu16SetNowTm % 60, 1); //时钟分钟
		}
		//时钟分钟闪烁
		else if (gu8SetTimeState == 2)
		{
			subDispNowHour(gu16SetNowTm / 60, 1); //时钟小时
			if (TwinkleTm % 2)					  //闪烁显示
			{
				subDispNowMin(gu16SetNowTm % 60, 1); //时钟分钟
			}
		}
		gu8DsipNum[7] |= One_SegFig; // P1分割图标":"显示
	}
	//时间段时钟
	else
	{
		//流量显示
		if (fg_time_spd_contorl)
		{
			data = u8ModeTime_GateSet_Tem[gu8SetTimeTmNum];
			subDispRpm(data, 1);		  //转速或流量显示
			subDispUnit(fg_ModeGateUnit); //流量单位
		}
		//转速显示
		else
		{
			data = u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum];
			if (data >= 26)
			{
				data = 30 + 5 * (data - 26);
				if (data > 100)
					data = 100;
				if (data < 30)
					data = 30;
			}
			else
			{
				data = 0;
			}
			subDispRpm(data, 1);		 //转速或流量显示
			gu8DsipNum[5] |= One_SegFig; //%单位
		}

		subDispWt(gu16MotorWt, 1);							   //功率显示						  //功率
		gu8DsipNum[1] |= One_SegFig;						   //功率单位					  //功率W图标
		subDispStartHour(SetStartTm[gu8SetTimeTmNum] / 60, 1); //开始小时
		subDispStartMin(SetStartTm[gu8SetTimeTmNum] % 60, 1);  //开始分钟
		subDispEndHour(SetEndTm[gu8SetTimeTmNum] / 60, 1);	   //结束小时
		subDispEndMin(SetEndTm[gu8SetTimeTmNum] % 60, 1);	   //结束分钟
		gu8DsipNum[7] |= One_SegFig;						   // Tm1分割图标":"显示
		gu8DsipNum[10] |= One_SegFig;						   // Tm1、2分割图标"-"显示
		gu8DsipNum[13] |= One_SegFig;						   // Tm2分割图标":"显示

		//下面1-4显示
		if (gu8SetTimeTmNum EQU 0)
			gu8DsipNum[8] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 1)
			gu8DsipNum[9] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 2)
			gu8DsipNum[11] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 3)
			gu8DsipNum[12] |= One_SegFig;

		if (TwinkleTm % 2 == 0) //闪烁显示（特殊，屏蔽后造成闪烁效果）
		{
			if (gu8SetTimeTmSn == 0)
			{
				gu8DsipNum[8] &= 0x01;
				gu8DsipNum[7]&= 0x01;
			}
			else if (gu8SetTimeTmSn == 1)
			{
				gu8DsipNum[10]&= 0x01;
				gu8DsipNum[9]&= 0x01;
			}
			else if (gu8SetTimeTmSn == 2)
			{
				gu8DsipNum[12]&= 0x01;
				gu8DsipNum[11] &= 0x01;
			}
			else if (gu8SetTimeTmSn == 3)
			{
				gu8DsipNum[14]&= 0x01;
				gu8DsipNum[13]&= 0x01;
			}
			else if (gu8SetTimeTmSn == 4)
			{
				gu8DsipNum[15] &= 0x01;
				gu8DsipNum[4]&= 0x01;
				gu8DsipNum[5] &= 0x01;
				gu8DsipNum[6]&= 0x01;
			}
		}
	}
}
//定时模式时的显示：时间段全部无效运行
void FunLcdModeTime_Invalid(void)
{
	subDispWt(gu16MotorWt, 1);	 //功率
	gu8DsipNum[1] |= One_SegFig; //功率W图标
	subDispRpm(70, 1);			 //转速
	gu8DsipNum[5] |= One_SegFig; //%单位

	//开始小时：--
	gu8DsipNum[7] = u8LcdDispTab1[22];
	gu8DsipNum[8] = u8LcdDispTab1[22];
	//开始分钟：--
	gu8DsipNum[9] = u8LcdDispTab1[22];
	gu8DsipNum[10] = u8LcdDispTab1[22];
	//结束小时：--
	gu8DsipNum[11] = u8LcdDispTab1[22];
	gu8DsipNum[12] = u8LcdDispTab1[22];
	//结束分钟：--
	gu8DsipNum[13] = u8LcdDispTab1[22];
	gu8DsipNum[14] = u8LcdDispTab1[22];

	gu8DsipNum[7] |= One_SegFig;  // Tm1分割图标":"显示
	gu8DsipNum[10] |= One_SegFig; // Tm1、2分割图标"-"显示
	gu8DsipNum[13] |= One_SegFig; // Tm2分割图标":"显示
}
//定时模式时的显示：正在运行
void FunLcdModeTime_Run(void)
{
	subDispWt(gu16MotorWt, 1);	 //功率
	gu8DsipNum[1] |= One_SegFig; //功率W图标
	//运行时查询
	if (gu8SetTimeQueryTm)
	{
		//转速运行
		if (fg_time_spd_contorl == 0)
		{
			FunLcdModeSpeed(u8ModeTime_SpeedSet[gu8SetTimeQuerySn]); //包含功率显示
		}
		//流量运行
		else
		{
			FunLcdModeGate(u8ModeTime_GateSet_Tem[gu8SetTimeQuerySn], fg_ModeGateUnit);
		}
		//时钟、数字码等显示
		subDispStartHour(SetStartTm[gu8SetTimeQuerySn] / 60, 1); //开始小时
		subDispStartMin(SetStartTm[gu8SetTimeQuerySn] % 60, 1);	 //开始分钟
		subDispEndHour(SetEndTm[gu8SetTimeQuerySn] / 60, 1);	 //结束小时
		subDispEndMin(SetEndTm[gu8SetTimeQuerySn] % 60, 1);		 //结束分钟
		gu8DsipNum[7] |= One_SegFig;  // Tm1分割图标":"显示
		gu8DsipNum[10] |= One_SegFig; // Tm1、2分割图标"-"显示
		gu8DsipNum[13] |= One_SegFig; // Tm2分割图标":"显示
		//下面1-4显示，闪烁
		if (TwinkleTm % 2)
		{
			if (gu8SetTimeQuerySn EQU 0)
				gu8DsipNum[8] |= One_SegFig;
			else if (gu8SetTimeQuerySn EQU 1)
				gu8DsipNum[9] |= One_SegFig;
			else if (gu8SetTimeQuerySn EQU 2)
				gu8DsipNum[11] |= One_SegFig;
			else if (gu8SetTimeQuerySn EQU 3)
				gu8DsipNum[12] |= One_SegFig;
		}
	}
	//运行不查询
	else
	{
		//转速运行
		if (fg_time_spd_contorl == 0)
		{
			FunLcdModeSpeed(u8ModeTimeRunSpeed); //包含功率显示
		}
		//流量运行
		else
		{
			FunLcdModeGate(u8ModeTimeRunGate, fg_ModeGateUnit);
		}
		//时钟、数字码等显示
		subDispStartHour(SetStartTm[gu8SetTimeTmNum] / 60, 1); //开始小时
		subDispStartMin(SetStartTm[gu8SetTimeTmNum] % 60, 1);  //开始分钟
		subDispEndHour(SetEndTm[gu8SetTimeTmNum] / 60, 1);	   //结束小时
		subDispEndMin(SetEndTm[gu8SetTimeTmNum] % 60, 1);	   //结束分钟
		gu8DsipNum[7] |= One_SegFig;  // Tm1分割图标":"显示
		gu8DsipNum[10] |= One_SegFig; // Tm1、2分割图标"-"显示
		gu8DsipNum[13] |= One_SegFig; // Tm2分割图标":"显示
		//下面1-4显示
		if (gu8SetTimeTmNum EQU 0)
			gu8DsipNum[8] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 1)
			gu8DsipNum[9] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 2)
			gu8DsipNum[11] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 3)
			gu8DsipNum[12] |= One_SegFig;
	}
}
//定时模式时的显示：等待运行
void FunLcdModeTime_Stay(void)
{

	u8 TimeSel = 0;
	u32 data;
	if (gu8SetTimeQueryTm) //等待查询时
		TimeSel = gu8SetTimeQuerySn;
	else //等待不查询时
		TimeSel = gu8SetTimeTmNum;

	//转速运行
	if (fg_time_spd_contorl == 0)
	{
		data = u8ModeTime_SpeedSet_Tem[TimeSel];
		if (data >= 26)
		{
			data = 30 + 5 * (data - 26);
			if (data > 100)
				data = 100;
			if (data < 30)
				data = 30;
		}
		else
		{
			data = 0;
		}
		subDispRpm(data, 1);		 //转速或流量显示
		gu8DsipNum[5] |= One_SegFig; //%单位
	}
	//流量运行
	else
	{
		data = u8ModeTime_GateSet_Tem[TimeSel];
		subDispRpm(data, 1);		  //转速或流量显示
		subDispUnit(fg_ModeGateUnit); //流量单位
	}
	subDispStartHour(SetStartTm[TimeSel] / 60, 1); //开始小时
	subDispStartMin(SetStartTm[TimeSel] % 60, 1);  //开始分钟
	subDispEndHour(SetEndTm[TimeSel] / 60, 1);	   //结束小时
	subDispEndMin(SetEndTm[TimeSel] % 60, 1);	   //结束分钟

	gu8DsipNum[7] |= One_SegFig;  // Tm1分割图标":"显示
	gu8DsipNum[10] |= One_SegFig; // Tm1、2分割图标"-"显示
	gu8DsipNum[13] |= One_SegFig; // Tm2分割图标":"显示
	//下面1-4显示，闪烁
	if (TwinkleTm % 2)
	{
		if (TimeSel EQU 0)
			gu8DsipNum[8] |= One_SegFig;
		else if (TimeSel EQU 1)
			gu8DsipNum[9] |= One_SegFig;
		else if (TimeSel EQU 2)
			gu8DsipNum[11] |= One_SegFig;
		else if (TimeSel EQU 3)
			gu8DsipNum[12] |= One_SegFig;
	}
}

/***************************************/
//系统的自检显示
void FunLcdSysTest_0(void)
{
	memset(gu8DsipNum, 0, 17);
	//在2.5s内，lcd液晶屏闪烁3次，全部数字图标的闪烁显示为数字8
	if (u8SysTestTm < 5) // yxl-5
	{
		if ((u8SysTestTm % 2) EQU 0)
		{
			memset(gu8DsipNum, 0xFF, 17);
		}
	}
	//从3秒开始，每0.5s从第一个数字图标往后依次显示数字8
	else if (u8SysTestTm == 6) // yxl-5
	{
		gu8DsipNum[0] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 7) // yxl-5
	{
		gu8DsipNum[1] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 8) // yxl-5
	{
		gu8DsipNum[2] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 9) // yxl-5
	{
		gu8DsipNum[3] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 10) // yxl-5
	{
		gu8DsipNum[15] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 11) // yxl-5
	{
		gu8DsipNum[4] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 12) // yxl-5
	{
		gu8DsipNum[5] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 13) // yxl-5
	{
		gu8DsipNum[6] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 14) // yxl-5
	{
		gu8DsipNum[7] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 15) // yxl-5
	{
		gu8DsipNum[8] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 16) // yxl-5
	{
		gu8DsipNum[9] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 17) // yxl-5
	{
		gu8DsipNum[10] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 18) // yxl-5
	{
		gu8DsipNum[11] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 19) // yxl-5
	{
		gu8DsipNum[12] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 20) // yxl-5
	{
		gu8DsipNum[13] = u8LcdDispTab1[8];
	}
	else if (u8SysTestTm == 21) // yxl-5
	{
		gu8DsipNum[14] = u8LcdDispTab1[8];

		////20211222 //////开启wifi测试

		mcu_start_wifitest(); // yxl-wifi 开启wifi测试
		WifiTestOk = 0;
		////20211222
	}
}
void FunLcdSysTest_1(const u8 TestNum)
{
	u32 data;
	//显示屏的功率数字部分、转速数字部分、下面的时钟数字部分显示为0
	data = TestNum + TestNum * 10 + TestNum * 100 + TestNum * 1000;
	subDispWt(data, 0);
	subDispRpm(data, 0);
	data = TestNum + TestNum * 10;
	subDispStartHour(data, 0); //开始时间小时的数字
	subDispStartMin(data, 0);  //开始时间分钟的数字
	subDispEndHour(data, 0);   //结束时间小时的数字
	subDispEndMin(data, 0);	   //结束时间分钟	的数字

	if (KeyLed1 > 0)
	{
		gu8DsipNum[16] |= One_Seg_F;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_F;
	}
	if (KeyLed2 > 0)
	{
		gu8DsipNum[16] |= One_Seg_E;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_E;
	}
	if (KeyLed3 > 0)
	{
		gu8DsipNum[16] |= One_Seg_D;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_D;
	}
	if (KeyLed4 > 0)
	{
		gu8DsipNum[16] |= One_Seg_C;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_C;
	}
	if (KeyLed5 > 0)
	{
		gu8DsipNum[16] |= One_Seg_B;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_B;
	}
	if (KeyLed6 > 0)
	{
		gu8DsipNum[16] |= One_Seg_A;
	}
	else
	{
		gu8DsipNum[16] &= ~One_Seg_A;
	}

	//拨码器为10101010时
	if (switch_key == 0x55)
	{
		gu8DsipNum[1] |= One_SegFig; //图标W显示
		gu8DsipNum[0] |= One_SegFig; // yxl-5
		gu8DsipNum[6] |= One_SegFig;
		gu8DsipNum[14] |= One_SegFig; // YXL-5  加灯
		gu8DsipNum[2] |= One_SegFig;
		gu8DsipNum[5] |= One_SegFig; // yxl-5
		gu8DsipNum[3] |= One_SegFig;
		gu8DsipNum[15] |= One_SegFig; // yxl-5 加灯
		gu8DsipNum[4] |= One_SegFig;
		gu8DsipNum[7] |= One_SegFig; // yxl-5
		gu8DsipNum[10] |= One_SegFig;
		gu8DsipNum[13] |= One_SegFig;
	}
	else
	{
		gu8DsipNum[1] &= ~One_SegFig; //图标W显示
		gu8DsipNum[0] &= ~One_SegFig; // yxl-5
		gu8DsipNum[6] &= ~One_SegFig;
		gu8DsipNum[14] &= ~One_SegFig; // YXL-5  加灯
		gu8DsipNum[2] &= ~One_SegFig;
		gu8DsipNum[5] &= ~One_SegFig;
		gu8DsipNum[3] &= ~One_SegFig;
		gu8DsipNum[15] &= ~One_SegFig; // yxl-5  加灯
		gu8DsipNum[4] &= ~One_SegFig;
		gu8DsipNum[7] &= ~One_SegFig;
		gu8DsipNum[10] &= ~One_SegFig;
		gu8DsipNum[13] &= ~One_SegFig;
	}

	/////////////20211222
	////////自检时，若wifi测试成功为1，则wifi图标亮
	if (WifiTestOk == 1)
	{
		gu8DsipNum[0] |= One_SegFig; // yxl-5
	}
	else
	{
		gu8DsipNum[0] &= ~One_SegFig; // yxl-5
	}
	/////////////20211222

	//模拟输入和传感器输入测试
	//模拟输入测试
	if ((Temp_Value_curr_zf10_test > 560) && (Temp_Value_curr_zf10_test < 960))
	{
		gu8DsipNum[8] |= One_SegFig;
		gu8DsipNum[9] &= ~One_SegFig;
	}
	else if ((Temp_Value_curr_zf10_test > 3124) && (Temp_Value_curr_zf10_test < 3524))
	{
		gu8DsipNum[9] |= One_SegFig;
		gu8DsipNum[8] &= ~One_SegFig;
	}
	//传感器输入测试
	if ((Temp_value_test > 545) && (Temp_value_test < 945))
	{
		gu8DsipNum[11] |= One_SegFig;
		gu8DsipNum[12] &= ~One_SegFig;
	}
	else if ((Temp_value_test > 3238) && (Temp_value_test < 3638))
	{
		gu8DsipNum[12] |= One_SegFig;
		gu8DsipNum[11] &= ~One_SegFig;
	}
}
u8 SysAgingTwinTm = 0; //老化闪烁时间，1min交替亮暗
//系统的老化显示
void FunLcdSysAging(void)
{
	memset(gu8DsipNum, 0xff, 17); //全亮
	if (SysAgingTwinTm <= 120)
	{
		fg_light_pwm = 0; //全亮
	}
	else
	{
		fg_light_pwm = 1; //半亮
	}
}

//系统的其他功能显示
void FunLcdOther(void)
{
	//自学习预备时间闪烁显示
	if ((fg_flash_rest_con == 1) && (gu8SetMode > 0))
	{
		if (TwinkleTm % 2) //闪烁显示
		{
			gu8DsipNum[4] &= ~One_SegFig;
			gu8DsipNum[5] &= ~One_SegFig;
			gu8DsipNum[2] &= ~One_SegFig;
			gu8DsipNum[6] &= ~One_SegFig;
			gu8DsipNum[14] &= ~One_SegFig;
			gu8DsipNum[3] &= ~One_SegFig;
			gu8DsipNum[15] &= ~One_SegFig;
		}
	}
	// wifi图标
	if (fg_wifi_led == 1)
	{
		gu8DsipNum[0] |= 0x01;
	}
	else
	{
		gu8DsipNum[0] &= ~0x01; // yxl-5
	}
}
/***************************************/

//不是自检模式时，液晶屏的显示
void FunWriteRam_1729(void)
{
	//按键灯根据gu8DsipNum[16]位的高点亮、低灭掉
	IC74HC595_Write(&displayIc, (gu8DsipNum[16] << 1)); //左移一位是因为前后版本的二极管与595芯片连接刚好左移了一接口
	SendDataToHt1621_Sub(gu8DsipNum);					// LCD液晶屏根据gu8DsipNum的数组数值进行显示
	memset(gu8DsipNum, 0x00, 17);						//清除缓存，准备下一轮显示
}

//解锁时按键灯全亮，锁定时只有解锁键亮
void FunKeyLockLight(void)
{
	//按键解锁时
	if (gu8LockKeyTm > 0)
	{
		gu8DsipNum[16] |= (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F);
	}
	//按键锁定时
	else
	{
		gu8DsipNum[16] |= (One_Seg_E);
	}
}
//背光和按键亮度
void FunLcdbrightness(void)
{
	//全亮
	if (gu8BakLgtDlyTm)
	{
		fg_light_pwm = 0; //全亮
	}
	//半亮
	else
	{
		fg_light_pwm = 1; //低亮度
	}
}
