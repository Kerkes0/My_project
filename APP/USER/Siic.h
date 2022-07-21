/*
 * @Description: In User Settings Edit
 * @Author: Deng Senyu
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2019-08-23 09:09:02
 * @LastEditors: Please set LastEditors
 */
#ifndef _SIIC_H_
#define _SIIC_H_

#include "syscore.h"

typedef struct{
    GPIOtag scl;
    GPIOtag sda;
	uint8_t time_delayus;
}Sciitag;

extern u8 Siic_Com(Sciitag* iic,u8 addr,const u8 comm[],u8 len);
extern u8 Siic_Send(Sciitag* iic,u8 addr,u8 comm,const u8 data[],u8 len);
extern u8 Siic_Recv(Sciitag* iic,u8 addr,u8 comm,u8 data[],u8 len);

#endif
