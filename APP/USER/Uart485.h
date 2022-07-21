#ifndef _UART485_H_
#define _UART485_H_

#include "Syscore.h"

#define UART485_RX_IO() PAin(10)         //模拟485串口接收引脚//YXL-5
#define UART485_TX_IO(val) PAout(9, val) //模拟485串口发送引脚

extern u8 gu8uart485corrTm; // 485非正常中断关闭时间

//对外函数
void Uart485_init(void);          // 485通信初始化
void Uart485_send(const u8 data); //发送数据
u8 Uart485_sendok(void);          //是否可以发送数据，可以则返回1
u8 Uart485_recv(void);            //读取接收缓存内的数据
u8 Uart485_handler(void);         //接收缓存内是否有内容，有则返回1
void Uart485_correction(void);    // 485非正常中断关闭时的纠错

//内部使用函数
void uart485_rx_onoff(u8 mode);
void uart485_rx(void); //定时接收数据处理
void uart485_tx(void); //定时发送数据处理

#endif
