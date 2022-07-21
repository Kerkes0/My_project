/*
 * @Description: MIAN FUNCTION
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-16 10:12:51
 * @note:
 */
#include "main.h"
#include "User.h"
#include "wifi.h" //yxl-wifi //20211222
#include "Adc.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Rtc.h"
#include "E2prom.h"
#include "Uart485.h"
#include "LCD.h"
uchar u8Tim500msCnt = 0;
uchar u8Base1msSn = 0;

uchar gu8WashOneSec = 0;										// yyxxll
uint fg_func_1ms = 0, count_func_10ms = 0, count_func_20ms = 0; // yxl-5 //20211222
uchar count_screen_real_watervalue = 0;							// yxl-5

uchar count_ageing = 0;

uint count_05h_error_clear = 0;

uchar count_5s_dis_realwater = 10;
uchar fg_tm_120s_con = 0;

uchar bTime3s_sec = 0; // 20211124//同时按上下键返回上一设置的时基

/////////////////wifi_and_单位切换//////////////////
/////////20211222
uint16_t count_1min_kilowatt_hour = 0; // yxl-wifi
uchar count2s_wifi_gu16MotorWt_up = 0;
/////////////////wifi_and_单位切换//////////////////
/////////20211222

Time_base T_base;

void FunSysPropetyTime(void)
{
	//接通电源时间
	if (SysProTime.PowerTm < 0xffffffff)
		SysProTime.PowerTm++;

	//开机运行时间
	if (gu8SetMode && gu16RunSpd)
	{
		if (SysProTime.RunTm < 0xffffffff)
			SysProTime.RunTm++;
	}
	else
	{
		SysProTime.RunTm = 0;
	}

	//开机时间、关机时间
	if (gu8SetMode)
	{
		SysProTime.OffTm = 0;
		if (SysProTime.OnTm < 0xffffffff)
			SysProTime.OnTm++;
	}
	else
	{
		SysProTime.OnTm = 0;
		if (SysProTime.OffTm < 0xffffffff)
			SysProTime.OffTm++;
	}

	//故障时间
	if (gu16TripFlg_Driver || gu16TripFlg_Board)
	{
		if (SysProTime.ErrorTm < 0xffffffff)
			SysProTime.ErrorTm++;
	}
	else
	{
		SysProTime.ErrorTm = 0;
	}
}

/******************************************************************
Name		:FunTimeCtrl
Input		:
Output	:
Comments: 5ms时基
******************************************************************/

static void FunTimeCtrl(void)
{
	if (gu8MonitorTxdTm) // rs485通讯*
		gu8MonitorTxdTm--;
	if (gMonitor1BtyeTm < 0xff) // rs485接收数据时，两字节间的最小间隔*
		gMonitor1BtyeTm++;

	if (gu8BuzzDlyTm) //蜂鸣器鸣响时间*
		gu8BuzzDlyTm--;
	if (gu16KeyPressedTm < 0xffff) //按键按住时间*
		gu16KeyPressedTm++;

	/*	wifi的ap配网用，已屏蔽ap配网
	if (gu16KeyPressedTm == 0xffff) //长按10s钟
	{
		if (gu16KeyPressedTm_wifi_ap < 0xffff)
			gu16KeyPressedTm_wifi_ap++;
	}
	*/
	if (fg_20ms_delay_ask < 0xff) // wifi功能再50ms检测一次是否需发送信息*
		fg_20ms_delay_ask++;

	if (++u8Tim500msCnt >= 100) // 500MS
	{
		u8Tim500msCnt = 0;
	}
	if (u8Tim500msCnt EQU 5)
	{
		FunSysPropetyTime(); //系统部分属性记录时间

		//----------------------------------Adc.c文件
		if (gu8SensorIserror500msTm < 0xff) //传感器的500ms时基*
		{
			gu8SensorIserror500msTm++;
		}
		if (gu8AutoUpWater500msTm < 0xff) //上水自吸的500ms时基*
		{
			gu8AutoUpWater500msTm++;
		}
		if (getwaterpressTm < 0xff) // 6s读取一次AD并转换为压力值*
		{
			getwaterpressTm++;
		}
		if (getadbaseTm < 0xff) //获取AD校准值所需时间*
		{
			getadbaseTm++;
		}

		//----------------------------------Adc.c文件

		//--------------------------------Mode.c文件
		//----------上水
		if (gu8upwateroverokTm < 0xff && gu8SetWastTm == 0) //上水提取结束需要的1min至少时间*
			gu8upwateroverokTm++;
		if (gu16upwateriserrorTm) //上水提取结束需要的1min至少时间*
			gu16upwateriserrorTm--;
		//----------上水
		//--------------------------------Mode.c文件

		SetBit(gu8TimeFlg, bTime500ms);
		while (bTime3s_sec++ > 2)
		{
			SetBit(gu8TimeFlg, bTime2s);
			bTime3s_sec = 0;
		} // 20211124
		gu8SetTimeFlashCnt++;

		if (gu8MotorComTripTm < 0xff)
			gu8MotorComTripTm++;
	}
	else if (u8Tim500msCnt EQU 25)
	{
		//--------------------------------Uart485.c文件
		if (gu8uart485corrTm < 0xff)
			gu8uart485corrTm++;
		if (u8Ctrl485OnUseTm) // 485结束控制后持续时间15s
			u8Ctrl485OnUseTm--;
		//--------------------------------Uart485.c文件

		//--------------------------------LCD.c文件
		if (TwinkleTm++ >= 0xfe)
			TwinkleTm = 0;
		if (SysProState == SysProState_Aging)
		{
			if (SysAgingTwinTm++ >= 240)
				SysAgingTwinTm = 0;
		}
		//--------------------------------LCD.c文件

		if (gu8MotorStopTm < 0xff)
			gu8MotorStopTm++;
		if (gu8SetTimeTm)
			gu8SetTimeTm--;
		if (gu8BakLgtDlyTm)
			gu8BakLgtDlyTm--;
	}
	else if (u8Tim500msCnt EQU 45)
	{
		if (gu8SetTimeQueryTm)
			gu8SetTimeQueryTm--;
		if (gu8SpmTempDwTm)
			gu8SpmTempDwTm--;
		if (u8SD_SoftWareVerTm)
			u8SD_SoftWareVerTm--;

		if (u8SD_ReStoreDisWaterTm)
			u8SD_ReStoreDisWaterTm--; // yxl-5
		if (u8SD_PSensorDisTm)
			u8SD_PSensorDisTm--; // yxl-5
		if (u8SD_PressAdDisTm)	 //关机时显示AD校准值
			u8SD_PressAdDisTm--;
		if (count_dis_WaterPress_value_tm) //转动时显示压力值
			count_dis_WaterPress_value_tm--;

		if (count_1min_kilowatt_hour < 0xffff)
			count_1min_kilowatt_hour++; // yxl-wifi//2021122
	}
	else if (u8Tim500msCnt EQU 65)
	{
		if (gu8LcdInitTm)
			gu8LcdInitTm--;
		if (gu8LockKeyTm)
			gu8LockKeyTm--;
		if (gu8MotorTripClrTm)
			gu8MotorTripClrTm--;
		if (gu8AcVoltLmtTm)
			gu8AcVoltLmtTm--;

		if (count_5s_dis_realwater < 0xff)
			count_5s_dis_realwater++;
	}
	else if (u8Tim500msCnt EQU 85)
	{
		//系统自检时间
		if (u8SysTestTm < 0xfe)
			u8SysTestTm++;
		if (gu8PowerOnTm < 0xff)
			gu8PowerOnTm++;
		if (u8SD_ParamSetTm)
			u8SD_ParamSetTm--;
		if (gu8MonitComErrTm < 0xff)
			gu8MonitComErrTm++;

		// E2记忆读取间隔
		if (u8E2RdWaitTm)
			u8E2RdWaitTm--;
		// RTC记忆读取间隔
		if (gu8RTCWaitTm)
			gu8RTCWaitTm--;

		///自吸的倒计时
		if (gu8SetWastTm)
			gu8SetWastTm--;
		if ((WashTm) && (gu8SetWastTm == 0)) // yxl-5
		{
			if (gu8WashOneSec == 1)
			{
				gu8WashOneSec = 0;
				WashTm--;
			}
			else
			{
				gu8WashOneSec++;
			}
		}
	}

	if ((u8Tim500msCnt % 10) EQU 0) // 50MS
	{
		if (gu8TripQueryTm)
			gu8TripQueryTm--;
		if (gu8MotorComTm) //等待发送信息给驱动板的时间
			gu8MotorComTm--;
		//		if (gu8LcdWrCirTm)		gu8LcdWrCirTm--;

		if (count_05h_error_clear < 0xffffffff)
			count_05h_error_clear++;

		/////////////20211222
		if (count_250ms_wifi_shine)
			count_250ms_wifi_shine--; // yxl-wifi
		if (count_1500ms_wifi_shine)
			count_1500ms_wifi_shine--; // yxl-wifi
		if (count2s_wifi_gu16MotorWt_up < 0xff)
			count2s_wifi_gu16MotorWt_up++;
		/////////////20211222
	}
}

/**
 * @brief  滴答计数器的触发函数，每1ms触发一次
 * @param  Null
 * @return Null
 * @note   Null
 */
void sys_Countdown(void)
{
	//下面是主程序的运行时基
	static u16 Time_base = 0;
	static u16 Time_base_bef = 0;
	if (++Time_base >= 500) // 500MS一次大循环
	{
		Time_base = 0;
	}
	if (Time_base != Time_base_bef) // 1ms时基
	{
		T_base.T_1ms_ok = 1;
	}
	if ((Time_base % 5) == 0 && Time_base != Time_base_bef) // 5ms时基
	{
		T_base.T_5ms_ok = 1;
		FunTimeCtrl(); //该函数是使用其他硬件的主要时基，为5ms
	}
	if ((Time_base % 10) == 0 && Time_base != Time_base_bef) // 10ms时基
	{
		T_base.T_10ms_ok = 1;
		FunAnalog_CycleGet(); //循环采集传感器IO口，需放在任意时基中
	}
	if ((Time_base % 20) == 0 && Time_base != Time_base_bef)
	{
		T_base.T_20ms_ok = 1; // 20ms时基
	}
	if (Time_base != Time_base_bef)
	{
		Time_base_bef = Time_base;
	}
}
/**
 * @brief  main函数
 * @param  Null
 * @return Null
 * @note   while(1)中的循环，若直接if(Time_base % 2 != 0)这样进行判断，会重复触发多次，因此需利用if(T_base.T_2ms_ok) 进行判断
 */
u8 u8SysExcuteOrder = 0; //系统执行顺序，从0开始

int main(void)
{

	delay_us(20000);	 //延时20ms后再运行
	Sysclock_init();	 //关于滴答定时器的初始化，是程序的基本时基
	FunMcuInit();		 //关于io等的初始化
	WIFI_U2_cycleInit(); //环形阵列的初始化，关于wifi发送数据的缓存

	IWDG_Config(); //看门狗的设置
	while (1)
	{
		if (T_base.T_1ms_ok)
		{
			T_base.T_1ms_ok = 0;
			//下面是1ms时基运行一次的程序
			WIFI_U2_SendData(); // WIFI的串口发送
		}

		if (T_base.T_5ms_ok)
		{
			T_base.T_5ms_ok = 0;
			//下面是5ms时基运行一次的程序
			IrqMotorCom(); //与驱动板的串口通信***

			switch (u8SysExcuteOrder++)
			{
			case 0:
				FunInOut();		 //数字输入、按键输入、蜂鸣器功能***
				switch_read();	 // IC74HC165输入读取（Din和拨码）***
				FunRtc_SysPro(); //对时钟芯片进行存读***

				break;
			case 1:
				FunPSonser();	//传感器工作***
				FunE2_SysPro(); //对e2p芯片进行存读***
				FunMotorCom();	//与变频器发送或接收数据***
				break;
			case 2: //主要操作逻辑
				if (SysProState == SysProState_Init)
				{
					FunSysProInit(); //系统接通电源后前8s初始化
				}
				else
				{
					FunMonitorCom();  // RS485通信
					FunAnalogCtrl();  //模拟量读取
					FunAutoUpWater(); //获取基准点和判断是否上水自学习
					FunSysProRun();	  //系统执行模式：老化、自检、通常
				}
				break;
			default:
				FunWriteRam_1729();	  //刷新数据，防止跳闪****
				IC74HC595_En();		  //液晶屏和按键灯的半亮/全亮控制***
				u8SysExcuteOrder = 0; //系统函数执行顺序，从0开始
				break;
			}
		}

		if (T_base.T_10ms_ok)
		{
			T_base.T_10ms_ok = 0;
			//下面是10ms时基运行一次的程序
		}

		if (T_base.T_20ms_ok)
		{
			T_base.T_20ms_ok = 0;

			//下面是20ms时基运行一次的程序

			/////////////////WiFi相关
			wifi_uart_service(); //对wifi串口传来的数据进行处理
			pro_wifi_handle();	 //发送配网命令，设置wifi的工作模式：smart模式或ap模式。
			pro_wifi_updata();	 //发送数据
			pro_all_updata();	 //状态查询时发布发送全部数据

			//对时间要求不高
			WWDG_Config(); //喂狗***
		}
	}
}

///////END///////////END/////////END/////////END/////////////END////////////////END////////////END///////////END///////////END///////////
