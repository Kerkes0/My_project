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
extern u8 getwaterpressTm; // 6s读取一次AD并转换为压力值，和大气压校准
extern u8 getadbaseTm;     // 6s读取一次AD之后求平均，计算得到大气压校准
extern uint32_t WaterPress_value;

extern u8 gu8SensorIserror500msTm;
extern u8 gu8AutoUpWater500msTm;

extern u8 gu8autoupwater2minTm;

extern u8 fg_selfstudy_upwater; // 0为不触发，触发自学习的上水
extern uint WashTmSet;

void McuAdcInit(void);

void FunPSensor_CycleGet(void); //循环采集传感器IO口，需放在1ms时基中
void FunPSonser(void);          //传感器工作:获取AD值、水压值、判断传感器故障

void FunAnalog_CycleGet(void); //循环采集传感器IO口，需放在任意时基中
void FunAnalogCtrl(void);      //模拟量读取

void FunAutoUpWater(void);//获取基准点和判断是否上水自学习

#endif
