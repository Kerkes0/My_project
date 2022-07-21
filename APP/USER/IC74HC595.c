/*
 * @Description: 
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data: 
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-14 17:30:18
 * @note: 
 */
#include "IC74HC595.h"
#include "User.h"


uchar fg_light_step = 0, fg_light_pwm = 0;

//液晶屏和按键灯的半亮/全亮控制
void IC74HC595_En(void)
{
	if (fg_light_pwm == 1)
	{
		TIM_SetCompare2(TIM15, 50); //按键灯半亮
		TIM3_SetPWM(50); //液晶屏全亮
	}
	else
	{
		TIM_SetCompare2(TIM15, 250); //按键灯全亮
		TIM3_SetPWM(250); //液晶屏全亮
	}
}

void IC74HC595_Write(IC74HC595tag *ic, const uint8_t data)
{
	u8 len = 8;
	GPIO_PinOut(ic->sck, 0);
	GPIO_PinOut(ic->clk, 0);
	while (len--)
	{
		GPIO_PinOut(ic->sda, (data >> (len % 8)) & 0x01);
		GPIO_PinOut(ic->clk, 1);
		delay_us(3);
		GPIO_PinOut(ic->clk, 0);
		delay_us(3);
	}
	GPIO_PinOut(ic->sck, 1);
	delay_us(3);
	GPIO_PinOut(ic->sck, 0);
	delay_us(3);
}

//由于这个设计中，595芯片只需控制按键灯，只需要1个8位数据就够了，所以在下面进行简化
//////////////////////////////20211108
// void IC74HC595_Write(IC74HC595tag* ic,const uint8_t data[] ,uint16_t len){
//	GPIO_PinOut(ic->sck,0);
//	GPIO_PinOut(ic->clk,0);
//	while(len--){
//		GPIO_PinOut(ic->sda,(data[len/8]>>(len%8))&0x01);
//		GPIO_PinOut(ic->clk,1);
//		delay_us(1);
//		GPIO_PinOut(ic->clk,0);
//		delay_us(1);
//	}
//	GPIO_PinOut(ic->sck,1);
//	delay_us(1);
//	GPIO_PinOut(ic->sck,0);
//	delay_us(1);
// }
