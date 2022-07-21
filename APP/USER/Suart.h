#ifndef _SUART_H_
#define _SUART_H_

#include "Syscore.h"

//#define SUART1_RX_IO() PCin(14) 	//模拟串口接收引脚
//#define SUART1_TX_IO(val) PCout(15,val) 	//模拟串口发送引脚

#define SUART1_RX_IO() PBin(11) 	//模拟串口接收引脚//YXL-5
#define SUART1_TX_IO(val) PBout(10,val) 	//模拟串口发送引脚





extern u8 test_flag;

extern void suart1_init(void);
extern void suart1_listen(u8 en);
extern void suart1_SetRxProcess(uint8_t (*fun)(uint8_t));
extern u8 suart1_recv(u8 data[],u8 len);
extern u8 suart1_recv_len(void);
extern uint8_t suart1_TxState(void);
extern void suart1_send(const u8* data, u8 len);

#endif
