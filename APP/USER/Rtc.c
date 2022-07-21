/******************************************************************
 * Module		:	RTC.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2018/7
 *******************************************************************/
#include "Rtc.h"
#include "User.h"
#include "Siic.h"
#include "Mode.h"
/*****************************************‘*************************
const
******************************************************************/

/******************************************************************
variables in file
******************************************************************/
static uchar gu8RTCNowTimeBuff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/******************************************************************
variables in project
******************************************************************/
uchar gu8RTCInitCnt = 0;
uchar gu8RTCRdTripCnt = 0;
uchar gu8RTCWaitTm = 7; // RTC读取间隔时间

Sciitag Pcf8563_iic = {
	.scl = {GPIOB, GPIO_Pin_8},
	.sda = {GPIOB, GPIO_Pin_9},
	.time_delayus = 20};

/******************************************************************
function in file
******************************************************************/

/******************************************************************
Name		:BinToBcd_Sub
Input		:
Output	:
Comments:
******************************************************************/
uchar BinToBcd_Sub(uchar Bin)
{
	uchar i;
	Bin %= 100;
	i = Bin / 10;
	i = i * 16;
	i |= (Bin % 10);
	return (i);
}

/******************************************************************
Name		:BcdToBin_Sub
Input		:
Output	:
Comments:
******************************************************************/
uchar BcdToBin_Sub(uchar Bcd)
{
	uchar i;
	i = (Bcd & 0x0F);
	i += ((Bcd >> 4) * 10);
	return (i);
}

void subRead8563(uchar Address, uchar *Ptr, uchar ByteNum)
{
	Siic_Recv(&Pcf8563_iic, 0xA2, Address, Ptr, ByteNum);
}

void subWrite8563(uchar Address, uchar *Ptr, uchar ByteNum)
{
	Siic_Send(&Pcf8563_iic, 0xA2, Address, Ptr, ByteNum);
}

u8 u8fg_Rtc_SysInitFirst = 1; //接通电源或初始化重新进入

//运行前的RTC芯片的初始化
static void FunRtc_SysInit(void)
{
	static u8 fg_InCheck = 0;		   //是否检测
	static u8 RtcCheckOrder = 0;	   //读取顺序
	static u8 RtcCheckNornalCount = 0; //检测正常次数
	// 1s一次循环
	if (gu8RTCWaitTm EQU 0)
	{
		gu8RTCWaitTm = 2; // 1s读一次
		fg_InCheck = 1;	  //进入检测
		if (u8fg_Rtc_SysInitFirst)
		{
			u8fg_Rtc_SysInitFirst = 0;
			RtcCheckOrder = 0; //读取顺序从头开始
			RtcCheckNornalCount = 0;
		}
		else
		{
			RtcCheckOrder = 2; //读取顺序从步骤2开始
		}
	}
	//对02芯片进行操作,10次
	if (fg_InCheck == 1 && RtcCheckNornalCount <= 10)
	{
		//对PCF8563芯片进行初始化设置，只有1次
		if (RtcCheckOrder == 0) //【步骤0】
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;
			gu8RTCNowTimeBuff[1] = 0;
			subWrite8563(0, gu8RTCNowTimeBuff, 2);
		}
		else if (RtcCheckOrder == 1) //【步骤1】
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0x80;		   //分钟报警无效
			gu8RTCNowTimeBuff[1] = 0x80;		   //小时报警无效
			gu8RTCNowTimeBuff[2] = 0x80;		   //日报警无效
			gu8RTCNowTimeBuff[3] = 0x80;		   //星期报警无效
			gu8RTCNowTimeBuff[4] = 0x00;		   // CLKOUT无效
			gu8RTCNowTimeBuff[5] = 0x00;		   //定时器无效
			gu8RTCNowTimeBuff[6] = 0x00;		   //定时器数值
			subWrite8563(9, gu8RTCNowTimeBuff, 7); //从地址09h开始写进
		}
		//循环2、3步骤
		else if (RtcCheckOrder == 2) //【步骤2】
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;			   //星期为星期日
			gu8RTCNowTimeBuff[1] = 1;			   //月为1月/世纪为20xx（xx为年）
			gu8RTCNowTimeBuff[2] = 2;			   //年为02年
			subWrite8563(6, gu8RTCNowTimeBuff, 3); ////从地址06h开始写进
		}
		//读取【步骤2】写入的数据，判断读取数据与写入数据是否一致来判断有无错误
		else if (RtcCheckOrder == 3) //【步骤3】
		{
			RtcCheckOrder++;
			subRead8563(2, gu8RTCNowTimeBuff, 7);
			gu8RTCNowTimeBuff[4] = gu8RTCNowTimeBuff[4] & 0x0f;
			gu8RTCNowTimeBuff[5] = gu8RTCNowTimeBuff[5] & 0x0f;
			gu8RTCNowTimeBuff[6] = gu8RTCNowTimeBuff[6] & 0x3f;

			if ((gu8RTCNowTimeBuff[5] EQU 0x01)AND(gu8RTCNowTimeBuff[6] EQU 0x02))
			{
				RtcCheckNornalCount++;
				gu8RTCRdTripCnt = 0; //清除rtc异常次数
			}
			else if (gu8SetMode EQU TIME) // yxl-5
			{
				if (gu8RTCRdTripCnt < 25)
					gu8RTCRdTripCnt++; // rtc异常次数
				else
					SetBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
			}
		}
		else
		{
			fg_InCheck = 0;
			RtcCheckOrder = 2;
		}
	}
}
//自检时RTC芯片的检测
static void FunRtc_SysTest(void)
{
	static u8 fg_InCheck = 0;	 //是否检测
	static u8 RtcCheckOrder = 0; //读取顺序
	// 1s一次循环
	if (gu8RTCWaitTm EQU 0)
	{
		gu8RTCWaitTm = 2;  // 1s读一次
		fg_InCheck = 1;	   //进入检测
		RtcCheckOrder = 0; //读取顺序从头开始
	}
	//对02芯片进行操作
	if (fg_InCheck == 1)
	{
		if (RtcCheckOrder == 0)
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;
			gu8RTCNowTimeBuff[1] = 1;
			gu8RTCNowTimeBuff[2] = 2;
			subWrite8563(6, gu8RTCNowTimeBuff, 3);
		}
		if (RtcCheckOrder == 1)
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;
			gu8RTCNowTimeBuff[1] = 0;
			gu8RTCNowTimeBuff[2] = 0;
			gu8RTCNowTimeBuff[3] = 0;
			gu8RTCNowTimeBuff[4] = 0;
			gu8RTCNowTimeBuff[5] = 0;
			subWrite8563(0, gu8RTCNowTimeBuff, 6);
		}
		else if (RtcCheckOrder == 2)
		{
			RtcCheckOrder++;
			subRead8563(6, gu8RTCNowTimeBuff, 3);
			gu8RTCNowTimeBuff[0] = gu8RTCNowTimeBuff[0] & 0x0f;
			gu8RTCNowTimeBuff[1] = gu8RTCNowTimeBuff[1] & 0x0f;
			gu8RTCNowTimeBuff[2] = gu8RTCNowTimeBuff[2] & 0x3f;

			if ((gu8RTCNowTimeBuff[1] EQU 0x01)AND(gu8RTCNowTimeBuff[2] EQU 0x02))
			{
				;
			}
			else
			{
				//异常后，记录次数
				if (gu8RTCRdTripCnt < 15)
					gu8RTCRdTripCnt++; // rtc异常次数
				else
					SetBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
			}
		}
		else
		{
			fg_InCheck = 0;
			RtcCheckOrder = 0;
		}
	}
}
//通常模式时RTC芯片的存储和读取
static void FunRtc_SysNol(void)
{
	uchar min = 0xff;
	uchar hour = 0xff;
	uchar sec = 0xff;
	uint data = 0;
	//对PCF8563芯片进行操作
	if (gu8RTCWaitTm EQU 0)
	{
		gu8RTCWaitTm = 3;
		if (ValBit(gu8SetTimeFlg, bNowTime))
		{
			ClrBit(gu8SetTimeFlg, bNowTime);
			gu8RTCNowTimeBuff[0] = BinToBcd_Sub(0);
			gu8RTCNowTimeBuff[1] = BinToBcd_Sub(0);
			gu8RTCNowTimeBuff[2] = BinToBcd_Sub(0);
			gu8RTCNowTimeBuff[3] = BinToBcd_Sub((uchar)(gu16SetNowTm % 60));
			gu8RTCNowTimeBuff[4] = BinToBcd_Sub((uchar)(gu16SetNowTm / 60));
			subWrite8563(0x00, &gu8RTCNowTimeBuff[0], 5);
		}
		else
		{
			subRead8563(0x00, gu8RTCNowTimeBuff, 5);
			sec = BcdToBin_Sub(gu8RTCNowTimeBuff[2] & 0x7f);
			min = BcdToBin_Sub(gu8RTCNowTimeBuff[3] & 0x7f);
			hour = BcdToBin_Sub(gu8RTCNowTimeBuff[4] & 0x3f);

			//读取的时间数据异常
			if ((min >= 60) OR(hour >= 24) OR(sec >= 60))
			{
				gu8RTCNowTimeBuff[0] = 0;
				gu8RTCNowTimeBuff[1] = 0;
				gu8RTCNowTimeBuff[2] = 0;
				gu8RTCNowTimeBuff[3] = 0;
				gu8RTCNowTimeBuff[4] = 0;
				gu8RTCNowTimeBuff[5] = 0;
				subWrite8563(0, &gu8RTCNowTimeBuff[0], 6); //重新写入
				if (gu8SetMode EQU TIME)
				{
					//异常后，记录次数
					if (gu8RTCRdTripCnt < 25)
						gu8RTCRdTripCnt++; // rtc异常次数
					else
						SetBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
				}
			}
			//读取的时间数据无异常
			else
			{
				data = (uint)(hour);
				data = data * 60;
				data += min;
				gs16RunNowTm = (sint)(data);
				gu8RTCRdTripCnt = 0;				   //清除rtc异常次数
				ClrBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
			}
		}
	}
}

//运行时RTC芯片的工作
void FunRtc_SysPro(void)
{
	//接通电源后前8s的初始化
	if (SysProState == SysProState_Init)
	{
		FunRtc_SysInit(); //运行前的时钟初始化
	}
	//进入自检
	if (SysProState == SysProState_Test)
	{
		FunRtc_SysTest(); //自检时RTC芯片的检测
	}
	//进入老化
	else if (SysProState == SysProState_Aging)
	{
	}
	//正常进入
	else if (SysProState == SysProState_Nol)
	{
		FunRtc_SysNol(); //通常模式时RTC芯片的存储和读取
	}
	else
	{
		FunRtc_SysNol(); //通常模式时RTC芯片的存储和读取
	}
}

/******************************************************************
							The End
******************************************************************/
