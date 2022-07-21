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

uint16_t gu16KeyPressedTm = 0; //������ס����ʱ��

uint8_t gu8KeyStatusBef = 0;
uint8_t u8DinLFTCnt[4] = {0, 0, 0, 0}; //�������������߳���ʱ��
uint8_t gu8LockKeyTm = 6;			   //������������ʱ��
uint8_t gu8PowerOnTm = 0;

uchar fg_selfimbi_ok = 0;

void FunInOut(void);

void subBuzCtrl(uchar i)
{
	//���÷�������Ӧʱ��
	if (!ValBit(gu16NoKeyFlg, bValBuz))
	{
		SetBit(gu16NoKeyFlg, bValBuz);
		gu8BuzzDlyTm = i;
	}
}

u8 u8OutCtrl_Din = 0; // Din����ʱ��������Ŀ
static void subDinScan(void)
{
	static u8 Din_bef = 0;
	u8 fg_Din_SelCore = 0; // 1Ϊ4о��4о5оѡ���־
	//�ⲿ����-������-�ӿ�״̬��ȡ
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

	u8OutCtrl_Din = 0; //����
	if (u8DinLFTCnt[0] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_Stop);
	else if (u8DinLFTCnt[1] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_1);
	else if (u8DinLFTCnt[2] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_2);
	else if (u8DinLFTCnt[3] > 40)
		SetBit(u8OutCtrl_Din, bOutCtrl_3);

	//����������ʱ��4о��5оѡ��Ĭ��5о
	if (switch_key & 0x01) // sw1��������1λ����ʱ
	{
		fg_Din_SelCore = 1; // 1Ϊ4о����
	}
	// 4о����ʱ
	if (fg_Din_SelCore)
	{
		ClrBit(u8OutCtrl_Din, bOutCtrl_Stop); //��Din1�߿���
		//��֮ǰ��din���ƣ���һ���˳�ʱ��ֱ�ӹػ�
		if (Din_bef && u8OutCtrl_Din == 0)
			u8fg_SysIsOn = 0; //�ػ�
		if (Din_bef != u8OutCtrl_Din)
			Din_bef = u8OutCtrl_Din;
	}

	if (u8OutCtrl_Din)
	{
		SetBit(u8OutCtrlState, Ctrl_Din); //����Din����
		if (ValBit(u8OutCtrl_Din, bOutCtrl_Stop))
		{
			SetBit(u8OutCtrlState, Ctrl_DinOff); //�ػ�
		}
	}
	else
	{
		ClrBit(u8OutCtrlState, Ctrl_Din);	 //�˳�Din����
		ClrBit(u8OutCtrlState, Ctrl_DinOff); // Din���ƿ���
	}
}

static void subKeyScan(void)
{
	// static u8 gu8KeyModeCnt = 0;
	//����״̬����
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

	// if(fg_ageing_ok == 1)u8KeyStatus = 0; //�ϻ�����ʱ��������Ч//20211201

	if (u8KeyStatus == NOKEYPRESS) //�ް��°���ʱ
	{
		//��ǰ�а������̲Żᴥ�����ɿ�������
		if (ValBit(gu16NoKeyFlg, bPress)) //��bPress=1���ܿ��Եȼ�Ϊ����������û��ʵ�ֹ����ܣ�
		{
			ClrBit(gu16NoKeyFlg, bPress);
			if (gu16KeyPressedTm >= Time100ms)
				subBuzCtrl(SHORTBE);

			//�������Լ죩��������ʱ�����ϵ�ʱ��С��5s
			if (SysProState == SysProState_Init || SysProState == SysProState_Test)
			{
				if (gu16KeyPressedTm >= Time100ms) //�̰�
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
			//��SPEED��TIME��CONTROLģʽ�£������󰴹��������ɼ��������lcd�����
			else if ((gu8LockKeyTm == 0) AND((gu8SetMode == SPEED) OR(gu8SetMode == TIME) OR(gu8SetMode == CONTROL)))
			{
				if (gu16KeyPressedTm >= Time100ms)
				{
					gu8BakLgtDlyTm = 120; // 500ms*120=60s����������0ʱ��lcd����ƺͰ�������
				}
			}
			//�ڹػ�ʱ��ʶ��̰����루power��up��down�������ػ�ʱֻ��power����ס���ɿ�ʱ��������
			else if (gu8SetMode == 0)
			{
				//�����Ŀ��ؼ��Ͳ������õ��ϡ��¼�
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
			//��SPEED �� CONTROL �� ��TIMEģʽ���Ҳ��Ƕ�ʱ�����ã��У���Ϊ����״̬��ʶ��̰����루��SPEED��������������
			else if ((gu8SetMode == SPEED) OR(gu8SetMode == CONTROL) OR(gu8SetMode == TIME))
			{ //�������С���ʱ����
				if ((gu16KeyPressedTm >= Time100ms) AND(gu16KeyPressedTm < 0xffff))
				{
					if (gu8KeyStatusBef == PRESS_POWER) //�ػ�
					{
						gu8KeyFlg = bPower;
						u8E2RdWaitTm = 2;
					}
					else if (gu8KeyStatusBef == PRESS_MODE)
					{
						gu8KeyFlg = bWash; //�̰���ϴ��
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
			//���ⲿ���ƣ���������ģʽʱ��ʶ��̰�power�������йػ�
			else if (gu8SetMode == MODE_Ctrl1)
			{
				if (gu16KeyPressedTm >= Time100ms)
				{
					if (gu8KeyStatusBef == PRESS_POWER) //�ػ�
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
	else //����
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
		//��������ʱ
		if (gu8LockKeyTm == 0) //��������
		{
			ClrBit(gu16NoKeyFlg, bPress);
			//��������ʱ
			if (gu8KeyStatusBef == PRESS_MODE)
			{
				//����������ȫ��
				if (gu16KeyPressedTm >= Time3sec)
				{
					gu16KeyPressedTm = 0xffff;
					gu8LockKeyTm = 120;
					gu8BakLgtDlyTm = 120;
					subBuzCtrl(SHORTBE);
				}
				//�̰�ȫ��
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
		//����������
		else
		{
			if (gu16KeyPressedTm >= Time100ms)
			{
				gu8LockKeyTm = 120;
				gu8BakLgtDlyTm = 120;
				if ((SysProState == SysProState_Test) AND(u8SysTestTm > 40)) //�Լ�ģʽʱ��ȫ��ʱ���ж�
					gu8BakLgtDlyTm = 5;
			}
			//�ػ�ʱ��������ϼ��Ĺ���ʵ�֣���鿴�汾��
			if (u8fg_SysIsOn == 0)
			{
				//����3s
				if ((gu16KeyPressedTm >= Time3sec) AND(gu16KeyPressedTm < Time3secP))
				{
					//��ʼ��
					if (gu8KeyStatusBef == PRESS_TIME_UP)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongTimeUp;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//��ʾ�汾��
					else if (gu8KeyStatusBef == PRESS_TIME_DOWN)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongTimeDown;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//�����������
					else if (gu8KeyStatusBef == PRESS_UP_DOWN)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUpDown;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//��ʼ������������ѹ��У׼//20211126/20211201��
					else if (gu8KeyStatusBef == PRESS_TIME_CONTROL_MODE)
					{
						gu16KeyPressedTm = 0xffff;
						u8SD_IntoReStoreDisWater = 1; //��������ѹ��У׼��־
						gu8KeyFlg = bLongTimeUp;	  // ͬʱ������������
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					//�Ƿ�򿪴�����
					else if (gu8KeyStatusBef == PRESS_MODE_CONTROL)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUnlockControl;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
					////��ʾѹ��ADУ׼ֵ
					else if (gu8KeyStatusBef == PRESS_TIME_MODE)
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongUnlockTime;
						ClrBit(gu16NoKeyFlg, bPress);
						subBuzCtrl(SHORTBE);
					}
				}
			}
			//����ʱ
			else
			{
				//����3s
				if (gu16KeyPressedTm >= Time3sec)
				{
					//�����ϡ��л���3s
					if ((gu8KeyStatusBef == PRESS_CONTROL_UP) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongControlUp;
						subBuzCtrl(SHORTBE);
						ClrBit(gu16NoKeyFlg, bPress);
					}
					//�����л����¼�3s
					else if ((gu8KeyStatusBef == PRESS_CONTROL_UP) AND(gu16KeyPressedTm < Time3secP))
					{
						gu16KeyPressedTm = 0xffff;
						gu8KeyFlg = bLongControlDown;
						subBuzCtrl(SHORTBE);
						ClrBit(gu16NoKeyFlg, bPress);
					}
				}

				//��TIMEģʽ�������ò���ʱ�����������Ĺ���ʵ�֣��籣��TIME����
				if ((gu8SetMode == TIME) AND(gu8SetTimeTm > 0))
				{
					//����3s����ʵ�ֹ��ܡ����а�����TIME3S����
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

						///�������¼�������һ������
						//////////////////////////////////////////20211124
						if (gu8KeyStatusBef == PRESS_UP_DOWN)
						{
							gu8KeyFlg = bLongUpDown;
							gu16KeyPressedTm = 0xffff;
						}
						////////////////////////////////////////20211124

						//����TIME��3s
						if ((gu8KeyStatusBef == PRESS_TIME) AND(gu16KeyPressedTm < Time3secP))
						{
							//�����ǳ�ϴģʽ���򱣴�֮ǰ���úõĲ���
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
				//��ת��ģʽʱ
				if ((gu8SetMode == SPEED) AND(gu8Wash == 0)) // YXL-5 //YXL-5
				{
					if (gu16KeyPressedTm >= Time3sec)
					{
						if ((gu8KeyStatusBef == PRESS_CONTROL) AND(gu16KeyPressedTm < Time3secP))
						{
							gu16KeyPressedTm = 0xffff;
							gu8KeyFlg = bLongTime;
							count_dis_WaterPress_value_tm = 10; //��ʾʵ��ѹ��ֵ
						}
					}
				}
			}

			//////////////////////////////////////////////////yxl-bash //20211210+
			//////////////////////////////////////////////////////////////////////////�������
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
			//����ʱ������MOED��CONTROL���볬Ƶ����
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
			//���ǹػ��Ͳ����Լ�ʱ�������ػ�����������
			if ((u8fg_SysIsOn) && (SysProState == SysProState_Nol)) // yxl_wifi
			{
				///����power��5s������wifi����Ϊsmartģʽ
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
				//ȡ��ap����20220105
				///����power��10s������wifi����Ϊapģʽ
				//		if (gu16KeyPressedTm_wifi_ap >=Time10sec_wifi)
				//		{
				//			if ((gu8KeyStatusBef == PRESS_POWER)AND(gu16KeyPressedTm_wifi_ap < Time15sec_wifi))
				//			{
				//				ClrBit(gu16NoKeyFlg, bValBuz);//��Ҫ��һ��
				//				gu16KeyPressedTm_wifi_ap = 0xffff;
				//				subBuzCtrl(SHORTBE);
				//				ClrBit(gu16NoKeyFlg, bPress);
				//				fg_wifi_ap_mode=1;
				//			}
				//		}
			}

			//����ϴʱ������SPEED���˳�
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
		//��������
		// PPGS |= 0x02; //yyxxll   ������
		TIM_SetCompare1(TIM14, 125); // yxl-5
	}
	else
	{
		//��������
		// PPGS &= ~0x02; ///yyxxll  ������
		TIM_SetCompare1(TIM14, 0); // yxl-5
	}
}

void FunInOut(void)
{
	subDinScan();
	subKeyScan();
	subBuzz();
}
