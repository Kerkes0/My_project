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

#define WashState_Normal 0x01     //反冲洗自吸
#define WashState_Super 0x02      //超频自吸
#define WashState_SuperStudy 0x12 //超频自吸中的自学习
#define WashState_UpWater 0x03    //上水自吸
#define WashState_SelfStudy 0x10  //自学习

extern u8 u8fg_SysIsOn;   //开关机
extern u8 gu8SetMode;     //模式类型
extern u8 gu8SetMode_Bef; //前一刻模式类型
extern u8 gu8SetMode_ago; //前一次模式类型
extern u8 u8WashState;    //自吸类型
extern u8 u8OutCtrlState; //外部控制类型

#define SysProState_Nol 0     //通常模式
#define SysProState_Test 1    //自检模式
#define SysProState_Aging 2   //老化模式
#define SysProState_Init 0xff //接通电源前8s初始化

//--------------------------------------定时
extern u8 u8ModeTime_SpeedSet[4];
extern u8 u8ModeTime_SpeedSet_Tem[4];
extern u16 u8ModeTime_GateSet[4];
extern u16 u8ModeTime_GateSet_Tem[4];

//--------------------------------------上水
extern u8 fg_auto_upwater;       //上水标志
extern uchar gu8upwateroverokTm; //上水超过1min后，可以开始判断是否提前结束
extern u8 count_upwater_iserror; //上水自吸失败的次数相关
extern u16 gu16upwateriserrorTm; //上水自吸失败重新进入等待时间

//-------------------------------------------系统部分属性记录时间
typedef struct
{
    u32 PowerTm; //接通电源时间
    u32 RunTm;   //开机运行时间
    u32 OnTm;    //开机时间
    u32 OffTm;   //关机时间
    u32 ErrorTm; //故障持续时间
} TypeSysProTime;
extern TypeSysProTime SysProTime; //系统部分属性记录时间
extern u8 SysProState;

//-------------------------------------------自学习


void sys_Countdown(void);

void FunSysProRun(void);  //系统执行模式：老化、自检、通常
void FunSysProInit(void); //系统接通电源后前8s初始化
#endif
