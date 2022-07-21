/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2022-07-02 15:24:10
 * @LastEditors: weipeng
 */
//系统时间戳功能
#ifndef _SYSCLOCK_H_
#define _SYSCLOCK_H_

#include "Sysdef.h"

#define time_t uint32_t	//时间类型

typedef struct{
	time_t loopt;	//循环间隔时间
	time_t his;	//历史时间
}timer;

extern volatile time_t time_ms; 	//毫秒级时间戳

extern void Sysclock_init(void);

#endif
