/******************************************************************
 * Module		:	E2prom.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2018/7
 *******************************************************************/
#include "User.h"
#include "Siic.h"
#include "string.h"
#include "E2prom.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Adc.h"

uchar gu8E2Para[200] = {0};
uchar gu8E2ParaBak[200] = {0};

uchar u8E2RdWaitTm = 10;

uint gu16RomMinSpd = 1200;

// void E2prom_Manage(void);
void Write24c02_Sub(uchar Address, uchar *Ptr, uchar ByteNum);
void Read24c02_Sub(uchar Address, uchar *Ptr, uchar ByteNum);
void FunE2_SysPro(void);

Sciitag At24c02ic = {
	.scl = {GPIOA, GPIO_Pin_5},
	.sda = {GPIOB, GPIO_Pin_2},
	.time_delayus = 100};

/******************************************************************
Name		:Write24c02_Sub
Input		:
Output	:
Comments:
******************************************************************/
void Write24c02_Sub(uchar Address, uchar *Ptr, uchar ByteNum)
{
	Siic_Send(&At24c02ic, 0xA0, Address, Ptr, ByteNum);
}

/******************************************************************
Name		:Read24c02_Sub
Input		:
Output	:
Comments:
******************************************************************/
void Read24c02_Sub(uchar Address, uchar *Ptr, uchar ByteNum)
{
	Siic_Recv(&At24c02ic, 0xA0, Address, Ptr, ByteNum);
}

/******************************************************************
Name		:FunE2_SysInit
Input		:
Output	:
Comments:
******************************************************************/
u16 atexttabm = 0;
u8 u8fg_E2_SysFirstIn = 1; //接通电源或初始化重新进入
//运行前的时钟初始化，只在上电时读取一次
static void FunE2_SysInit(void)
{
	uchar i, sum;
	static u8 E2RdAddress = 0;
	static u8 fg_InCheck = 0;	//是否检测
	static u8 E2CheckOrder = 0; //读取顺序
	static u8 E2CheckCount = 3; //检测次数
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 3; // 1.5s读一次
		fg_InCheck = 1;	  //进入检测
		E2CheckOrder = 0; //读取顺序从头开始
		for (i = 0; i < 200; i++)
			gu8E2Para[i] = 0;
		if (u8fg_E2_SysFirstIn)
		{
			u8fg_E2_SysFirstIn = 0;
			E2CheckCount = 3; //检测次数
		}
	}
	if (fg_InCheck == 1 && E2CheckCount)
	{
		//依次读取10次
		if (E2CheckOrder < 10)
		{
			E2CheckOrder++;
			E2RdAddress = 20 * (E2CheckOrder - 1);
			Read24c02_Sub(E2RdAddress, gu8E2Para + E2RdAddress, 20);
		}
		else if (E2CheckOrder == 10)
		{
			E2CheckOrder++;
			memcpy(gu8E2ParaBak, gu8E2Para, 200);
			E2CheckCount--; //检测次数
			sum = 0;
			for (i = 0; i < 199; i++)
			{
				sum += gu8E2Para[i];
			}
			sum = ~sum;
			if ((gu8E2Para[0] != 0xaa && gu8E2Para[0] != 0xbb) || gu8E2Para[199] != sum)
			{
				// e2芯片第一次安装后上电时全为0xff
				for (u8 i = 0, j = 0; i < 200; i++)
				{
					if (gu8E2Para[i] == 0xff)
						j++;
					if (i == 199 && j == 200)
					{
						fg_InCheck = 0;
						E2CheckCount = 0;
						return;
					}
				}
				//读取错误时
				if (E2CheckCount == 0)
				{
					// SetBit(gu16TripFlg_Board,Oweeperror);
				}
				else
				{
					fg_InCheck = 0;
					return;
				}
			}
			else if (gu8E2Para[0] == 0xbb)
			{
				fg_InCheck = 0;
				//E2CheckCount = 1;
				return;
			}
			else if (gu8E2Para[0] == 0xaa)
			{
				//E2CheckCount =1;
			}
		}
		else
		{
			fg_InCheck = 0;
			E2CheckCount =0;
			/******系统***************/
			u8fg_SysIsOn = gu8E2Para[1]; //开关机
			if (u8fg_SysIsOn)
			{
				gu8LockKeyTm = 120;
				gu8BakLgtDlyTm = 120;
			}
			fg_hand_close = gu8E2Para[2]; //手动关机标志
			if (u8fg_SysIsOn || fg_hand_close > 1)
			{
				fg_hand_close = 0;
			}
			gu8SetMode = gu8E2Para[3]; //当前模式
			if (gu8SetMode == 0 || gu8SetMode > 3)
			{
				gu8SetMode = SPEED;
			}
			gu8SetMode_ago = gu8E2Para[4]; //前一次模式
			if (gu8SetMode_ago == 0 || gu8SetMode_ago > 3)
			{
				gu8SetMode_ago = SPEED;
			}

			/***********转速模式**************************/
			u8ModeSpeed_SpeedSet = gu8E2Para[7]; //转速模式的转速设置量（26-40）
			if (u8ModeSpeed_SpeedSet < 26 || u8ModeSpeed_SpeedSet > 40)
			{
				u8ModeSpeed_SpeedSet = 36;
			}
			/***********流量模式**************************/
			u16ModeGate_GateSet = gu8E2Para[8]; //流量模式的流量设定值
			fg_ModeGateUnit = gu8E2Para[9];		//流量模式的单位
			u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, fg_ModeGateUnit);

			/***********定时模式**************************/
			for (u8 i = 0; i < 4; i++) //定时模式的各设置值
			{
				// 10-37
				gs16RunStartTm[i] = (gu8E2Para[10 + 7 * i] << 8) + gu8E2Para[11 + 7 * i];	  //
				gs16RunEndTm[i] = (gu8E2Para[12 + 7 * i] << 8) + gu8E2Para[13 + 7 * i];		  //
				u8ModeTime_SpeedSet[i] = gu8E2Para[14 + 7 * i];								  //
				u8ModeTime_GateSet[i] = (gu8E2Para[15 + 7 * i] << 8) + gu8E2Para[16 + 7 * i]; //
				if (u8ModeTime_SpeedSet[i] < 26 || u8ModeTime_SpeedSet[i] > 40)
				{
					u8ModeTime_SpeedSet[i] = 36;
				}
				u8ModeTime_GateSet[i] = FunModeGateLimit(u8ModeTime_GateSet[i], fg_ModeGateUnit);
			}
			fg_time_spd_contorl = gu8E2Para[40]; //定时模式的转速流量切换
			if (fg_time_spd_contorl > 1)
			{
				fg_time_spd_contorl = 0;
			}
			fg_ModeTimeUnit = gu8E2Para[41]; //定时模式的单位
			if (fg_ModeTimeUnit > 3)
			{
				fg_ModeTimeUnit = 0;
			}
			/***********参数设置里的参数**************************/
			u8SD_ParamSet_OCtrlSpd1 = gu8E2Para[50];	// Din控制时的转速设置，Din1默认100%
			u8SD_ParamSet_OCtrlSpd2 = gu8E2Para[51];	// Din控制时的转速设置，Din1默认80%
			u8SD_ParamSet_OCtrlSpd3 = gu8E2Para[52];	// Din控制时的转速设置，Din1默认40%
			u8SD_ParamSet_HandWashSpd = gu8E2Para[53];	//手动自吸时的转速设置
			u8fg_SD_ParamSet_AnalogSel = gu8E2Para[54]; // 0为电流，模拟量控制电流电压切换标志位
			u8SD_PowerOnTm = gu8E2Para[55];				// 25为开启，开机是否进入25min上水
			if (u8SD_ParamSet_OCtrlSpd1 < 26 || u8SD_ParamSet_OCtrlSpd1 > 40)
			{
				u8SD_ParamSet_OCtrlSpd1 = 36;
			}
			if (u8SD_ParamSet_OCtrlSpd2 < 26 || u8SD_ParamSet_OCtrlSpd2 > 40)
			{
				u8SD_ParamSet_OCtrlSpd2 = 36;
			}
			if (u8SD_ParamSet_OCtrlSpd3 < 26 || u8SD_ParamSet_OCtrlSpd3 > 40)
			{
				u8SD_ParamSet_OCtrlSpd3 = 36;
			}
			if (u8SD_ParamSet_HandWashSpd > 4)
			{
				u8SD_ParamSet_HandWashSpd = 0;
			}
			if (u8fg_SD_ParamSet_AnalogSel > 1)
			{
				u8fg_SD_ParamSet_AnalogSel = 0;
			}
			if (u8SD_PowerOnTm != 0 && u8SD_PowerOnTm != 25)
			{
				u8SD_PowerOnTm = 25;
			}

			/***********关机时功能*************************/
			fg_air_cali = gu8E2Para[60];		 // 1为触发，是否触发大气压力校准
			u8fg_SD_PSensorOpen = gu8E2Para[61]; //是否开启传感器故障
			if (fg_air_cali > 1)
			{
				fg_air_cali = 0;
			}
			if (u8fg_SD_PSensorOpen > 1)
			{
				u8fg_SD_PSensorOpen = 1;
			}

			/***********自吸和自学习**************************/
			u8 ab[2] = {1,2};
			atexttabm = (ab[0]<<8)+ab[1];
			WashTmSet_Normal = gu8E2Para[65] * 30; //反冲洗自吸的时间设置，记忆用
			for (u8 i = 0; i < 6; i++)			   //各流量的范围
			{
				// 66-113
				*(&ModeGate_m3h.StudyMax + i) = (gu8E2Para[66 + 8 * i] << 8) + gu8E2Para[67 + 8 * i];
				*(&ModeGate_Imp.TabMax + i) = (gu8E2Para[68 + 8 * i] << 8) + gu8E2Para[69 + 8 * i];
				*(&ModeGate_Lmin.TabMax + i) = (gu8E2Para[70 + 8 * i] << 8) + gu8E2Para[71 + 8 * i];
				*(&ModeGate_US.TabMax + i) = (gu8E2Para[72 + 8 * i] << 8) + gu8E2Para[73 + 8 * i];
			}
			temp_cail_add = (gu8E2Para[120] << 8) + gu8E2Para[121];	   // AD校准值：正
			temp_cail_subtra = (gu8E2Para[122] << 8) + gu8E2Para[123]; // AD校准值：负
			fg_rest_con_ok = gu8E2Para[125];						   //自学习标志
			if (temp_cail_add > 150)
			{
				temp_cail_add = 0;
			}
			if (temp_cail_subtra > 150)
			{
				temp_cail_add = 0;
			}

			/********初始化后第一次上电标志记录***************/
			u8FirstFunIn = gu8E2Para[130]; //首次上电需进行功能
		}
	}
}

//将数据写入e29p芯片（只有数据发生变化，才会重新写入）
static void FunE2_SysNol(void)
{
	uchar i, j, sum, len;
	u8 E2RdAddress = 0;
	static u8 u8fg_isWrIn = 0;	//是否有数据更改导致需要重新写入
	static u8 fg_InCheck = 0;	//是否检测
	static u8 E2CheckOrder = 0; //读取顺序
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 6;
		fg_InCheck = 1;	  //进入检测
		E2CheckOrder = 0; //读取顺序
		u8fg_isWrIn = 0;  //是否有数据更改导致需要重新写入
	}
	if (fg_InCheck == 1)
	{
		if (E2CheckOrder == 0)
		{
			E2CheckOrder++;
			j = 0;

			/******系统***************/
			if (gu8E2Para[1] != u8fg_SysIsOn) //开关机
				j++;
			if (gu8E2Para[2] != fg_hand_close) //手动关机标志
				j++;
			if (gu8E2Para[3] != gu8SetMode) //当前模式
				j++;
			if (gu8E2Para[4] != gu8SetMode_ago) //前一次模式
				j++;
			/***********转速模式**************************/
			if (gu8E2Para[7] != u8ModeSpeed_SpeedSet) //转速模式的转速设置量（26-40）
				j++;
			/***********流量模式**************************/
			if (gu8E2Para[8] != u16ModeGate_GateSet) //流量模式的流量设定值
				j++;
			if (gu8E2Para[9] != fg_ModeGateUnit) //流量模式的单位
				j++;
			/***********定时模式**************************/
			for (u8 i = 0; i < 4; i++) //定时模式的各设置值
			{
				// 10-37
				if (((gu8E2Para[10 + 7 * i] << 8) + gu8E2Para[11 + 7 * i]) != gs16RunStartTm[i]) //
					j++;
				if (((gu8E2Para[12 + 7 * i] << 8) + gu8E2Para[13 + 7 * i]) != gs16RunEndTm[i]) //
					j++;
				if (gu8E2Para[14 + 7 * i] != u8ModeTime_SpeedSet[i]) //
					j++;
				if (((gu8E2Para[15 + 7 * i] << 8) + gu8E2Para[16 + 7 * i]) != u8ModeTime_GateSet[i]) //
					j++;
			}
			if (gu8E2Para[40] != fg_time_spd_contorl) //定时模式的转速流量切换
				j++;
			if (gu8E2Para[41] != fg_ModeTimeUnit) //定时模式的单位
				j++;
			/***********参数设置里的参数**************************/
			if (gu8E2Para[50] != u8SD_ParamSet_OCtrlSpd1) // Din控制时的转速设置，Din1默认100%
				j++;
			if (gu8E2Para[51] != u8SD_ParamSet_OCtrlSpd2) // Din控制时的转速设置，Din1默认80%
				j++;
			if (gu8E2Para[52] != u8SD_ParamSet_OCtrlSpd3) // Din控制时的转速设置，Din1默认40%
				j++;
			if (gu8E2Para[53] != u8SD_ParamSet_HandWashSpd) //手动自吸时的转速设置
				j++;
			if (gu8E2Para[54] != u8fg_SD_ParamSet_AnalogSel) // 0为电流，模拟量控制电流电压切换标志位
				j++;
			if (gu8E2Para[55] != u8SD_PowerOnTm) // 25为开启，开机是否进入25min上水
				j++;
			/***********关机时功能*************************/
			if (gu8E2Para[60] != fg_air_cali) // 1为触发，是否触发大气压力校准
				j++;
			if (gu8E2Para[61] != u8fg_SD_PSensorOpen) //是否开启传感器故障
				j++;

			/***********自吸和自学习**************************/
			if (gu8E2Para[65] != (WashTmSet_Normal / 30)) //反冲洗自吸的时间设置，记忆用
				j++;
			for (u8 i = 0; i < 6; i++) //各流量的范围
			{
				// 66-113
				if (((gu8E2Para[66 + 8 * i] << 8) + gu8E2Para[67 + 8 * i]) != *(&ModeGate_m3h.StudyMax + i)) //
					j++;
				if (((gu8E2Para[68 + 8 * i] << 8) + gu8E2Para[69 + 8 * i]) != *(&ModeGate_Imp.TabMax + i)) //
					j++;
				if (((gu8E2Para[70 + 8 * i] << 8 )+ gu8E2Para[71 + 8 * i]) != *(&ModeGate_Lmin.TabMax + i)) //
					j++;
				if (((gu8E2Para[72 + 8 * i] << 8) + gu8E2Para[73 + 8 * i]) != *(&ModeGate_US.TabMax + i)) //
					j++;
			}
			if (((gu8E2Para[120] << 8 + gu8E2Para[121])) != (temp_cail_add)) // AD校准值
				j++;
			if (((gu8E2Para[123] << 8 + gu8E2Para[124])) != (temp_cail_subtra)) // AD校准值
				j++;
			if (gu8E2Para[125] != fg_rest_con_ok) //自学习标志
				j++;

			/********初始化后第一次上电标志记录***************/
			if (gu8E2Para[130] != u8FirstFunIn) //首次上电需进行功能
				j++;

			/*********发生故障时*****************************/
			//使用了gu8E2Para[140]——gu8E2Para[155];特殊：155被用作批次区分标签
			if (ValBit(gu8SetFlg, bTripSave)) //发生故障时立即进行存储
				j++;

			//
			u8fg_isWrIn = j;
		}
		else if ((E2CheckOrder == 1) && u8fg_isWrIn)
		{
			E2CheckOrder++;
			gu8E2Para[0] = 0xaa;

			/******系统***************/
			gu8E2Para[1] = u8fg_SysIsOn;   //开关机
			gu8E2Para[2] = fg_hand_close;  //手动关机标志
			gu8E2Para[3] = gu8SetMode;	   //当前模式
			gu8E2Para[4] = gu8SetMode_ago; //前一次模式

			/***********转速模式**************************/
			gu8E2Para[7] = u8ModeSpeed_SpeedSet; //转速模式的转速设置量（26-40）

			/***********流量模式**************************/
			gu8E2Para[8] = u16ModeGate_GateSet; //流量模式的流量设定值
			gu8E2Para[9] = fg_ModeGateUnit;		//流量模式的单位

			/***********定时模式**************************/
			for (u8 i = 0; i < 4; i++) //定时模式的各设置值
			{
				// 10-37
				gu8E2Para[10 + 7 * i] = (u8)(gs16RunStartTm[i] >> 8);
				gu8E2Para[11 + 7 * i] = (u8)(gs16RunStartTm[i]);
				gu8E2Para[12 + 7 * i] = (u8)(gs16RunEndTm[i] >> 8);
				gu8E2Para[13 + 7 * i] = (u8)(gs16RunEndTm[i]);
				gu8E2Para[14 + 7 * i] = u8ModeTime_SpeedSet_Tem[i];
				gu8E2Para[15 + 7 * i] = (u8)(u8ModeTime_GateSet[i] >> 8);
				gu8E2Para[16 + 7 * i] = (u8)(u8ModeTime_GateSet[i]);
			}
			gu8E2Para[40] = fg_time_spd_contorl; //定时模式的转速流量切换
			gu8E2Para[41] = fg_ModeTimeUnit;	 //定时模式的单位

			/***********参数设置里的参数**************************/
			gu8E2Para[50] = u8SD_ParamSet_OCtrlSpd1;	// Din控制时的转速设置，Din1默认100%
			gu8E2Para[51] = u8SD_ParamSet_OCtrlSpd2;	// Din控制时的转速设置，Din1默认80%
			gu8E2Para[52] = u8SD_ParamSet_OCtrlSpd3;	// Din控制时的转速设置，Din1默认40%
			gu8E2Para[53] = u8SD_ParamSet_HandWashSpd;	//手动自吸时的转速设置
			gu8E2Para[54] = u8fg_SD_ParamSet_AnalogSel; // 0为电流，模拟量控制电流电压切换标志位
			gu8E2Para[55] = u8SD_PowerOnTm;				// 25为开启，开机是否进入25min上水

			/***********关机时功能*************************/
			gu8E2Para[60] = fg_air_cali;		 // 1为触发，是否触发大气压力校准
			gu8E2Para[61] = u8fg_SD_PSensorOpen; //是否开启传感器故障

			/***********自吸和自学习**************************/
			gu8E2Para[65] = WashTmSet_Normal / 30; //反冲洗自吸的时间设置，记忆用
			for (u8 i = 0; i < 6; i++)			   //各流量的范围
			{
				// 66-113
				gu8E2Para[66 + 8 * i] = (u8)((*(&ModeGate_m3h.StudyMax + i)) >> 8);
				gu8E2Para[67 + 8 * i] = (u8)(*(&ModeGate_m3h.StudyMax + i));
				gu8E2Para[68 + 8 * i] = (u8)((*(&ModeGate_Imp.TabMax + i)) >> 8);
				gu8E2Para[69 + 8 * i] = (u8)(*(&ModeGate_Imp.TabMax + i));
				gu8E2Para[70 + 8 * i] = (u8)((*(&ModeGate_Lmin.TabMax + i)) >> 8);
				gu8E2Para[71 + 8 * i] = (u8)(*(&ModeGate_Lmin.TabMax + i));
				gu8E2Para[72 + 8 * i] = (u8)((*(&ModeGate_US.TabMax + i)) >> 8);
				gu8E2Para[73 + 8 * i] = (u8)(*(&ModeGate_US.TabMax + i));
			}
			gu8E2Para[120] = (u8)(temp_cail_add >> 8);	  // AD校准值：正
			gu8E2Para[121] = (u8)(temp_cail_add);		  // AD校准值：正
			gu8E2Para[122] = (u8)(temp_cail_subtra >> 8); // AD校准值：负
			gu8E2Para[123] = (u8)(temp_cail_subtra);	  // AD校准值：负
			gu8E2Para[125] = fg_rest_con_ok;			  //自学习标志

			/********初始化后第一次上电标志记录***************/
			gu8E2Para[130] = u8FirstFunIn; //首次上电需进行功能

			/*********发生故障时,记忆故障*****************************/
			//存储形式为：aabb xxxx ，aa为批次区分标签，bb为故障类型标签，xxxx为故障序号在Main_lib.h内查看
			u8 batch = gu8E2Para[155];
			if (ValBit(gu8SetFlg, bTripSave)) //发生故障时立即进行存储
			{
				ClrBit(gu8SetFlg, bTripSave);
				batch++;
				for (i = 140; i < 154; i++)
				{
					gu8E2Para[i + 1] = gu8E2Para[i];
				}
				if (gu16TripFlg_Driver)
				{
					for (u8 i = 1; i <= 16; i++)
					{
						if (ValBit(gu16TripFlg_Driver, i))
						{
							for (u8 j = 140; j < 154; j++)
							{
								gu8E2Para[j + 1] = gu8E2Para[j];
							}
							gu8E2Para[140] = (batch << 6) + (0 << 4) + i;
						}
					}
				}
				if (gu16TripFlg_Board)
				{
					for (u8 i = 1; i <= 16; i++)
					{
						if (ValBit(gu16TripFlg_Board, i))
						{
							for (u8 j = 140; j < 154; j++)
							{
								gu8E2Para[j + 1] = gu8E2Para[j];
							}
							gu8E2Para[140] = (batch << 6) + (0 << 4) + i;
						}
					}
				}
				if (gu16TripFlg_Warn)
				{
					for (u8 i = 1; i <= 16; i++)
					{
						if (ValBit(gu16TripFlg_Warn, i))
						{
							for (u8 j = 140; j < 154; j++)
							{
								gu8E2Para[j + 1] = gu8E2Para[j];
							}
							gu8E2Para[140] = (batch << 6) + (0 << 4) + i;
						}
					}
				}
				gu8E2Para[155] = batch;
			}
			/************校验码******************************/
			sum = 0;
			for (i = 0; i < 199; i++)
			{
				sum += gu8E2Para[i];
			}
			gu8E2Para[199] = ~sum;
		}
		else if ((E2CheckOrder < 2 + 25) && u8fg_isWrIn)
		{
			//有数据更改导致需要重新写入
			if (u8fg_isWrIn)
			{
				for (u8 i = (E2CheckOrder - 2); i < 25; i++)
				{
					E2CheckOrder++;
					len = 8;
					E2RdAddress = 8 * i;
					if (memcmp(gu8E2ParaBak + E2RdAddress, gu8E2Para + E2RdAddress, len))
					{
						Write24c02_Sub(E2RdAddress, gu8E2Para + E2RdAddress, len);
						memcpy(gu8E2ParaBak + E2RdAddress, gu8E2Para + E2RdAddress, len);
						i++;
						break;
					}
				}
			}
			else
			{
				fg_InCheck = 0;
			}
		}
	}
}
//自检时E2芯片的检测
static void FunE2_SysTest(void)
{
	uchar j, sum, len;
	u8 E2RdAddress = 0;
	static u8 fg_InCheck = 0;		 //是否检测
	static u8 E2CheckOrder = 0;		 //读取顺序
	static u8 E2CheckCount = 3;		 //检测次数
	static u8 E2CheckErrorCount = 0; //异常次数
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 3; // 3s读一次
		fg_InCheck = 1;	  //进入检测
		E2CheckOrder = 0; //读取顺序

		/**************************/
		gu8E2Para[0] = 0xbb;

		for (u8 i = 1; i < 199; i++)
		{
			gu8E2Para[i] = 0x00;
		}
		sum = 0;
		for (u8 i = 0; i < 199; i++)
		{
			sum += gu8E2Para[i];
		}
		gu8E2Para[199] = ~sum;
		/*************接通电源后若没进入自检，则不会=0xfe************/
		if (u8fg_E2_SysFirstIn != 0xfe)
		{
			u8fg_E2_SysFirstIn = 0xfe;
			E2CheckErrorCount = 0;
		}
	}
	if (fg_InCheck)
	{
		if (E2CheckOrder < 25)
		{
			E2CheckOrder++;
			len = 8;
			E2RdAddress = 8 * (E2CheckOrder - 1);
			Write24c02_Sub(E2RdAddress, gu8E2Para + E2RdAddress, len);
		}
		else if (E2CheckOrder == 25)
		{
			E2CheckOrder++;
			for (u8 i = 0; i < 100; i++)
			{
				gu8E2Para[i] = i;
			}
		}
		else if (E2CheckOrder < (25 + 5))
		{
			E2CheckOrder++;
			E2RdAddress = 20 * (E2CheckOrder - 25 - 1);
			Read24c02_Sub(E2RdAddress, gu8E2Para + E2RdAddress, 20);
		}
		else if (E2CheckOrder == 30)
		{
			memcpy(gu8E2ParaBak, gu8E2Para, 200);
			sum = 0;
			j = 0;
			for (u8 i = 1; i < 199; i++)
			{
				if (gu8E2Para[i] != 0x00)
					j++;
			}
			for (u8 i = 0; i < 199; i++)
			{
				sum += gu8E2Para[i];
			}
			sum = ~sum;
			E2CheckCount--;
			if ((gu8E2Para[0] == 0xbb) && (sum == gu8E2Para[199]) && (j == 0))
			{
				ClrBit(gu16TripFlg_Board, Oweeperror);
			}
			else
			{
				if (++E2CheckErrorCount >= 2)
				{
					E2CheckCount = 0;
					SetBit(gu16TripFlg_Board, Oweeperror);
				}
			}
		}
		else
		{
			fg_InCheck = 0;
		}
	}
}

//对e2p芯片进行操作
void FunE2_SysPro(void)
{
	//接通电源后前8s的初始化
	if (SysProState == SysProState_Init)
	{
		FunE2_SysInit(); //读取e2p芯片数据
	}
	//进入自检
	else if (SysProState == SysProState_Test)
	{
		FunE2_SysTest(); //自检时E2芯片的检测
	}
	//进入老化
	else if (SysProState == SysProState_Aging)
	{
	}
	//正常进入
	else if (SysProState == SysProState_Nol)
	{
		FunE2_SysNol(); //将数据写入e29p芯片（只有数据发生变化，才会重新写入）
	}
	//防错
	else
	{
		FunE2_SysNol(); //将数据写入e29p芯片（只有数据发生变化，才会重新写入）
	}
}
