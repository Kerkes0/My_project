/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-19 14:51:20
 * @note:
 */
#include "User.h"
#include "Key.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Uart.h"

#define Time100ms 10 // 50ms
#define Time3sec 500
#define Time3secP 1000
//#define Time5sec				100

// 20211222
#define Time5sec_wifi 1000 // yxl-wifi
#define Time10sec_wifi 1000
#define Time15sec_wifi 2000
// 20211222

#define Time1seclongupdown 100 // yxl-bash//20211211+

uint16_t gu8InCtrlFlg = 0; // yxl-5
uint16_t gu16NoKeyFlg = 0;

uint16_t gu16KeyPressedTm_wifi_ap = 0; // 20211222

u8 gu8KeyFlg = 0; // YXL-5

uint16_t gu16KeyPressedTm = 0; //按键按住持续时间

uint8_t gu8KeyStatusBef = 0;
uint8_t u8DinLFTCnt[4] = {0, 0, 0, 0}; //数字量控制连线持续时间
uint8_t gu8LockKeyTm = 6;			   //按键解锁持续时间
uint8_t gu8PowerOnTm = 0;

uchar fg_selfimbi_ok = 0;

void FunInOut(void);

void subBuzCtrl(uchar i)
{
	//设置蜂鸣器响应时间
	if (!ValBit(gu16NoKeyFlg, bValBuz))
	{
		SetBit(gu16NoKeyFlg, bValBuz);
		gu8BuzzDlyTm = i;
	}
}

u8 u8OutCtrl_Din = 0; // Din控制时触发的项目
static void subDinScan(void)
{
	static u8 Din_bef = 0;
	u8 fg_Din_SelCore = 0; // 1为4芯，4芯5芯选择标志
	//外部控制-数字量-接口状态读取
	if (DIN1_PIN_Rd)
		u8DinLFTCnt[0] = 0;
	else if (u8DinLFTCnt[0] < 0xff)
		u8DinLFTCnt[0]++;

	if (DIN2_PIN_Rd)
		u8DinLFTCnt[1] = 0;
	else if (u8DinLFTCnt[1] < 0xff)
		u8DinLFTCnt[1]++;

	if (DIN3_PIN_Rd)
		u8DinLFTCnt[2] = 0;
	else if (u8DinLFTCnt[2] < 0xff)
		u8DinLFTCnt[2]++;

	if (DIN4_PIN_Rd)
		u8DinLFTCnt[3] = 0;
	else if (u8DinLFTCnt[3] < 0xff)
		u8DinLFTCnt[3]++;

	u8OutCtrl_Din = 0; //清零
	if (u8DinLFTCnt[0] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_Stop);
	else if (u8DinLFTCnt[1] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_1);
	else if (u8DinLFTCnt[2] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_2);
	else if (u8DinLFTCnt[3] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_3);

	//数字量控制时，4芯或5芯选择，默认5芯
	if (switch_key & 0x01) // sw1拨码器的1位拨上时
	{
		fg_Din_SelCore = 1; // 1为4芯控制
	}
	// 4芯控制时
	if (fg_Din_SelCore)
	{
		ClrBit(u8OutCtrl_Din, bOutCtrl_Stop); //无Din1线控制
		//若之前有din控制，下一刻退出时，直接关机
		if (Din_bef && u8OutCtrl_Din == 0)
			u8fg_SysIsOn = 0; //关机
		if (Din_bef != u8OutCtrl_Din)
			Din_bef = u8OutCtrl_Din;
	}

	if (u8OutCtrl_Din)
	{
		SetBit(u8OutCtrlState, Ctrl_Din); //开启Din控制
		if (ValBit(u8OutCtrl_Din, bOutCtrl_Stop))
		{
			SetBit(u8OutCtrlState, Ctrl_DinOff); //关机
		}
	}
	else
	{
		ClrBit(u8OutCtrlState, Ctrl_Din);	 //退出Din控制
		ClrBit(u8OutCtrlState, Ctrl_DinOff); // Din控制开机
	}
}

static void subKeyScan(void)
{
	// static u8 gu8KeyModeCnt = 0;
	//按键状态接收
	uchar u8KeyStatus = 0;

	if (!KEYPOWER_PIN_Rd)
		u8KeyStatus = 0x01;
	if (!KEYMODE_PIN_Rd)
		u8KeyStatus |= 0x02;
	if (!KEYTIME_PIN_Rd)
		u8KeyStatus |= 0x04;
	if (!KEYUP_PIN_Rd)
		u8KeyStatus |= 0x08;
	if (!KEYDOWN_PIN_Rd)
		u8KeyStatus |= 0x10;
	if (!KEYCONTROL_PIN_Rd)
		u8KeyStatus |= 0x20;

	// if(fg_ageing_ok == 1)u8KeyStatus = 0; //老化测试时，按键无效//20211201

	if (u8KeyStatus == NOKEYPRESS) //无按下按键时
	{
		//先前有按过键盘才会触发（松开按键）
		if (ValBit(gu16NoKeyFlg, bPress)) //（bPress=1可能可以等价为按过按键后没有实现过功能）
		{
			ClrBit(gu16NoKeyFlg, bPress);
			if (gu16KeyPressedTm >= Time100ms)
				subBuzCtrl(SHORTBE);

			//（进入自检）正常进入时、在上电时间小于5s
			if (SysProState == SysProState_Init || SysProState == SysProState_Test)
			{
				if (gu16KeyPressedTm >= Time100ms) //短按
				{
					if (gu8KeyStatusBef == PRESS_POWER)
						gu8KeyFlg = bPower;
					if (gu8KeyStatusBef == PRESS_MODE)
						gu8KeyFlg = bUnlock;
					if (gu8KeyStatusBef == PRESS_TIME)
						gu8KeyFlg = bTime;
					if (gu8KeyStatusBef == PRESS_UP)
						gu8KeyFlg = bUp;
					if (gu8KeyStatusBef == PRESS_DOWN)
						gu8KeyFlg = bDown;
					if (gu8KeyStatusBef == PRESS_CONTROL)
						gu8KeyFlg = bControl;
				}
			}
			//在SPEED、TIME或CONTROL模式下，锁屏后按过按键后松键，则点亮lcd背光灯
			else if ((gu8LockKeyTm == 0) AND((gu8SetMode == SPEED) OR(gu8SetMode == TIME) OR(gu8SetMode == CONTROL)))
			{
				if (gu16KeyPressedTm >= Time100ms)
				{
					gu8BakLgtDlyTm = 120; // 500ms*120=60s，当他大于0时，lcd背光灯和按键灯亮
				}
			}
			//在关机时，识别短按输入（power、up、down键）【关机时只有power键按住再松开时，开机】
			else if (gu8SetMode == 0)
			{
				//开机的开关键和参数设置的上、下键
				if (gu16KeyPressedTm >= Time100ms)
				{
					if (gu8KeyStatusBef == PRESS_POWER)
					{
						gu8KeyFlg = bPower;
						u8E2RdWaitTm = 2;
					}
					if (gu8KeyStatusBef == PRESS_UP)
						gu8KeyFlg = bUp;
					if (gu8KeyStatusBef == PRESS_DOWN)
						gu8KeyFlg = bDown;
					if (gu8KeyStatusBef == PRESS_CONTROL)
						gu8KeyFlg = bControl;
				}
			}
			//在SPEED 或 CONTROL 或 （TIME模式并且不是定时的设置）中，且为运行状态，识别短按输入（无SPEED键），进行设置
			else if ((gu8SetMode == SPEED) OR(gu8SetMode == CONTROL) OR(gu8SetMode == TIME))
			{ //正常运行、定时运行
				if ((gu16KeyPressedTm >= Time100ms) AND(gu16KeyPressedTm < 0xffff))
				{
					if (gu8KeyStatusBef == PRESS_POWER) //关机
					{
						gu8KeyFlg = bPower;
						u8E2RdWaitTm = 2;
					}
					else if (gu8KeyStatusBef == PRESS_MODE)
					{
						gu8KeyFlg = bWash; //短按冲洗键
					}
					if (gu8KeyStatusBef == PRESS_TIME)
						gu8KeyFlg = bTime;
					if (gu8KeyStatusBef == PRESS_UP)
						gu8KeyFlg = bUp;
					if (gu8KeyStatusBef == PRESS_DOWN)
						gu8KeyFlg = bDown;
					if (gu8KeyStatusBef == PRESS_CONTROL) // YXL-5
						gu8KeyFlg = bControl;
				}
			}
			//在外部控制：数字输入模式时，识别短按power键，进行关机
			else if (gu8SetMode == MODE_Ctrl1)
			{
				if (gu16KeyPressedTm >= Time100ms)
				{
					if (gu8KeyStatusBef == PRESS_POWER) //关机
					{
						gu8KeyFlg = bPower;
						u8E2RdWaitTm = 2;
					}
				}
			}
		}

		ClrBit(gu16NoKeyFlg, bValBuz);
		gu16KeyPressedTm = 0;
		gu16KeyPressedTm_wifi_ap = 0; // yxl-wifi//2021122
		gu8KeyStatusBef = u8KeyStatus;
	}
	else //按下
	{
		u8E2RdWaitTm = 6;

		if (gu8KeyStatusBef == u8KeyStatus)
		{
			SetBit(gu16NoKeyFlg, bPress);
		}
		else
		{
			gu8KeyStatusBef = u8KeyStatus;
			gu16KeyPressedTm = 0;
			gu16KeyPressedTm_wifi_ap = 0; // yxl-wifi//2021122
		}
		//按键锁定时
		if (gu8LockKeyTm == 0) //按键锁定
		{
			ClrBit(gu16NoKeyFlg, bPress);
			//按解锁键时
			if (gu8KeyStatusBef == PRESS_MODE)
			{
				//长按解锁并全亮
				if (gu16KeyPressedTm >= Time3sec)
				{
					gu16KeyPressedTm = 0xffff;
					gu8LockKeyTm = 120;
					gu8BakLgtDlyTm = 120;
					subBuzCtrl(SHORTBE);
				}
				//短按全亮
				else if (gu16KeyPressedTm >= Time100ms)
				{
					SetBit(gu16NoKeyFlg, bPress);
					gu8BakLgtDlyTm = 120;
				}
			}
			else if ((gu16KeyPressedTm >= Time100ms) AND((gu8SetMode == SPEED) OR(gu8SetMode == MODE_Ctrl1) OR(gu8SetMode == TIME) OR(gu8SetMode == CONTROL)))
			{
				gu8BakLgtDlyTm = 120;
			}
		}
		//按键解锁后
		else
		{
			if (gu16KeyPressedTm >= Time100ms)
			{
				gu8LockKeyTm = 120;
				gu8BakLgtDlyTm = 120;
				if ((SysProState == SysProState_Test) AND(u8SysTestTm > 40)) //自检模式时的全亮时间判断
					gu8BakLgtDlyTm = 5;
			}
			//关机时，长按组合键的功能实现，如查看版本号
			if (u8fg_SysIsOn == 0)
			{
				//长按3s
				if ((gu16KeyPressedTm >= Time3sec) AND(gu16KeyPressedTm < Time3secP))
				{
					//初始化
					if (gu8KeyStatusBef == PRESS_TIME_UP)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongTimeUp;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//显示版本号
					else if (gu8KeyStatusBef == PRESS_TIME_DOWN)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongTimeDown;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//进入参数设置
					else if (gu8KeyStatusBef == PRESS_UP_DOWN)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUpDown;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//初始化并触发空气压力校准//20211126/20211201；
					else if (gu8KeyStatusBef == PRESS_TIME_CONTROL_MODE)
					{
						gu16KeyPressedTm = 0xffff;
						u8SD_IntoReStoreDisWater = 1; //触发空气压力校准标志
						gu8KeyFlg = bLongTimeUp;	  // 同时触发出厂设置
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//是否打开传感器
					else if (gu8KeyStatusBef == PRESS_MODE_CONTROL)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUnlockControl;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					////显示压力AD校准值
					else if (gu8KeyStatusBef == PRESS_TIME_MODE)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUnlockTime;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
				}
			}
			//开机时
			else
			{
				//长按3s
				if (gu16KeyPressedTm >= Time3sec)
				{
					//长按上、切换键3s
					if ((gu8KeyStatusBef == PRESS_CONTROL_UP) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongControlUp;
						subBuzCtrl(SHORTBE);
						ClrBit(gu16NoKeyFlg, bPress);
					}
					//长按切换、下键3s
					else if ((gu8KeyStatusBef == PRESS_CONTROL_UP) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongControlDown;
						subBuzCtrl(SHORTBE);
						ClrBit(gu16NoKeyFlg, bPress);
					}
				}

				//在TIME模式且在设置参数时，长按按键的功能实现，如保存TIME设置
				if ((gu8SetMode == TIME) AND(gu8SetTimeTm > 0))
				{
					//长按3s按键实现功能。其中包括按TIME3S保存
					if (gu16KeyPressedTm >= Time3sec)
					{
						if ((gu8SetTimeState < 3) OR(gu8SetTimeTmSn != 4))
						{
							if (gu8KeyStatusBef == PRESS_UP)
							{
								gu8KeyFlg = bLongUp;
								gu16KeyPressedTm = 0xffff;
								//	ClrBit(gu16NoKeyFlg,bPress);
							}
							if (gu8KeyStatusBef == PRESS_DOWN)
							{
								gu8KeyFlg = bLongDown;
								gu16KeyPressedTm = 0xffff;
								//	ClrBit(gu16NoKeyFlg,bPress);
							}
						}

						///长按上下键返回上一个参数
						//////////////////////////////////////////20211124
						if (gu8KeyStatusBef == PRESS_UP_DOWN)
						{
							gu8KeyFlg = bLongUpDown;
							gu16KeyPressedTm = 0xffff;
						}
						////////////////////////////////////////20211124

						//长按TIME键3s
						if ((gu8KeyStatusBef == PRESS_TIME) AND(gu16KeyPressedTm < Time3secP))
						{
							//若不是冲洗模式，则保存之前设置好的参数
							if (gu8Wash == 0) // yyxxll
							{
								gu16KeyPressedTm = 0xffff;
								gu8KeyFlg = bLongTime;
								ClrBit(gu16NoKeyFlg, bPress);
								subBuzCtrl(SHORTBE);
							}
						}
					}
					if ((gu16KeyPressedTm >= Time3sec) AND(gu16KeyPressedTm < Time3secP))
					{
						if (gu8KeyStatusBef == PRESS_MODE)
						{
							gu16KeyPressedTm = 0xffff;
							ClrBit(gu16NoKeyFlg, bPress);
							subBuzCtrl(SHORTBE);
						}
					}
				}
				if ((gu8SetMode == TIME) AND(gu8SetTimeTm == 0) AND(gu8Wash == 0))
				{
					if (gu16KeyPressedTm >= Time3sec)
					{

						if ((gu8KeyStatusBef == PRESS_TIME) AND(gu16KeyPressedTm < Time3secP))
						{
							gu16KeyPressedTm = 0xffff;
							gu8KeyFlg = bLongTime;
							ClrBit(gu16NoKeyFlg, bPress);
							subBuzCtrl(SHORTBE);
						}
					}
				}
				//在转速模式时
				if ((gu8SetMode == SPEED) AND(gu8Wash == 0)) // YXL-5 //YXL-5
				{
					if (gu16KeyPressedTm >= Time3sec)
					{
						if ((gu8KeyStatusBef == PRESS_CONTROL) AND(gu16KeyPressedTm < Time3secP))
						{
							gu16KeyPressedTm = 0xffff;
							gu8KeyFlg = bLongTime;
							count_dis_WaterPress_value_tm = 10; //显示实际压力值
						}
					}
				}
			}

			//////////////////////////////////////////////////yxl-bash //20211210+
			//////////////////////////////////////////////////////////////////////////快增快减
			if (((gu8SetTimeTm) && (gu8SetTimeTmSn == 4)) || (gu8SetMode == SPEED) || (gu8SetMode == CONTROL) || (gu8SetMode == TIME && gu8Wash == 1))
			{
				if (gu16KeyPressedTm >= Time1seclongupdown) //>=100
				{
					if (gu8KeyStatusBef == PRESS_UP)
					{
						gu16KeyPressedTm = 0;
						gu8KeyFlg = bUp;
					}
					if (gu8KeyStatusBef == PRESS_DOWN)
					{
						gu16KeyPressedTm = 0;
						gu8KeyFlg = bDown;
					}
				}
			}

			//////////////////////////////////////////////////yxl-bash //20211210+
			/*----------------------------------------------------------------------------------------------------*/
			//////////////////////////////////////////////////yxl-bash //20211210+
			//开机时，长按MOED和CONTROL进入超频自吸
			if ((u8fg_SysIsOn)) // 20211211+
			{
				if (gu16KeyPressedTm >= Time3sec)
				{
					if ((gu8KeyStatusBef == PRESS_MODE_CONTROL) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUnlockControl; /// yyxxll
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
				}
			}
			//////////////////////////////////////////////////yxl-bash //20211210+

			/////////////////////////////20211222			 wifi
			//不是关机和不是自检时，长按关机键进入配网
			if ((u8fg_SysIsOn) && (SysProState == SysProState_Nol)) // yxl_wifi
			{
				///长按power键5s，设置wifi连接为smart模式
				if (gu16KeyPressedTm >= Time5sec_wifi)
				{
					if ((gu8KeyStatusBef == PRESS_POWER) AND(gu16KeyPressedTm < Time15sec_wifi))
					{
						gu16KeyPressedTm = 0xffff;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
						fg_wifi_smart_mode = 1;
					}
				}
				//取消ap配网20220105
				///长按power键10s，设置wifi连接为ap模式
				//		if (gu16KeyPressedTm_wifi_ap >=Time10sec_wifi)
				//		{
				//			if ((gu8KeyStatusBef == PRESS_POWER)AND(gu16KeyPressedTm_wifi_ap < Time15sec_wifi))
				//			{
				//				ClrBit(gu16NoKeyFlg, bValBuz);//还要响一次
				//				gu16KeyPressedTm_wifi_ap = 0xffff;
				//				subBuzCtrl(SHORTBE);
				//				ClrBit(gu16NoKeyFlg, bPress);
				//				fg_wifi_ap_mode=1;
				//			}
				//		}
			}

			//反冲洗时，长按SPEED键退出
			if (u8WashState == WashState_Normal)
			{
				if (gu16KeyPressedTm >= Time3sec)
				{

					if ((gu8KeyStatusBef == PRESS_MODE) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongWash;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
				}
			}
		}
	}
}

static void subBuzz(void)
{
	if (gu8BuzzDlyTm > 0)
	{
		//蜂鸣器开
		// PPGS |= 0x02; //yyxxll   蜂鸣器
		TIM_SetCompare1(TIM14, 125); // yxl-5
	}
	else
	{
		//蜂鸣器关
		// PPGS &= ~0x02; ///yyxxll  蜂鸣器
		TIM_SetCompare1(TIM14, 0); // yxl-5
	}
}

void FunInOut(void)
{
	subDinScan();
	subKeyScan();
	subBuzz();
}
