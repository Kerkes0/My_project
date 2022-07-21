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

//Һ�����Ͱ����Ƶİ���/ȫ������
void IC74HC595_En(void)
{
	if (fg_light_pwm == 1)
	{
		TIM_SetCompare2(TIM15, 50); //�����ư���
		TIM3_SetPWM(50); //Һ����ȫ��
	}
	else
	{
		TIM_SetCompare2(TIM15, 250); //������ȫ��
		TIM3_SetPWM(250); //Һ����ȫ��
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

//�����������У�595оƬֻ����ư����ƣ�ֻ��Ҫ1��8λ���ݾ͹��ˣ�������������м�
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
