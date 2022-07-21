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
/*****************************************��*************************
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
uchar gu8RTCWaitTm = 7; // RTC��ȡ���ʱ��

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

u8 u8fg_Rtc_SysInitFirst = 1; //��ͨ��Դ���ʼ�����½���

//����ǰ��RTCоƬ�ĳ�ʼ��
static void FunRtc_SysInit(void)
{
	static u8 fg_InCheck = 0;		   //�Ƿ���
	static u8 RtcCheckOrder = 0;	   //��ȡ˳��
	static u8 RtcCheckNornalCount = 0; //�����������
	// 1sһ��ѭ��
	if (gu8RTCWaitTm EQU 0)
	{
		gu8RTCWaitTm = 2; // 1s��һ��
		fg_InCheck = 1;	  //������
		if (u8fg_Rtc_SysInitFirst)
		{
			u8fg_Rtc_SysInitFirst = 0;
			RtcCheckOrder = 0; //��ȡ˳���ͷ��ʼ
			RtcCheckNornalCount = 0;
		}
		else
		{
			RtcCheckOrder = 2; //��ȡ˳��Ӳ���2��ʼ
		}
	}
	//��02оƬ���в���,10��
	if (fg_InCheck == 1 && RtcCheckNornalCount <= 10)
	{
		//��PCF8563оƬ���г�ʼ�����ã�ֻ��1��
		if (RtcCheckOrder == 0) //������0��
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;
			gu8RTCNowTimeBuff[1] = 0;
			subWrite8563(0, gu8RTCNowTimeBuff, 2);
		}
		else if (RtcCheckOrder == 1) //������1��
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0x80;		   //���ӱ�����Ч
			gu8RTCNowTimeBuff[1] = 0x80;		   //Сʱ������Ч
			gu8RTCNowTimeBuff[2] = 0x80;		   //�ձ�����Ч
			gu8RTCNowTimeBuff[3] = 0x80;		   //���ڱ�����Ч
			gu8RTCNowTimeBuff[4] = 0x00;		   // CLKOUT��Ч
			gu8RTCNowTimeBuff[5] = 0x00;		   //��ʱ����Ч
			gu8RTCNowTimeBuff[6] = 0x00;		   //��ʱ����ֵ
			subWrite8563(9, gu8RTCNowTimeBuff, 7); //�ӵ�ַ09h��ʼд��
		}
		//ѭ��2��3����
		else if (RtcCheckOrder == 2) //������2��
		{
			RtcCheckOrder++;
			gu8RTCNowTimeBuff[0] = 0;			   //����Ϊ������
			gu8RTCNowTimeBuff[1] = 1;			   //��Ϊ1��/����Ϊ20xx��xxΪ�꣩
			gu8RTCNowTimeBuff[2] = 2;			   //��Ϊ02��
			subWrite8563(6, gu8RTCNowTimeBuff, 3); ////�ӵ�ַ06h��ʼд��
		}
		//��ȡ������2��д������ݣ��ж϶�ȡ������д�������Ƿ�һ�����ж����޴���
		else if (RtcCheckOrder == 3) //������3��
		{
			RtcCheckOrder++;
			subRead8563(2, gu8RTCNowTimeBuff, 7);
			gu8RTCNowTimeBuff[4] = gu8RTCNowTimeBuff[4] & 0x0f;
			gu8RTCNowTimeBuff[5] = gu8RTCNowTimeBuff[5] & 0x0f;
			gu8RTCNowTimeBuff[6] = gu8RTCNowTimeBuff[6] & 0x3f;

			if ((gu8RTCNowTimeBuff[5] EQU 0x01)AND(gu8RTCNowTimeBuff[6] EQU 0x02))
			{
				RtcCheckNornalCount++;
				gu8RTCRdTripCnt = 0; //���rtc�쳣����
			}
			else if (gu8SetMode EQU TIME) // yxl-5
			{
				if (gu8RTCRdTripCnt < 25)
					gu8RTCRdTripCnt++; // rtc�쳣����
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
//�Լ�ʱRTCоƬ�ļ��
static void FunRtc_SysTest(void)
{
	static u8 fg_InCheck = 0;	 //�Ƿ���
	static u8 RtcCheckOrder = 0; //��ȡ˳��
	// 1sһ��ѭ��
	if (gu8RTCWaitTm EQU 0)
	{
		gu8RTCWaitTm = 2;  // 1s��һ��
		fg_InCheck = 1;	   //������
		RtcCheckOrder = 0; //��ȡ˳���ͷ��ʼ
	}
	//��02оƬ���в���
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
				//�쳣�󣬼�¼����
				if (gu8RTCRdTripCnt < 15)
					gu8RTCRdTripCnt++; // rtc�쳣����
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
//ͨ��ģʽʱRTCоƬ�Ĵ洢�Ͷ�ȡ
static void FunRtc_SysNol(void)
{
	uchar min = 0xff;
	uchar hour = 0xff;
	uchar sec = 0xff;
	uint data = 0;
	//��PCF8563оƬ���в���
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

			//��ȡ��ʱ�������쳣
			if ((min >= 60) OR(hour >= 24) OR(sec >= 60))
			{
				gu8RTCNowTimeBuff[0] = 0;
				gu8RTCNowTimeBuff[1] = 0;
				gu8RTCNowTimeBuff[2] = 0;
				gu8RTCNowTimeBuff[3] = 0;
				gu8RTCNowTimeBuff[4] = 0;
				gu8RTCNowTimeBuff[5] = 0;
				subWrite8563(0, &gu8RTCNowTimeBuff[0], 6); //����д��
				if (gu8SetMode EQU TIME)
				{
					//�쳣�󣬼�¼����
					if (gu8RTCRdTripCnt < 25)
						gu8RTCRdTripCnt++; // rtc�쳣����
					else
						SetBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
				}
			}
			//��ȡ��ʱ���������쳣
			else
			{
				data = (uint)(hour);
				data = data * 60;
				data += min;
				gs16RunNowTm = (sint)(data);
				gu8RTCRdTripCnt = 0;				   //���rtc�쳣����
				ClrBit(gu16TripFlg_Board, Owrtcerror); // yxl-5
			}
		}
	}
}

//����ʱRTCоƬ�Ĺ���
void FunRtc_SysPro(void)
{
	//��ͨ��Դ��ǰ8s�ĳ�ʼ��
	if (SysProState == SysProState_Init)
	{
		FunRtc_SysInit(); //����ǰ��ʱ�ӳ�ʼ��
	}
	//�����Լ�
	if (SysProState == SysProState_Test)
	{
		FunRtc_SysTest(); //�Լ�ʱRTCоƬ�ļ��
	}
	//�����ϻ�
	else if (SysProState == SysProState_Aging)
	{
	}
	//��������
	else if (SysProState == SysProState_Nol)
	{
		FunRtc_SysNol(); //ͨ��ģʽʱRTCоƬ�Ĵ洢�Ͷ�ȡ
	}
	else
	{
		FunRtc_SysNol(); //ͨ��ģʽʱRTCоƬ�Ĵ洢�Ͷ�ȡ
	}
}

/******************************************************************
							The End
******************************************************************/
