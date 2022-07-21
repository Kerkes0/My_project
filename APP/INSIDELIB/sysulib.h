/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-09-19 13:35:44
 * @LastEditTime: 2019-09-19 13:35:44
 * @LastEditors: your name
 */
#ifndef _SYSULIB_H_
#define _SYSULIB_H_

#include "Sysdef.h"

extern u8 CheckSum(u8 data[],u8 len);
extern uint8_t CheckSum(uint8_t data[],uint8_t len);
extern s8 umemcmp(const u8 str1[], const u8 str2[], u16 n);
extern void umemcpy(u8 des[], const u8 src[], u16 n);

extern u8 u8tora(u8 num,char str[],u8 n);
extern u8 u8tona(u8 num,char str[],u8 n);
extern void u8tola(u8 num,char str[],u8 n);
extern void s8tora(s8 num,char str[],u8 n);
extern u32 FastLog2(u32 x);

#endif
