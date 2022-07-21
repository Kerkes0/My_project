/******************************************************
User headfile
*******************************************************/

#ifndef _User_H__
#define _User_H__

//device headfile
#include "F2MC_8FX_EEPROM.h"

#include "syscore.h"

//main headfine
#include "Main_lib.h"
#include "Ram.h"     

#include "LCD_1621.h"
#include "IC74HC595.h"
#include "Uart.h"
#include "E2prom.h"

#include "IC74HC165.h"
#include "mcu_api.h"
#include "protocol.h"


extern  uint Self_run2wt;



void sys_Countdown(void);
#endif
