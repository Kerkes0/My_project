#include "ModeFunc.h"
#include "main.h"
#include "Mode.h"
#include "User.h"
#include "LcdFunc.h"
#include "Key.h"
#include "Adc.h"
#include "string.h"

/***********�ػ����빦����ʾ**********************************/
//�鿴�汾
u8 u8SD_SoftWareVerTm = 0; //����汾����ʾʱ��
//��������
u8 u8SD_ParamSetTm = 0;			   //�������õ���ʾ������ʱ��
u8 u8SD_ParamSetOrder = 0;		   //�������õڼ���
u8 u8SD_ParamSet_OCtrlSpd1 = 40;   // Din����ʱ��ת�����ã�Din1Ĭ��100%
u8 u8SD_ParamSet_OCtrlSpd2 = 36;   // Din����ʱ��ת�����ã�Din1Ĭ��80%
u8 u8SD_ParamSet_OCtrlSpd3 = 28;   // Din����ʱ��ת�����ã�Din1Ĭ��40%
u8 u8SD_ParamSet_HandWashSpd = 0;  // �ֶ�����ʱ��ת������
u8 u8fg_SD_ParamSet_AnalogSel = 0; // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
u8 u8SD_PowerOnTm = 25;			   // 25Ϊ�����������Ƿ����25min��ˮ
//�ָ��������ã���ˮ��ʼ����
u8 u8SD_ReStoreDisWaterTm = 0;	 //�ָ��������ã���ˮ��ʼ������ɺ�CA01����ʾʱ��
u8 u8SD_IntoReStoreDisWater = 0; // 1Ϊ���룬�ָ��������ã���ˮ��ʼ����
u8 fg_air_cali = 0;				 // 1Ϊ�������Ƿ񴥷�����ѹ��У׼
//�������Ƿ���
u8 u8SD_PSensorDisTm = 0;	//�������Ƿ�����־����ʾʱ��
u8 u8fg_SD_PSensorOpen = 1; // 1Ϊ��������������������
//�鿴ADУ׼ֵ
u8 u8SD_PressAdDisTm = 0; //��ʾѹ��ADУ׼ֵʱ��

//-------------------------------------------------------
//�鿴ʵ��ѹ��ֵ
u8 count_dis_WaterPress_value_tm = 0; //��ʾʵ��ѹ��ʱ��

//----------------------------------------------����
u8 gu8Wash = 0;				 //
uint WashTmSet = 180;		 //��������ʱ��
uint WashTmSet_Normal = 180; //����ϴ������ʱ�����ã�������
uint WashTm = 0;			 //�����ĵ���ʱ��

uchar fg_rest_con_ok = 0; //��ѧϰ��־
//----------------------------------------------����
u8 fg_ModeGateUnit = 0; //����ģʽ��������λ
u8 fg_ModeTimeUnit = 0; //��ʱģʽ��������λ
TypeGateRange ModeGate_m3h;
TypeGateRange ModeGate_Imp;
TypeGateRange ModeGate_Lmin;
TypeGateRange ModeGate_US;

u8 fg_auto_upwater_end = 0; //��ˮ����

/**
 * @brief  ��������ʱ���ϡ��¼��������
 * @param[in] {mode��1/-1} ���ϻ�����
 * @return Null
 * @note   modeΪ1��ʾ��ǰ���õĲ������ӣ���֮Ϊ-1ʱ��ʾ����
 */
void subSetRomUpDown(schar mode)
{

	gu16RomMinSpd = u16LadderSpd[0]; // 850�̶������ɵ���
	/////////��һ����������
	if (u8SD_ParamSetOrder == 0)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd1 = u8SD_ParamSet_OCtrlSpd1 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd1 = u8SD_ParamSet_OCtrlSpd1 - 1;
	}
	/////////�ڶ�����������
	else if (u8SD_ParamSetOrder == 1)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd2 = u8SD_ParamSet_OCtrlSpd2 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd2 = u8SD_ParamSet_OCtrlSpd2 - 1;
	}
	/////////��������������
	else if (u8SD_ParamSetOrder == 2)
	{
		if (mode > 0)
			u8SD_ParamSet_OCtrlSpd3 = u8SD_ParamSet_OCtrlSpd3 + 1;
		if (mode < 0)
			u8SD_ParamSet_OCtrlSpd3 = u8SD_ParamSet_OCtrlSpd3 - 1;
	}
	/////////���ĸ���������
	else if (u8SD_ParamSetOrder == 3) // yxl-5
	{
		if (mode > 0)
			u8SD_ParamSet_HandWashSpd = u8SD_ParamSet_HandWashSpd + 1;
		if (u8SD_ParamSet_HandWashSpd > 0)
		{
			if (mode < 0)
				u8SD_ParamSet_HandWashSpd = u8SD_ParamSet_HandWashSpd - 1;
		}
		if (u8SD_ParamSet_HandWashSpd > 4)
			u8SD_ParamSet_HandWashSpd = 4;
		if (u8SD_ParamSet_HandWashSpd < 1)
			u8SD_ParamSet_HandWashSpd = 0;
	}
	/////////�������������
	else if (u8SD_ParamSetOrder == 4) // yxl-5
	{
		if (mode > 0)
			u8fg_SD_ParamSet_AnalogSel = 1; // 1Ϊ��ѹ��ģ�������Ƶ�����ѹ�л���־λ
		if (mode < 0)
			u8fg_SD_ParamSet_AnalogSel = 0; // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
	}
	/////////��������������
	else if (u8SD_ParamSetOrder == 5) //�����Ƿ������ˮ
	{
		if (mode > 0)
			u8SD_PowerOnTm = 25;
		if (mode < 0)
			u8SD_PowerOnTm = 0;
	}

	/////////��һ���������÷�Χ��30%-100%
	if (u8SD_ParamSet_OCtrlSpd1 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd1 = 26; // yxl-30%								 // yyxxll
	if (u8SD_ParamSet_OCtrlSpd1 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd1 = 40; // 100%
	/////////�ڶ����������÷�Χ��30%-100%
	if (u8SD_ParamSet_OCtrlSpd2 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd2 = 26; // yxl-30%
	if (u8SD_ParamSet_OCtrlSpd2 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd2 = 40; // yyxxll
	/////////�������������÷�Χ��30%-100%
	if (u8SD_ParamSet_OCtrlSpd3 < 26) // yxl-30%
		u8SD_ParamSet_OCtrlSpd3 = 26; // yxl-30%
	if (u8SD_ParamSet_OCtrlSpd3 > 40) // yxl-5
		u8SD_ParamSet_OCtrlSpd3 = 40; // yyxxll
}

/*********************ϵͳ����������***********************/
//��ʼ���������ָ��������ú�
void FunSysParamInit(void)
{
	SysProState = SysProState_Init;
	u8fg_E2_SysFirstIn = 1;						//��ͨ��Դ���ʼ�����½���
	u8SysExcuteOrder = 0;						//ϵͳ����ִ��˳�򣬴�0��ʼ
												//	count_TestPress = 0;						//�����Լ�İ�������
	memset(&T_base, 0, sizeof(T_base));			//ϵͳʱ��
	memset(&SysProTime, 0, sizeof(SysProTime)); //ϵͳ�������Լ�¼ʱ��

	/************����**********************/
	// m3/h
	ModeGate_m3h.TabMax = 20;
	ModeGate_m3h.TabMin = 5;
	ModeGate_m3h.StudyMax = 50;
	ModeGate_m3h.StudyMin = 5;
	ModeGate_m3h.Set = 15;
	ModeGate_m3h.Real = 15;
	// IMP gpm
	ModeGate_Imp.TabMax = 70;
	ModeGate_Imp.TabMin = 15;
	ModeGate_Imp.StudyMax = 185;
	ModeGate_Imp.StudyMin = 15;
	ModeGate_Imp.Set = 45;
	ModeGate_Imp.Real = 45;
	// L/min
	ModeGate_Lmin.TabMax = 320;
	ModeGate_Lmin.TabMin = 80;
	ModeGate_Lmin.StudyMax = 840;
	ModeGate_Lmin.StudyMin = 80;
	ModeGate_Lmin.Set = 220;
	ModeGate_Lmin.Real = 220;
	// US gpm
	ModeGate_US.TabMax = 85;
	ModeGate_US.TabMin = 20;
	ModeGate_US.StudyMax = 220;
	ModeGate_US.StudyMin = 20;
	ModeGate_US.Set = 60;
	ModeGate_US.Real = 60;

	/************���Ϻ�Ԥ��**********************/
	gu16TripFlg_Board = 0;
	gu16TripFlg_Driver = 0;
	gu16TripFlg_Warn = 0;
}

//��ʼ�����ã��ָ��������ã�
void sub1stRam(void)
{
	u8 i;

	u8ModeSpeed_SpeedSet = 36; // OEM������ֱ��Ϊ80%����ֵ

	//-----------------------------------------------��ʱ��ʼ��
	gu16SetNowTm = 0; //��ʱ���õ�ʱ��ʱ��

	gs16RunStartTm[0] = 7 * 60; //��ʱ���õ�ԤԼʱ��ε�1�εĿ�ʼʱ��
	gs16RunEndTm[0] = 21 * 60;	//��ʱ���õ�ԤԼʱ��ε�1�εĽ���ʱ��
	if (fg_time_spd_contorl == 0)
		u8ModeTime_SpeedSet[0] = 10; //��ʱ���õ�ԤԼʱ��ε�1��ת��Ĭ��ֵ70
	else
		u8ModeTime_GateSet_Tem[0] = 20; //��ʱ���õ�ԤԼʱ��ε�1������Ĭ��ֵ

	for (i = 1; i < 4; i++) // yyxxll
	{

		gs16RunStartTm[i] = 0; //��ʱ���õ�ԤԼʱ��ε�2��3��4�εĿ�ʼʱ��
		gs16RunEndTm[i] = 0;   //��ʱ���õ�ԤԼʱ��ε�2��3��4�εĽ���ʱ��
		if (fg_time_spd_contorl == 0)
			u8ModeTime_SpeedSet[i] = 10; //��ʱ���õ�ԤԼʱ��ε�2��3��4��ת��Ĭ��ֵ70
		else
			u8ModeTime_GateSet_Tem[i] = 20; //��ʱ���õ�ԤԼʱ��ε�1������Ĭ��ֵ20
	}

	for (i = 0; i < 4; i++)
	{
		SetStartTm[i] = gs16RunStartTm[i];					 //��ʱ���õ�ԤԼʱ��εĿ�ʼʱ��
		SetEndTm[i] = gs16RunEndTm[i];						 //��ʱ���õ�ԤԼʱ��εĽ���ʱ��
		u8ModeTime_SpeedSet_Tem[i] = u8ModeTime_SpeedSet[i]; //��ʱ���õ�ԤԼʱ��ε�ת��Ĭ��ֵ70
															 // gu8SetTmGrd[i] = gu8RunTmGrd[i];					 //��
	}
	//-----------------------------------------------��ʱ��ʼ��

	gu16RomMinSpd = u16LadderSpd[0]; // gu16RomMinSpd = 1200; yxl-5

	//	gu8RomSelfSpd = 29;

	WashTmSet = 180;		//��ϴ����ʱ��
	WashTmSet_Normal = 180; //����ϴ��ϴ����ʱ��

	u8FirstFunIn = 0; //�ָ��������ú����ѧϰ�����н����ж�

	u8ModeTimeRunGate = 20; // yxl-5

	fg_time_spd_contorl = 0; // 0Ϊת�٣���ʱת�ٻ��������Ʊ�־λ

	WaterGate_set = 20; //��ǰ����
	water_max_set = 25; //�����������ֵ
	water_min_set = 5;	//����������Сֵ

	fg_hand_close = 0; // 0Ϊ���Ƿ�Ϊ�ֶ�������־λ

	fg_rest_con_ok = 1; //��ѧϰ��־

	//--------------------------------��������
	u8SD_ParamSet_OCtrlSpd1 = 40;	//��1��//���ֿ���DIN1���ٶ�
	u8SD_ParamSet_OCtrlSpd2 = 36;	//��2��//���ֿ���DIN2���ٶ�
	u8SD_ParamSet_OCtrlSpd3 = 28;	//��3��//���ֿ���DIN3���ٶ�
	u8SD_ParamSet_HandWashSpd = 4;	//��4��//����ϴ�����ٶ�
	u8fg_SD_ParamSet_AnalogSel = 0; //��5��//0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
	u8SD_PowerOnTm = 25;			//��6��//Ĭ�Ͽ�����Ϊ25minn�������Ƿ������ˮ

	//--------------------------------��������

	//	for (i = 0; i < 45; i++)
	//	{
	//		WaterPress_value_con[i] = 0;
	//		fg_waterPress_value_con[i] = 0;
	//	}

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////�ָ��������ã���ͬ���Ͳ�����һ��
	////////////////////////////////////////////////////////////////////

	////��ʱ�����У�����������ذ����沦�������ð汾���лָ�
	if ((switch_key & 0x06) == 0) // IP20
	{
		water_max_set = 20;
		water_min_set = 5;
		WaterGate_set = 15;
		u8ModeTime_GateSet_Tem[0] = 15;
		u8ModeTime_GateSet_Tem[1] = 15;
		u8ModeTime_GateSet_Tem[2] = 15;
		u8ModeTime_GateSet_Tem[3] = 15;
	}
	else if ((switch_key & 0x06) == 2) // IP25
	{
		water_max_set = 25;
		water_min_set = 5;
		WaterGate_set = 20;
		u8ModeTime_GateSet_Tem[0] = 20;
		u8ModeTime_GateSet_Tem[1] = 20;
		u8ModeTime_GateSet_Tem[2] = 20;
		u8ModeTime_GateSet_Tem[3] = 20;
	}
	else if ((switch_key & 0x06) == 4) // IP30
	{
		water_max_set = 30;
		water_min_set = 5;
		WaterGate_set = 25;
		u8ModeTime_GateSet_Tem[0] = 25;
		u8ModeTime_GateSet_Tem[1] = 25;
		u8ModeTime_GateSet_Tem[2] = 25;
		u8ModeTime_GateSet_Tem[3] = 25;
	}
	// 20211223
	else //Ĭ��IP20
	{
		water_max_set = 20;
		water_min_set = 5;
		WaterGate_set = 15;
		u8ModeTime_GateSet_Tem[0] = 15;
		u8ModeTime_GateSet_Tem[1] = 15;
		u8ModeTime_GateSet_Tem[2] = 15;
		u8ModeTime_GateSet_Tem[3] = 15;
	}
	// 20211223

	water_uint_switch = 0; // 20211222

	gu8SetMode_eep = 0; // yxl  �ָ�Ĭ������ģʽ
}
//����ػ������еĳ���ʱ��
static void subModeSDClearTime(void)
{
	u8SD_SoftWareVerTm = 0;		//����汾����ʾʱ��
	u8SD_ParamSetTm = 0;		//�������õ���ʾ������ʱ��
	u8SD_ReStoreDisWaterTm = 0; //��ˮ��ʼ����CA01����ʾʱ��
	u8SD_PSensorDisTm = 0;		//�������Ƿ�����־����ʾʱ��
	u8SD_PressAdDisTm = 0;		// ADУ׼ֵ����ʾʱ��
}

//�豸�ػ��Ĺ��ܲ���
u8 fg_InParamSet = 0; // 0Ϊ�����룬�Ƿ�����������
void FunModeShutDown(void)
{
	//����
	if (gu8KeyFlg == bPower)
	{
		u8fg_SysIsOn = 1;
		fg_hand_close = 0;	  //�����ֶ��ػ�ʱ
		fg_InParamSet = 0;	  //�˳��ػ�����
		subModeSDClearTime(); //����ػ������еĳ���ʱ��
	}

	//�ָ���������
	if (gu8KeyFlg == bLongTimeUp)
	{
		subModeSDClearTime();			//����ػ������еĳ���ʱ��
		sub1stRam();					//�ָ���������
		SysProState = SysProState_Init; //ϵͳ�����ʼ��״̬
		SysProTime.PowerTm = 0;			//��ͨ��Դʱ�����¼�ʱ

		gu8LcdInitTm = 15; //Һ��7.5s��ʼ��
		gu8RTCWaitTm = 0;
		SetBit(gu8SetTimeFlg, bNowTime);
	}
	//����汾��
	else if (gu8KeyFlg == bLongTimeDown)
	{
		subModeSDClearTime();	 //����ػ������еĳ���ʱ��
		u8SD_SoftWareVerTm = 12; //�汾����ʾʱ��12*500ms=6s
	}
	//��������
	else if (gu8KeyFlg == bLongUpDown)
	{
		if (fg_InParamSet == 0)
		{
			gu8KeyFlg = 0;			//���⣺���ֶ��尴��
			subModeSDClearTime();	//����ػ������еĳ���ʱ��
			u8SD_ParamSetTm = 30;	//�������ó���ʱ��
			fg_InParamSet = 1;		//�����������
			u8SD_ParamSetOrder = 0; //�ӵ�1��������ʼ����
		}
	}
	//������ˮ��ʼ��������MODE��CONTROL��TIME�������ڻָ���������֮��
	else if (u8SD_IntoReStoreDisWater == 1) // yxl-5
	{
		subModeSDClearTime();		  //����ػ������еĳ���ʱ��
		u8SD_IntoReStoreDisWater = 0; //�ֶ����־
		u8SD_ReStoreDisWaterTm = 15;  //��ʾ500ms*15=7.5sʱ���//��ˮ��ʼ����CA01����ʾʱ��
		temp_cail_add = 0;			  //У׼���ݹ���
		temp_cail_subtra = 0;		  //У׼���ݹ���
		fg_air_cali = 1;			  //����У׼�����»��У׼����
		u8fg_SD_PSensorOpen = 1;	  //��������������        //ֻ�г����ű�֤����Ч
	}
	//�������Ƿ�����־�����������Ͷ�ʱ����
	else if (gu8KeyFlg == bLongUnlockControl)
	{
		subModeSDClearTime(); //����ػ������еĳ���ʱ��
		//���͹ء��ؾͿ�
		if (u8fg_SD_PSensorOpen)
		{
			u8fg_SD_PSensorOpen = 0; //�رմ�����
		}
		else
		{
			u8fg_SD_PSensorOpen = 1; //�򿪴�����
		}
		fg_rest_con_ok = 1;		//������ѧϰ
		u8SD_PSensorDisTm = 15; //��ʾ500ms*15=7.5sʱ�����ˮ�����Ƿ���
	}
	// ADУ׼ֵ��ʾ
	else if (gu8KeyFlg == bLongUnlockTime)
	{
		subModeSDClearTime();	//����ػ������еĳ���ʱ��
		u8SD_PressAdDisTm = 10; //��ʾѹ��ADУ׼ֵʱ��
	}

	//-----------��������
	if (fg_InParamSet) //�ػ�ʱ�Ĳ�������ģʽ
	{
		if (gu8KeyFlg == bUp)
		{
			subSetRomUpDown(1);
			u8SD_ParamSetTm = 30;
		}
		else if (gu8KeyFlg == bDown)
		{
			subSetRomUpDown(-1);
			u8SD_ParamSetTm = 30;
		}
		else if (gu8KeyFlg == bControl)
		{
			u8SD_ParamSetTm = 30; //�������ó���ʱ��
			if (++u8SD_ParamSetOrder > 5)
			{
				fg_InParamSet = 0;
				u8SD_ParamSetTm = 0;
			}
		}
		else if (gu8KeyFlg == bLongUpDown)
		{
			u8SD_ParamSetTm = 30; //�������ó���ʱ��
			if (++u8SD_ParamSetOrder > 5)
			{
				fg_InParamSet = 0;
				u8SD_ParamSetTm = 0;
			}
		}

		if (u8SD_ParamSetTm EQU 0)
		{
			fg_InParamSet = 0;
		}
	}
}
//�豸ģʽ����
void FunModeRemeber(void)
{
	static u8 Mode_bef = SPEED;
	//�豸ģʽ��¼
	gu8SetMode_Bef = gu8SetMode;
	if (Mode_bef != gu8SetMode && gu8SetMode > 0 && gu8SetMode < 4)
	{
		gu8SetMode_ago = Mode_bef; //����ǰһ������ģʽ
		Mode_bef = gu8SetMode;
	}
}

/*********************�ⲿ����***********************/
// Din����ʱ�Ĺ���ʵ��
void FunModeOutCtrl_Din(void)
{
	if (ValBit(u8OutCtrl_Din, bOutCtrl_Stop))
	{
		SetBit(u8OutCtrlState, Ctrl_DinOff); //�ػ�
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_1))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_2))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
	else if (ValBit(u8OutCtrl_Din, bOutCtrl_3))
	{
		gu16RunSpd = u8SD_ParamSet_OCtrlSpd1 * 100;
		for (u8 k = 0; k < 17; k++) // yxl-5
		{
			if (gu16RunSpd == (k + 12) * 100)
				gu16RunSpd = u16LadderSpd[k];
		}
	}
}
//ģ��������ʱ�Ĺ���ʵ��
void FunModeOutCtrl_Analog(void)
{
	if ((u16Analog_AD < 1303))
		gu16RunSpd = u16LadderSpd[2]; // 1.05
	else if ((u16Analog_AD < 1489))
		gu16RunSpd = u16LadderSpd[3]; // 1.2
	else if ((u16Analog_AD < 1675))
		gu16RunSpd = u16LadderSpd[4]; // 1.35
	else if ((u16Analog_AD < 1861))
		gu16RunSpd = u16LadderSpd[5]; // 1.5
	else if ((u16Analog_AD < 2048))
		gu16RunSpd = u16LadderSpd[6]; // 1.65
	else if ((u16Analog_AD < 2234))
		gu16RunSpd = u16LadderSpd[7]; // 1.8
	else if ((u16Analog_AD < 2420))
		gu16RunSpd = u16LadderSpd[8]; // 1.95
	else if ((u16Analog_AD < 2606))
		gu16RunSpd = u16LadderSpd[9]; // 2.1
	else if ((u16Analog_AD < 2792))
		gu16RunSpd = u16LadderSpd[10]; // 2.25
	else if ((u16Analog_AD < 2978))
		gu16RunSpd = u16LadderSpd[11]; // 2.4
	else if ((u16Analog_AD < 3165))
		gu16RunSpd = u16LadderSpd[12]; // 2.55
	else if ((u16Analog_AD < 3351))
		gu16RunSpd = u16LadderSpd[13]; // 2.7
	else if ((u16Analog_AD < 3537))
		gu16RunSpd = u16LadderSpd[14]; // 2.85
	else if ((u16Analog_AD < 3723))
		gu16RunSpd = u16LadderSpd[15]; // 3
	else
		gu16RunSpd = u16LadderSpd[16];
}
// 485����ʱ�Ĺ���ʵ��
void FunModeOutCtrl_485(void)
{
	if (gu16UartSpd)
	{
		for (u8 k = 0; k < 17; k++) // YXL-5
		{
			if ((gu16UartSpd >= (20 + 5 * k)) AND(gu16UartSpd < (25 + 5 * k)))
			{
				gu16RunSpd = u16LadderSpd[k];
			}
		}
	}
	// 485�ػ�
	else
	{
		u8fg_SysIsOn = 0;
	}
}

/*********************��ѧϰ��������Χ���**************/
//������������ֵ�ڷ�Χ��
u16 FunModeGateLimit(const u16 gate, const u8 unit)
{
	u16 TabMax, TabMin, StudyMax, StudyMin;
	u16 GateSet;
	switch (unit)
	{
	case Unit_m3h:
		TabMax = ModeGate_m3h.TabMax;
		TabMin = ModeGate_m3h.TabMin;
		StudyMax = ModeGate_m3h.StudyMax;
		StudyMin = ModeGate_m3h.StudyMin;
		break;
	case Unit_Imp:
		TabMax = ModeGate_Imp.TabMax;
		TabMin = ModeGate_Imp.TabMin;
		StudyMax = ModeGate_Imp.StudyMax;
		StudyMin = ModeGate_Imp.StudyMin;
		break;
	case Unit_Lmin:
		TabMax = ModeGate_Lmin.TabMax;
		TabMin = ModeGate_Lmin.TabMin;
		StudyMax = ModeGate_Lmin.StudyMax;
		StudyMin = ModeGate_Lmin.StudyMin;
		break;
	case Unit_Us:
		TabMax = ModeGate_US.TabMax;
		TabMin = ModeGate_US.TabMin;
		StudyMax = ModeGate_US.StudyMax;
		StudyMin = ModeGate_US.StudyMin;
		break;
	default:
		TabMax = ModeGate_m3h.TabMax;
		TabMin = ModeGate_m3h.TabMin;
		StudyMax = ModeGate_m3h.StudyMax;
		StudyMin = ModeGate_m3h.StudyMin;
		break;
	}
	if (GateSet > TabMax)
		GateSet = TabMax;
	if (GateSet < TabMin)
		GateSet = TabMin;
	if (GateSet > StudyMax)
		GateSet = StudyMax;
	if (GateSet < StudyMin)
		GateSet = StudyMin;
	return GateSet;
}
//��ȡ��ѧϰ������Χ���ֵ
u16 FunModeGate_StudyMax(const u8 unit)
{
	switch (unit)
	{
	case Unit_m3h:
		return ModeGate_m3h.StudyMax;

	case Unit_Imp:
		return ModeGate_Imp.StudyMax;

	case Unit_Lmin:
		return ModeGate_Lmin.StudyMax;

	case Unit_Us:
		return ModeGate_US.StudyMax;

	default:

		break;
	}
	return ModeGate_m3h.StudyMax;
}
//ת��ת��Ϊ��Ӧ��λ����
u16 FunMode_SpeedToGate(const u32 speed, const u8 unit)
{
	u16 GateSet;
	u16 StudyMax;
	StudyMax = FunModeGate_StudyMax(unit);
	GateSet = StudyMax * speed / u16LadderSpd[16];
	GateSet = FunModeGateLimit(GateSet, unit);
	return GateSet;
}
//����������ת��Ϊת��
u32 FunMode_GateToSpeed(const u16 GateSet, const u8 unit)
{
	u32 speed;
	u16 StudyMax;
	StudyMax = FunModeGate_StudyMax(unit);
	speed = u16LadderSpd[16] * GateSet / StudyMax;
	return speed;
}
//ת��ת��Ϊ�������ȡ��
u16 FunMode_UnitRound(const u16 gate, const u8 unit)
{
	u16 gate_set = gate;
	switch (unit)
	{
	case Unit_m3h:
		gate_set = gate;
		break;
	case Unit_Imp:
		gate_set = 5 * (gate / 5 + ((gate % 5 > 2) ? 1 : 0));
		break;
	case Unit_Lmin:
		gate_set = 20 * (gate / 20 + ((gate % 20 > 10) ? 1 : 0));
		break;
	case Unit_Us:
		gate_set = 5 * (gate / 5 + ((gate % 5 > 2) ? 1 : 0));
		break;
	default:
		gate_set = gate;
		break;
	}
	return gate_set;
}

//��m3/h��λת��Ϊ������λ
u16 FunModeGate_m3hToOhter(const u16 GateSet, const u8 unit)
{
	u16 UnitSet = 5;
	u16 tab;
	switch (unit)
	{
	case Unit_m3h:
		UnitSet = GateSet;
		break;
	case Unit_Imp:
		tab = GateSet * 11 / 3;
		UnitSet = 5 * (tab / 5 + ((tab % 5 >= 2) ? 1 : 0)); //��������
		break;
	case Unit_Lmin:
		tab = GateSet * 50 / 3;
		UnitSet = 20 * (tab / 20 + ((tab % 20 >= 10) ? 1 : 0)); //��������
		break;
	case Unit_Us:
		tab = GateSet * 22 / 5;
		UnitSet = 5 * (tab / 5 + ((tab % 5 >= 2) ? 1 : 0)); //��������
		break;
	default:
		break;
	}
	return UnitSet;
}

//ת��ת��Ϊת�ٰٷֱ���
u8 FunMode_SpeedToPercent(u32 speed)
{
	u8 spdset;
	for (u8 i = 0; i < 17; i++)
	{
		if (speed > (u16LadderSpd[i] - u16LadderSpd_rang))
		{
			spdset = i + 24;
		}
	}
	if (speed > u16LadderSpd[16])
		spdset = 40;
	if (speed && speed < u16LadderSpd[2])
		spdset = 26;
	if (speed == 0)
		spdset = 0;
	return spdset;
}
/*********************��ͨģʽ***********************/
//ת��ģʽʱ����ת���ʰٷֱ����ã�ʹ�����¼����ڣ�
void subModeUpDown(schar i)
{
	if (i >= 0)
		u8ModeSpeed_SpeedSet += 1;
	else
		u8ModeSpeed_SpeedSet -= 1;

	if (u8ModeSpeed_SpeedSet > 40)
		u8ModeSpeed_SpeedSet = 40;
	if (u8ModeSpeed_SpeedSet < 24 + 2)
		u8ModeSpeed_SpeedSet = 24 + 2; // yxl-30%
}

//����ģʽʱ����ת�����������ã�ʹ�����¼����ڣ�
void FunModeGateUpDown(const char i, const u8 unit)
{
	u8 GateDif;
	u16 *GateSet; //, *GateReal;
	switch (unit)
	{
	case Unit_m3h:
		GateDif = 1;
		GateSet = &ModeGate_m3h.Set;
		break;
	case Unit_Imp:
		GateDif = 5;
		GateSet = &ModeGate_Imp.Set;
		break;
	case Unit_Lmin:
		GateDif = 20;
		GateSet = &ModeGate_Lmin.Set;
		break;
	case Unit_Us:
		GateDif = 5;
		GateSet = &ModeGate_US.Set;
		break;
	default:
		GateDif = 1;
		GateSet = &ModeGate_m3h.Set;
		break;
	}
	if (i >= 0)
	{
		u16ModeGate_GateSet += GateDif;
		*GateSet += GateDif;
	}
	else
	{
		u16ModeGate_GateSet -= GateDif;
		*GateSet -= GateDif;
	}
	u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, unit);
	*GateSet = FunModeGateLimit(*GateSet, unit);
}
//����ģʽʱ������������λ�л�
void FunModeGate_UnitSwitch(const char i)
{
	u8 a, fg_ModeGateUnit_bef;
	u32 speed;

	//	fg_ModeUnit_IsChange = 1; // 1Ϊ�ڣ���λ�л�Ӱ���־
	fg_ModeGateUnit_bef = fg_ModeGateUnit;
	if (i >= 0)
	{
		fg_ModeGateUnit += i;
		if (fg_ModeGateUnit >= 4)
			fg_ModeGateUnit = fg_ModeGateUnit - 4;
	}
	else
	{
		a = _abs(i);
		if (fg_ModeGateUnit < a)
			fg_ModeGateUnit = 4 + fg_ModeGateUnit - a;
		else
			fg_ModeGateUnit -= a;
	}
	//������ֵת��Ϊѡ��ĵ�λ��ֵ
	speed = FunMode_GateToSpeed(u16ModeGate_GateSet, fg_ModeGateUnit_bef);
	u16ModeGate_GateSet = FunMode_SpeedToGate(speed, fg_ModeGateUnit);
	u16ModeGate_GateSet = FunMode_UnitRound(u16ModeGate_GateSet, fg_ModeGateUnit);//ȡ��
	u16ModeGate_GateSet = FunModeGateLimit(u16ModeGate_GateSet, fg_ModeGateUnit);//����
}

//��ʱģʽ
//��ʱģʽʱ������������λ�л�
void FunModeTime_UnitSwitch(const char i)
{
	u8 a, fg_ModeTimeUnit_bef;

	//	fg_ModeUnit_IsChange = 1; // 1Ϊ�ڣ���λ�л�Ӱ���־
	fg_ModeTimeUnit_bef = fg_ModeTimeUnit;
	if (i >= 0)
	{
		fg_ModeTimeUnit += i;
		if (fg_ModeTimeUnit >= 4)
			fg_ModeTimeUnit = fg_ModeTimeUnit - 4;
	}
	else
	{
		a = _abs(i);
		if (fg_ModeTimeUnit < a)
			fg_ModeTimeUnit = 4 + fg_ModeTimeUnit - a;
		else
			fg_ModeTimeUnit -= a;
	}
	//��ʱģʽ����ʱ
	if (gu8SetTimeTm)
	{
		for (u8 speed, gate, i = 0; i < 4; i++)
		{
			gate = u8ModeTime_GateSet_Tem[i];
			speed = FunMode_GateToSpeed(gate, fg_ModeTimeUnit_bef);
			gate = FunMode_SpeedToGate(speed, fg_ModeTimeUnit);
			gate = FunMode_UnitRound(gate, fg_ModeTimeUnit);//ȡ��
			gate = FunModeGateLimit(gate, fg_ModeTimeUnit);//����
			u8ModeTime_GateSet_Tem[i] = gate;
		}
	}
	//��ʱģʽ����ʱ
	else
	{
		for (u8 speed, gate, i = 0; i < 4; i++)
		{
			gate = u8ModeTime_GateSet[i];
			speed = FunMode_GateToSpeed(gate, fg_ModeTimeUnit_bef);
			gate = FunMode_SpeedToGate(speed, fg_ModeTimeUnit);
			gate = FunMode_UnitRound(gate, fg_ModeTimeUnit);
			gate = FunModeGateLimit(gate, fg_ModeTimeUnit);
			u8ModeTime_GateSet[i] = gate;
		}
	}
}

/*********************����***********************/
//���뷴��ϴģʽ
void FunModeToNormalWash(void)
{
	if (u8WashState == 0) //������ѧϰ
	{
		u8WashState = WashState_Normal; //���뷴��ϴ����
		gu8SetWastTm = 10;				//����ϴ����ʱ������ʾ������˸ʱ��
		WashTmSet = WashTmSet_Normal;	//����ϴ��������ʱ�������ʾ���ĳ�ϴ����ʱ��
		WashTm = WashTmSet;
	}
}
//�ڷ���ϴ����ʱ����ת�١�ʱ����ڡ��˳�
void FunNormalWash_Set(void)
{
	//ת������
	if ((gu8SetWastTm == 0) || (WashTmSet != 0))
	{
		if (u8SD_ParamSet_HandWashSpd == 0)
			gu16RunSpd = u16LadderSpd[12]; // yxl-5
		else if (u8SD_ParamSet_HandWashSpd == 1)
			gu16RunSpd = u16LadderSpd[13];
		else if (u8SD_ParamSet_HandWashSpd == 2)
			gu16RunSpd = u16LadderSpd[14];
		else if (u8SD_ParamSet_HandWashSpd == 3)
			gu16RunSpd = u16LadderSpd[15];
		else if (u8SD_ParamSet_HandWashSpd == 4)
			gu16RunSpd = u16LadderSpd[16];
		else
			gu16RunSpd = u16LadderSpd[16];
	}
	//ʱ�����
	if (gu8KeyFlg == bUp)
	{
		gu8SetWastTm = WASH_TWINLETIME; //��������ʱ��ı�ʱ����ʾ������˸ʱ��
		WashTmSet_Normal += 30;
		if (WashTmSet_Normal > 1500) // 20211210+
		{
			WashTmSet_Normal = 1500;
		}
		WashTmSet = WashTmSet_Normal;
	}
	else if (gu8KeyFlg == bDown)
	{
		gu8SetWastTm = WASH_TWINLETIME; //��������ʱ��ı�ʱ����ʾ������˸ʱ��
		if (WashTmSet_Normal > 30)
		{
			WashTmSet_Normal -= 30;
		}
		else if (WashTmSet_Normal <= 30) // 20220530+
		{
			WashTmSet_Normal = 0; // yxl-5
		}
		WashTmSet = WashTmSet_Normal;
	}
	else if ((u8fg_SysIsOn == 0)											  //�ػ�
			 || (gu8KeyFlg == bLongWash)									  //����������
			 || (gu16TripFlg_Driver || gu16TripFlg_Board || gu16TripFlg_Warn) //����ʱ
			 || (u8OutCtrlState > 0)										  //�ⲿ����
			 || (WashTm == 0)												  //ʱ�����
	)
	{
		u8WashState = 0; //�˳�����
	}
}
//���볬Ƶ����ģʽ
void FunModeToSuperWash(void)
{
	if (u8WashState == 0) //������ѧϰ
	{
		u8WashState = WashState_Super; //���볬Ƶ����ģʽ
		WashTmSet = 600;
		WashTm = WashTmSet;
		gu8SetWastTm = 10;	//����ϴ����ʱ������ʾ������˸ʱ��
		fg_rest_con_ok = 1; //��Ҫ��ѧϰ
	}
}
//�ڳ�Ƶ����ʱ����ת�١�ʱ����ڡ��˳�
void FunSuperWash_Set(void)
{
	//ת������
	gu16RunSpd = 2850;
	//ʱ�����
	if (gu8KeyFlg == bUp)
	{
		gu8SetWastTm = WASH_TWINLETIME; //��������ʱ��ı�ʱ����ʾ������˸ʱ��
		WashTmSet += 30;
		if (WashTmSet > 1500)
		{
			WashTmSet = 1500;
		}
	}
	else if (gu8KeyFlg == bDown)
	{
		gu8SetWastTm = WASH_TWINLETIME; //��������ʱ��ı�ʱ����ʾ������˸ʱ��
		WashTmSet -= 30;
		if (WashTmSet < 600)
		{
			WashTmSet = 600; // yxl-5
		}
	}
	if ((gu16TripFlg_Driver || gu16TripFlg_Board || gu16TripFlg_Warn)) //����ʱ
	{
		u8WashState = 0; //�˳�����
	}
	else if ((gu8KeyFlg == bLongWash)	//����
			 || (u8fg_SysIsOn == 0)		//�ػ�
			 || (u8OutCtrlState > 0)	//�ⲿ����
			 || (fg_rest_con_wifi == 1) // wifi//20220105+
	)
	{
		u8WashState = WashState_SelfStudy; //�˳�����,������ѧϰ
		fg_rest_con_wifi = 0;
	}
	else if (WashTm == 180) //����ʱ120s
	{
		u8WashState = WashState_SuperStudy; //�˳�����,������ѧϰ
		FunModeToSelfStudy();
	}
}
//������ˮ����ģʽ
void FunModeToUpWash(void)
{
	if (u8WashState == 0 && u8SD_PowerOnTm)
	{
		u8WashState = WashState_UpWater; //���볬Ƶ����ģʽ
		WashTmSet = u8SD_PowerOnTm * 60;
		WashTm = WashTmSet;
		gu8SetWastTm = WASH_TWINLETIME; // ��������ʱ������ʾ������˸ʱ��
		gu8upwateroverokTm = 0;			// 60/2s=30s�ж�ʱ�俪ʼ��ʱ
	}
}
//����ˮ����ʱ�����˳�
void FunUpWash_Set(void)
{
	//ת������
	gu16RunSpd = 2850;

	//��ˮ����
	if (WashTm == 0)
	{
		fg_auto_upwater_end = 1; //��ˮ����
	}
	//��ˮ������
	if (fg_auto_upwater_end == 1)
	{
		fg_auto_upwater_end = 0;
		//�鲻��ˮ��������ʧ�ܹ���E209
		if (gu16MotorWt < 500 && (u8fg_SysIsOn) && (gu16TripFlg_Driver == 0 && gu16TripFlg_Board == 0))
		{
			SetBit(gu16TripFlg_Board, Owupwaterfail); //����ʧ��E209
			count_upwater_iserror++;				  //����ʧ�ܴ������������ϣ��������Σ�������
			gu16upwateriserrorTm = 240;				  //��һ����ˮ����ʧ�ܺ󣬵�0.5s*240=2min�����½�����ˮ
		}
		//�����ˮ�����E209
		else if (gu16MotorWt >= 500)
		{
			u8WashState = 0;
			ClrBit(gu16TripFlg_Board, Owupwaterfail); //�������ʧ��E209
			count_upwater_iserror = 0;				  //�������ʧ�ܴ���������������
		}
	}
	//����>=500����ˮʱ��>60s,
	if ((gu16MotorWt >= 500) && (WashTm > 60)) // yxl-e207
	{
		if (gu8upwateroverokTm > 60) // 0.5S*60=30S
		{
			WashTm = 60;
		}
	}
	else
	{
		gu8upwateroverokTm = 0;
	}
}
//������ѧϰģʽ
void FunModeToSelfStudy(void)
{
	if (u8WashState == WashState_SuperStudy)
	{
		u8WashState = WashState_SelfStudy; //���볬Ƶ����ģʽ
	}
	else
	{
		u8WashState = WashState_SelfStudy; //���볬Ƶ����ģʽ
		WashTmSet = 180;
		WashTm = WashTmSet;
		gu8SetWastTm = 10; // ��������ʱ������ʾ������˸ʱ��
	}
}
//��ѧϰ����Ϻ���Ҫ���½���
void FunReEnterSelfStudy(void)
{
	if (u8fg_SysIsOn == 1 && u8WashState == 0)
	{
		if (fg_rest_con_ok == 1)
		{
			FunModeToSelfStudy();
		}
	}
}
	 u8 fg_SelfStudy_GetAd = 1;
	u8 count_SelfStudy = 0;
	 struct
	{
		u32 GetTab;
		u32 GetTab_max;
		u32 GetTab_min;
		u32 GetTab_Sum;

	} SelfStudy_Gate;
//����ѧϰʱ���豸������Χѧϰ
void FunSelfStudy_Set(void)
{
	u8 s;

	//ת������
	gu16RunSpd = u16LadderSpd[16];

	//����ʵ��
	// 66sǰ�ȵȴ��ȶ������г�ʼ��
	if (WashTm >= 66) // 6s/�Σ���10�Σ�����12s���һ�Σ�66=12+6*��10-1��
	{
		fg_SelfStudy_GetAd = 1;
		count_SelfStudy = 0;
		SelfStudy_Gate.GetTab = 0;
		SelfStudy_Gate.GetTab_max = 0;
		SelfStudy_Gate.GetTab_min = 4096;
		SelfStudy_Gate.GetTab_Sum = 0;
	}
	// 66s��ʼ��ȡѹ������ѧϰ
	else
	{
		if (WashTm % 6)
		{
			fg_SelfStudy_GetAd = 1;
		}
		if (WashTm % 6 == 0 && fg_SelfStudy_GetAd == 1)
		{
			fg_SelfStudy_GetAd = 0;
			count_SelfStudy++;
			SelfStudy_Gate.GetTab = WaterGate_cail_ot_preset; //ֻ��һ��ת��2600
			for (u8 k = 0; k < 35; k++)
			{
				if ((WaterPress_value >= sp_2600_con_press[k] * 10) && (WaterPress_value < sp_2600_con_press[k + 1] * 10))
				{
					SelfStudy_Gate.GetTab = sp_2600_con_flow[k];
				}
			}

			if (SelfStudy_Gate.GetTab_max < SelfStudy_Gate.GetTab)
				SelfStudy_Gate.GetTab_max = SelfStudy_Gate.GetTab;
			if (SelfStudy_Gate.GetTab_min > SelfStudy_Gate.GetTab)
				SelfStudy_Gate.GetTab_min = SelfStudy_Gate.GetTab;

			SelfStudy_Gate.GetTab_Sum += SelfStudy_Gate.GetTab;
		}
		if (count_SelfStudy >= 10)
		{
			count_SelfStudy = 0;

			SelfStudy_Gate.GetTab_Sum -= SelfStudy_Gate.GetTab_max;
			SelfStudy_Gate.GetTab_Sum -= SelfStudy_Gate.GetTab_min;
			water_max_set = (SelfStudy_Gate.GetTab_Sum >> 3) / 100;
			s = (SelfStudy_Gate.GetTab_Sum >> 3) % 100;
			if (s > 50)
				water_max_set += 1;

			water_min_set = water_max_set * u16LadderSpd[0] / u16LadderSpd[16];

			//����ת��Ϊת�ٵı���
			if (water_max_set < 5)
				water_max_set = 5;
			if (water_max_set > 50)
				water_max_set = 50; //���Գ���25
			if (water_min_set < 3)
				water_min_set = 3;
			if (water_min_set > 10)
				water_min_set = 10;

			//����ת��Ϊת�ٵı���
			ModeGate_m3h.StudyMax = water_max_set;
			ModeGate_m3h.StudyMin = water_min_set;
			if (ModeGate_m3h.StudyMax < 5)
				ModeGate_m3h.StudyMax = 5;
			if (ModeGate_m3h.StudyMax > 50)
				ModeGate_m3h.StudyMax = 50; //���Գ���25
			if (ModeGate_m3h.StudyMin < 3)
				ModeGate_m3h.StudyMin = 3;
			if (ModeGate_m3h.StudyMin > 10)
				ModeGate_m3h.StudyMin = 10;
			ModeGate_Imp.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Imp);
			ModeGate_Imp.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Imp);
			ModeGate_Lmin.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Lmin);
			ModeGate_Lmin.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Lmin);
			ModeGate_US.StudyMax = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMax, Unit_Us);
			ModeGate_US.StudyMin = FunModeGate_m3hToOhter(ModeGate_m3h.StudyMin, Unit_Us);

			//�ָ��������ú�ĵ�һ����ѧϰ�������������
			if (ValBit(u8FirstFunIn, LowGate))
			{
				ClrBit(u8FirstFunIn, LowGate);
				if (WaterGate_set >= water_max_set) //�������ٵ�80%
				{
					WaterGate_set = water_max_set * 4 / 5;
					if (WaterGate_set < 5)
						WaterGate_set = 5;
				}
			}
		}
	}

	if (WashTm == 0)
	{
		u8WashState = 0;
		fg_rest_con_ok = 0;
	}
}

//��ƽ������ʱ���豸�ж��Ƿ����ˮ����
