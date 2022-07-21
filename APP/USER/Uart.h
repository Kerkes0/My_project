/*
 * @Description: 
 * @Version: 2.0
 * @Autor: Weipeng
 * @Data: 
 * @LastEditors: weipeng
 * @LastEditTime: 2022-07-05 17:24:13
 * @note: 
 */
#ifndef _UART_H_
#define _UART_H_

#include "syscore.h"

extern u8 u8Ctrl485OnUseTm;//485结束控制后持续时间15s

extern unsigned char fg_485_cont_ok;//0为未屏蔽，485控制时屏蔽部分按键
extern uchar gu8MotorTripClrTm;//接收到无故障持续5s后恢复正常

extern uchar gu8MotorTripBuf;

void IrqMotorCom(void);
void FunMotorCom(void);

void IrqMonitorCom(void);//485发送
void USART485_Handler(void);//485接收



#endif
