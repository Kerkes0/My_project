/*
 * @Description: 
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data: 
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-14 18:42:24
 * @note: 
 */
#ifndef _LCDFUNC_H__
#define _LCDFUNC_H__



#include "syscore.h"

#define One_Seg_A 0x02
#define One_Seg_B 0x04
#define One_Seg_C 0x08
#define One_Seg_D 0x10
#define One_Seg_E 0x20
#define One_Seg_F 0x40
#define One_Seg_G 0x80
#define One_SegFig 0x01

extern const uchar u8LcdDispTab1[];
extern const uchar u8LcdWtRatioTab[];
extern const uchar u8LcdCircle_0[];
extern const uchar u8LcdCircle_1[];
extern const uchar u8LcdCircle_2[];
extern const uchar u8TestTab1[];


void DispToItemHi(uchar *p, uint input, uchar Num);
void DispToItemLo(uchar *p, uint input, uchar Num);


void subDispWt(uint data, uchar mode);//功率的四位数码管
void subDispRpm(uint data, uchar mode);//转速四位数码管
void subDispRpm_Special(const u8 *data);//转速的四位数码管_特殊
void subDispNowHour(uint data, uchar mode);//时钟时间小时的二位数码管
void subDispNowMin(uint data, uchar mode);//时钟时间分钟的二位数码管
void subDispStartHour(uint data, uchar mode);//开始时间小时显示


void subDispStartMin(uint data, uchar mode);//开始时间分钟显示

void subDispEndHour(uint data, uchar mode);//结束时间小时显示

void subDispEndMin(uint data, uchar mode);//结束时间分钟显示

void subDispUnit(u8 unit);//转速和流量单位显示单位选择








#endif

