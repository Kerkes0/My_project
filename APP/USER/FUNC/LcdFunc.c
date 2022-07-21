#include "LcdFunc.h"
#include "User.h"
#include "Mode.h"
#include "ModeFunc.h"

/******************************************************************
const
******************************************************************/
// 7~0
///*    H  L        ____A____
// COM1  X  A        F        B
// COM2  F  B        |___G____|
// COM3  G  C        E        C
// COM4  E  D        |___D____|

#define DIG_A_0 (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F)
#define DIG_A_1 (One_Seg_B | One_Seg_C)
#define DIG_A_2 (One_Seg_A | One_Seg_B | One_Seg_D | One_Seg_E | One_Seg_G)
#define DIG_A_3 (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_G)
#define DIG_A_4 (One_Seg_B | One_Seg_C | One_Seg_F | One_Seg_G)
#define DIG_A_5 (One_Seg_A | One_Seg_C | One_Seg_D | One_Seg_F | One_Seg_G)
#define DIG_A_6 (One_Seg_A | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G)
#define DIG_A_7 (One_Seg_A | One_Seg_B | One_Seg_C)
#define DIG_A_8 (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G)
#define DIG_A_9 (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_F | One_Seg_G)
#define LET_A_A (One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_b (One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_C (One_Seg_A | One_Seg_D | One_Seg_E | One_Seg_F)
#define LET_A_d (One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_G)
#define LET_A_E (One_Seg_A | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_F (One_Seg_A | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_H (One_Seg_B | One_Seg_C | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_L (One_Seg_D | One_Seg_E | One_Seg_F)
#define LET_A_n (One_Seg_C | One_Seg_E | One_Seg_G)
#define LET_A_o (One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_G)
#define LET_A_P (One_Seg_A | One_Seg_B | One_Seg_E | One_Seg_F | One_Seg_G)
#define LET_A_U (One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F)
#define LET_A__ (One_Seg_G)

const uchar u8LcdDispTab1[] = {
	DIG_A_0, DIG_A_1, DIG_A_2, DIG_A_3, DIG_A_4,
	DIG_A_5, DIG_A_6, DIG_A_7, DIG_A_8, DIG_A_9,
	LET_A_A, LET_A_b, LET_A_C, LET_A_d, LET_A_E,
	LET_A_F, LET_A_H, LET_A_L, LET_A_n, LET_A_o,
	LET_A_P, LET_A_U, LET_A__, 0x00, 0x00,
	0x00};

const uchar u8LcdWtRatioTab[] = {
	2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13, 14, 14, 15, 15, 16, 17,
	17, 18, 19, 19, 20};

const uchar u8LcdCircle_0[] = {
	One_Seg_F,
	One_Seg_F | One_Seg_E,
	One_Seg_F | One_Seg_E | One_Seg_D,
	One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C,
	One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B,
	One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B | One_Seg_A,
	One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B | One_Seg_A | One_SegFig,
	One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B | One_Seg_A | One_SegFig | One_Seg_G};

const uchar u8LcdCircle_1[] = {
	One_SegFig,
	One_SegFig | One_Seg_G,
	One_SegFig | One_Seg_G | One_Seg_F,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E | One_Seg_D,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E | One_Seg_D | One_Seg_C | One_Seg_B | One_Seg_A};

const uchar u8LcdCircle_2[] = {
	One_Seg_A,
	One_Seg_A | One_Seg_F,
	One_Seg_A | One_Seg_F | One_Seg_E,
	One_SegFig | One_Seg_G | One_Seg_F | One_Seg_E | One_Seg_A};

const uchar u8TestTab1[] = {
	One_Seg_A,
	One_Seg_A | One_Seg_B,
	One_Seg_A | One_Seg_B | One_Seg_C,
	One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D,
	One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E,
	One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F,
	One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G,
	One_Seg_A | One_Seg_B | One_Seg_C | One_Seg_D | One_Seg_E | One_Seg_F | One_Seg_G | One_SegFig};

void DispToItemHi(uchar *p, uint input, uchar Num)
{
	uchar j = Num;
	uint data = input;

	while (j--)
	{
		*(p + j) = data % 10;
		data = data / 10;
	}
}

void DispToItemLo(uchar *p, uint input, uchar Num)
{
	uchar j = 0;
	uint data = input;

	while (j < Num)
	{
		*(p + j) = data % 10;
		data = data / 10;
		j++;
	}
}

//���ʵ���λ�����
void subDispWt(uint data, uchar mode) // modeΪ1ʱ����������103������ʾ����ʾ103��������ʾ0103
{
	uchar i;
	uchar dataTab[4] = {0, 0, 0, 0};

	if (data > 9999)
		data = 9999;
	DispToItemLo(&dataTab[0], data, 4);
	for (i = 0; i < 4; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[0] = u8LcdDispTab1[dataTab[3]];
	gu8DsipNum[1] = u8LcdDispTab1[dataTab[2]];
	gu8DsipNum[2] = u8LcdDispTab1[dataTab[1]];
	gu8DsipNum[3] = u8LcdDispTab1[dataTab[0]];
	if (mode)
	{
		if (data < 1000)
			gu8DsipNum[0] = 0x00;
		if (data < 100)
			gu8DsipNum[1] = 0x00;
		if (data < 10)
			gu8DsipNum[2] = 0x00;
	}
}

//ת����λ�����
void subDispRpm(uint data, uchar mode) // modeΪ1ʱ����������103������ʾ����ʾ103��������ʾ0103
{
	uchar i; // yyxxll
	uchar dataTab[4] = {0, 0, 0, 0};

	if (data > 9999)
		data = 9999;

	//������dataת��Ϊ�������ִ洢��ataTab[]������
	DispToItemLo(&dataTab[0], data, 4);
	for (i = 0; i < 4; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[15] = u8LcdDispTab1[dataTab[3]];
	gu8DsipNum[4] = u8LcdDispTab1[dataTab[2]];
	gu8DsipNum[5] = u8LcdDispTab1[dataTab[1]];
	gu8DsipNum[6] = u8LcdDispTab1[dataTab[0]];

	if (mode)
	{
		if (data < 1000)
			gu8DsipNum[15] = 0x00; // yxl-5
		if (data < 100)
			gu8DsipNum[4] = 0x00;
		if (data < 10)
			gu8DsipNum[5] = 0x00;
	}
}

//ת�ٵ���λ�����_����
void subDispRpm_Special(const u8 *data)
{
	u8 tab[4];
	for (u8 i = 0; i < 4; i++)
	{
		tab[i] = data[i];
	}
	gu8DsipNum[15] = u8LcdDispTab1[tab[0]];
	gu8DsipNum[4] = u8LcdDispTab1[tab[1]];
	gu8DsipNum[5] = u8LcdDispTab1[tab[2]];
	gu8DsipNum[6] = u8LcdDispTab1[tab[3]];
}

//ʱ��ʱ��Сʱ�Ķ�λ�����
void subDispNowHour(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 24) AND(mode))
		data = 24;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[8] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[7] = u8LcdDispTab1[dataTab[1]];
}

//ʱ��ʱ����ӵĶ�λ�����
void subDispNowMin(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 60) AND(mode))
		data = 60;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[10] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[9] = u8LcdDispTab1[dataTab[1]];
}

//��ʼʱ��Сʱ��ʾ
void subDispStartHour(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 24) AND(mode))
		data = 24;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[8] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[7] = u8LcdDispTab1[dataTab[1]];
}

//��ʼʱ�������ʾ
void subDispStartMin(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 60) AND(mode))
		data = 60;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[10] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[9] = u8LcdDispTab1[dataTab[1]];
}

//����ʱ��Сʱ��ʾ
void subDispEndHour(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 24) AND(mode))
		data = 24;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[12] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[11] = u8LcdDispTab1[dataTab[1]];
}

//����ʱ�������ʾ
void subDispEndMin(uint data, uchar mode)
{
	uchar i;
	uchar dataTab[2] = {0, 0};

	if ((data > 60) AND(mode))
		data = 60;
	DispToItemLo(&dataTab[0], data, 2);
	for (i = 0; i < 2; i++)
	{
		if (dataTab[i] > 25)
			dataTab[i] = 25;
	}
	gu8DsipNum[14] = u8LcdDispTab1[dataTab[0]];
	gu8DsipNum[13] = u8LcdDispTab1[dataTab[1]];
}

//ת�ٺ�������λ��ʾ��λѡ��
void subDispUnit(u8 unit)
{
	switch (unit)
	{
	case Unit_percent:
		gu8DsipNum[5] |= One_SegFig; //%��λ
		break;
	case Unit_m3h:
		gu8DsipNum[4] |= One_SegFig; // m3/h��λ
		break;
	case Unit_Imp:
		gu8DsipNum[6] |= One_SegFig; // IMP gpm��λ
		break;
	case Unit_Lmin:
		gu8DsipNum[2] |= One_SegFig; // L/min��λ
		break;
	case Unit_Us:
		gu8DsipNum[3] |= One_SegFig; // US gpm��λ
		break;
	case Unit_ALL:
		gu8DsipNum[5] |= One_SegFig; //%��λ
		gu8DsipNum[4] |= One_SegFig; // m3/h��λ
		gu8DsipNum[6] |= One_SegFig; // IMP gpm��λ
		gu8DsipNum[2] |= One_SegFig; // L/min��λ
		gu8DsipNum[3] |= One_SegFig; // US gpm��λ
		break;
	default:
		break;
	}
}
