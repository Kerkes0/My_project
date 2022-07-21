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

extern u8 u8Ctrl485OnUseTm;//485�������ƺ����ʱ��15s

extern unsigned char fg_485_cont_ok;//0Ϊδ���Σ�485����ʱ���β��ְ���
extern uchar gu8MotorTripClrTm;//���յ��޹��ϳ���5s��ָ�����

extern uchar gu8MotorTripBuf;

void IrqMotorCom(void);
void FunMotorCom(void);

void IrqMonitorCom(void);//485����
void USART485_Handler(void);//485����



#endif
