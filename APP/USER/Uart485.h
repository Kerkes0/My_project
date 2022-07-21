#ifndef _UART485_H_
#define _UART485_H_

#include "Syscore.h"

#define UART485_RX_IO() PAin(10)         //ģ��485���ڽ�������//YXL-5
#define UART485_TX_IO(val) PAout(9, val) //ģ��485���ڷ�������

extern u8 gu8uart485corrTm; // 485�������жϹر�ʱ��

//���⺯��
void Uart485_init(void);          // 485ͨ�ų�ʼ��
void Uart485_send(const u8 data); //��������
u8 Uart485_sendok(void);          //�Ƿ���Է������ݣ������򷵻�1
u8 Uart485_recv(void);            //��ȡ���ջ����ڵ�����
u8 Uart485_handler(void);         //���ջ������Ƿ������ݣ����򷵻�1
void Uart485_correction(void);    // 485�������жϹر�ʱ�ľ���

//�ڲ�ʹ�ú���
void uart485_rx_onoff(u8 mode);
void uart485_rx(void); //��ʱ�������ݴ���
void uart485_tx(void); //��ʱ�������ݴ���

#endif
