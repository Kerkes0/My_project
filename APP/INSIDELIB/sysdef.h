/*
 * @Description: In User Settings Edit
 * @Author: Deng Senyu
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime : 2020-01-20 15:31:34
 * @LastEditors  : Please set LastEditors
 */
#ifndef _SYSDEF_H_
#define _SYSDEF_H_

#include "stm32f0xx.h"

#define GPIO_PinOut(io,val) GPIO_WriteBit(io.GPIO,io.Pin,val?Bit_SET:Bit_RESET)
#define GPIO_PinIn(io)     GPIO_ReadInputDataBit(io.GPIO,io.Pin)

typedef struct{
	GPIO_TypeDef* GPIO;
	uint16_t Pin;
}GPIOtag;

#define PinIn(GPIO,pin)				((((GPIO)->IDR)>>pin)&0x00000001)
#define PinOut(GPIO,pin,val)	((GPIO)->BSRR=(uint32_t)1<<((pin)+((val)?0:16)))
//#define PinType(GPIO,pin,val)	{(GPIO)->MODER &= ~(0x3 << ((pin)<<1));(GPIO)->MODER |= val? 0x1<< ((pin)<<1):0;}
#define PinType(GPIO,pin,val) {}
	
#define PAin(pin) PinIn(GPIOA,pin)
#define PBin(pin) PinIn(GPIOB,pin)
#define PCin(pin) PinIn(GPIOC,pin)
#define PDin(pin) PinIn(GPIOD,pin)
#define PEin(pin) PinIn(GPIOE,pin)
#define PFin(pin) PinIn(GPIOF,pin)

#define PAout(pin,val) PinOut(GPIOA,pin,val)
#define PBout(pin,val) PinOut(GPIOB,pin,val)
#define PCout(pin,val) PinOut(GPIOC,pin,val)
#define PDout(pin,val) PinOut(GPIOD,pin,val)
#define PEout(pin,val) PinOut(GPIOE,pin,val)
#define PFout(pin,val) PinOut(GPIOF,pin,val)

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

#define _int(x)	((int32_t) (x))	//浮点数取整
#define _abs(x)	((x)<0?-(x):(x))		//绝对值
#define _equal(a,b) ((a)==(b))	//是否相等
#define _max(a,b) ((a)>(b)?(a):(b))
#define _min(a,b) ((a)<(b)?(a):(b))


#define _dec(x)	((x)?--(x):(x))
#define _singleinc(x,max) ((x)>=(max)?((x)=(max)):++(x))	//自增限定，X必须为变量
#define _singledec(x,min) ((x)<=(min)?((x)=(min)):--(x))	//自减限定，X必须为变量
#define _singleninc(x,n,max) ((x)>(max)-(n)?((x)=(max)):(x+=n))	//n增限定，X必须为变量
#define _singlendec(x,n,min) ((x)<(min)+(n)?((x)=(min)):(x-=n))	//n减限定，X必须为变量
// #define _cycleinc(x,min,max) ((x)<(max)?(++(x)<=(min)?((x)=(min)):(x)):((x)=(min)))	//限定范围循环自增，X必须为变量，min值必须小于max
// #define _cycledec(x,min,max) ((x)>(min)?(--(x)>=(max)?((x)=(max)):(x)):((x)=(max)))	//限定范围循环自减，X必须为变量，min值必须小于max
#define _cycleinc(x,min,max) ((x)<(max)?++(x):((x)=(min)))	//限定范围循环自增，X必须为变量，min值必须小于max
#define _cycledec(x,min,max) ((x)>(min)?--(x):((x)=(max)))	//限定范围循环自减，X必须为变量，min值必须小于max

#define _setbit(var,place) ((var)|=1<<(place))
#define _clrbit(var,place) ((var)&=~(1<<(place)))
#define _valbit(var,place) ( var & (1<<place))

#define _u8BCD2BIN(x)	(((x)&0x0F)+((x)>>4)*10)
#define _u8BIN2BCD(x)	((x)%10+(((x)/10)<<4))

//             __attribute__((section(".ARM.__at_0x0800299C")));
#define _at(x) __attribute__((at(x)))	//at绝对地址设置

#endif
