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

uchar bTime3s_sec = 0; // 20211124//ͬʱ�����¼�������һ���õ�ʱ��

/////////////////wifi_and_��λ�л�//////////////////
/////////20211222
uint16_t count_1min_kilowatt_hour = 0; // yxl-wifi
uchar count2s_wifi_gu16MotorWt_up = 0;
/////////////////wifi_and_��λ�л�//////////////////
/////////20211222

Time_base T_base;

void FunSysPropetyTime(void)
{
	//��ͨ��Դʱ��
	if (SysProTime.PowerTm < 0xffffffff)
		SysProTime.PowerTm++;

	//��������ʱ��
	if (gu8SetMode && gu16RunSpd)
	{
		if (SysProTime.RunTm < 0xffffffff)
			SysProTime.RunTm++;
	}
	else
	{
		SysProTime.RunTm = 0;
	}

	//����ʱ�䡢�ػ�ʱ��
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

	//����ʱ��
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
Comments: 5msʱ��
******************************************************************/

static void FunTimeCtrl(void)
{
	if (gu8MonitorTxdTm) // rs485ͨѶ*
		gu8MonitorTxdTm--;
	if (gMonitor1BtyeTm < 0xff) // rs485��������ʱ�����ֽڼ����С���*
		gMonitor1BtyeTm++;

	if (gu8BuzzDlyTm) //����������ʱ��*
		gu8BuzzDlyTm--;
	if (gu16KeyPressedTm < 0xffff) //������סʱ��*
		gu16KeyPressedTm++;

	/*	wifi��ap�����ã�������ap����
	if (gu16KeyPressedTm == 0xffff) //����10s��
	{
		if (gu16KeyPressedTm_wifi_ap < 0xffff)
			gu16KeyPressedTm_wifi_ap++;
	}
	*/
	if (fg_20ms_delay_ask < 0xff) // wifi������50ms���һ���Ƿ��跢����Ϣ*
		fg_20ms_delay_ask++;

	if (++u8Tim500msCnt >= 100) // 500MS
	{
		u8Tim500msCnt = 0;
	}
	if (u8Tim500msCnt EQU 5)
	{
		FunSysPropetyTime(); //ϵͳ�������Լ�¼ʱ��

		//----------------------------------Adc.c�ļ�
		if (gu8SensorIserror500msTm < 0xff) //��������500msʱ��*
		{
			gu8SensorIserror500msTm++;
		}
		if (gu8AutoUpWater500msTm < 0xff) //��ˮ������500msʱ��*
		{
			gu8AutoUpWater500msTm++;
		}
		if (getwaterpressTm < 0xff) // 6s��ȡһ��AD��ת��Ϊѹ��ֵ*
		{
			getwaterpressTm++;
		}
		if (getadbaseTm < 0xff) //��ȡADУ׼ֵ����ʱ��*
		{
			getadbaseTm++;
		}

		//----------------------------------Adc.c�ļ�

		//--------------------------------Mode.c�ļ�
		//----------��ˮ
		if (gu8upwateroverokTm < 0xff && gu8SetWastTm == 0) //��ˮ��ȡ������Ҫ��1min����ʱ��*
			gu8upwateroverokTm++;
		if (gu16upwateriserrorTm) //��ˮ��ȡ������Ҫ��1min����ʱ��*
			gu16upwateriserrorTm--;
		//----------��ˮ
		//--------------------------------Mode.c�ļ�

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
		//--------------------------------Uart485.c�ļ�
		if (gu8uart485corrTm < 0xff)
			gu8uart485corrTm++;
		if (u8Ctrl485OnUseTm) // 485�������ƺ����ʱ��15s
			u8Ctrl485OnUseTm--;
		//--------------------------------Uart485.c�ļ�

		//--------------------------------LCD.c�ļ�
		if (TwinkleTm++ >= 0xfe)
			TwinkleTm = 0;
		if (SysProState == SysProState_Aging)
		{
			if (SysAgingTwinTm++ >= 240)
				SysAgingTwinTm = 0;
		}
		//--------------------------------LCD.c�ļ�

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
		if (u8SD_PressAdDisTm)	 //�ػ�ʱ��ʾADУ׼ֵ
			u8SD_PressAdDisTm--;
		if (count_dis_WaterPress_value_tm) //ת��ʱ��ʾѹ��ֵ
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
		//ϵͳ�Լ�ʱ��
		if (u8SysTestTm < 0xfe)
			u8SysTestTm++;
		if (gu8PowerOnTm < 0xff)
			gu8PowerOnTm++;
		if (u8SD_ParamSetTm)
			u8SD_ParamSetTm--;
		if (gu8MonitComErrTm < 0xff)
			gu8MonitComErrTm++;

		// E2�����ȡ���
		if (u8E2RdWaitTm)
			u8E2RdWaitTm--;
		// RTC�����ȡ���
		if (gu8RTCWaitTm)
			gu8RTCWaitTm--;

		///�����ĵ���ʱ
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
		if (gu8MotorComTm) //�ȴ�������Ϣ���������ʱ��
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
 * @brief  �δ�������Ĵ���������ÿ1ms����һ��
 * @param  Null
 * @return Null
 * @note   Null
 */
void sys_Countdown(void)
{
	//�����������������ʱ��
	static u16 Time_base = 0;
	static u16 Time_base_bef = 0;
	if (++Time_base >= 500) // 500MSһ�δ�ѭ��
	{
		Time_base = 0;
	}
	if (Time_base != Time_base_bef) // 1msʱ��
	{
		T_base.T_1ms_ok = 1;
	}
	if ((Time_base % 5) == 0 && Time_base != Time_base_bef) // 5msʱ��
	{
		T_base.T_5ms_ok = 1;
		FunTimeCtrl(); //�ú�����ʹ������Ӳ������Ҫʱ����Ϊ5ms
	}
	if ((Time_base % 10) == 0 && Time_base != Time_base_bef) // 10msʱ��
	{
		T_base.T_10ms_ok = 1;
		FunAnalog_CycleGet(); //ѭ���ɼ�������IO�ڣ����������ʱ����
	}
	if ((Time_base % 20) == 0 && Time_base != Time_base_bef)
	{
		T_base.T_20ms_ok = 1; // 20msʱ��
	}
	if (Time_base != Time_base_bef)
	{
		Time_base_bef = Time_base;
	}
}
/**
 * @brief  main����
 * @param  Null
 * @return Null
 * @note   while(1)�е�ѭ������ֱ��if(Time_base % 2 != 0)���������жϣ����ظ�������Σ����������if(T_base.T_2ms_ok) �����ж�
 */
u8 u8SysExcuteOrder = 0; //ϵͳִ��˳�򣬴�0��ʼ

int main(void)
{

	delay_us(20000);	 //��ʱ20ms��������
	Sysclock_init();	 //���ڵδ�ʱ���ĳ�ʼ�����ǳ���Ļ���ʱ��
	FunMcuInit();		 //����io�ȵĳ�ʼ��
	WIFI_U2_cycleInit(); //�������еĳ�ʼ��������wifi�������ݵĻ���

	IWDG_Config(); //���Ź�������
	while (1)
	{
		if (T_base.T_1ms_ok)
		{
			T_base.T_1ms_ok = 0;
			//������1msʱ������һ�εĳ���
			WIFI_U2_SendData(); // WIFI�Ĵ��ڷ���
		}

		if (T_base.T_5ms_ok)
		{
			T_base.T_5ms_ok = 0;
			//������5msʱ������һ�εĳ���
			IrqMotorCom(); //��������Ĵ���ͨ��***

			switch (u8SysExcuteOrder++)
			{
			case 0:
				FunInOut();		 //�������롢�������롢����������***
				switch_read();	 // IC74HC165�����ȡ��Din�Ͳ��룩***
				FunRtc_SysPro(); //��ʱ��оƬ���д��***

				break;
			case 1:
				FunPSonser();	//����������***
				FunE2_SysPro(); //��e2pоƬ���д��***
				FunMotorCom();	//���Ƶ�����ͻ��������***
				break;
			case 2: //��Ҫ�����߼�
				if (SysProState == SysProState_Init)
				{
					FunSysProInit(); //ϵͳ��ͨ��Դ��ǰ8s��ʼ��
				}
				else
				{
					FunMonitorCom();  // RS485ͨ��
					FunAnalogCtrl();  //ģ������ȡ
					FunAutoUpWater(); //��ȡ��׼����ж��Ƿ���ˮ��ѧϰ
					FunSysProRun();	  //ϵͳִ��ģʽ���ϻ����Լ졢ͨ��
				}
				break;
			default:
				FunWriteRam_1729();	  //ˢ�����ݣ���ֹ����****
				IC74HC595_En();		  //Һ�����Ͱ����Ƶİ���/ȫ������***
				u8SysExcuteOrder = 0; //ϵͳ����ִ��˳�򣬴�0��ʼ
				break;
			}
		}

		if (T_base.T_10ms_ok)
		{
			T_base.T_10ms_ok = 0;
			//������10msʱ������һ�εĳ���
		}

		if (T_base.T_20ms_ok)
		{
			T_base.T_20ms_ok = 0;

			//������20msʱ������һ�εĳ���

			/////////////////WiFi���
			wifi_uart_service(); //��wifi���ڴ��������ݽ��д���
			pro_wifi_handle();	 //���������������wifi�Ĺ���ģʽ��smartģʽ��apģʽ��
			pro_wifi_updata();	 //��������
			pro_all_updata();	 //״̬��ѯʱ��������ȫ������

			//��ʱ��Ҫ�󲻸�
			WWDG_Config(); //ι��***
		}
	}
}

///////END///////////END/////////END/////////END/////////////END////////////////END////////////END///////////END///////////END///////////
