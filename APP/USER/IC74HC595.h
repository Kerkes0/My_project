#ifndef _IC74HC595_H_
#define _IC74HC595_H_

#include "syscore.h"

typedef struct{
    GPIOtag clk;
    GPIOtag sck;
    GPIOtag sda;
	GPIOtag en;
}IC74HC595tag;

//extern void IC74HC595Init(void);
//extern void IC74HC595_Write(IC74HC595tag* ic,const uint8_t data[] ,uint16_t len);
extern void IC74HC595_Write(IC74HC595tag* ic,const uint8_t data);
extern void  IC74HC595_En(void);


#endif
