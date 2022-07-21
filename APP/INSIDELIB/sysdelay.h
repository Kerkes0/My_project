#ifndef _SYSDELAY_H_
#define _SYSDELAY_H_

#include "Sysdef.h"


typedef struct{
    void (*fun)();
}delayfr;


extern void delay_ms(uint16_t t);
extern void delay_msf(uint32_t t,const delayfr* list,u8 len);
extern void delay_nop(uint16_t t);
extern void delay_us(uint32_t t);

#endif
