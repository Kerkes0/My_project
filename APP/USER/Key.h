#ifndef _KEY_H__
#define _KEY_H__

#include "syscore.h"
#include "main.h"
#include "IC74HC165.h"

/////////////////////////////////////////
//#define DIN1_PIN_Rd PAin(7) 
//#define DIN2_PIN_Rd PBin(0)
//#define DIN3_PIN_Rd PAin(1)
//#define DIN4_PIN_Rd PAin(0)
//�������Ķ�ȡ��������������޸�
////////////////////////�޸ġ���20211020
#define DIN1_PIN_Rd (switch_din & 0x01)
#define DIN2_PIN_Rd (switch_din & 0x02)
#define DIN3_PIN_Rd (switch_din & 0x04)
#define DIN4_PIN_Rd (switch_din & 0x08)


#define KEYTIME_PIN_Rd PCin(15)
#define KEYDOWN_PIN_Rd PBin(0)
#define KEYPOWER_PIN_Rd PAin(0)
#define KEYUP_PIN_Rd PCin(14)
#define KEYMODE_PIN_Rd PAin(1)
#define KEYCONTROL_PIN_Rd PAin(7)
/////////////////////////////////20211020

extern u16 gu16NoKeyFlg;
extern u8 u8OutCtrl_Din;//Din����ʱ��������Ŀ
extern void Key_Init(void);

#endif
