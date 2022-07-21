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
/****����궨��****/
u8 TwinkleTm = 0; // 0.5s��˸ʱ��

/******����궨��******/

uchar gu8SetWastTm; //����ϴ����ʱ������ʾ������˸ʱ��

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
	2470, // 15 95%//yxl  ���ʲ��ȶ�
	2600  // 16 100%

};

/******************************************************************
variables in project
******************************************************************/
uint gu16TripFlg_Board = 0; //��������
uint gu16TripFlg_Board_Bef = 0;
uint gu16TripFlg_Driver = 0; //��ʾ�����
uint gu16TripFlg_Driver_bef = 0;
uint gu16TripFlg_Warn = 0; //���ٹ���
uint gu16TripFlg_Warn_bef = 0;

uchar gu8DsipNum[17]; //�Դ�
uchar gu8SetTimeFlashCnt = 0;

uchar gu8LcdInitTm = 15; //��ʾ������

uchar gu8TripQueryTm = 0;

void subDispRpm(uint data, uchar mode);

IC74HC595tag displayIc = {
	.clk = {GPIOB, GPIO_Pin_13},
	.sck = {GPIOB, GPIO_Pin_14},
	.sda = {GPIOB, GPIO_Pin_12},
	.en = {GPIOB, GPIO_Pin_15} // yxl-5
};

/*************ϵͳ��ͨ��Դ���ʼ����ʾ**********************/
//ȫ��ʾ
void FunLcdSysUpPowerInit_0(void)
{
	memset(gu8DsipNum, 0xFF, 17);
}
//��ʾ�����롢���롢ѹ��ֵ
void FunLcdSysUpPowerInit_1(void)
{

	memset(gu8DsipNum, 0, 17);
	//������
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

	//����
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

	//��ǰѹ��ֵ
	static u16 Temp_value_sensor_lcdinit = 0; //��ʾѹ��ֵ
	static u8 fg_FirstIn = 1;
	if (fg_FirstIn)
	{
		fg_FirstIn = 0;
		Temp_value_sensor_lcdinit = AD_fin_buf;
	}
	subDispWt(Temp_value_sensor_lcdinit, 1);
}

/*************ϵͳ���͹���**************************/
u32 gu16TripNum = 0;
//��ʾ����ʱ�䣬3s�л���һ��
void subDispTrip(void)
{
	static uchar u8LcdTripSn = 0;

	switch (u8LcdTripSn)
	{
	case 0: //�����ѹ�쳣E001
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Involterror))
		{
			gu16TripNum = 001;
			gu8TripQueryTm = 60;
			break;
		}
	case 1: //�������������ֵE002
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Incurroverh))
		{
			gu16TripNum = 002;
			gu8TripQueryTm = 60;
			break;
		}
	case 2: //ɢ��������E101
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, IpmTempoverh))
		{
			gu16TripNum = 101;
			gu8TripQueryTm = 60;
			break;
		}
	case 3: //ɢ��������������E102
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Ipmnotemp))
		{
			gu16TripNum = 102;
			gu8TripQueryTm = 60;
			break;
		}
	case 4: //������������E103
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Insysdriverr))
		{
			gu16TripNum = 103;
			gu8TripQueryTm = 60;
			break;
		}
	case 5: //���ȱ�ౣ��E104
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotmisitem))
		{
			gu16TripNum = 104;
			gu8TripQueryTm = 60;
			break;
		}
	case 6: //��������������·����E105
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Incurrerror))
		{
			gu16TripNum = 105;
			gu8TripQueryTm = 60;
			break;
		}
	case 7: // DC��ѹ�쳣E106
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Indccurrer))
		{
			gu16TripNum = 106;
			gu8TripQueryTm = 60;
			break;
		}
	case 8: // PFC����E107
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inpfcprot))
		{
			gu16TripNum = 107;
			gu8TripQueryTm = 60;
			break;
		}
	case 9: //������ʳ���E108
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotoverload))
		{
			gu16TripNum = 108;
			gu8TripQueryTm = 60;
			break;
		}
	case 10: //�����������·����E201
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inmotcurrerr))
		{
			gu16TripNum = 201;
			gu8TripQueryTm = 60;
			break;
		}
	case 11: //������EEPROM��д����E202
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Insyseepeer))
		{
			gu16TripNum = 202;
			gu8TripQueryTm = 60;
			break;
		}
	case 12: // RTCʱ�Ӷ�д����E203
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Owrtcerror))
		{
			gu16TripNum = 203;
			gu8TripQueryTm = 60;
			break;
		}
	case 13: //����EEPROM��д����E204
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Oweeperror))
		{
			gu16TripNum = 204;
			gu8TripQueryTm = 60;
			break;
		}
	case 14: //������������ͨ�Ź���E205
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inuarterr))
		{
			gu16TripNum = 205;
			gu8TripQueryTm = 60;
			break;
		}
	case 15: //�ⲿ485ͨѶ����E206
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Ow485error))
		{
			gu16TripNum = 206;
			gu8TripQueryTm = 60;
			break;
		}
	case 16: //���ձ���E207
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Driver, Inlowflowerr))
		{
			gu16TripNum = 207; // yxl-e207
			gu8TripQueryTm = 60;
			break;
		}
	case 17: //����������E208
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Ownowaterpr))
		{
			gu16TripNum = 208;
			gu8TripQueryTm = 60;
			break;
		}
	case 18: //����ʧ��E209
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Board, Owupwaterfail))
		{
			gu16TripNum = 209; // yxl-e207
			gu8TripQueryTm = 60;
			break;
		}
	case 19: // ����Ԥ������AL01
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Warn, LsHighTemp)) // ����Ԥ������
		{
			gu16TripNum = 901;
			gu8TripQueryTm = 60;
			break;
		}
	case 20: // ACǷѹ����AL02
		u8LcdTripSn++;
		if (ValBit(gu16TripFlg_Warn, Lslowvolt))
		{
			gu16TripNum = 902;
			gu8TripQueryTm = 60;
			break;
		}
	case 21: //��������AL03
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
//������ʾ(3sһ��)
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
//ͣ��ʱ���ϵ���ʾ(����Ԥ������)
void FunLcdError(void)
{
	// 3��һ�����ϴ��룬��˸��ʾ3��
	if ((gu16TripFlg_Board)OR(gu16TripFlg_Driver)) // yxl-5
	{
		if (gu8TripQueryTm EQU 0)
			subDispTrip();
		else if ((gu8TripQueryTm % 20) > 10)
			subDispTripFlash();
	}
}

//ת��ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��
void FunLcdWarnSpeed(const u32 spd)
{
	u32 data;
	static u8 Warn_TwinkleTm_bef = 0xff;
	static u8 count_WarnTwinkle = 0;
	u8 spdset;
	//��ʼ����ʱ�����г�ʼ��
	if (gu16TripFlg_Warn_bef == 0 && gu16TripFlg_Warn)
	{
		count_WarnTwinkle = 0;
	}
	//ÿ0.5s������+1
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
		//��ʾת��
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
			gu8DsipNum[5] |= One_SegFig; //%��λ
		}
		//��ʾAL01
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
		//��ʾAL02
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
		//��ʾAL03
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
	//��ʾѹ��ֵ
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//������ʾ����
	else
	{
		subDispWt(gu16MotorWt, 1);	 //����
		gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	}
}
//����ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��
void FunLcdWarnGate(const u32 spd, const u8 unit)
{
	static u8 Warn_TwinkleTm_bef = 0xff;
	static u8 count_WarnTwinkle = 0;
	u32 gate = 0;
	//��ʼ����ʱ�����г�ʼ��
	if (gu16TripFlg_Warn_bef == 0 && gu16TripFlg_Warn)
	{
		count_WarnTwinkle = 0;
	}
	//ÿ0.5s������+1
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
		//��ʾת��
		if (count_WarnTwinkle <= 6)
		{
			subDispRpm(gate, 1); //����
			switch (unit)		 //��λ
			{
			case Unit_m3h:
				gu8DsipNum[4] |= One_SegFig; // m3/h��λ
				break;
			case Unit_Imp:
				gu8DsipNum[6] |= One_SegFig; // IMP gpm��λ
				break;
			case Unit_Lmin:
				gu8DsipNum[2] |= One_SegFig; // L/min��λ
				break;
			case Unit_Us:
				gu8DsipNum[3] |= One_SegFig; // US gpm��λ
				break;
			default:
				break;
			}
		}
		//��ʾAL01
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
		//��ʾAL02
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
		//��ʾAL03
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
	//��ʾѹ��ֵ
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//������ʾ����
	else
	{
		subDispWt(gu16MotorWt, 1);	 //����
		gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	}
}

/***************************************/
//�ػ����ܵ�ʹ��ʱ����ʾ
void FunLcdModeShutDown(void)
{
	u32 data;
	//����汾����ʾ
	if (u8SD_SoftWareVerTm)
	{
		data = gstCodeTime.Day * 100;
		data += (gstCodeTime.Ver & 0x00ff);
		subDispRpm(data, 0); //
	}
	//�������õ���ʾ
	else if (u8SD_ParamSetTm)
	{
		subDispWt(u8SD_ParamSetOrder + 1, 1); //�����ϵĵڼ���Ŀ��ʾ
		if (gu8SetTimeFlashCnt % 2)
		{
			if (u8SD_ParamSetOrder EQU 0) //����1
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd1 - 26);
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 1) //����2
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd2 - 26);
				subDispRpm(data, 1);
			}

			if (u8SD_ParamSetOrder EQU 2) //����3
			{
				data = 30 + 5 * (u8SD_ParamSet_OCtrlSpd3 - 26);
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 3) // ����4
			{
				data = 80 + 5 * u8SD_ParamSet_HandWashSpd;
				subDispRpm(data, 1);
			}
			if (u8SD_ParamSetOrder EQU 4) //����5
			{
				if (u8fg_SD_ParamSet_AnalogSel == 1)
					subDispRpm(1, 1);
				if (u8fg_SD_ParamSet_AnalogSel == 0)
					subDispRpm(0, 1);
			}
			if (u8SD_ParamSetOrder EQU 5) //����6
			{
				subDispRpm(u8SD_PowerOnTm, 1);
			}
		}
		// 0��1��2��3��ʾ%��λ��4��5��6��7����ʾ
		if (u8SD_ParamSetOrder <= 3)
		{
			gu8DsipNum[5] |= One_SegFig; //��λ%��ʾ
		}
		else
		{
			gu8DsipNum[5] &= ~One_SegFig; //����ʾ&��λ
		}
	}
	//��ѹУ׼��ʾ��CAL1
	else if (u8SD_ReStoreDisWaterTm)
	{
		gu8DsipNum[6] = u8LcdDispTab1[1];
		gu8DsipNum[5] = u8LcdDispTab1[17];
		gu8DsipNum[4] = u8LcdDispTab1[10];
		gu8DsipNum[15] = u8LcdDispTab1[12];
	}
	//�������Ƿ�����־��A--0/1
	else if (u8SD_PSensorDisTm)
	{
		gu8DsipNum[6] = u8LcdDispTab1[u8fg_SD_PSensorOpen];
		gu8DsipNum[5] = u8LcdDispTab1[22];
		gu8DsipNum[4] = u8LcdDispTab1[22];
		gu8DsipNum[15] = u8LcdDispTab1[10];
	}
	// ADУ׼ֵ��ʾ
	else if (u8SD_PressAdDisTm)
	{
		if (temp_cail_add > 0)
		{
			data = temp_cail_add;
			gu8DsipNum[4] |= One_SegFig; //������λ
		}
		else if (temp_cail_subtra > 0)
		{
			data = temp_cail_subtra;
			gu8DsipNum[5] |= One_SegFig; //%��λ
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
//����ʱ����ʾ
void FunLcdModeWash(void)
{
	subDispWt(gu16MotorWt, 1);	 //����
	gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	u32 data;
	// 5s��˸ʱ��
	if (gu8SetWastTm > 0)
	{
		WashTm = WashTmSet;
		if (TwinkleTm % 2)
		{
			data = WashTmSet;
			subDispRpm(data, 1); //����ʱ��
		}
	}
	//��˸���ʼ����
	else
	{
		data = WashTm;
		subDispRpm(data, 1); //����ʱ��
	}
}

/***************************************/
//�ⲿ����ʱ����ʾ
void FunLcdOutCtrl(const u32 spd)
{
	subDispWt(gu16MotorWt, 1);	 //����
	gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	u32 data = 30;
	if (spd > u16LadderSpd[16])
		data = 100;
	//��ת�ٵ���ֵת��Ϊ%��λ����ֵ
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
	gu8DsipNum[5] |= One_SegFig; //%��λ
}

/***************************************/
//ת��ģʽʱ����ʾ
void FunLcdModeSpeed(const u8 spdset)
{
	u32 data = 30;
	//��ʾѹ��ֵ
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//������ʾ����
	else
	{
		subDispWt(gu16MotorWt, 1);	 //����
		gu8DsipNum[1] |= One_SegFig; //����Wͼ��
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
	gu8DsipNum[5] |= One_SegFig; //%��λ
}

//����ģʽʱ����ʾ
void FunLcdModeGate(const u16 gate, const u8 fg_unit)
{
	//��ʾѹ��ֵ
	if (count_dis_WaterPress_value_tm)
	{
		subDispWt(WaterPress_value, 1);
	}
	//������ʾ����
	else
	{
		subDispWt(gu16MotorWt, 1);	 //����
		gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	}

	subDispRpm(gate, 1); //����

	switch (fg_unit)
	{
	case Unit_m3h:
		gu8DsipNum[4] |= One_SegFig; // m3/h��λ
		break;
	case Unit_Imp:
		gu8DsipNum[6] |= One_SegFig; // IMP gpm��λ
		break;
	case Unit_Lmin:
		gu8DsipNum[2] |= One_SegFig; // L/min��λ
		break;
	case Unit_Us:
		gu8DsipNum[3] |= One_SegFig; // US gpm��λ
		break;
	default:
		break;
	}
}

//��ʱģʽʱ����ʾ������ʱ��
void FunLcdModeTime_Set(void)
{
	u32 data;
	//ʱ��������ʾ
	if (gu8SetTimeState < 3)
	{
		//ʱ��Сʱ��˸
		if (gu8SetTimeState == 1)
		{
			if (TwinkleTm % 2) //��˸��ʾ
			{
				subDispNowHour(gu16SetNowTm / 60, 1); //ʱ��Сʱ
			}
			subDispNowMin(gu16SetNowTm % 60, 1); //ʱ�ӷ���
		}
		//ʱ�ӷ�����˸
		else if (gu8SetTimeState == 2)
		{
			subDispNowHour(gu16SetNowTm / 60, 1); //ʱ��Сʱ
			if (TwinkleTm % 2)					  //��˸��ʾ
			{
				subDispNowMin(gu16SetNowTm % 60, 1); //ʱ�ӷ���
			}
		}
		gu8DsipNum[7] |= One_SegFig; // P1�ָ�ͼ��":"��ʾ
	}
	//ʱ���ʱ��
	else
	{
		//������ʾ
		if (fg_time_spd_contorl)
		{
			data = u8ModeTime_GateSet_Tem[gu8SetTimeTmNum];
			subDispRpm(data, 1);		  //ת�ٻ�������ʾ
			subDispUnit(fg_ModeGateUnit); //������λ
		}
		//ת����ʾ
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
			subDispRpm(data, 1);		 //ת�ٻ�������ʾ
			gu8DsipNum[5] |= One_SegFig; //%��λ
		}

		subDispWt(gu16MotorWt, 1);							   //������ʾ						  //����
		gu8DsipNum[1] |= One_SegFig;						   //���ʵ�λ					  //����Wͼ��
		subDispStartHour(SetStartTm[gu8SetTimeTmNum] / 60, 1); //��ʼСʱ
		subDispStartMin(SetStartTm[gu8SetTimeTmNum] % 60, 1);  //��ʼ����
		subDispEndHour(SetEndTm[gu8SetTimeTmNum] / 60, 1);	   //����Сʱ
		subDispEndMin(SetEndTm[gu8SetTimeTmNum] % 60, 1);	   //��������
		gu8DsipNum[7] |= One_SegFig;						   // Tm1�ָ�ͼ��":"��ʾ
		gu8DsipNum[10] |= One_SegFig;						   // Tm1��2�ָ�ͼ��"-"��ʾ
		gu8DsipNum[13] |= One_SegFig;						   // Tm2�ָ�ͼ��":"��ʾ

		//����1-4��ʾ
		if (gu8SetTimeTmNum EQU 0)
			gu8DsipNum[8] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 1)
			gu8DsipNum[9] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 2)
			gu8DsipNum[11] |= One_SegFig;
		else if (gu8SetTimeTmNum EQU 3)
			gu8DsipNum[12] |= One_SegFig;

		if (TwinkleTm % 2 == 0) //��˸��ʾ�����⣬���κ������˸Ч����
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
//��ʱģʽʱ����ʾ��ʱ���ȫ����Ч����
void FunLcdModeTime_Invalid(void)
{
	subDispWt(gu16MotorWt, 1);	 //����
	gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	subDispRpm(70, 1);			 //ת��
	gu8DsipNum[5] |= One_SegFig; //%��λ

	//��ʼСʱ��--
	gu8DsipNum[7] = u8LcdDispTab1[22];
	gu8DsipNum[8] = u8LcdDispTab1[22];
	//��ʼ���ӣ�--
	gu8DsipNum[9] = u8LcdDispTab1[22];
	gu8DsipNum[10] = u8LcdDispTab1[22];
	//����Сʱ��--
	gu8DsipNum[11] = u8LcdDispTab1[22];
	gu8DsipNum[12] = u8LcdDispTab1[22];
	//�������ӣ�--
	gu8DsipNum[13] = u8LcdDispTab1[22];
	gu8DsipNum[14] = u8LcdDispTab1[22];

	gu8DsipNum[7] |= One_SegFig;  // Tm1�ָ�ͼ��":"��ʾ
	gu8DsipNum[10] |= One_SegFig; // Tm1��2�ָ�ͼ��"-"��ʾ
	gu8DsipNum[13] |= One_SegFig; // Tm2�ָ�ͼ��":"��ʾ
}
//��ʱģʽʱ����ʾ����������
void FunLcdModeTime_Run(void)
{
	subDispWt(gu16MotorWt, 1);	 //����
	gu8DsipNum[1] |= One_SegFig; //����Wͼ��
	//����ʱ��ѯ
	if (gu8SetTimeQueryTm)
	{
		//ת������
		if (fg_time_spd_contorl == 0)
		{
			FunLcdModeSpeed(u8ModeTime_SpeedSet[gu8SetTimeQuerySn]); //����������ʾ
		}
		//��������
		else
		{
			FunLcdModeGate(u8ModeTime_GateSet_Tem[gu8SetTimeQuerySn], fg_ModeGateUnit);
		}
		//ʱ�ӡ����������ʾ
		subDispStartHour(SetStartTm[gu8SetTimeQuerySn] / 60, 1); //��ʼСʱ
		subDispStartMin(SetStartTm[gu8SetTimeQuerySn] % 60, 1);	 //��ʼ����
		subDispEndHour(SetEndTm[gu8SetTimeQuerySn] / 60, 1);	 //����Сʱ
		subDispEndMin(SetEndTm[gu8SetTimeQuerySn] % 60, 1);		 //��������
		gu8DsipNum[7] |= One_SegFig;  // Tm1�ָ�ͼ��":"��ʾ
		gu8DsipNum[10] |= One_SegFig; // Tm1��2�ָ�ͼ��"-"��ʾ
		gu8DsipNum[13] |= One_SegFig; // Tm2�ָ�ͼ��":"��ʾ
		//����1-4��ʾ����˸
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
	//���в���ѯ
	else
	{
		//ת������
		if (fg_time_spd_contorl == 0)
		{
			FunLcdModeSpeed(u8ModeTimeRunSpeed); //����������ʾ
		}
		//��������
		else
		{
			FunLcdModeGate(u8ModeTimeRunGate, fg_ModeGateUnit);
		}
		//ʱ�ӡ����������ʾ
		subDispStartHour(SetStartTm[gu8SetTimeTmNum] / 60, 1); //��ʼСʱ
		subDispStartMin(SetStartTm[gu8SetTimeTmNum] % 60, 1);  //��ʼ����
		subDispEndHour(SetEndTm[gu8SetTimeTmNum] / 60, 1);	   //����Сʱ
		subDispEndMin(SetEndTm[gu8SetTimeTmNum] % 60, 1);	   //��������
		gu8DsipNum[7] |= One_SegFig;  // Tm1�ָ�ͼ��":"��ʾ
		gu8DsipNum[10] |= One_SegFig; // Tm1��2�ָ�ͼ��"-"��ʾ
		gu8DsipNum[13] |= One_SegFig; // Tm2�ָ�ͼ��":"��ʾ
		//����1-4��ʾ
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
//��ʱģʽʱ����ʾ���ȴ�����
void FunLcdModeTime_Stay(void)
{

	u8 TimeSel = 0;
	u32 data;
	if (gu8SetTimeQueryTm) //�ȴ���ѯʱ
		TimeSel = gu8SetTimeQuerySn;
	else //�ȴ�����ѯʱ
		TimeSel = gu8SetTimeTmNum;

	//ת������
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
		subDispRpm(data, 1);		 //ת�ٻ�������ʾ
		gu8DsipNum[5] |= One_SegFig; //%��λ
	}
	//��������
	else
	{
		data = u8ModeTime_GateSet_Tem[TimeSel];
		subDispRpm(data, 1);		  //ת�ٻ�������ʾ
		subDispUnit(fg_ModeGateUnit); //������λ
	}
	subDispStartHour(SetStartTm[TimeSel] / 60, 1); //��ʼСʱ
	subDispStartMin(SetStartTm[TimeSel] % 60, 1);  //��ʼ����
	subDispEndHour(SetEndTm[TimeSel] / 60, 1);	   //����Сʱ
	subDispEndMin(SetEndTm[TimeSel] % 60, 1);	   //��������

	gu8DsipNum[7] |= One_SegFig;  // Tm1�ָ�ͼ��":"��ʾ
	gu8DsipNum[10] |= One_SegFig; // Tm1��2�ָ�ͼ��"-"��ʾ
	gu8DsipNum[13] |= One_SegFig; // Tm2�ָ�ͼ��":"��ʾ
	//����1-4��ʾ����˸
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
//ϵͳ���Լ���ʾ
void FunLcdSysTest_0(void)
{
	memset(gu8DsipNum, 0, 17);
	//��2.5s�ڣ�lcdҺ������˸3�Σ�ȫ������ͼ�����˸��ʾΪ����8
	if (u8SysTestTm < 5) // yxl-5
	{
		if ((u8SysTestTm % 2) EQU 0)
		{
			memset(gu8DsipNum, 0xFF, 17);
		}
	}
	//��3�뿪ʼ��ÿ0.5s�ӵ�һ������ͼ������������ʾ����8
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

		////20211222 //////����wifi����

		mcu_start_wifitest(); // yxl-wifi ����wifi����
		WifiTestOk = 0;
		////20211222
	}
}
void FunLcdSysTest_1(const u8 TestNum)
{
	u32 data;
	//��ʾ���Ĺ������ֲ��֡�ת�����ֲ��֡������ʱ�����ֲ�����ʾΪ0
	data = TestNum + TestNum * 10 + TestNum * 100 + TestNum * 1000;
	subDispWt(data, 0);
	subDispRpm(data, 0);
	data = TestNum + TestNum * 10;
	subDispStartHour(data, 0); //��ʼʱ��Сʱ������
	subDispStartMin(data, 0);  //��ʼʱ����ӵ�����
	subDispEndHour(data, 0);   //����ʱ��Сʱ������
	subDispEndMin(data, 0);	   //����ʱ�����	������

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

	//������Ϊ10101010ʱ
	if (switch_key == 0x55)
	{
		gu8DsipNum[1] |= One_SegFig; //ͼ��W��ʾ
		gu8DsipNum[0] |= One_SegFig; // yxl-5
		gu8DsipNum[6] |= One_SegFig;
		gu8DsipNum[14] |= One_SegFig; // YXL-5  �ӵ�
		gu8DsipNum[2] |= One_SegFig;
		gu8DsipNum[5] |= One_SegFig; // yxl-5
		gu8DsipNum[3] |= One_SegFig;
		gu8DsipNum[15] |= One_SegFig; // yxl-5 �ӵ�
		gu8DsipNum[4] |= One_SegFig;
		gu8DsipNum[7] |= One_SegFig; // yxl-5
		gu8DsipNum[10] |= One_SegFig;
		gu8DsipNum[13] |= One_SegFig;
	}
	else
	{
		gu8DsipNum[1] &= ~One_SegFig; //ͼ��W��ʾ
		gu8DsipNum[0] &= ~One_SegFig; // yxl-5
		gu8DsipNum[6] &= ~One_SegFig;
		gu8DsipNum[14] &= ~One_SegFig; // YXL-5  �ӵ�
		gu8DsipNum[2] &= ~One_SegFig;
		gu8DsipNum[5] &= ~One_SegFig;
		gu8DsipNum[3] &= ~One_SegFig;
		gu8DsipNum[15] &= ~One_SegFig; // yxl-5  �ӵ�
		gu8DsipNum[4] &= ~One_SegFig;
		gu8DsipNum[7] &= ~One_SegFig;
		gu8DsipNum[10] &= ~One_SegFig;
		gu8DsipNum[13] &= ~One_SegFig;
	}

	/////////////20211222
	////////�Լ�ʱ����wifi���Գɹ�Ϊ1����wifiͼ����
	if (WifiTestOk == 1)
	{
		gu8DsipNum[0] |= One_SegFig; // yxl-5
	}
	else
	{
		gu8DsipNum[0] &= ~One_SegFig; // yxl-5
	}
	/////////////20211222

	//ģ������ʹ������������
	//ģ���������
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
	//�������������
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
u8 SysAgingTwinTm = 0; //�ϻ���˸ʱ�䣬1min��������
//ϵͳ���ϻ���ʾ
void FunLcdSysAging(void)
{
	memset(gu8DsipNum, 0xff, 17); //ȫ��
	if (SysAgingTwinTm <= 120)
	{
		fg_light_pwm = 0; //ȫ��
	}
	else
	{
		fg_light_pwm = 1; //����
	}
}

//ϵͳ������������ʾ
void FunLcdOther(void)
{
	//��ѧϰԤ��ʱ����˸��ʾ
	if ((fg_flash_rest_con == 1) && (gu8SetMode > 0))
	{
		if (TwinkleTm % 2) //��˸��ʾ
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
	// wifiͼ��
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

//�����Լ�ģʽʱ��Һ��������ʾ
void FunWriteRam_1729(void)
{
	//�����Ƹ���gu8DsipNum[16]λ�ĸߵ����������
	IC74HC595_Write(&displayIc, (gu8DsipNum[16] << 1)); //����һλ����Ϊǰ��汾�Ķ�������595оƬ���Ӹպ�������һ�ӿ�
	SendDataToHt1621_Sub(gu8DsipNum);					// LCDҺ��������gu8DsipNum��������ֵ������ʾ
	memset(gu8DsipNum, 0x00, 17);						//������棬׼����һ����ʾ
}

//����ʱ������ȫ��������ʱֻ�н�������
void FunKeyLockLight(void)
{
	//��������ʱ
	if (gu8LockKeyTm > 0)
	{
		gu8DsipNum[16] |= (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F);
	}
	//��������ʱ
	else
	{
		gu8DsipNum[16] |= (One_Seg_E);
	}
}
//����Ͱ�������
void FunLcdbrightness(void)
{
	//ȫ��
	if (gu8BakLgtDlyTm)
	{
		fg_light_pwm = 0; //ȫ��
	}
	//����
	else
	{
		fg_light_pwm = 1; //������
	}
}
