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

/***********关机进入功能显示**********************************/
//查看版本
extern u8 u8SD_SoftWareWerTm; //软件版本的显示时间
//参数设置
extern u8 u8SD_ParamSetTm;            //参数设置的显示和设置时间
extern u8 u8SD_ParamSetOrder;         //参数设置第几项
extern u8 u8SD_ParamSet_OCtrlSpd1;    // Din控制时的转速设置，Din1默认100%
extern u8 u8SD_ParamSet_OCtrlSpd2;    // Din控制时的转速设置，Din1默认80%
extern u8 u8SD_ParamSet_OCtrlSpd3;    // Din控制时的转速设置，Din1默认40%
extern u8 u8SD_ParamSet_HandWashSpd;  // 手动自吸时的转速设置
extern u8 u8fg_SD_ParamSet_AnalogSel; // 0为电流，模拟量控制电流电压切换标志位
extern u8 u8SD_PowerOnTm;             // 25为开启，开机是否进入25min上水
//恢复出厂设置（无水初始化）
extern u8 u8SD_ReStoreDisWaterTm;   //恢复出厂设置（无水初始化）完成后，CA01的显示时间
extern u8 u8SD_IntoReStoreDisWater; // 1为进入，恢复出厂设置（无水初始化）
//传感器是否开启
extern u8 u8SD_PSensorDisTm;   //传感器是否开启标志的显示时间
extern u8 u8fg_SD_PSensorOpen; // 1为开启，开启传感器故障
//查看AD校准值
extern u8 u8SD_PressAdDisTm; //显示压力AD校准值时间

//-------------------------------------------------------
//查看实际压力值
extern uchar count_dis_WaterPress_value_tm; //显示实际压力时间

//--------------------------------------反冲洗自吸
extern uchar gu8Wash;         // yyxxll
extern uint WashTmSet;        //自吸设置时间
extern uint WashTmSet_Normal; //反冲洗自吸的时间设置，记忆用
extern uint WashTm;           //自吸的倒数时间

extern uchar fg_rest_con_ok; //自学习标志

//----------------------------------------------流量
#define Unit_ALL 0xff
#define Unit_percent 0x10 //转速的%单位
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

/*********************设备***********************/
void FunSysParamInit(void); //初始化变量（一般是接通电源后）
void sub1stRam(void);       //初始化设置（恢复出厂设置）
void FunModeShutDown(void); //设备关机的功能操作
void FunModeRemeber(void);  //设备模式记忆
void FunSysParamInit(void);//初始化变量（一般是接通电源后）

/*********************外部控制***********************/
void FunModeOutCtrl_Din(void);    // Din控制时的功能实现
void FunModeOutCtrl_Analog(void); //模拟量控制时的功能实现
void FunModeOutCtrl_485(void);    // 485控制时的功能实现

/*********************自学习与流量范围相关**************/
u16 FunModeGateLimit(const u16 gate, const u8 unit); //限制流量设置值在范围内
u16 FunModeGate_StudyMax(const u8 unit);             //获取自学习流量范围最大值
// u16 FunModeGate_StudyMin(const u8 unit); //获取自学习流量范围最小值
u16 FunMode_SpeedToGate(const u32 speed, const u8 unit);   //转速转换为流量设置量
u32 FunMode_GateToSpeed(const u16 GateSet, const u8 unit); //流量设置量转换为转速
u16 FunMode_UnitRound(const u16 gate, const u8 unit);      //转速转为流量后的取整

u8 FunMode_SpeedToPercent(u32 speed); //转速转换为转速百分比量

/*********************普通模式***********************/
void subModeUpDown(signed char i);                   //运转速率百分比设置（使用上下键调节）
void FunModeGateUpDown(const char i, const u8 unit); //运转流量速率设置（使用上下键调节）
void FunModeGate_UnitSwitch(const char i);           //运行流量单位切换
void FunModeTime_UnitSwitch(const char i);           //定时模式时，运行流量单位切换

u16 FunModeGate_m3hToOhter(const u16 GateSet, const u8 unit); //由m3/h单位转换为其他单位

/*********************自吸***********************/
#define WASH_TWINLETIME 7
void FunModeToNormalWash(void); //进入反冲洗模式
void FunNormalWash_Set(void);   //在反冲洗自吸时，其转速、时间调节、退出

void FunModeToSuperWash(void); //进入超频自吸模式
void FunSuperWash_Set(void);   //在超频自吸时，其转速、时间调节、退出

void FunModeToUpWash(void); //进入上水自吸模式
void FunUpWash_Set(void);   //在上水自吸时，其退出

void FunModeToSelfStudy(void);  //进入自学习模式
void FunReEnterSelfStudy(void); //自学习被打断后需要重新进入
void FunSelfStudy_Set(void);    //在自学习时，设备流量范围学习

#endif
