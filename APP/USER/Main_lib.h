/******************************************************
general constant
*******************************************************/
#ifndef _Main_lib_H_
#define _Main_lib_H_

typedef unsigned char BOOL;
typedef signed char schar;
typedef signed short int sint;
typedef signed long int slong;

#define _SYSDEBUG_ 0
#define _RS485DEBUG_ 0
#define _RS485_1200BPS_ 1

///#define DispBoardVer      12//19 year 9 month 24 day  12 Ver Yuhuaping//yyxxll
#define DispBoardVer 12 // 2022年第一版的OEM其版本号为120x                           //22 year 01 month 19 day  11 Ver N机/OEM//yxl-5   第一版

#define EQU ==
#define AND &&
#define OR ||

#define SetBit(VAR, Place) (VAR |= (1 << Place))
#define ClrBit(VAR, Place) (VAR &= ((1 << Place) ^ 0xFFFF))
#define ValBit(VAR, Place) (VAR & (1 << Place))

//驱动故障：gu16TripFlg_Driver
#define Involterror 1	 //输入电压异常E001
#define Incurroverh 2	 //输出电流超过限值E002
#define IpmTempoverh 3	 //散热器过热E101
#define Ipmnotemp 4		 //散热器传感器故障E102
#define Insysdriverr 5	 //主控驱动故障E103
#define Inmotmisitem 6	 //电机缺相保护E104
#define Incurrerror 7	 //交流电流采样电路故障E105
#define Indccurrer 8	 // DC电压异常E106
#define Inpfcprot 9		 // PFC保护E107
#define Inmotoverload 10 //电机功率超载E108
#define Inmotcurrerr 11	 //电机电流检测电路故障E201
#define Insyseepeer 12	 //主控制EEPROM读写故障E202
#define Inuarterr 13	 //键盘与主控制通信故障E205
#define Inlowflowerr 14	 //干烧保护E207

//显示板故障：gu16TripFlg_Board
#define Owrtcerror 1	// RTC时钟读写故障E203
#define Oweeperror 2	//键盘EEPROM读写故障E204
#define Ow485error 3	// 485通讯故障E206
#define Ownowaterpr 4	//传感器故障E208
#define Owupwaterfail 5 //自吸失败E209

//降速故障：gu16TripFlg_Warn
#define LsHighTemp 1 //高温预警降速AL01
#define Lslowvolt 2	 // AC欠压降速AL02
#define Wnfrostpro 3 //防冻保护AL03

//gu8TimeFlg
#define bTime5ms 1
#define bTime500ms 2
#define bTime2s 3

//通常模式：gu8SetMode
#define SPEED 0x01
#define CONTROL 0x02
#define TIME 0x03

//外部控制模式：u8OutCtrlState
#define Ctrl_485 0
#define Ctrl_Analog 1
#define Ctrl_Din 2
#define Ctrl_485Off 4
#define Ctrl_AnalogOff 5
#define Ctrl_DinOff 6


#define MODE_Ctrl1 0x12
//#define MODE_Ctrl2 0x22

#define NOKEYPRESS 0x00
#define PRESS_POWER 0x01
#define PRESS_MODE 0x02
#define PRESS_TIME 0x04
#define PRESS_UP 0x08
#define PRESS_DOWN 0x10
#define PRESS_CONTROL 0x20
#define PRESS_TIME_UP 0x0C
#define PRESS_TIME_DOWN 0x14
#define PRESS_UP_DOWN 0x18
#define PRESS_TIME_CONTROL_MODE 0x26 // yxl-5
#define PRESS_TIME_CONTROL 0x24		 // yxl-5
#define PRESS_TIME_UP_MODE 0x0e		 // 20211126+
#define PRESS_TIME_MODE 0x06		 // 20211126+
#define PRESS_MODE_CONTROL 0x22		 // yxl-bash //20211210+
#define PRESS_MODE_UP 0x0A			 // yxl-bash //20220113+
#define PRESS_CONTROL_UP 0x28		 // yxl-5  //20211222

//gu8KeyFlg
#define bPower 0x01
#define bUnlock 0x02
#define bWash 0x02
#define bUp 0x04
#define bControl 0x08
#define bDown 0x10
#define bTime 0x20

#define bLongPower (0x80 | bPower)
#define bLongMode (0x80 | bUnlock)
#define bLongWash (0x80 | bWash)
#define bLongUp (0x80 | bUp)
#define bLongControl (0x80 | bControl) // YXL-5
#define bLongDown (0x80 | bDown)
#define bLongTime (0x80 | bTime)

#define bLongUpDown (0x80 | bUp | bDown)
#define bLongTimeUp (0x80 | bTime | bUp)
#define bLongTimeDown (0x80 | bTime | bDown)
#define bLongUnlockControl (0x80 | bUnlock | bControl)
#define bLongUnlockTime (0x80 | bUnlock | bTime)
#define bLongControlUp (0x80 | bControl | bUp)
#define bLongControlDown (0x80 | bControl | bDown)

//gu16NoKeyFlg
#define bPress 1
#define bValBuz 4


//u8OutCtrl_Din
//#define bOutCtrl_SelCore 7 //不使用时，默认5芯
#define bOutCtrl_Stop 1	   // Din1与Gnd短接
#define bOutCtrl_1 2	   // Din2与Gnd短接
#define bOutCtrl_2 3	   // Din3与Gnd短接
#define bOutCtrl_3 4	   // Din4与Gnd短接

//gu8SetTimeFlg
#define bNowTime 1	   //在RTC芯片中保存时钟时间
#define bSetTime 6	   //保存时钟时间和时间段
#define bInvalid 2	   //全部定时无效
#define bRun 3		   //有定时段运行
#define bStay 4		   //在等待定时段运行
#define bQuitSetTime 5 //

//gu8SetFlg
#define bTripSave 2 //发生故障时立即进行存储

//MonitorComStatus
#define Tx 0
#define Rx 1
#define RxWt 2
#define TxWt 3

//subBuzCtrl();
#define SHORTBE 15
#define LONGBE 45
#define TESTBE 150

//首次接通电源后进入的功能：u8FirstFunIn
#define LowGate 1  //流量降至4/5

// typedef struct _stSETTIME{
//	schar Spd;
//	uchar SpdGrade;
// }_stSETTIME;

typedef struct _stCodeDate
{
	uchar Year;
	uchar Month;
	uchar Day;
	uchar Ver;
} _stCodeDate;

#define EI() __EI()
#define DI() __DI()
#define Nop __asm(" NOP")



#endif
