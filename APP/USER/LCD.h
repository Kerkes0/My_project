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

void FunLcdSysUpPowerInit_0(void); //全显示
void FunLcdSysUpPowerInit_1(void); //显示机型码、拨码、压力值

void subDispTrip(void);      //显示故障时间，3s切换下一个
void subDispTripFlash(void); //故障显示(3s一个)

void FunLcdError(void);                               //停机时故障的显示(包括预警代码)
void FunLcdWarnSpeed(const u32 spd);                  //转速模式时，有警告降速时，预警代码交替的显示（只有预警时）
void FunLcdWarnGate(const u32 spd, const u8 unit); //流量模式时，有警告降速时，预警代码交替的显示（只有预警时）

void subDispWt(uint data, uchar mode);
void subDispRpm_Special(const u8 *data); //转速数码管的特殊显示

void FunLcdModeShutDown(void);                         //关机功能的使用时的显示
void FunLcdModeWash(void);                             //自吸时的显示
void FunLcdOutCtrl(const u32 spd);                     //外部控制时的显示
void FunLcdModeSpeed(const u8 spdset);                //转速模式时的显示
void FunLcdModeGate(const u16 gate, const u8 fg_unit); //流量模式时的显示
void FunLcdModeTime_Set(void);                         //定时模式时的显示：设置时间
void FunLcdModeTime_Invalid(void);                     //定时模式时的显示：时间段全部无效运行
void FunLcdModeTime_Run(void);                         //定时模式时的显示：正在运行
void FunLcdModeTime_Stay(void);                        //定时模式时的显示：等待运行

//系统的自检显示
void FunLcdSysTest_0(void);
void FunLcdSysTest_1(const u8 TestNum);
void FunLcdSysAging(void); //系统的老化显示

//系统的其他功能显示
void FunLcdOther(void);

void FunKeyLockLight(void);  //解锁时按键灯全亮，锁定时只有解锁键亮
void FunLcdbrightness(void); //背光和按键亮度
#endif
