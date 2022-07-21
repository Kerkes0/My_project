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
/*****************ϵͳ����************************/
u8 u8fg_SysIsOn = 0;	   //���ػ�
u8 gu8SetMode = SPEED;	   //ͨ��ģʽ����
u8 gu8SetMode_Bef = SPEED; //ǰһ��ͨ��ģʽ����
u8 gu8SetMode_ago = SPEED; //ǰһ��ͨ��ģʽ����
u8 u8WashState = 0;		   //��������
u8 u8OutCtrlState = 0;	   //�ⲿ��������

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

uint gu16MotorSetSpd = 0; //���͸��������ת��
u8 bCompOnCmd = 0;		  //�Ƿ���ͣ������

//--------------------------------
u8 fg_hand_close = 0; //�ֶ��ػ���־

//--------------------------------ת��ģʽ
u8 u8ModeSpeed_SpeedSet = 36; //ת���趨ֵ��26-40��
//--------------------------------����ģʽ
u16 u16ModeGate_GateSet = 5; //�����趨ֵ

u8 fg_ModeUnit_IsChange = 0; //������λ�Ƿ��л���
//--------------------------------��ʱģʽ
u8 gu8SetTimeState = 0;
sint gu16SetNowTm;
uint gs16RunNowTm;
sint SetStartTm[4];		//�����ã���ʱ��ʱ��ε����п�ʼʱ��
sint SetEndTm[4];		//�����ã���ʱ��ʱ��ε����п�ʼʱ��
sint gs16RunStartTm[4]; //��ʱ��ʱ��ε����п�ʼʱ��
sint gs16RunEndTm[4];	//��ʱ��ʱ��ε����н���ʱ��

u8 u8ModeTime_SpeedSet[4];	   //��ʱ��ʱ��ε�ת���趨ֵ��26-40��
u8 u8ModeTime_SpeedSet_Tem[4]; //�����ã���ת���趨ֵ��26-40��
u16 u8ModeTime_GateSet[4];	   //��ʱ��ʱ��ε������趨ֵ
u16 u8ModeTime_GateSet_Tem[4]; //�����ã����������趨ֵ

u8 gu8SetTimeTmSn = 0;
u8 gu8SetTimeTmNum = 0;
u16 gu8SetTimeFlg = 0;
u8 u8ModeTimeRunSpeed = 0;	//��ʱģʽ��ǰʱ��ε�ת��ֵ
uint u8ModeTimeRunGate = 0; //��ʱģʽ��ǰʱ��ε�����ֵ
u8 gu8SetTimeQuerySn = 0;
u8 gu8TimeRunNum = 0;

u8 gu8BuzzDlyTm = 0; //���������г���ʱ��
u8 gu8TimeFlg = 0;	 //��ʱģʽʱ�������

u8 gu8SetFlg = 0; //���ϲ��洢

//-------------------------------------Ԥ��
u8 gu8AcVoltLmtTm = 1; //�Ƿ���Ԥ�����ٵļ����ʱ��

//-------------------------------------������
uint gu16UartRunSpd = 0; //�������ת��

//--------------------------------------�Լ�
u8 u8SysTestTm = 0; //ϵͳ�Լ�ʱ��ʱ��

//--------------------------------------�Լ�ʱ����Ӧ�����Ƶ�����0Ϊ��
u8 KeyLed1 = 0;
u8 KeyLed2 = 0;
u8 KeyLed3 = 0;
u8 KeyLed4 = 0;
u8 KeyLed5 = 0;
u8 KeyLed6 = 0;

u8 u8FirstFunIn = 0; //�ָ��������ú����ѧϰ�����н����ж�

uint WaterGate_set = 0;		//����ģʽ������ֵ���뵥λ�йأ�
u8 fg_time_spd_contorl = 0; // 0Ϊת�٣���ʱģʽʱ����������ת�ٿ���

u8 fg_slave_tempoverhigh = 0; //�ӻ�ɢ��������E101
u8 fg_host_tempoverhigh = 0;  //����ɢ��������E101

///////////////////////Ĭ����ѭ������ģʽ��������
u8 gu8SetMode_eep = 0;	 // yxl-5//20211115/20211118
u8 fg_rest_con_wifi = 0; // 20220105+

//-----------------------------------------------������ˮ
u8 fg_gu8SetMode_laststate_lowwater = 0;
u8 fg_auto_upwater = 0; //��ˮ��־����ɾ��

u8 gu8upwateroverokTm = 0; //��ˮ����1min�󣬿��Կ�ʼ�ж��Ƿ���ǰ����

u8 fg_TimeSetSpd = 0;		  //��ʱͣ����ˮ��־
u8 count_upwater_iserror = 0; //��ˮ����ʧ�ܵĴ������
u16 gu16upwateriserrorTm = 0; //��ˮ����ʧ�����½���ȴ�ʱ��

//-------------------------------------------ϵͳ�������Լ�¼ʱ��
TypeSysProTime SysProTime = {0, 0, 0, 0, 0}; //ϵͳ�������Լ�¼ʱ��
u8 SysProState = SysProState_Init;			 //ϵͳ��ִ��״̬���ǽ����ʼ�����ǽ���ģʽ����

//-------------------------------------------��ѧϰ

/**
 * @brief  ǷѹԤ��
 * @param[in]  {spd}ת��
 * @return Ƿѹ�󽵵͵��ٶ�
 * @note   ACǷѹ���٣�����Ƿѹ���򷵻�ԭ����ֵ����֮�����Ҵ������ٶȣ��򷵻����ٵ���ֵ
 */
uint AcVoltLmt(uint spd)
{
	static u32 gu16SysMaxSpd = 2600; // // u16LadderSpd[16];
	if (gu8AcVoltLmtTm == 0)
	{
		// AC��ѹ����ʱ��ÿ��5����һ�Σ������٣����ת��Ϊ100%�������ACǷѹ����
		if (gu16AcVolt > 199)
		{
			gu8AcVoltLmtTm = 10;				 // 500ms*10 = 5s��ÿ��2����һ��
			ClrBit(gu16TripFlg_Warn, Lslowvolt); //���ACǷѹ����AL02
			gu16SysMaxSpd = u16LadderSpd[16];	 //������
		}
		// AC��ѹ�쳣ʱ��ÿ��120����һ�Σ��������٣���ACǷѹ����
		else
		{
			gu8AcVoltLmtTm = 120;				 // 500ms*120 = 60s��ÿ��60����һ��
			SetBit(gu16TripFlg_Warn, Lslowvolt); //��ACǷѹ����AL02
			//��������
			if (gu16AcVolt <= 180)				  // yxl-5
				gu16SysMaxSpd = u16LadderSpd[10]; // 1820��70%�� //yyxxll
			else if (gu16AcVolt <= 190)
				gu16SysMaxSpd = u16LadderSpd[11]; // 1980��75%��  //yyxxll
			else
				gu16SysMaxSpd = u16LadderSpd[13]; // 2340��85%�� //yyxxkk
		}
	}
	//���ٺ�����ǰ����ת�ٴ������ٵ��ٶȣ���ǰת��ǿ�ƽ��������ٶ�
	if (gu16SysMaxSpd < spd)
		spd = gu16SysMaxSpd;

	return (spd);
}

/**
 * @brief  ����Ԥ��
 * @param[in]  {spd}ת��
 * @return ����Ԥ���󽵵͵��ٶ�
 * @note   �������٣�����Ƿѹ���򷵻�ԭ����ֵ����֮�����Ҵ������ٶȣ��򷵻����ٵ���ֵ
 */
uint SpmTempProtect(uint spd)
{
	u8 k;

	static u8 gu8SpmZeroTempAvg = 0;
	static uint sProtectSpd = 870; //���½���
	static u8 gu8SpmTempDnSn = 0;

	// gu8SpmTempAvg=dgc;

	if (!bCompOnCmd)
	{
		//���ͣ��500ms*60=30sʱ
		if (gu8MotorStopTm > 60)
		{
			//�¶ȵ���81��Cʱ
			if (gu8SpmTempAvg <= u8SpmTempTab[1]) //�¶�С��81��Cʱ
			{
				sProtectSpd = spd;
				if (gu8SpmTempOVHCnt < 3)
				{
					fg_host_tempoverhigh = 0;
				}
			}
			//�¶ȸ���81��Cʱ
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
		//�¶ȸ���85��Cʱ
		if (gu8SpmTempAvg > u8SpmTempTab[2])
		{
			fg_host_tempoverhigh = 1; //ɢ�������ȹ���E101
			sProtectSpd = 0;		  //���ͺ���ٶ�
			gu8SpmTempOVHCnt++;		  //����ͣ������
		}
		//�¶ȸ���81��Cʱ
		else if (gu8SpmTempAvg > u8SpmTempTab[1])
		{

			SetBit(gu16TripFlg_Warn, LsHighTemp); //����Ԥ������AL01
		}
		//�¶ȵ��ڵ���78��Cʱ
		else if (gu8SpmTempAvg <= u8SpmTempTab[0])
		{

			ClrBit(gu16TripFlg_Warn, LsHighTemp); //�������Ԥ������AL01
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
			//����ʱ��ÿ2min���һ�Σ��ж��Ƿ��������
			if (gu8SpmTempDwTm == 0)
			{
				//�¶ȶԱ�֮ǰ����
				if (gu8SpmTempAvg > gu8SpmZeroTempAvg)
				{
					gu8SpmTempDwTm = 240; // yxl-5  2min

					gu8SpmZeroTempAvg = gu8SpmTempAvg; // yyxxll  yxl-5  Ҫ����һ�����Ա�

					for (k = 1; k < 17; k++)
					{
						if (sProtectSpd == u16LadderSpd[k]) //�޸Ĳ�����������
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

//Сʱ����ʱ���ܷ���������1440��-1440��С��0��+1440����Ҫ������ʹ��Сʱ������24���0
static sint subMinTime(sint data)
{
	if (data > 1440)
		data -= 1440;
	if (data < 0)
		data += 1440;

	return (data);
}

//����ԤԼʱ��εĿ�ʼСʱ����������Сʱ��������ж�
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
//����ԤԼʱ��εĽ���Сʱ����������Сʱ��������ж�
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

//����ϻ��¼������ö�ʱ���ܵ�ʱ�Ӻ�ԤԼʱ���
static void subKeyUpDownSetTime(const char mode)
{
	sint Min, Hour;
	u8 unit = fg_ModeGateUnit;
	u8 GateDif;

	///============================//yyxxll
	//��ʱ������ʱ�ӵ�Сʱ����
	if (gu8SetTimeState == 1)
	{
		gu16SetNowTm += (sint)(mode)*60;
		if (gu16SetNowTm > 1439)
			gu16SetNowTm -= 1440;
		if (gu16SetNowTm < 0)
			gu16SetNowTm += 1440;
	}
	//��ʱ������ʱ�ӵķ�������
	else if (gu8SetTimeState == 2)
	{
		Min = gu16SetNowTm % 60; //֮ǰ����
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
	//��ʱ������ԤԼʱ���1~4������
	else
	{
		//��ʱ������ԤԼʱ��Σ�gu8SetTimeTmNum�Ŀ�ʼСʱ���ã�gu8SetTimeTmNum=0~4������0~3Ϊʱ���1~4��4Ϊ����/�������ã�
		if (gu8SetTimeTmSn == 0)
		{
			SetStartTm[gu8SetTimeTmNum] += mode * 60;
			SetStartTm[gu8SetTimeTmNum] = subMinTime(SetStartTm[gu8SetTimeTmNum]);
			subStartTm(mode);
		}
		//��ʱ������ԤԼʱ��Σ�gu8SetTimeTmNum�Ŀ�ʼ�������ã�gu8SetTimeTmNum=0~4������0~3Ϊʱ���1~4��4Ϊ����/�������ã�
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
		//��ʱ������ԤԼʱ��Σ�gu8SetTimeTmNum�Ľ���Сʱ���ã�gu8SetTimeTmNum=0~4������0~3Ϊʱ���1~4��4Ϊ����/�������ã�
		else if (gu8SetTimeTmSn == 2)
		{
			SetEndTm[gu8SetTimeTmNum] += mode * 60;
			SetEndTm[gu8SetTimeTmNum] = subMinTime(SetEndTm[gu8SetTimeTmNum]);
			subEndTm(mode);
		}
		//��ʱ������ԤԼʱ��Σ�gu8SetTimeTmNum�Ľ����������ã�gu8SetTimeTmNum=0~4������0~3Ϊʱ���1~4��4Ϊ����/�������ã�
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
		//��ʱ������ԤԼʱ��Σ�gu8SetTimeTmNum������/�������ã�gu8SetTimeTmNum=0~4������0~3Ϊʱ���1~4��4Ϊ����/�������ã�
		else
		{
			//��������
			if (fg_time_spd_contorl == 1) // yxl-5   ��������
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
			//ת������
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
//ͬʱ�����ϡ��¼����˻���һ��ʱ������
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
//��ʱģʽ������ʱ
void FunModeTimeSet(void)
{
	u8 i;
	//�����ʱ���������¸�����
	if (gu8KeyFlg == bTime)
	{
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��

		if (++gu8SetTimeState >= 23)
		{
			gu8SetTimeState = 0;
			gu8SetTimeTm = 0;
			gu8SetTimeTmSn = 0;
		}

		if (gu8SetTimeState < 3)
		{
			SetBit(gu8SetTimeFlg, bNowTime); //��RTCоƬ�б���ʱ��
		}
		else
		{
			SetBit(gu8SetTimeFlg, bSetTime); //��E2pоƬ�б���ʱ��ת�ٵ�
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
			//��֤��һ����ʼʱ�䲻��С��ǰһ������ʱ��
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
	//����ϼ�
	else if (gu8KeyFlg == bUp)
	{
		subKeyUpDownSetTime(1);
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
	}
	//�����ϼ�
	else if (gu8KeyFlg == bLongUp)
	{
		if (ValBit(gu8TimeFlg, bTime500ms))
			subKeyUpDownSetTime(2);
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
	}
	//����¼�
	else if (gu8KeyFlg == bDown)
	{
		subKeyUpDownSetTime(-1);
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
	}
	//�����¼�
	else if (gu8KeyFlg == bLongDown)
	{
		if (ValBit(gu8TimeFlg, bTime500ms))
			subKeyUpDownSetTime(-2);
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
	}
	//�������¼����˻��ϸ�����
	else if (gu8KeyFlg == bLongUpDown)
	{
		if (ValBit(gu8TimeFlg, bTime2s))
		{
			suKeyUp_DownSetTime();
			subBuzCtrl(SHORTBE);
		}
		gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
	}
	//��control�����ڵ�һ��ת���������л�ת�ٻ�����
	else if (gu8KeyFlg == bControl)
	{
		if ((gu8SetTimeTmNum == 0) && (gu8SetTimeTmSn == 4))
		{
			fg_time_spd_contorl ^= 1; //����ת�ٻ������л�
		}
	}
	//������ʱ�����������ò��˳�����
	else if (gu8KeyFlg == bLongTime)
	{
		gu8SetTimeTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime); //��RTCоƬ�б���ʱ��
		SetBit(gu8SetTimeFlg, bSetTime); //��E2pоƬ�б���ʱ��ת�ٵ�
	}

	//���õ���ʱ��ǰ0.5s��ǰ����������
	if (gu8SetTimeTm == 1)
	{
		gu8SetTimeTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime); //��RTCоƬ�б���ʱ��
		SetBit(gu8SetTimeFlg, bSetTime); //��E2pоƬ�б���ʱ��ת�ٵ�
	}

	//����Ŀ�ʼʱ����Զ��С��ǰ��ĳ������ʱ�䡢ͬһʱ����ڵĽ���ʱ�䲻С�ڿ�ʼʱ��
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
	//����ʱ��Ρ�ת��
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

//������ʱ��󣬽��붨ʱ���С��ȴ�������Чʱ�����˸
static void subSetTimeCtrl(void)
{
	u8 i, k = 0;

	for (i = 0; i < 4; i++)
	{
		if (gs16RunStartTm[i] == gs16RunEndTm[i])
			k++;
	}
	ClrBit(gu8SetTimeFlg, bInvalid); //������Ч��ʱ��ʱ
	ClrBit(gu8SetTimeFlg, bRun);	 //���ĳ��ʱ������
	ClrBit(gu8SetTimeFlg, bStay);	 //���ĳ��ʱ�εȴ�
	//ȫ����ʱ����Чʱ
	if (k >= 4)
	{
		SetBit(gu8SetTimeFlg, bInvalid); //ȫ����ʱ����Чʱ
		u8ModeTimeRunSpeed = 0;
		u8ModeTimeRunGate = 0;
		gu8TimeRunNum = 0;
	}
	//��ʱ�δ�����Чʱ
	else
	{
		//��ʱ����ʱ
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
		//��ʱ���еȴ�ʱ
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
//�ڶ�ʱ������������״̬ʱ�����ϡ��¼��������
//����ʱ����״̬�£����ϡ��¼��������ĸ�Timer�����á���
void subSetTimeQuery(void)
{
	if (gu8KeyFlg == bUp)
	{
		if (++gu8SetTimeQuerySn >= 4)
			gu8SetTimeQuerySn = 0;

		gu8SetTimeQueryTm = 20; //��ʱ����ʱ��ѯ��ѯʱ��ˢ��
	}

	if (gu8KeyFlg == bDown)
	{
		if (gu8SetTimeQuerySn)
			gu8SetTimeQuerySn--;
		else
			gu8SetTimeQuerySn = 3;

		gu8SetTimeQueryTm = 20; //��ʱ����ʱ��ѯ��ѯʱ��ˢ��
	}

	if (gu8SetTimeQueryTm == 0)
		gu8SetTimeQuerySn = gu8TimeRunNum; //�˳���ѯ
}

//�����������ܺ����
static void subClrKeyBit(void)
{
	gu8KeyFlg = 0;
	gu8TimeFlg = 0;
}
//����ʾ���ٶȴ�������ٶȣ�����ٶ�ͨ������ͨ�Ŵ�����Ƶ����
void subSpdToMotor(const u32 Spd)
{
	//�Լ�ʱ��ת��
	if (SysProState == SysProState_Test)
	{
		gu16MotorSetSpd = 0;
	}
	//�ϻ�ʱ100%ת��
	else if (SysProState == SysProState_Aging)
	{
		gu16MotorSetSpd = u16LadderSpd[16] / 2;
	}
	//�ػ�ʱ��ת��
	else if (u8fg_SysIsOn == 0)
	{
		gu16MotorSetSpd = 0;
	}
	//����
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

			if ((gu16TripFlg_Board == 0) && (gu16TripFlg_Driver == 0)) // yxl-5 ��ѹ�����͸���Ԥ�����������ϲ�ͣ��
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

//�Լ캯��
void subTestMode(void) F
{
	static u8 gu8FastTestSn = 8; //�Լ���ʾ������
	gu8LockKeyTm = 120;			 //������������ʱ��
	//�й���ʱ
	if ((gu16TripFlg_Board)OR(gu16TripFlg_Driver)) // yxl-5
	{
		ClrBit(gu16TripFlg_Driver, Inlowflowerr); //��E207���ձ���
		ClrBit(gu16TripFlg_Board, Owupwaterfail); //��E209����ʧ��
		gu16TripFlg_Warn = 0;					  //��Ԥ������
		FunLcdError();							  //������ʾ
	}
	//�޹���ʱ
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
		subClrKeyBit(); //�����������
	}
}

void subSetMode(void)
{

	/**************************���ػ�״̬*************************/
	//����״̬�������ؼ��ػ���485�ػ�
	if (u8fg_SysIsOn)
	{
		if ((gu8KeyFlg == bPower))
		{
			u8fg_SysIsOn = 0;  //�ػ�
			fg_hand_close = 1; //  �ֶ��ػ�������485����

			//-------+
			subBuzCtrl(LONGBE);	   //�����������죨����
			gu8SetTimeTm = 0;	   //��ʱ����ģʽ�ĳ���ʱ��
			gu8SetTimeQueryTm = 0; //��ʱ����ʱ�Ĳ�ѯ��˸ʱ��
		}
	}
	//�ػ�״̬�������ؼ�������485�ػ�����������
	else
	{
		FunModeShutDown();	  //�ػ�ʱ���ܵĽ�������
		FunLcdModeShutDown(); //�ػ�ʱ���ܵ���ʾ
		/***ת��****/
		gu16RunSpd = 0;
		/*****�������******/
		u8WashState = 0; //�˳�����ģʽ
		WashTm = 0;
		count_upwater_iserror = 0;				  //��ˮ����ʧ�ܴ�������
		ClrBit(gu16TripFlg_Board, Owupwaterfail); //�ػ�������ʧ�ܹ���E209
	}

	/**************************��ˮ*************************/
	// TIMEģʽʱ��ͣ����ˮ������ʱ����ߣ�
	if (gu8SetMode == TIME && ValBit(gu8SetTimeFlg, bStay))
	{
		fg_TimeSetSpd = 1;
	}
	else if ((gu8SetMode != TIME) || !ValBit(gu8SetTimeFlg, bStay))
	{
		fg_TimeSetSpd = 0;
	}
	//��ˮ��������
	if (u8fg_SysIsOn == 0							 //���ػ�
		|| (gu16TripFlg_Driver || gu16TripFlg_Board) //����(���½��ٺ͵�ѹ���ٲ��ᴥ��)
		|| (fg_TimeSetSpd == 1)						 //��ʱ
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

	/**************************���ڶ�Ӧģʽʱ������*************************/
	if (gu8SetMode != SPEED)
	{
		count_dis_WaterPress_value_tm = 0; //��ʾѹ��ֵ��ʱ��
	}
	if (gu8SetMode != CONTROL)
	{
		count_dis_WaterPress_value_tm = 0; //��ʾѹ��ֵ��ʱ��
	}
	if (gu8SetMode != TIME)
	{
		gu8SetTimeTm = 0;	   //��ʱ����ģʽʱ������
		gu8SetTimeState = 0;   //��ʱ���õ�һ������
		gu8SetTimeQueryTm = 0; //��ʱ����ʱ�Ĳ�ѯ��˸ʱ��
	}

	/**************************��ģʽ���к���ʾ*************************/
	//����ʱ
	if (u8fg_SysIsOn)
	{
		//������ģʽʱ
		if (u8WashState)
		{
			//�ڷ���ϴ����ʱ����ת�١�ʱ����ڡ��˳�
			if (u8WashState == WashState_Normal)
			{
				FunNormalWash_Set();
			}
			//�ڳ�Ƶ����ʱ����ת�١�ʱ����ڡ��˳�����ѧϰ
			else if (u8WashState == WashState_Super)
			{
				FunSuperWash_Set();
			}
			//����ˮ����ʱ
			else if (u8WashState == WashState_UpWater)
			{
				FunUpWash_Set();
			}
			//����ѧϰʱ
			else if (u8WashState == WashState_SelfStudy)
			{
				FunSelfStudy_Set();
			}
			//--------------------------LCD��ʾ
			FunLcdModeWash();
		}
		//�ⲿ���ƴ���ʱ
		else if (u8OutCtrlState)
		{
			if (fg_hand_close)
			{
				u8OutCtrlState = 0;
			}
			//ת�ٸı�ʱ������Ļ
			if (gu16RunSpd_bef != gu16RunSpd)
			{
				gu8BakLgtDlyTm = 120;
			}
			//�ⲿ���ƹػ�
			if (u8OutCtrlState & 0xf0) //ǰ4λΪ1ʱ��ʾ�ػ�
			{
				u8fg_SysIsOn = 0; //�ػ�
			}
			//�ⲿ���ƿ���
			else
			{
				//����������
				if (ValBit(u8OutCtrlState, Ctrl_Din))
				{
					FunModeOutCtrl_Din(); // Din����ʱ�Ĺ���ʵ��
				}
				//ģ��������
				else if (ValBit(u8OutCtrlState, Ctrl_Analog))
				{
					FunModeOutCtrl_Analog(); //ģ��������ʱ�Ĺ���ʵ��
				}
				// 485����
				else if (ValBit(u8OutCtrlState, Ctrl_485))
				{
					FunModeOutCtrl_485(); // 485����ʱ�Ĺ���ʵ��
				}
				//--------------------------ת������
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD��ʾ
				FunLcdOutCtrl(gu16RunSpd);
			}
		}
		//��ͨģʽʱ
		else if (gu8SetMode)
		{
			//ת��ģʽʱ
			if (gu8SetMode == SPEED)
			{
				//---------------------------�л�
				//�����ʱ�����붨ʱģʽ
				if (gu8KeyFlg == bTime)
				{
					gu8SetMode = TIME;
				}
				//��control����������������ģʽ
				else if (gu8KeyFlg == bControl) // YXL-5
				{
					gu8SetMode = CONTROL;
				}
				//������������뷴��ϴģʽ
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //���뷴��ϴģʽ
				}
				//�����������л������볬Ƶ����
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //���볬Ƶ����ģʽ
				}

				//--------------------------����
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
					count_dis_WaterPress_value_tm = 15; //��ʾѹ��ֵ
				}

				//--------------------------ת������
				if (u8ModeSpeed_SpeedSet > 40)
					u8ModeSpeed_SpeedSet = 40;
				if (u8ModeSpeed_SpeedSet < 26)
					u8ModeSpeed_SpeedSet = 26;
				gu16RunSpd = u16LadderSpd[u8ModeSpeed_SpeedSet - 24];
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD��ʾ
				//�ޱ�������ʱ
				if (gu16TripFlg_Warn == 0)
				{
					FunLcdModeSpeed(u8ModeSpeed_SpeedSet); //ת��ģʽʱ����ʾ
				}
				//��������ʱ
				else
				{
					FunLcdWarnSpeed(gu16RunSpd); //ת��ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��
				}
			}
			//����ģʽʱ
			else if (gu8SetMode == CONTROL)
			{
				//---------------------------�л�
				//�����ʱ�����붨ʱģʽ
				if (gu8KeyFlg == bTime)
				{
					gu8SetMode = TIME;
				}
				//����л�������ת��ģʽ
				else if (gu8KeyFlg == bControl) // YXL-5
				{
					gu8SetMode = SPEED;
				}
				//������������뷴��ϴģʽ
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //���뷴��ϴģʽ
				}
				//�����������л������볬Ƶ����
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //���볬Ƶ����ģʽ
				}

				//--------------------------����
				if (count_5s_dis_realwater >= 10) // yxl-5  �ɵ�
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
					count_dis_WaterPress_value_tm = 15; //��ʾѹ��ֵ
				}
				else if (gu8KeyFlg == bLongControlUp)
				{
					FunModeGate_UnitSwitch(1);
				}
				else if (gu8KeyFlg == bLongControlDown)
				{
					FunModeGate_UnitSwitch(-1);
				}

				//--------------------------ת������
				//�ڵ�λ�л�����Ӱ��״̬(һ�㲻��)
				if (fg_ModeUnit_IsChange)
				{
					gu16RunSpd = gu16RunSpd_bef;
				}
				//���ڵ�λ�л�����Ӱ��״̬
				else
				{
					gu16RunSpd = u16LadderSpd[16] * u16ModeGate_GateSet / FunModeGate_StudyMax(fg_ModeGateUnit);
				}
				gu16RunSpd = SpmTempProtect(gu16RunSpd);
				gu16RunSpd = AcVoltLmt(gu16RunSpd);
				//--------------------------LCD��ʾ
				//�ޱ�������ʱ
				if (gu16TripFlg_Warn == 0)
				{
					FunLcdModeGate(u16ModeGate_GateSet, fg_ModeGateUnit); //����ģʽʱ��������ʾ
				}
				//��������ʱ
				else
				{
					FunLcdWarnGate(gu16RunSpd, fg_ModeGateUnit); //����ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��
				}
			}
			//��ʱģʽʱ
			else if (gu8SetMode == TIME)
			{
				//---------------------------�л�
				//�ڶ�ʱ����ʱ�ĳ�����ʱ��������ԭ��ģʽ
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
				//����л������˻�ת�ٻ�����ģʽ
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
				//�����л����ϼ����ڶ�ʱΪ��������ʱ�����л�������λ
				else if (gu8KeyFlg == bLongControlUp)
				{
					if (fg_time_spd_contorl)
					{
						FunModeTime_UnitSwitch(1);
						gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
					}
				}
				//�����л����¼����ڶ�ʱΪ��������ʱ�����л�������λ
				else if (gu8KeyFlg == bLongControlDown)
				{
					if (fg_time_spd_contorl)
					{
						FunModeTime_UnitSwitch(-1);
						gu8SetTimeTm = 20; //��ʱ����ʱ��ˢ��
					}
				}
				//������������뷴��ϴģʽ
				else if (gu8KeyFlg == bWash)
				{
					FunModeToNormalWash(); //���뷴��ϴģʽ
				}
				//�����������л������볬Ƶ����
				else if (gu8KeyFlg == bLongUnlockControl)
				{
					FunModeToSuperWash(); //���볬Ƶ����ģʽ
				}

				//--------------------------����
				//���붨ʱ����
				if (gu8SetTimeTm == 0 && gu8KeyFlg == bTime) //���¶�ʱ��
				{
					for (u8 i = 0; i < 4; i++)
					{
						SetStartTm[i] = gs16RunStartTm[i];
						SetEndTm[i] = gs16RunEndTm[i];
						u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i];
						u8ModeTime_GateSet_Tem[i] = u8ModeTime_GateSet[i];
					}
					gu16SetNowTm = gs16RunNowTm;

					gu8SetTimeState = 0; //��ʱ���õ�һ������
					gu8SetTimeTm = 20;	 //���붨ʱ���ú������ʱ��
				}
				//������ʱ���˳���ʱ���ã��˳���ʱģʽʱҲͬ��һ����
				else if (gu8KeyFlg == bLongTime)
				{
					gu8SetTimeTm = 0; //�˳���ʱ����
				}

				//��ʱ����ʱ�������趨ֵ���Ҳ����趨��Сֵ�Ƚ�����
				if (gu8SetTimeTm)
				{
					gu8SetTimeQueryTm = 0;		//��ʱ����ʱ��ѯ��ѯʱ��ˢ��
					count_5s_dis_realwater = 0; //��ʱ�������ˣ����޶���Χ
				}

				//��ʱ����ʱ
				if (gu8SetTimeTm)
				{
					FunModeTimeSet(); //��ʱ����ʱ�����úͱ���ʱ��ʱ�䡢ʱ��Ρ�ת��
				}
				//��ʱ���С��ȴ����ѯʱ
				else
				{
					subSetTimeCtrl();  //������ʱ��󣬽��붨ʱ���л�����Чʱ�����˸�����ת������
					subSetTimeQuery(); //��ʱ����״̬�£����ϡ��¼��������ĸ�Timer�����á�
				}

				//--------------------------ת������
				if (ValBit(gu8SetTimeFlg, bInvalid))
				{
					gu16RunSpd = u16LadderSpd[10]; // ��ʱ����Чʱ��ǿ��70%ת������
				}
				else if (ValBit(gu8SetTimeFlg, bRun))
				{
					if (fg_time_spd_contorl == 0) //ת������
					{
						gu16RunSpd = u16LadderSpd[u8ModeTimeRunSpeed - 24];
					}
					else //��������
					{
						if (fg_ModeUnit_IsChange)
						{
							gu16RunSpd = gu16RunSpd_bef;
						}
						//���ڵ�λ�л�����Ӱ��״̬
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

				//--------------------------LCD��ʾ
				//�ޱ�������ʱ
				if (gu16TripFlg_Warn == 0)
				{
					//��ʱ����ʱ
					if (gu8SetTimeTm)
					{
						FunLcdModeTime_Set();
					}
					//��ʱ����ʱ
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
				//��������ʱ
				else
				{
					gu8SetTimeTm = 0; //�޷����붨ʱ����
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
			//����
			else
			{
				gu8SetMode = SPEED;
			}
		}
		//����
		else
		{
			gu8SetMode = SPEED;
		}
	}
	//�ػ�ʱ
	else
	{
	}
	/**************************�������*************************/
	//���ֹ���ʱ�ָ�
	subMotorTrip();
	//���ֹ���ʱ
	if ((gu16TripFlg_Board) || (gu16TripFlg_Driver))
	{
		bCompOnCmd = 0;			   //����������
		u8WashState = 0;		   //�˳�����
		gu16RunSpd = 0;			   //ת��Ϊ0��ͣ��
		memset(gu8DsipNum, 0, 16); //���������ʾ��׼����ʾ����
		//����ʱ��ʾ
		if (u8fg_SysIsOn)
		{
			FunLcdError(); //������ʾ
		}
	}
	//�޹���ʱ
	else
	{
		bCompOnCmd = 1;		//��������
		gu8TripQueryTm = 0; //������˸��ʾʱ��
	}

	/**************************ϵͳ����������*************************/
	FunReEnterSelfStudy(); //��ѧϰ����Ϻ���Ҫ���½���
	FunLcdOther();		   // WIFI����ѧϰʱ��λ��˸��
	FunKeyLockLight();	   //���������ͽ���ʱ���ĸ���
	FunLcdbrightness();	   //����Ͱ�������
	FunModeRemeber();	   //�豸ģʽ����
	FunErrorRemeber();	   //ϵͳ���ϼ���
	subClrKeyBit();		   //�����������
	gu16RunSpd_bef = gu16RunSpd;
	subSpdToMotor(gu16RunSpd); //����ʾ��ת�ٴ��������崮�ڷ��ͻ���
}

/******************************************************************
Name		:FunSysProRun
Input		:
Output	:
Comments:
******************************************************************/
//ϵͳִ��ģʽ���ϻ����Լ졢ͨ��
u8 fg_atext485Error = 1;
void FunSysProRun(void)
{
	//�����Լ�
	if (SysProState == SysProState_Test)
	{
		//�ж��Ƿ�����ⲿ485ͨѶ���ϣ�E206��
		if (gu8MonitComErrTm >= 30 && fg_atext485Error)
		{
			SetBit(gu16TripFlg_Board, Ow485error); //�����ⲿ485ͨѶ���ϣ�E206��//�����Լ�ģʽ���ⲿ485ͨѶ���ϣ�E206��ʱ������ע��
		}
		else
		{
			ClrBit(gu16TripFlg_Board, Ow485error); //û�г����ⲿ485ͨѶ���ϣ�E206��
		}

		subTestMode(); //�Լ�ʱ����������Һ����ʾ

		FunLcdbrightness(); //����Ͱ�������
		subMotorTrip();		//���ϻָ����
		subSpdToMotor(0);	//�Լ�ʱ�ٶ�0
	}
	//�����ϻ�
	else if (SysProState == SysProState_Aging)
	{
		FunLcdSysAging();				 //ϵͳ���ϻ���ʾ
		subSpdToMotor(u16LadderSpd[16]); //�ϻ�ʱ�ٶ�100%
	}
	//��������
	else if (SysProState == SysProState_Nol)
	{
		subSetMode(); //����ʱ�Ĺ�������
	}
	//������
	else
	{
		subSetMode(); //����ʱ�Ĺ�������
	}
}

//ϵͳ��ͨ��Դ��ǰ8s��ʼ��
void FunSysProInit(void)
{
	static u8 count_TestPress = 0;		   //�����Լ�İ�������
	ClrBit(gu16TripFlg_Board, Ow485error); //�������벻��E206
	if (SysProTime.PowerTm <= 20)
	{
		//------------------���3�ν������������Լ�
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
			u8SysTestTm = 0;				//�Լ�ʱ�俪ʼ
			SysProState = SysProState_Test; //�����Լ�ģʽ
		}
		//------------------���յ��������ϻ���־
		if (fg_ageing_ok)
		{
			SysProState = SysProState_Aging; //�����ϻ�ģʽ
		}
		//------------------����ʱ
		if (SysProTime.PowerTm < 6)
		{
			//		FunSysParamInit();	  //��ʼ��������һ���ǽ�ͨ��Դ��

			FunLcdSysUpPowerInit_0(); //ȫ��ʾ
			fg_light_pwm = 0;		  //ȫ��
		}
		else if (SysProTime.PowerTm < 12)
		{
			FunLcdSysUpPowerInit_1(); //��ʾ�����롢���롢ѹ��ֵ
			gu8DsipNum[16] = 0xff;	  //������ȫ��
			fg_light_pwm = 0;		  //ȫ��
		}
		else if (SysProTime.PowerTm <= 16)
		{
			FunForVersionToSet();	  //���ݲ������Ĳ���ѡ��汾�����øð汾��Ĭ����ֵ��Χ
			FunLcdSysUpPowerInit_1(); //��ʾ�����롢���롢ѹ��ֵ
			gu8DsipNum[16] = 0xff;	  //������ȫ��
			fg_light_pwm = 1;		  //����
		}
		else
		{
			SysProState = SysProState_Nol; //����ͨ��ģʽ
		}
	}
	//������
	else
	{
		SysProState = SysProState_Nol; //����ͨ��ģʽ
	}
	subClrKeyBit(); //�����������
}

/******************************************************************
							The End
******************************************************************/