#ifndef _LCD_1621_
#define _LCD_1621_
#include "User.h"
//#include "string.h"



//void	DispTranBit_Sub(uint8_t Data, uint8_t Length);
void TIM3_SetPWM(u16 Compare);//����˫��ģ��PWM���Ƶ���������

void	SendDataToHt1621_Sub( uint8_t *Prt );
void McuLCDInit(void);



#endif
