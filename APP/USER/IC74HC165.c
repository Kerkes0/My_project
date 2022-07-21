/*
 * @Description: In User Settings Edit
 * @Author: Deng Senyu
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2022-07-14 17:35:51
 * @LastEditors: weipeng
 */
#include "IC74HC165.h"
#include "ModeFunc.h"
#include "User.h"

const uint u16LadderSpd_ip20[] = {
	820, 820, 820, 870, 936, 1053, 1170, 1287, 1404, 1521, 1638, 1755, 1872, 1989, 2106, 2223, 2340};

const uint u16LadderSpd_ip25[] = {
	850, 850, 850, 910, 1040, 1170, 1300, 1430, 1560, 1690, 1820, 1950, 2080, 2210, 2340, 2470, 2600};

const uint u16LadderSpd_ip30[] = {
	890, 890, 890, 998, 1140, 1283, 1425, 1568, 1710, 1853, 1995, 2138, 2280, 2423, 2565, 2708, 2850};
// 0     1    2    3    4      5     6     7     8     9     10    11    12    13    14   15    16
//   1    2     3   4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30    31  32    33 34  35  36
const uchar sp_2600_con_press_ip20[] = {45, 50, 53, 56, 59, 62, 65, 68, 71, 74, 75, 78, 81, 84, 86, 88, 91, 94, 97, 100, 102, 105, 108, 111, 114, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145}; // 36
const uint16_t sp_2600_con_flow_ip20[] = {2476, 2405, 2374, 2323, 2278, 2221, 2160, 2091, 2022, 1967, 1887, 1843, 1746, 1660, 1592, 1528, 1422, 1303, 1174, 1036, 872, 703, 559, 369, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const uchar sp_2600_con_press_ip25[] = {55, 59, 62, 64, 67, 70, 72, 75, 78, 80, 83, 86, 88, 91, 94, 96, 99, 102, 104, 107, 109, 112, 115, 117, 120, 123, 126, 129, 132, 135, 138, 170, 170, 170, 170, 170}; // 36
const uint16_t sp_2600_con_flow_ip25[] = {2702, 2625, 2574, 2539, 2489, 2453, 2399, 2340, 2278, 2225, 2166, 2129, 2067, 2033, 1971, 1921, 1830, 1761, 1670, 1601, 1512, 1466, 1320, 1217, 1121, 998, 860, 740, 567, 394, 100, 0, 0, 0, 0, 0};
const uchar sp_2600_con_press_ip30[] = {72, 75, 78, 81, 84, 86, 88, 91, 94, 96, 98, 100, 103, 106, 109, 111, 114, 117, 120, 123, 124, 127, 130, 133, 136, 138, 141, 144, 147, 150, 153, 156, 159, 162, 165, 195}; // 36
const uint16_t sp_2600_con_flow_ip30[] = {3022, 2989, 2953, 2894, 2848, 2819, 2775, 2711, 2625, 2599, 2553, 2502, 2476, 2415, 2351, 2282, 2246, 2185, 2112, 2051, 1986, 1913, 1847, 1767, 1690, 1573, 1476, 1382, 1284, 1193, 1032, 895, 732, 600, 400, 0};

uchar water_set_range_max = 0;
uchar water_set_range_min = 0;
uint16_t WaterGate_cail_ot_preset = 2000;

uchar water_uint_switch = 0; // 20211222
/////////////20211222

u8 switch_key = 0;
u8 switch_din = 0; /// 20211020
void IC74HC165_Read(IC74HC165tag *ic, uint8_t data[], uint16_t len)
{
	GPIO_PinOut(ic->sck, 0);
	GPIO_PinOut(ic->clk, 0);
	GPIO_PinOut(ic->sck, 1);
	for (uint16_t i = 0; i < len; i++)
	{
		data[i / 8] &= ~(1 << (7 - i % 8));
		data[i / 8] |= (GPIO_PinIn(ic->sda) & 0x01) << (7 - i % 8);
		GPIO_PinOut(ic->clk, 1);
		delay_us(2);
		GPIO_PinOut(ic->clk, 0);
	}
}

///////////////////////////
//////////////IC74HC165芯片数据输入读取
//////////////20211020
/**
 * @brief  读取2个165芯片的输入脚，
 * @param  Null
 * @return Null
 * @note   是以m3/h单位的数值进行转换的
 */
void switch_read()
{
	unsigned char i;
	u16 temp = 0;
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);

	if (PFin(7) == 0)
		temp |= 0x01;		 //此时HC165_OUT为Temp的最高位。
	for (i = 0; i < 15; i++) //循环15次，依次把次高位移到最高位
	{
		temp = temp << 1;
		GPIO_ResetBits(GPIOF, GPIO_Pin_6); // HC165_CLK = 0 ——> HC165_CLK = 1 产生上升沿，次高位D(n-1)移到高位D(n)
		GPIO_SetBits(GPIOF, GPIO_Pin_6);
		if (PFin(7) == 0)
			temp |= 0x01;
	}

	//两个ic74hc165输入得到后的数据，用到时是相反的
	switch_key = ((u8)(temp & 0x00ff));		  //拨码开关
	switch_din = ~(u8)((temp >> 8) & 0x00ff); // din1-4口输入

}

/////20211223
/**
 * @brief  根据拨码器的拨码选择版本，设置该版本的默认数值范围
 * @param[in]  {switch_key_self}拨码器SW1的拨码
 * @return Null
 * @note   该函数只在上电时的初始化7.5s内才会运行
 */
void FunForVersionToSet(void)
{
	u8 k;
	u8 switch_key_self;

	switch_key_self = switch_key;

	if ((switch_key_self & 0x06) == 0) // IP20
	{

		for (k = 0; k < 17; k++) // yxl-5
		{
			u16LadderSpd[k] = u16LadderSpd_ip20[k];
		}
		for (k = 0; k < 36; k++) // yxl-5
		{
			sp_2600_con_press[k] = sp_2600_con_press_ip20[k];
			sp_2600_con_flow[k] = sp_2600_con_flow_ip20[k];
		}
		//流量范围的最大和最小值
		water_set_range_max = 20;
		water_set_range_min = 5;
		WaterGate_cail_ot_preset = 2000; // yxl  默认22立方

		ModeGate_m3h.TabMax = 20;
		ModeGate_m3h.TabMin = 5;
		ModeGate_Imp.TabMax = 70;
		ModeGate_Imp.TabMin = 15;
		ModeGate_Lmin.TabMax = 320;
		ModeGate_Lmin.TabMin = 80;
		ModeGate_US.TabMax = 85;
		ModeGate_US.TabMin = 20;
	}
	else if ((switch_key_self & 0x06) == 2) // IP25
	{
		for (k = 0; k < 17; k++) // yxl-5
		{
			u16LadderSpd[k] = u16LadderSpd_ip25[k];
		}
		for (k = 0; k < 36; k++) // yxl-5
		{
			sp_2600_con_press[k] = sp_2600_con_press_ip25[k];
			sp_2600_con_flow[k] = sp_2600_con_flow_ip25[k];
		}
		//流量范围的最大和最小值
		water_set_range_max = 25;
		water_set_range_min = 5;
		WaterGate_cail_ot_preset = 2500;

		ModeGate_m3h.TabMax = 25;
		ModeGate_m3h.TabMin = 5;
		ModeGate_Imp.TabMax = 95;
		ModeGate_Imp.TabMin = 15;
		ModeGate_Lmin.TabMax = 420;
		ModeGate_Lmin.TabMin = 80;
		ModeGate_US.TabMax = 110;
		ModeGate_US.TabMin = 20;
	}
	else if ((switch_key_self & 0x06) == 4) // IP30
	{
		for (k = 0; k < 17; k++) // yxl-5
		{
			u16LadderSpd[k] = u16LadderSpd_ip30[k];
		}
		for (k = 0; k < 36; k++) // yxl-5
		{
			sp_2600_con_press[k] = sp_2600_con_press_ip30[k];
			sp_2600_con_flow[k] = sp_2600_con_flow_ip30[k];
		}
		//流量范围的最大和最小值

		water_set_range_max = 30;
		water_set_range_min = 5;
		WaterGate_cail_ot_preset = 3000;

		ModeGate_m3h.TabMax = 30;
		ModeGate_m3h.TabMin = 5;
		ModeGate_Imp.TabMax = 120;
		ModeGate_Imp.TabMin = 15;
		ModeGate_Lmin.TabMax = 520;
		ModeGate_Lmin.TabMin = 80;
		ModeGate_US.TabMax = 135;
		ModeGate_US.TabMin = 20;
	}
	//其他情况都为IP20
	else
	{
		for (k = 0; k < 17; k++) // yxl-5
		{
			u16LadderSpd[k] = u16LadderSpd_ip20[k];
		}
		for (k = 0; k < 36; k++) // yxl-5
		{
			sp_2600_con_press[k] = sp_2600_con_press_ip20[k];
			sp_2600_con_flow[k] = sp_2600_con_flow_ip20[k];
		}
		//流量范围的最大和最小值
		water_set_range_max = 20;
		water_set_range_min = 5;
		WaterGate_cail_ot_preset = 2000; // yxl  默认22立方

		ModeGate_m3h.TabMax = 20;
		ModeGate_m3h.TabMin = 5;
		ModeGate_Imp.TabMax = 70;
		ModeGate_Imp.TabMin = 15;
		ModeGate_Lmin.TabMax = 320;
		ModeGate_Lmin.TabMin = 80;
		ModeGate_US.TabMax = 85;
		ModeGate_US.TabMin = 20;
	}
	u16LadderSpd_rang = (u16LadderSpd[16] - u16LadderSpd[15]) / 2;
}
