/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-16 11:51:40
 * @note:
 */
#ifndef _MODEFUNC_H__
#define _MODEFUNC_H_

#include "syscore.h"

/***********�ػ����빦����ʾ**********************************/
//�鿴�汾
extern u8 u8SD_SoftWareWerTm; //����汾����ʾʱ��
//��������
extern u8 u8SD_ParamSetTm;            //�������õ���ʾ������ʱ��
extern u8 u8SD_ParamSetOrder;         //�������õڼ���
extern u8 u8SD_ParamSet_OCtrlSpd1;    // Din����ʱ��ת�����ã�Din1Ĭ��100%
extern u8 u8SD_ParamSet_OCtrlSpd2;    // Din����ʱ��ת�����ã�Din1Ĭ��80%
extern u8 u8SD_ParamSet_OCtrlSpd3;    // Din����ʱ��ת�����ã�Din1Ĭ��40%
extern u8 u8SD_ParamSet_HandWashSpd;  // �ֶ�����ʱ��ת������
extern u8 u8fg_SD_ParamSet_AnalogSel; // 0Ϊ������ģ�������Ƶ�����ѹ�л���־λ
extern u8 u8SD_PowerOnTm;             // 25Ϊ�����������Ƿ����25min��ˮ
//�ָ��������ã���ˮ��ʼ����
extern u8 u8SD_ReStoreDisWaterTm;   //�ָ��������ã���ˮ��ʼ������ɺ�CA01����ʾʱ��
extern u8 u8SD_IntoReStoreDisWater; // 1Ϊ���룬�ָ��������ã���ˮ��ʼ����
//�������Ƿ���
extern u8 u8SD_PSensorDisTm;   //�������Ƿ�����־����ʾʱ��
extern u8 u8fg_SD_PSensorOpen; // 1Ϊ��������������������
//�鿴ADУ׼ֵ
extern u8 u8SD_PressAdDisTm; //��ʾѹ��ADУ׼ֵʱ��

//-------------------------------------------------------
//�鿴ʵ��ѹ��ֵ
extern uchar count_dis_WaterPress_value_tm; //��ʾʵ��ѹ��ʱ��

//--------------------------------------����ϴ����
extern uchar gu8Wash;         // yyxxll
extern uint WashTmSet;        //��������ʱ��
extern uint WashTmSet_Normal; //����ϴ������ʱ�����ã�������
extern uint WashTm;           //�����ĵ���ʱ��

extern uchar fg_rest_con_ok; //��ѧϰ��־

//----------------------------------------------����
#define Unit_ALL 0xff
#define Unit_percent 0x10 //ת�ٵ�%��λ
#define Unit_m3h 0
#define Unit_Imp 1
#define Unit_Lmin 2
#define Unit_Us 3
extern u8 fg_ModeGateUnit;
extern u8 fg_ModeTimeUnit;
typedef struct
{
    u16 TabMax;
    u16 TabMin;
    u16 StudyMax;
    u16 StudyMin;
    u16 Set;
    u16 Real;
} TypeGateRange;
extern TypeGateRange ModeGate_m3h;
extern TypeGateRange ModeGate_Imp;
extern TypeGateRange ModeGate_Lmin;
extern TypeGateRange ModeGate_US;

/*********************�豸***********************/
void FunSysParamInit(void); //��ʼ��������һ���ǽ�ͨ��Դ��
void sub1stRam(void);       //��ʼ�����ã��ָ��������ã�
void FunModeShutDown(void); //�豸�ػ��Ĺ��ܲ���
void FunModeRemeber(void);  //�豸ģʽ����
void FunSysParamInit(void);//��ʼ��������һ���ǽ�ͨ��Դ��

/*********************�ⲿ����***********************/
void FunModeOutCtrl_Din(void);    // Din����ʱ�Ĺ���ʵ��
void FunModeOutCtrl_Analog(void); //ģ��������ʱ�Ĺ���ʵ��
void FunModeOutCtrl_485(void);    // 485����ʱ�Ĺ���ʵ��

/*********************��ѧϰ��������Χ���**************/
u16 FunModeGateLimit(const u16 gate, const u8 unit); //������������ֵ�ڷ�Χ��
u16 FunModeGate_StudyMax(const u8 unit);             //��ȡ��ѧϰ������Χ���ֵ
// u16 FunModeGate_StudyMin(const u8 unit); //��ȡ��ѧϰ������Χ��Сֵ
u16 FunMode_SpeedToGate(const u32 speed, const u8 unit);   //ת��ת��Ϊ����������
u32 FunMode_GateToSpeed(const u16 GateSet, const u8 unit); //����������ת��Ϊת��
u16 FunMode_UnitRound(const u16 gate, const u8 unit);      //ת��תΪ�������ȡ��

u8 FunMode_SpeedToPercent(u32 speed); //ת��ת��Ϊת�ٰٷֱ���

/*********************��ͨģʽ***********************/
void subModeUpDown(signed char i);                   //��ת���ʰٷֱ����ã�ʹ�����¼����ڣ�
void FunModeGateUpDown(const char i, const u8 unit); //��ת�����������ã�ʹ�����¼����ڣ�
void FunModeGate_UnitSwitch(const char i);           //����������λ�л�
void FunModeTime_UnitSwitch(const char i);           //��ʱģʽʱ������������λ�л�

u16 FunModeGate_m3hToOhter(const u16 GateSet, const u8 unit); //��m3/h��λת��Ϊ������λ

/*********************����***********************/
#define WASH_TWINLETIME 7
void FunModeToNormalWash(void); //���뷴��ϴģʽ
void FunNormalWash_Set(void);   //�ڷ���ϴ����ʱ����ת�١�ʱ����ڡ��˳�

void FunModeToSuperWash(void); //���볬Ƶ����ģʽ
void FunSuperWash_Set(void);   //�ڳ�Ƶ����ʱ����ת�١�ʱ����ڡ��˳�

void FunModeToUpWash(void); //������ˮ����ģʽ
void FunUpWash_Set(void);   //����ˮ����ʱ�����˳�

void FunModeToSelfStudy(void);  //������ѧϰģʽ
void FunReEnterSelfStudy(void); //��ѧϰ����Ϻ���Ҫ���½���
void FunSelfStudy_Set(void);    //����ѧϰʱ���豸������Χѧϰ

#endif
