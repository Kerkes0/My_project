/*
 * @Description:
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data:
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-06 13:54:43
 * @note:
 */
#ifndef _MAIN_H__
#define _MAIN_H__

#include "syscore.h"

typedef struct
{
    unsigned T_1ms_ok : 1;
    unsigned T_2ms_ok : 1;
    unsigned T_5ms_ok : 1;
    unsigned T_10ms_ok : 1;
    unsigned T_20ms_ok : 1;
    unsigned T_50ms_ok : 1;
    unsigned T_100ms_ok : 1;
    unsigned T_500ms_ok : 1;
} Time_base;
extern Time_base T_base;



extern u8 u8SysExcuteOrder; //系统执行顺序，从0开始
#endif
