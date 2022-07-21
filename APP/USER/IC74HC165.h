/*
 * @Description: 
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data: 
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-14 17:05:42
 * @note: 
 */
#ifndef _IC74HC165_H_
#define _IC74HC165_H_

#include "syscore.h"

typedef struct{
    GPIOtag clk;
    GPIOtag sck;
    GPIOtag sda;
}IC74HC165tag;


extern const uint u16LadderSpd_ip20[];
extern const uint u16LadderSpd_ip25[];
extern const uint u16LadderSpd_ip30[];


extern uchar     sp_2600_con_press[] ;
extern uint16_t  sp_2600_con_flow[] ;


extern const uchar     sp_2600_con_press_ip30[];
extern const uint16_t  sp_2600_con_flow_ip30[]; 



extern void IC74HC165_Read(IC74HC165tag* ic,uint8_t data[],uint16_t len);

void switch_read(void);
void FunForVersionToSet(void);//���ݲ������Ĳ���ѡ��汾�����øð汾��Ĭ����ֵ��Χ

#endif
