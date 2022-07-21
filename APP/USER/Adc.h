/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-07 19:53:04
 * @note:
 */
#ifndef _ADC_H_
#define _ADC_H_
#include "sysdef.h"

extern u32 AD_fin;
extern u32 AD_fin_buf;
extern u8 getwaterpressTm; // 6s��ȡһ��AD��ת��Ϊѹ��ֵ���ʹ���ѹУ׼
extern u8 getadbaseTm;     // 6s��ȡһ��AD֮����ƽ��������õ�����ѹУ׼
extern uint32_t WaterPress_value;

extern u8 gu8SensorIserror500msTm;
extern u8 gu8AutoUpWater500msTm;

extern u8 gu8autoupwater2minTm;

extern u8 fg_selfstudy_upwater; // 0Ϊ��������������ѧϰ����ˮ
extern uint WashTmSet;

void McuAdcInit(void);

void FunPSensor_CycleGet(void); //ѭ���ɼ�������IO�ڣ������1msʱ����
void FunPSonser(void);          //����������:��ȡADֵ��ˮѹֵ���жϴ���������

void FunAnalog_CycleGet(void); //ѭ���ɼ�������IO�ڣ����������ʱ����
void FunAnalogCtrl(void);      //ģ������ȡ

void FunAutoUpWater(void);//��ȡ��׼����ж��Ƿ���ˮ��ѧϰ

#endif
