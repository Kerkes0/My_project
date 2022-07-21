/******************************************************************
 * Module		:	Uart.c
 * Version		: V1.0
 * Compiler	: F2MC-8L/8FX Family SOFTUNE Workbench V30L33
 * Mcu				: MB95F636K
 * Programmer: Yuhp
 * Date			: 2018/7
 *******************************************************************/
#include "Uart.h"
#include "User.h"
#include "Suart.h"
#include "Uart485.h"
#include "IC74HC165.h"
#include "Mode.h"
/******************************************************************
const
******************************************************************/
#define bRxdEnd 1
#define bRxdMEnd 2

#define UTStartCode 0xAA //������ͨѶͷ��
#define UTEndCode 0x55	 //������ͨѶ������

uint atextwt = 0xffffffff; //
/******************************************************************
variables in file
******************************************************************/
uchar sndBuffer[7] = {0};
uchar rcvBuffer[18] = {0};
uchar RxDataLen = 0;
uchar commCount = 0;
uchar MotorComStatus = Tx;
uchar gu8MotorComFlg = 0;
uchar gu8MotorComTm = 0; //�ȴ�������Ϣ���������ʱ��
uchar gu8MotorComTripTm = 0;

/// uchar  gu8MonitorTxd[10] = {0};//yxl
uchar gu8MonitorTxd[20] = {0}; // yxl

uchar gu8MonitorTxdSn = 0;
uchar MonitorComStatus = Rx;
// uchar  gu8MonitorComTm = 2;
_stCodeDate gstCodeTime = {0, 0, 0, 1};

/// uchar  gu8MonitorRxd[10] = {0};//yxl
uchar gu8MonitorRxd[20] = {0}; // yxl

uchar gu8MonitorRxdSn = 0;
uint gu16UartSpd = 0;
uchar gu8MonitorTxdTm = 0;
uchar gu8MonitComErrTm = 0;

///-----------------------------//yxl
uchar gMonitorMaxSn = 9;
uchar gMonitor1BtyeTm = 0;
uchar gu8MonitorTxdBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint gu16UartSpd_buf = 0;	  // yyxxll4
uchar gu8senddata_handle[10]; // yxl-5
uchar Modcode[4];			  // yxl-5
uchar fg_ageing_ok = 0;		  //�����ϻ����Ա�־

uint gu16AcVolt_lvbuf = 220;
uchar gu8SpmTempAvg_lvbuf = 30;
uchar count_gu8SpmTempAvg_lvbuf = 0;
uchar count_gu16AcVolt_lvbuf = 0;
uchar fg_host_uart_error = 0;
uchar fg_slave_uart_error = 0;

u8 fg_485_cont_ok = 0;		 // 20220530+0Ϊδ���Σ�485����ʱ���β��ְ���
u8 u8Ctrl485OnUseTm = 0;	 // 485�������ƺ����ʱ��15s
uchar gu8MotorTripClrTm = 0; //���յ��޹��ϳ���5s��ָ�����

uchar gu8MotorTripBuf = 0;

///-----------------------------//yxl

uchar atextTripWt = 0xff;  //�����ã��ǵ�ɾ����������
uchar atextTemp = 0xff;	   //�����ã��ǵ�ɾ���¶�
uint atextAc = 0xffffffff; //�����ã��ǵ�ɾ��ac��ѹ
u8 atextageing = 0xff;	   //�����ã��ǵ�ɾ���ϻ���־
// uchar atext2 = 0;//�����ã��ǵ�ɾ
// uchar atext3 = 0;//�����ã��ǵ�ɾ

void IrqMotorCom(void);
void FunMotorCom(void);
static uchar MotorComCheckSum(uchar u8Tmp[], uchar Max);

//�������ݰ�������
uint8_t Com_rx_process(uint8_t data)
{
	rcvBuffer[RxDataLen] = data;
	if (rcvBuffer[0] != UTStartCode)
	{
		RxDataLen = 0;
	}
	else
	{
		if (++RxDataLen >= 18)
		{
			SetBit(gu8MotorComFlg, bRxdEnd);
			gu8MotorComTm = 5; //���յ���������Ϣ�󣬵ȴ��ظ���ʱ��
			MotorComStatus = TxWt;
		}
	}
	return 0;
}

void IrqMotorCom(void)
{

	if (MotorComStatus EQU Rx) //����
	{
		suart1_SetRxProcess(Com_rx_process);
	}
	else if (MotorComStatus EQU Tx)
	{
		static uint8_t txFlag = 0;

		if (!txFlag)
		{
			suart1_send(sndBuffer, 7);
			txFlag = 1;
		}
		else if (!suart1_TxState())
		{
			MotorComStatus = Rx;
			txFlag = 0;
		}
	}
}

static uchar MotorComCheckSum(uchar u8Tmp[], uchar Max)
{
	uchar i, sum = 0;

	for (i = 1; i < Max; i++)
	{
		sum += u8Tmp[i];
	}

	return (sum);
}

#if _SYSDEBUG_
/******************************************************************
Name		:subMotorRunSpd
Input		:
Output	:
Comments:
******************************************************************/
void subMotorRunSpd(void)
{
	uint data;

	if (gu16MotorSetSpd EQU 0)
	{
		gu16MotorRunSpd = 0;
	}
	else
	{
		data = (uint)(gu16MotorSetSpd << 1);

		if (gu16MotorRunSpd EQU 0)
			gu16MotorRunSpd = 600;
		else
		{
			if (gu16MotorRunSpd < data)
			{
				gu16MotorRunSpd += 50;
				if (gu16MotorRunSpd > data)
					gu16MotorRunSpd = data;
			}
			else if (gu16MotorRunSpd > data)
			{
				gu16MotorRunSpd -= 50;
				if (gu16MotorRunSpd < data)
					gu16MotorRunSpd = data;
			}
		}
	}
}
#endif

void FunMotorCom(void)
{
	uint data = 0;
	//�ȴ�����ʱ��Ϊ0����������
	if (gu8MotorComTm EQU 0)
	{
		sndBuffer[1] = 0x01;

		if (gu16MotorSetSpd > 0)
			sndBuffer[2] = 0x01;
		else
			sndBuffer[2] = 0x02;
		sndBuffer[3] = (uchar)((gu16MotorSetSpd & 0xff00) >> 8);
		sndBuffer[4] = (uchar)(gu16MotorSetSpd & 0xff);

		sndBuffer[0] = 0xAA;							   //ͷ��
		sndBuffer[6] = 0x55;							   //������
		sndBuffer[5] = MotorComCheckSum(&sndBuffer[0], 5); //У����

		rcvBuffer[0] = 0;
		RxDataLen = 0;

		gu8MotorComTm = 10; //�ȴ�������Ϣ��������ļ��ʱ��
		commCount = 0;
		MotorComStatus = Tx;

#if _SYSDEBUG_
		gu8MotorComTripTm = 0;
		subMotorRunSpd();
		gstCodeTime.Year = 19;
		gstCodeTime.Day = 23;
		gstCodeTime.Month = 04;
		gstCodeTime.Ver = 0x03;
#else

		if ((gu8MotorComTripTm >= 30) OR((gu8MotorTrip EQU 17)AND(gu8PowerOnTm > 6)))
		{
			fg_host_uart_error = 1;
		}
		else
		{
			fg_host_uart_error = 0;
		}

		if ((fg_slave_uart_error == 1) || (fg_host_uart_error == 1)) // yxl-5  �����ӻ����ܳ�������
		{

			SetBit(gu16TripFlg_Driver, Inuarterr); // yyxxll   ͨѶ����  YXL-5
		}

		else
		{
			ClrBit(gu16TripFlg_Driver, Inuarterr); // yyxxll   ͨѶ����  YXL-5
		}

#endif
	}

	if (ValBit(gu8MotorComFlg, bRxdEnd))
	{
		ClrBit(gu8MotorComFlg, bRxdEnd);

		if ((MotorComCheckSum(&rcvBuffer[0], 16) EQU rcvBuffer[16])AND(rcvBuffer[17] EQU UTEndCode))
		{
			gu8MotorComTripTm = 0;

			if (rcvBuffer[1] EQU 0x01)
			{
				gu8MotorTripBuf = rcvBuffer[3]; //�������ϣ���8λΪӲ�����ϡ���8λΪ������ϣ�	//YXL-5  ����ʱ������
				if (atextTripWt == 0xff)
				{
				}
				else
				{
					gu8MotorTripBuf = atextTripWt; //������,�ǵ�ɾ����������
				}
				if (gu8MotorTripBuf)
				{
					if (gu8MotorTrip EQU 0)
					{
						gu8MotorTrip = gu8MotorTripBuf;
						if (gu8MotorTrip EQU 17)
							gu8MotorTripClrTm = 0; // yxl-5
						else
							gu8MotorTripClrTm = 10;
					}
				}
				else
				{
					//�޹���5s��ȷ���޹���
					if (gu8MotorTripClrTm == 0)
					{
						gu8MotorTrip = 0; //��ʱȷ������Ϊ0
					}
				}

#if _SYSDEBUG_
				gu8MotorTrip = 0;
				gu8MotorTripBuf = 0;
#endif

				//��ȡ�¶���ֵ�Ĵ���
				data = (uint)(rcvBuffer[4]) * 256 + (uint)(rcvBuffer[5]);
				if (data < 100)
					data = 100;
				if (data > 1200)
					data = 1200;
				gu8SpmTempAvg_lvbuf = (uchar)((data) / 10); // IPMƽ���¶�

				if (gu8SpmTempAvg_lvbuf > u8SpmTempTab[0]) // yxl-5  �����˲�
				{
					count_gu8SpmTempAvg_lvbuf++;
					if (count_gu8SpmTempAvg_lvbuf >= 3)
					{
						count_gu8SpmTempAvg_lvbuf = 3;
						gu8SpmTempAvg = gu8SpmTempAvg_lvbuf;
					}
				}
				else
				{
					count_gu8SpmTempAvg_lvbuf = 0;
					gu8SpmTempAvg = gu8SpmTempAvg_lvbuf;
				}
				if (atextTemp == 0xff)
				{
				}
				else
				{
					gu8SpmTempAvg = atextTemp; //�����ã��ǵ�ɾ���¶�
				}

				// AC��ѹ��ֵ�Ĵ���
				gu16AcCurrent = (uint)(rcvBuffer[6]) * 256 + (uint)(rcvBuffer[7]);	  // AC��������Чֵ��
				gu16AcVolt_lvbuf = (uint)(rcvBuffer[8]) * 256 + (uint)(rcvBuffer[9]); // AC��ѹ��RMS��
				if (gu16AcVolt_lvbuf < 200)											  // yxl-5  �����˲�
				{
					count_gu16AcVolt_lvbuf++;
					if (count_gu16AcVolt_lvbuf >= 5) // yxl-5  5���˲�
					{
						count_gu16AcVolt_lvbuf = 5;
						gu16AcVolt = gu16AcVolt_lvbuf; //���AC��ѹֵ
					}
				}
				else
				{
					count_gu16AcVolt_lvbuf = 0;
					gu16AcVolt = gu16AcVolt_lvbuf; //���AC��ѹֵ
				}
				if (atextAc == 0xffffffff)
				{
				}
				else
				{
					gu16AcVolt = atextAc; //�����ã��ǵ�ɾ��ac��ѹ
				}

				gu16PhaseCurrent = (uint)(rcvBuffer[10]) * 256 + (uint)(rcvBuffer[11]); //������������Чֵ��
				gu16DcVolt = (uint)(rcvBuffer[12]) * 256 + (uint)(rcvBuffer[13]);		// DC��ѹ��ƽ��ֵ��

				data = (uint)(rcvBuffer[14]) * 256 + (uint)(rcvBuffer[15]); //����
				if (data < 5)
					data = 0; // yxl  ��Ϊ5 50W����ҲҪ��ʾ

				gu16MotorWt = data;
				if (gu16MotorWt < 5)
				{
					gu16MotorWt = 0;
				}
				if (atextwt == 0xffffffff)
				{
				}
				else
				{
					gu16MotorWt = atextwt;
				}
			}
			//�������ٶȡ�ʱ�䡢����汾
			else if (rcvBuffer[1] EQU 0x02)
			{
				gu16UartRunSpd = (uint)(rcvBuffer[2]) * 256 + (uint)(rcvBuffer[3]);
				gstCodeTime.Year = rcvBuffer[13];
				gstCodeTime.Day = DispBoardVer; // rcvBuffer[14];
				gstCodeTime.Month = (rcvBuffer[15] & 0xf0) >> 4;
				gstCodeTime.Ver = (rcvBuffer[15] & 0x0f);
			}
			//��������ϻ�
			else if (rcvBuffer[1] EQU 0x03) // yxl-5
			{

				Modcode[0] = rcvBuffer[2];
				Modcode[1] = rcvBuffer[3];
				Modcode[2] = rcvBuffer[4];
				Modcode[3] = rcvBuffer[5];

				//���յ��ϻ���־
				if (rcvBuffer[7] & 0x80)
					fg_ageing_ok = 1; //�����ϻ�����
				else
					fg_ageing_ok = 0;
				//				fg_ageing_ok  = atext;//�����ã��ǵ�ɾ���ϻ���־
			}
			if (atextageing == 0xff)
			{
			}
			else
			{
				fg_ageing_ok = atextageing;
			}
		}
	}
}

const unsigned char auchCRCHi[] =
	{
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

// CRC??????
const unsigned char auchCRCLo[] =
	{
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
		0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
		0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
		0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
		0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
		0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
		0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
		0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
		0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
		0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
		0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
		0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
		0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
		0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
		0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
		0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

uint CRC_SwCalCRC16(uchar *pData, uchar Size)
{
	uint crc = 0;
	uchar uchCRCHi = 0xFF;
	uchar uchCRCLo = 0xFF;
	uchar uIndex;

	while (Size)
	{
		uIndex = uchCRCHi ^ *pData++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
		Size--;
	}

	crc = (((uint)uchCRCHi << 8) | uchCRCLo);

	return crc;
}
/*
void IrqMonitorCom(void)
{
	if (MonitorComStatus EQU Tx)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		// while(gu8MonitorTxdSn < gMonitorMaxSn)
		if (gu8MonitorTxdSn < gMonitorMaxSn)
		{
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
				;
			USART_SendData(USART1, gu8MonitorTxd[gu8MonitorTxdSn]);
			gu8MonitorTxdSn++;
		}
		else
		{
			gu8MonitorRxd[1] = 0;
			gu8MonitorRxdSn = 0;
			gu8MonitorTxdTm = 1;
			MonitorComStatus = RxWt;
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
				;
			GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		}
	}
}

void USART1_IRQHandler(void) // YXL-5
{
	uchar data;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //
	{
		if (MonitorComStatus EQU Rx) //����
		{
			data = USART_ReceiveData(USART1); //��ȡ���յ�������
			gu8MonitorRxd[gu8MonitorRxdSn] = data;
			if (gu8MonitorRxd[0] != UTStartCode)
			{
				gu8MonitorRxdSn = 0;
			}
			else
			{
				if (gu8MonitorRxdSn < 19)
				{
					gu8MonitorRxdSn++;
				}
				else
				{
					gu8MonitorRxdSn = 0;
				}
				gMonitor1BtyeTm = 0;
			}
		}
	}
}
*/

//--------------------------------------------��485���պͷ���
void IrqMonitorCom(void)
{
	if (MonitorComStatus EQU Tx)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		if (gu8MonitorTxdSn < gMonitorMaxSn)
		{
			if (Uart485_sendok()) //���Է���ʱ
			{
				Uart485_send(gu8MonitorTxd[gu8MonitorTxdSn]);
				gu8MonitorTxdSn++;
			}
		}
		else
		{
			gu8MonitorRxd[1] = 0;
			gu8MonitorRxdSn = 0;
			gu8MonitorTxdTm = 1;
			MonitorComStatus = RxWt;
		}
	}
	if (Uart485_sendok()) //������ʱ�����Է���ʱ��
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	}
}

void USART485_Handler(void) // YXL-5
{
	uchar data;
	if (Uart485_handler()) // 485���ջ�����������ʱ
	{
		if (MonitorComStatus EQU Rx) //����
		{
			data = Uart485_recv(); //��ȡ���յ�������
			gu8MonitorRxd[gu8MonitorRxdSn] = data;

			if (gu8MonitorRxd[0] != UTStartCode)
			{
				gu8MonitorRxdSn = 0;
			}
			else
			{
				if (gu8MonitorRxdSn < 19)
				{
					gu8MonitorRxdSn++;
				}
				else
				{
					gu8MonitorRxdSn = 0;
				}
				gMonitor1BtyeTm = 0;

				///------------------------------//yxl
			}
		}
	}
}
//--------------------------------------------��485���պͷ���
void FunMonitorCom(void)
{
	Uart485_correction(); // 485�������жϹر�ʱ�����´��ж�
	uint data, Num, Addr, j;
	uchar Crc_L, Crc_H, i, Byte, k;

	if (u8Ctrl485OnUseTm == 0)
	{
		ClrBit(u8OutCtrlState, Ctrl_485);	 // 485���ƽ���
		ClrBit(u8OutCtrlState, Ctrl_485Off); // 485���ƿ���
		gu16UartSpd = 0;
	}

	if ((MonitorComStatus EQU Rx)AND(gu8MonitorRxd[0] EQU UTStartCode))
	{
		if (gMonitor1BtyeTm > 5)
		{
			if (gu8MonitorRxd[1] EQU 0x03)
			{
				if (gu8MonitorRxdSn > 7)
					SetBit(gu8MotorComFlg, bRxdMEnd);
				Byte = gu8MonitorRxdSn;
			}
			else if (gu8MonitorRxd[1] EQU 0x10)
			{
				if (gu8MonitorRxdSn > 9)
					SetBit(gu8MotorComFlg, bRxdMEnd);
				Byte = gu8MonitorRxdSn;
			}
		}
	}
	else
	{
		gMonitor1BtyeTm = 0;
	}

	if (ValBit(gu8MotorComFlg, bRxdMEnd))
	{
		MonitorComStatus = TxWt;
		ClrBit(gu8MotorComFlg, bRxdMEnd);

		if ((Byte > 7) AND(Byte < 20))
		{
			data = CRC_SwCalCRC16(&gu8MonitorRxd[0], Byte - 2);

			Crc_L = (uchar)(data & 0xff);
			Crc_H = (uchar)((data & 0xff00) >> 8);
			///-----------------------//yyxxll
			if ((Crc_L EQU gu8MonitorRxd[Byte - 1]) AND(Crc_H EQU gu8MonitorRxd[Byte - 2]))
			{
				Addr = gu8MonitorRxd[2];
				Addr = Addr << 8;
				Addr += gu8MonitorRxd[3];
				Num = gu8MonitorRxd[4];
				Num = Num << 8;
				Num += gu8MonitorRxd[5];
				if (gu8MonitorRxd[1] EQU 0x03)
				{
					if ((Addr > 2000) AND(Addr < 2005) AND(Num > 0) AND(Num < 5) AND((Addr + Num) < 2006)) // yyxxll2����Э��
					{
						if ((gu16UartSpd >= 24) && (gu8SetMode != 0)) // yyxxll  ��������Ҫ����  yxl-5
						{
							gu8SetMode = SPEED;
							gu8SetMode_eep = SPEED; // yxl  �ⲿ���ƻָ�Ϊת��ģʽ
						}
						gu8MonitorTxd[0] = 0xAA;
						gu8MonitorTxd[1] = 0x03;
						gu8MonitorTxd[2] = (uchar)(Num * 2);
						j = Addr - 2001;
						j = j * 2;
						i = 0;
						while (i < gu8MonitorTxd[2])
						{
							gu8MonitorTxd[3 + i] = gu8MonitorTxdBuf[i + j];
							i++;
						}
						gMonitorMaxSn = i + 5;
						i += 3;
						data = CRC_SwCalCRC16(&gu8MonitorTxd[0], i);
						gu8MonitorTxd[i] = (uchar)((data & 0xff00) >> 8);
						gu8MonitorTxd[i + 1] = (uchar)(data & 0xff);
						//	gu8MonitorTxdTm = 100;
						gu8MonitorTxdTm = 6; //�޸�Ϊ50ms

						gu8MonitComErrTm = 0;
					}
					else
					{
						gu8MonitorRxdSn = 0;
						gu8MonitorRxd[1] = 0;
						MonitorComStatus = Rx;
					}
				}
				else if (gu8MonitorRxd[1] EQU 0x10)
				{
					if (Addr EQU 3001)
					{
						u8Ctrl485OnUseTm = 30;			  // 485�������ƺ����ʱ��15s
						SetBit(u8OutCtrlState, Ctrl_485); // 485��������

						gu16UartSpd = gu8MonitorRxd[7];
						gu16UartSpd = gu16UartSpd << 8;
						gu16UartSpd += gu8MonitorRxd[8];
						//
						if (gu16UartSpd >= 100)
						{
							gu16UartSpd = u16LadderSpd[16];
						}
						if (gu16UartSpd < 30 && gu16UartSpd > 0)
						{
							gu16UartSpd = 30;
						}
						if (gu16UartSpd == 0)
							ClrBit(u8OutCtrlState, Ctrl_485Off); // 485���ƿ���
						else
							SetBit(u8OutCtrlState, Ctrl_485Off); // 485���ƹػ�
					}

					for (i = 0; i < 6; i++)
						gu8MonitorTxd[i] = gu8MonitorRxd[i];
					gMonitorMaxSn = 8;
					data = CRC_SwCalCRC16(&gu8MonitorTxd[0], 6);
					gu8MonitorTxd[6] = (uchar)((data & 0xff00) >> 8);
					gu8MonitorTxd[7] = (uchar)(data & 0xff);
					// gu8MonitorTxdTm = 100;

					gu8MonitorTxdTm = 6; //�޸�Ϊ50ms

					gu8MonitComErrTm = 0;
				}
				else
				{
					gu8MonitorRxdSn = 0;
					gu8MonitorRxd[1] = 0;
					MonitorComStatus = Rx;
				}
			}
			else
			{
				gu8MonitorRxdSn = 0;
				gu8MonitorRxd[1] = 0;
				MonitorComStatus = Rx;
			}
		}
		else
		{
			gu8MonitorRxdSn = 0;
			gu8MonitorRxd[1] = 0;
			MonitorComStatus = Rx;
		}
	}
	else
	{

		gu8MonitorTxdBuf[0] = 0x00;
		gu8MonitorTxdBuf[1] = 0x00;
		gu8MonitorTxdBuf[2] = 0x00;
		gu8MonitorTxdBuf[3] = 0x00;
		gu8MonitorTxdBuf[4] = 0x00;
		gu8MonitorTxdBuf[5] = 0x00;
		gu8MonitorTxdBuf[6] = 0x00;
		gu8MonitorTxdBuf[7] = 0x00;

		//

		if (ValBit(gu16TripFlg_Driver, Involterror)) //�����ѹ�쳣E001
			gu8MonitorTxdBuf[0] |= 0x80;
		if (ValBit(gu16TripFlg_Driver, Incurroverh)) //�������������ֵE002
			gu8MonitorTxdBuf[0] |= 0x40;
		if (ValBit(gu16TripFlg_Driver, IpmTempoverh)) //ɢ��������E101
			gu8MonitorTxdBuf[0] |= 0x20;
		if (ValBit(gu16TripFlg_Driver, Ipmnotemp)) //ɢ��������������E102
			gu8MonitorTxdBuf[0] |= 0x10;
		if (ValBit(gu16TripFlg_Driver, Insysdriverr)) //������������E103
			gu8MonitorTxdBuf[0] |= 0x08;
		if (ValBit(gu16TripFlg_Driver, Inmotmisitem)) //���ȱ�ౣ��E104
			gu8MonitorTxdBuf[0] |= 0x04;
		if (ValBit(gu16TripFlg_Driver, Incurrerror)) //��������������·����E105
			gu8MonitorTxdBuf[0] |= 0x02;
		if (ValBit(gu16TripFlg_Driver, Indccurrer)) // DC��ѹ�쳣E106
			gu8MonitorTxdBuf[0] |= 0x01;

		if (ValBit(gu16TripFlg_Driver, Inpfcprot)) // PFC����E107
			gu8MonitorTxdBuf[1] |= 0x80;
		if (ValBit(gu16TripFlg_Driver, Inmotoverload)) //������ʳ���E108
			gu8MonitorTxdBuf[1] |= 0x40;
		if (ValBit(gu16TripFlg_Driver, Inmotcurrerr)) //�����������·����E201
			gu8MonitorTxdBuf[1] |= 0x20;
		if (ValBit(gu16TripFlg_Driver, Insyseepeer)) //������EEPROM��д����E202
			gu8MonitorTxdBuf[1] |= 0x10;

		if (ValBit(gu16TripFlg_Board, Owrtcerror)) // RTCʱ�Ӷ�д����E203
			gu8MonitorTxdBuf[1] |= 0x08;
		if (ValBit(gu16TripFlg_Driver, Inlowflowerr)) //���ձ���E207
			gu8MonitorTxdBuf[1] |= 0x04;

		if (ValBit(gu16TripFlg_Driver, Inuarterr)) //������������ͨ�Ź���E205
			gu8MonitorTxdBuf[1] |= 0x02;
		if (ValBit(gu16TripFlg_Board, Ownowaterpr)) //����������E208
			gu8MonitorTxdBuf[1] |= 0x01;

		if ((gu16RunSpd)AND(bCompOnCmd))
		{
			gu8MonitorTxdBuf[3] |= 0x01;
			for (k = 1; k < 17; k++) // yxl-5
			{
				if ((gu16RunSpd >= u16LadderSpd[k - 1]) && (gu16RunSpd < u16LadderSpd[k]))
				{
					gu8MonitorTxdBuf[4] = 0;
					gu8MonitorTxdBuf[5] = (15 + 5 * k);
				}
			}

			if (gu16RunSpd == 0)
			{
				gu8MonitorTxdBuf[4] = 0;
				gu8MonitorTxdBuf[5] = 0;
			}

			if (gu16RunSpd == u16LadderSpd[16])
			{
				gu8MonitorTxdBuf[4] = 0;
				gu8MonitorTxdBuf[5] = 100;
			}

			///=========================//yyxxll2  ����Э��
			gu8MonitorTxdBuf[6] = gu16MotorWt >> 8;
			gu8MonitorTxdBuf[7] = gu16MotorWt;

			///=========================//yyxxll2  ����Э��

			///==========================//yyxxll
		}
	}

#if _RS485DEBUG_
	if (gu8MonitorTxdTm EQU 0)
	{
		gu8MonitorTxd[0] = 0xAA;
		gu8MonitorTxd[1] = 0xC3;
		gu8MonitorTxd[2] = 0x01;
		gu8MonitorTxd[3] = 0x02;
		gu8MonitorTxd[4] = 0x03;
		gu8MonitorTxd[5] = 0x04;
		gu8MonitorTxd[6] = 0x05;
		data = CRC_SwCalCRC16(&gu8MonitorTxd[0], 7);
		gu8MonitorTxd[7] = (uchar)((data & 0xff00) >> 8);
		gu8MonitorTxd[8] = (uchar)(data & 0xff);

		gu8MonitorTxdTm = 100;
		gu8MonitorTxdSn = 0;
		MonitorComStatus = Tx;
	}
#else
	if ((gu8MonitorTxdTm EQU 0)AND(MonitorComStatus EQU TxWt))
	{
		gu8MonitorTxdSn = 0;
		MonitorComStatus = Tx;
	}
	if ((gu8MonitorTxdTm EQU 0)AND(MonitorComStatus EQU RxWt))
	{
		gu8MonitorRxdSn = 0;
		for (i = 0; i < 19; i++)
			gu8MonitorRxd[i] = 0;
		MonitorComStatus = Rx;
	}
#endif
}
