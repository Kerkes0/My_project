/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2019-08-31 15:50:28
 * @LastEditors: Please set LastEditors
 */
#include "stm32f0xx.h"
#include "Sysdelay.h"
#include "Sysclock.h"

void delay_ms(uint16_t t){
	time_t a=time_ms;
	while(time_ms-a<t)
		continue;
}

void delay_msf(uint32_t t,const delayfr* list,u8 len){
    if(!len--)
        return;
    u8 i=0;
	time_t a=time_ms;
	while(time_ms-a<t){
        list[i].fun(1);
        _cycleinc(i,0,len);
    }
}

//4MHz机器时钟的情况下，延迟1ms，参数值为150
void delay_nop(uint16_t t){

}

void delay_us(uint32_t t){
	t*=6;
	while(t--)
        continue;
}
