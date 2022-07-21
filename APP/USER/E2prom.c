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
u8 u8fg_E2_SysFirstIn = 1; //��ͨ��Դ���ʼ�����½���
//����ǰ��ʱ�ӳ�ʼ����ֻ���ϵ�ʱ��ȡһ��
static void FunE2_SysInit(void)
{
	uchar i, sum;
	static u8 E2RdAddress = 0;
	static u8 fg_InCheck = 0;	//�Ƿ���
	static u8 E2CheckOrder = 0; //��ȡ˳��
	static u8 E2CheckCount = 3; //������
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 3; // 1.5s��һ��
		fg_InCheck = 1;	  //������
		E2CheckOrder = 0; //��ȡ˳���ͷ��ʼ
		for (i = 0; i < 200; i++)
			gu8E2Para[i] = 0;
		if (u8fg_E2_SysFirstIn)
		{
			u8fg_E2_SysFirstIn = 0;
			E2CheckCount = 3; //������
		}
	}
	if (fg_InCheck == 1 && E2CheckCount)
	{
		//���ζ�ȡ10��
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
			E2CheckCount--; //������
			sum = 0;
			for (i = 0; i < 199; i++)
			{
				sum += gu8E2Para[i];
			}
			sum = ~sum;
			if ((gu8E2Para[0] != 0xaa && gu8E2Para[0] != 0xbb) || gu8E2Para[199] != sum)
			{
				// e2оƬ��һ�ΰ�װ���ϵ�ʱȫΪ0xff
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
				//��ȡ����ʱ
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
			/******ϵͳ***************/
			u8fg_SysIsOn = gu8E2Para[1]; //���ػ�
			if (u8fg_SysIsOn)
			{
				gu8LockKeyTm = 120;
				gu8BakLgtDlyTm = 120;
			}
			fg_hand_close = gu8E2Para[2]; //�ֶ��ػ���־
			if (u8fg_SysIsOn || fg_hand_close > 1)
			{
				fg_hand_close = 0;
			}
			gu8SetMode = gu8E2Para[3]; //��ǰģʽ
			if (gu8SetMode == 0 || gu8SetMode > 3)
			{
				gu8SetMode = SPEED;
			}
			gu8SetMode_ago = gu8E2Para[4]; //ǰһ��ģʽ
			if (gu8SetMode_ago == 0 || gu8SetMode_ago > 3)
			{
				gu8SetMode_ago = SPEED;
			}

			/***********ת��ģʽ**************************/
			u8ModeSpeed_SpeedSet = gu8E2Para[7]; //ת��ģʽ��ת����������26-40��
			if (u8ModeSpeed_SpeedSet < 26 || u8ModeSpeed_SpeedSet > 40)
			{
				u8ModeSpeed_SpeedSet = 36;
			}
			/***********����ģʽ**************************/
			u16ModeGate_GateSet = gu8E2Para[8]; //����ģʽ�������趨ֵ
			fg_ModeGateUnit = gu8E2Para[9];		//����ģʽ�ĵ�λ
			u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, fg_ModeGateUnit);

			/***********��ʱģʽ**************************/
			for (u8 i = 0; i < 4; i++) //��ʱģʽ�ĸ�����ֵ
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
			fg_time_spd_contorl = gu8E2Para[40]; //��ʱģʽ��ת�������л�
			if (fg_time_spd_contorl > 1)
			{
				fg_time_spd_contorl = 0;
			}
			fg_ModeTimeUnit = gu8E2Para[41]; //��ʱģʽ�ĵ�λ
			if (fg_ModeTimeUnit > 3)
			{
				fg_ModeTimeUnit = 0;
			}
			/***********����������Ĳ���**************************/
			u8SD_ParamSet_OCtrlSpd1 = gu8E2Para[50];	// Din����ʱ��ת�����ã�Din1Ĭ��100%
			u8SD_ParamSet_OCtrlSpd2 = gu8E2Para[51];	// Din����ʱ��ת�����ã�Din1Ĭ��80%
			u8SD_ParamSet_OCtrlSpd3 = gu8E2Para[52];	// Din����ʱ��ת�����ã�Din1Ĭ��40%
			u8SD_ParamSet_HandWashSpd = gu8E2Para[53];	//�ֶ�����ʱ��ת������
			u8fg_SD_ParamSet_AnalogSel = gu8E2Para[54]; // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
			u8SD_PowerOnTm = gu8E2Para[55];				// 25Ϊ�����������Ƿ����25min��ˮ
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

			/***********�ػ�ʱ����*************************/
			fg_air_cali = gu8E2Para[60];		 // 1Ϊ�������Ƿ񴥷�����ѹ��У׼
			u8fg_SD_PSensorOpen = gu8E2Para[61]; //�Ƿ�������������
			if (fg_air_cali > 1)
			{
				fg_air_cali = 0;
			}
			if (u8fg_SD_PSensorOpen > 1)
			{
				u8fg_SD_PSensorOpen = 1;
			}

			/***********��������ѧϰ**************************/
			u8 ab[2] = {1,2};
			atexttabm = (ab[0]<<8)+ab[1];
			WashTmSet_Normal = gu8E2Para[65] * 30; //����ϴ������ʱ�����ã�������
			for (u8 i = 0; i < 6; i++)			   //�������ķ�Χ
			{
				// 66-113
				*(&ModeGate_m3h.StudyMax + i) = (gu8E2Para[66 + 8 * i] << 8) + gu8E2Para[67 + 8 * i];
				*(&ModeGate_Imp.TabMax + i) = (gu8E2Para[68 + 8 * i] << 8) + gu8E2Para[69 + 8 * i];
				*(&ModeGate_Lmin.TabMax + i) = (gu8E2Para[70 + 8 * i] << 8) + gu8E2Para[71 + 8 * i];
				*(&ModeGate_US.TabMax + i) = (gu8E2Para[72 + 8 * i] << 8) + gu8E2Para[73 + 8 * i];
			}
			temp_cail_add = (gu8E2Para[120] << 8) + gu8E2Para[121];	   // ADУ׼ֵ����
			temp_cail_subtra = (gu8E2Para[122] << 8) + gu8E2Para[123]; // ADУ׼ֵ����
			fg_rest_con_ok = gu8E2Para[125];						   //��ѧϰ��־
			if (temp_cail_add > 150)
			{
				temp_cail_add = 0;
			}
			if (temp_cail_subtra > 150)
			{
				temp_cail_add = 0;
			}

			/********��ʼ�����һ���ϵ��־��¼***************/
			u8FirstFunIn = gu8E2Para[130]; //�״��ϵ�����й���
		}
	}
}

//������д��e29pоƬ��ֻ�����ݷ����仯���Ż�����д�룩
static void FunE2_SysNol(void)
{
	uchar i, j, sum, len;
	u8 E2RdAddress = 0;
	static u8 u8fg_isWrIn = 0;	//�Ƿ������ݸ��ĵ�����Ҫ����д��
	static u8 fg_InCheck = 0;	//�Ƿ���
	static u8 E2CheckOrder = 0; //��ȡ˳��
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 6;
		fg_InCheck = 1;	  //������
		E2CheckOrder = 0; //��ȡ˳��
		u8fg_isWrIn = 0;  //�Ƿ������ݸ��ĵ�����Ҫ����д��
	}
	if (fg_InCheck == 1)
	{
		if (E2CheckOrder == 0)
		{
			E2CheckOrder++;
			j = 0;

			/******ϵͳ***************/
			if (gu8E2Para[1] != u8fg_SysIsOn) //���ػ�
				j++;
			if (gu8E2Para[2] != fg_hand_close) //�ֶ��ػ���־
				j++;
			if (gu8E2Para[3] != gu8SetMode) //��ǰģʽ
				j++;
			if (gu8E2Para[4] != gu8SetMode_ago) //ǰһ��ģʽ
				j++;
			/***********ת��ģʽ**************************/
			if (gu8E2Para[7] != u8ModeSpeed_SpeedSet) //ת��ģʽ��ת����������26-40��
				j++;
			/***********����ģʽ**************************/
			if (gu8E2Para[8] != u16ModeGate_GateSet) //����ģʽ�������趨ֵ
				j++;
			if (gu8E2Para[9] != fg_ModeGateUnit) //����ģʽ�ĵ�λ
				j++;
			/***********��ʱģʽ**************************/
			for (u8 i = 0; i < 4; i++) //��ʱģʽ�ĸ�����ֵ
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
			if (gu8E2Para[40] != fg_time_spd_contorl) //��ʱģʽ��ת�������л�
				j++;
			if (gu8E2Para[41] != fg_ModeTimeUnit) //��ʱģʽ�ĵ�λ
				j++;
			/***********����������Ĳ���**************************/
			if (gu8E2Para[50] != u8SD_ParamSet_OCtrlSpd1) // Din����ʱ��ת�����ã�Din1Ĭ��100%
				j++;
			if (gu8E2Para[51] != u8SD_ParamSet_OCtrlSpd2) // Din����ʱ��ת�����ã�Din1Ĭ��80%
				j++;
			if (gu8E2Para[52] != u8SD_ParamSet_OCtrlSpd3) // Din����ʱ��ת�����ã�Din1Ĭ��40%
				j++;
			if (gu8E2Para[53] != u8SD_ParamSet_HandWashSpd) //�ֶ�����ʱ��ת������
				j++;
			if (gu8E2Para[54] != u8fg_SD_ParamSet_AnalogSel) // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
				j++;
			if (gu8E2Para[55] != u8SD_PowerOnTm) // 25Ϊ�����������Ƿ����25min��ˮ
				j++;
			/***********�ػ�ʱ����*************************/
			if (gu8E2Para[60] != fg_air_cali) // 1Ϊ�������Ƿ񴥷�����ѹ��У׼
				j++;
			if (gu8E2Para[61] != u8fg_SD_PSensorOpen) //�Ƿ�������������
				j++;

			/***********��������ѧϰ**************************/
			if (gu8E2Para[65] != (WashTmSet_Normal / 30)) //����ϴ������ʱ�����ã�������
				j++;
			for (u8 i = 0; i < 6; i++) //�������ķ�Χ
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
			if (((gu8E2Para[120] << 8 + gu8E2Para[121])) != (temp_cail_add)) // ADУ׼ֵ
				j++;
			if (((gu8E2Para[123] << 8 + gu8E2Para[124])) != (temp_cail_subtra)) // ADУ׼ֵ
				j++;
			if (gu8E2Para[125] != fg_rest_con_ok) //��ѧϰ��־
				j++;

			/********��ʼ�����һ���ϵ��־��¼***************/
			if (gu8E2Para[130] != u8FirstFunIn) //�״��ϵ�����й���
				j++;

			/*********��������ʱ*****************************/
			//ʹ����gu8E2Para[140]����gu8E2Para[155];���⣺155�������������ֱ�ǩ
			if (ValBit(gu8SetFlg, bTripSave)) //��������ʱ�������д洢
				j++;

			//
			u8fg_isWrIn = j;
		}
		else if ((E2CheckOrder == 1) && u8fg_isWrIn)
		{
			E2CheckOrder++;
			gu8E2Para[0] = 0xaa;

			/******ϵͳ***************/
			gu8E2Para[1] = u8fg_SysIsOn;   //���ػ�
			gu8E2Para[2] = fg_hand_close;  //�ֶ��ػ���־
			gu8E2Para[3] = gu8SetMode;	   //��ǰģʽ
			gu8E2Para[4] = gu8SetMode_ago; //ǰһ��ģʽ

			/***********ת��ģʽ**************************/
			gu8E2Para[7] = u8ModeSpeed_SpeedSet; //ת��ģʽ��ת����������26-40��

			/***********����ģʽ**************************/
			gu8E2Para[8] = u16ModeGate_GateSet; //����ģʽ�������趨ֵ
			gu8E2Para[9] = fg_ModeGateUnit;		//����ģʽ�ĵ�λ

			/***********��ʱģʽ**************************/
			for (u8 i = 0; i < 4; i++) //��ʱģʽ�ĸ�����ֵ
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
			gu8E2Para[40] = fg_time_spd_contorl; //��ʱģʽ��ת�������л�
			gu8E2Para[41] = fg_ModeTimeUnit;	 //��ʱģʽ�ĵ�λ

			/***********����������Ĳ���**************************/
			gu8E2Para[50] = u8SD_ParamSet_OCtrlSpd1;	// Din����ʱ��ת�����ã�Din1Ĭ��100%
			gu8E2Para[51] = u8SD_ParamSet_OCtrlSpd2;	// Din����ʱ��ת�����ã�Din1Ĭ��80%
			gu8E2Para[52] = u8SD_ParamSet_OCtrlSpd3;	// Din����ʱ��ת�����ã�Din1Ĭ��40%
			gu8E2Para[53] = u8SD_ParamSet_HandWashSpd;	//�ֶ�����ʱ��ת������
			gu8E2Para[54] = u8fg_SD_ParamSet_AnalogSel; // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
			gu8E2Para[55] = u8SD_PowerOnTm;				// 25Ϊ�����������Ƿ����25min��ˮ

			/***********�ػ�ʱ����*************************/
			gu8E2Para[60] = fg_air_cali;		 // 1Ϊ�������Ƿ񴥷�����ѹ��У׼
			gu8E2Para[61] = u8fg_SD_PSensorOpen; //�Ƿ�������������

			/***********��������ѧϰ**************************/
			gu8E2Para[65] = WashTmSet_Normal / 30; //����ϴ������ʱ�����ã�������
			for (u8 i = 0; i < 6; i++)			   //�������ķ�Χ
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
			gu8E2Para[120] = (u8)(temp_cail_add >> 8);	  // ADУ׼ֵ����
			gu8E2Para[121] = (u8)(temp_cail_add);		  // ADУ׼ֵ����
			gu8E2Para[122] = (u8)(temp_cail_subtra >> 8); // ADУ׼ֵ����
			gu8E2Para[123] = (u8)(temp_cail_subtra);	  // ADУ׼ֵ����
			gu8E2Para[125] = fg_rest_con_ok;			  //��ѧϰ��־

			/********��ʼ�����һ���ϵ��־��¼***************/
			gu8E2Para[130] = u8FirstFunIn; //�״��ϵ�����й���

			/*********��������ʱ,�������*****************************/
			//�洢��ʽΪ��aabb xxxx ��aaΪ�������ֱ�ǩ��bbΪ�������ͱ�ǩ��xxxxΪ���������Main_lib.h�ڲ鿴
			u8 batch = gu8E2Para[155];
			if (ValBit(gu8SetFlg, bTripSave)) //��������ʱ�������д洢
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
			/************У����******************************/
			sum = 0;
			for (i = 0; i < 199; i++)
			{
				sum += gu8E2Para[i];
			}
			gu8E2Para[199] = ~sum;
		}
		else if ((E2CheckOrder < 2 + 25) && u8fg_isWrIn)
		{
			//�����ݸ��ĵ�����Ҫ����д��
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
//�Լ�ʱE2оƬ�ļ��
static void FunE2_SysTest(void)
{
	uchar j, sum, len;
	u8 E2RdAddress = 0;
	static u8 fg_InCheck = 0;		 //�Ƿ���
	static u8 E2CheckOrder = 0;		 //��ȡ˳��
	static u8 E2CheckCount = 3;		 //������
	static u8 E2CheckErrorCount = 0; //�쳣����
	if (u8E2RdWaitTm == 0)
	{
		u8E2RdWaitTm = 3; // 3s��һ��
		fg_InCheck = 1;	  //������
		E2CheckOrder = 0; //��ȡ˳��

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
		/*************��ͨ��Դ����û�����Լ죬�򲻻�=0xfe************/
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

//��e2pоƬ���в���
void FunE2_SysPro(void)
{
	//��ͨ��Դ��ǰ8s�ĳ�ʼ��
	if (SysProState == SysProState_Init)
	{
		FunE2_SysInit(); //��ȡe2pоƬ����
	}
	//�����Լ�
	else if (SysProState == SysProState_Test)
	{
		FunE2_SysTest(); //�Լ�ʱE2оƬ�ļ��
	}
	//�����ϻ�
	else if (SysProState == SysProState_Aging)
	{
	}
	//��������
	else if (SysProState == SysProState_Nol)
	{
		FunE2_SysNol(); //������д��e29pоƬ��ֻ�����ݷ����仯���Ż�����д�룩
	}
	//����
	else
	{
		FunE2_SysNol(); //������д��e29pоƬ��ֻ�����ݷ����仯���Ż�����д�룩
	}
}
