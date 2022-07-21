/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-12 08:53:58
 * @note:
 */
#ifndef _LCD_H__
#define _LCD_H__

#include "syscore.h"
extern u8 TwinkleTm;
extern u8 SysAgingTwinTm;

void FunLcdSysUpPowerInit_0(void); //ȫ��ʾ
void FunLcdSysUpPowerInit_1(void); //��ʾ�����롢���롢ѹ��ֵ

void subDispTrip(void);      //��ʾ����ʱ�䣬3s�л���һ��
void subDispTripFlash(void); //������ʾ(3sһ��)

void FunLcdError(void);                               //ͣ��ʱ���ϵ���ʾ(����Ԥ������)
void FunLcdWarnSpeed(const u32 spd);                  //ת��ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��
void FunLcdWarnGate(const u32 spd, const u8 unit); //����ģʽʱ���о��潵��ʱ��Ԥ�����뽻�����ʾ��ֻ��Ԥ��ʱ��

void subDispWt(uint data, uchar mode);
void subDispRpm_Special(const u8 *data); //ת������ܵ�������ʾ

void FunLcdModeShutDown(void);                         //�ػ����ܵ�ʹ��ʱ����ʾ
void FunLcdModeWash(void);                             //����ʱ����ʾ
void FunLcdOutCtrl(const u32 spd);                     //�ⲿ����ʱ����ʾ
void FunLcdModeSpeed(const u8 spdset);                //ת��ģʽʱ����ʾ
void FunLcdModeGate(const u16 gate, const u8 fg_unit); //����ģʽʱ����ʾ
void FunLcdModeTime_Set(void);                         //��ʱģʽʱ����ʾ������ʱ��
void FunLcdModeTime_Invalid(void);                     //��ʱģʽʱ����ʾ��ʱ���ȫ����Ч����
void FunLcdModeTime_Run(void);                         //��ʱģʽʱ����ʾ����������
void FunLcdModeTime_Stay(void);                        //��ʱģʽʱ����ʾ���ȴ�����

//ϵͳ���Լ���ʾ
void FunLcdSysTest_0(void);
void FunLcdSysTest_1(const u8 TestNum);
void FunLcdSysAging(void); //ϵͳ���ϻ���ʾ

//ϵͳ������������ʾ
void FunLcdOther(void);

void FunKeyLockLight(void);  //����ʱ������ȫ��������ʱֻ�н�������
void FunLcdbrightness(void); //����Ͱ�������
#endif
