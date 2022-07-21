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


void subDispWt(uint data, uchar mode);//���ʵ���λ�����
void subDispRpm(uint data, uchar mode);//ת����λ�����
void subDispRpm_Special(const u8 *data);//ת�ٵ���λ�����_����
void subDispNowHour(uint data, uchar mode);//ʱ��ʱ��Сʱ�Ķ�λ�����
void subDispNowMin(uint data, uchar mode);//ʱ��ʱ����ӵĶ�λ�����
void subDispStartHour(uint data, uchar mode);//��ʼʱ��Сʱ��ʾ


void subDispStartMin(uint data, uchar mode);//��ʼʱ�������ʾ

void subDispEndHour(uint data, uchar mode);//����ʱ��Сʱ��ʾ

void subDispEndMin(uint data, uchar mode);//����ʱ�������ʾ

void subDispUnit(u8 unit);//ת�ٺ�������λ��ʾ��λѡ��








#endif

