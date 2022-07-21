/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2019-08-28 15:46:55
 * @LastEditors: Please set LastEditors
 */
#include "stm32f0xx.h"
#include "Sysclock.h"
#include "User.h"
#include "Adc.h"
volatile time_t time_ms=0;


void Sysclock_init(void){

	//系统时钟在SetSysClock()里初始化

	//滴答定时器设置
	SysTick_Config(SystemCoreClock/2000);//500us一次
	
}


u8 SysTick_1ms = 1;
void SysTick_Handler(void)//500us一次
{
	
	
	if(SysTick_1ms)
	{
		SysTick_1ms = 0;
	}
	else
	{
		SysTick_1ms = 1;	
		time_ms++;	//计数
		
		FunPSensor_CycleGet();		  //读取传感器AD值
		sys_Countdown();
	}
	
	IrqMonitorCom();//485发送
	USART485_Handler();//485接收
	

}
