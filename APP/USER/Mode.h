/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-11 18:55:01
 * @note:
 */

#ifndef _MODE_H__
#define _MODE_H__

// device headfile

#include "syscore.h"

#define WashState_Normal 0x01     //����ϴ����
#define WashState_Super 0x02      //��Ƶ����
#define WashState_SuperStudy 0x12 //��Ƶ�����е���ѧϰ
#define WashState_UpWater 0x03    //��ˮ����
#define WashState_SelfStudy 0x10  //��ѧϰ

extern u8 u8fg_SysIsOn;   //���ػ�
extern u8 gu8SetMode;     //ģʽ����
extern u8 gu8SetMode_Bef; //ǰһ��ģʽ����
extern u8 gu8SetMode_ago; //ǰһ��ģʽ����
extern u8 u8WashState;    //��������
extern u8 u8OutCtrlState; //�ⲿ��������

#define SysProState_Nol 0     //ͨ��ģʽ
#define SysProState_Test 1    //�Լ�ģʽ
#define SysProState_Aging 2   //�ϻ�ģʽ
#define SysProState_Init 0xff //��ͨ��Դǰ8s��ʼ��

//--------------------------------------��ʱ
extern u8 u8ModeTime_SpeedSet[4];
extern u8 u8ModeTime_SpeedSet_Tem[4];
extern u16 u8ModeTime_GateSet[4];
extern u16 u8ModeTime_GateSet_Tem[4];

//--------------------------------------��ˮ
extern u8 fg_auto_upwater;       //��ˮ��־
extern uchar gu8upwateroverokTm; //��ˮ����1min�󣬿��Կ�ʼ�ж��Ƿ���ǰ����
extern u8 count_upwater_iserror; //��ˮ����ʧ�ܵĴ������
extern u16 gu16upwateriserrorTm; //��ˮ����ʧ�����½���ȴ�ʱ��

//-------------------------------------------ϵͳ�������Լ�¼ʱ��
typedef struct
{
    u32 PowerTm; //��ͨ��Դʱ��
    u32 RunTm;   //��������ʱ��
    u32 OnTm;    //����ʱ��
    u32 OffTm;   //�ػ�ʱ��
    u32 ErrorTm; //���ϳ���ʱ��
} TypeSysProTime;
extern TypeSysProTime SysProTime; //ϵͳ�������Լ�¼ʱ��
extern u8 SysProState;

//-------------------------------------------��ѧϰ


void sys_Countdown(void);

void FunSysProRun(void);  //ϵͳִ��ģʽ���ϻ����Լ졢ͨ��
void FunSysProInit(void); //ϵͳ��ͨ��Դ��ǰ8s��ʼ��
#endif
