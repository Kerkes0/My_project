
#include "User.h"
#include "wifi.h" //yxl-wifi
#include "Mode.h"
#include "ModeFunc.h"
#define u2cylen 150 //发送数据的缓存大小

uchar fg_wifi_ap_mode = 0;
uchar fg_wifi_smart_mode = 0;
uchar fg_wifi_led = 0;

uint16_t count_250ms_wifi_shine = 0;
uint16_t count_1500ms_wifi_shine = 0;
uint32_t sum_kilowatt_hour = 0;

uchar WifiTestOk = 0;

uint32_t wifi_error_fault = 0;				//当前故障
uint32_t wifi_gu16MotorWt_value = 0;		//当前当前功率
uint32_t wifi_water_setrange_max_value = 0; //当前流量设定最大值
uint32_t wifi_gu16RunSpd_real_value = 0;	//当前实时运行速率
uint8_t wifi_gu8SetMode_enum = 0;			//当前模式
uint32_t wifi_WashTmSet_value = 0;			//当前反冲洗设置时间
uint8_t wifi_power_switch_bool = 0;			//当前开关机
uint32_t wifi_WaterGate_set_value = 0;		//当前流量设定值
uint32_t wifi_water_setrange_min_value = 0; //当前流量设定最小值
uint32_t wifi_WashTm_value = 0;				//当前反冲洗倒计时
uint32_t wifi_kilowatt_hour_value = 0;		//当前耗电量
uint8_t wifi_water_uint_switch_enum = 0;	//当前单位切换
uint32_t wifi_gu16RunSpd_set_value = 0;		//当前速率设定值
uint32_t wifi_WaterGate_value = 0;			//当前实时水流量
uint32_t wifi_WaterGate_set_step_value = 0; //当前流量步进值
uint8_t wifi_flow_press_warn_bool = 0;		//水压告警
//对比缓存
uint32_t wifi_error_fault_buf = 0;//当前故障
uint32_t wifi_gu16MotorWt_value_buf = 0;//当前当前功率
uint32_t wifi_water_setrange_max_value_buf = 0;//当前流量设定最大值
uint32_t wifi_gu16RunSpd_real_value_buf = 0;//当前实时运行速率
uint8_t wifi_gu8SetMode_enum_buf = 0;//当前模式
uint32_t wifi_WashTmSet_value_buf = 0;//反冲洗设置时间
uint8_t wifi_power_switch_bool_buf = 0;//当前开关机
uint32_t wifi_WaterGate_set_value_buf = 0;//当前流量设定值
uint32_t wifi_water_setrange_min_value_buf = 0;//当前流量设定最小值
uint32_t wifi_WashTm_value_buf = 0;//当前反冲洗倒计时
uint32_t wifi_kilowatt_hour_value_buf = 0;	//当前耗电量
uint8_t wifi_water_uint_switch_enum_buf = 0;//当前单位切换
uint32_t wifi_gu16RunSpd_set_value_buf = 0;//当前速率设定值
uint32_t wifi_WaterGate_value_buf = 0;//当前实时水流量
uint32_t wifi_WaterGate_set_step_value_buf = 0;//当前流量步进值
uint8_t wifi_flow_press_warn_bool_buf = 0;//水压告警

uchar fg_dp_download_mode = 0;				//接收到模式标志
uchar fg_dp_download_backwash_time_set = 0; //接收到反冲洗时间标志
uchar fg_dp_download_swtich = 0;			//接收到流量单位标志
uchar fg_dp_download_uint_switch = 0;		//接收到开关机标志
uchar fg_dp_download_flow_rate_set = 0;		//接收到流量值标志
uchar fg_dp_download_speed_set = 0;			//接收到转速标志

uchar fg_20ms_delay_ask = 0; // wifi发送后等20ms后才能在发送（实际有50ms）
uchar fg_1min_kilowatt_hour = 0;

uchar all_updata_step = 0; //发送pro_all_updata();数据中数据的第几步骤

void USART2_IRQHandler(void)
{
	unsigned char data;

	while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART2);

		uart_receive_input(data); // yxl-wifi
	}
}

void pro_wifiuart_tx(uchar tx_value)
{

	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
		;
	USART_SendData(USART2, tx_value);
}

//////////20220225+wifi串口发送
char u2overfg = 0;
uint u2oversec = 0; //溢出次数
//获取溢出次数
void getoveru2data(void)
{
	if (u2overfg == -1)
	{
		u2overfg = 1;
		u2oversec++;
	}
}

char u2cytab[u2cylen];
typedef struct
{
	uchar head;
	uchar tail;
	char *data;
	uchar len;
} cycletab;
cycletab u2_cy;
void WIFI_U2_cycleInit(void)
{
	u2_cy.head = 0;
	u2_cy.tail = 0;
	u2_cy.data = u2cytab;
	u2_cy.len = u2cylen;
}
char WIFI_U2_saveData(char *sdata, unsigned short len)
{
	uchar k;
	if (u2_cy.head >= u2_cy.tail)
	{
		if ((u2_cy.len - (u2_cy.head - u2_cy.tail)) > len)
		{
			for (k = 0; k < len; k++)
			{
				u2_cy.data[u2_cy.head] = sdata[k];
				u2_cy.head++;
				if (u2_cy.head >= u2_cy.len)
				{
					u2_cy.head = 0;
				}
			}
			return 1;
		}
	}
	else if (u2_cy.head < u2_cy.tail)
	{
		if ((u2_cy.tail - u2_cy.head) > len)
		{
			for (k = 0; k < len; k++)
			{
				u2_cy.data[u2_cy.head] = sdata[k];
				u2_cy.head++;
			}
			return 1;
		}
	}
	u2overfg = -1;
	return -1;
}

void WIFI_U2_SendData(void)
{
	if (u2_cy.tail != u2_cy.head)
	{

		// while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
		if (USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET) //传输完成时
		{
			USART_ClearFlag(USART2, USART_FLAG_TC);
			USART_SendData(USART2, u2_cy.data[u2_cy.tail++]);
			if (u2_cy.tail >= u2_cy.len)
				u2_cy.tail = 0;
		}
		if (u2_cy.head >= u2_cy.len) // 20220311
		{
			u2_cy.head = 0;
		}
		if (u2_cy.tail >= u2_cy.len) // 20220311
		{
			u2_cy.tail = 0;
		}
	}
}
//////////20220225+wifi串口发送

//////////////////////////////////////////////////////////////////////////////
/**
 *@brief  发送配网命令，设置wifi的工作模式：smart模式或ap模式。
 * @param  Null
 * @return Null
 */
void pro_wifi_handle(void)
{
	uchar data;

	if (fg_wifi_smart_mode == 1) //智能配网
	{
		fg_wifi_smart_mode = 0;
		mcu_set_wifi_mode(SMART_CONFIG);
	}

	if (fg_wifi_ap_mode == 1) // ap配网
	{
		fg_wifi_ap_mode = 0;
		mcu_set_wifi_mode(AP_CONFIG);
	}

	/////////////////////////////////////////////////////////////////////////
	data = mcu_get_wifi_work_state(); //状态查询

	if (data == SMART_CONFIG_STATE) // smart配置状态
	{
		if (count_250ms_wifi_shine == 0)
		{
			count_250ms_wifi_shine = 5;
			fg_wifi_led ^= 1;
		}
	}
	else if (data == AP_STATE) // ap配置状态
	{
		if (count_1500ms_wifi_shine == 0)
		{
			count_1500ms_wifi_shine = 30;

			fg_wifi_led ^= 1;
		}
	}
	else if (data == WIFI_NOT_CONNECTED) //已配置但未连上路由器
	{
		fg_wifi_led = 0;
	}
	else if (data == WIFI_CONNECTED) //连上路由器
	{
		fg_wifi_led = 1;
	}
	else if (data == WIFI_CONN_CLOUD) //连上云端
	{
		fg_wifi_led = 1;
	}
	else if (data == WIFI_LOW_POWER) //低功耗模式
	{
		fg_wifi_led = 0;
	}
	else if (mcu_get_wifi_work_state() == SMART_AND_AP_STATE) // Wi-Fi 设备处于 Smart 和 AP 共存配置状态
	{
		if (count_250ms_wifi_shine == 0)
		{
			count_250ms_wifi_shine = 5;
			fg_wifi_led ^= 1;
		}
	}
	else if (mcu_get_wifi_work_state() == WIFI_SATE_UNKNOW) //未知模式
	{
		fg_wifi_led = 0;
	}
	else
	{
		fg_wifi_led = 0;
	}
	//////////////////////////////////////////////////////////////////////////
}

/**
 * @brief  发送数据
 * @param  Null
 * @return Null
 */
void pro_wifi_updata(void)
{
	uchar k, s;

	if (fg_20ms_delay_ask >= 10) // wifi发送后等20ms后才能在发送（实际有50ms）
	{
		/////////////////////////////////////////////////////////////////////////////故障

		wifi_error_fault = 0;

		if (ValBit(gu16TripFlg_Driver, Involterror))//输入电压异常E001
			wifi_error_fault |= 1<<0;//0x00000001;
		if (ValBit(gu16TripFlg_Driver, Incurroverh))//输出电流超过限值E002
			wifi_error_fault |= 1<<1;//0x00000002;
		
		if (ValBit(gu16TripFlg_Driver, IpmTempoverh))//散热器过热E101
			wifi_error_fault |= 1<<2;//0x00000004;
		if (ValBit(gu16TripFlg_Driver, Ipmnotemp))//散热器传感器故障E102
			wifi_error_fault |= 1<<3;//0x00000008;
		if (ValBit(gu16TripFlg_Driver, Insysdriverr))//主控驱动故障E103
			wifi_error_fault |= 1<<4;//0x00000010;
		if (ValBit(gu16TripFlg_Driver, Inmotmisitem))//电机缺相保护E104
			wifi_error_fault |= 1<<5;//0x00000020;
		if (ValBit(gu16TripFlg_Driver, Incurrerror))//交流电流采样电路故障E105
			wifi_error_fault |= 1<<6;//0x00000040;
		if (ValBit(gu16TripFlg_Driver, Indccurrer))// DC电压异常E106
			wifi_error_fault |= 1<<7;//0x00000080;
		if (ValBit(gu16TripFlg_Driver, Inpfcprot))// PFC保护E107
			wifi_error_fault |= 1<<8;//0x00000100;
		if (ValBit(gu16TripFlg_Driver, Inmotoverload)) //电机功率超载E108
			wifi_error_fault |= 1<<9;//0x00000200;
		
		if (ValBit(gu16TripFlg_Driver, Inmotcurrerr)) //电机电流检测电路故障E201
			wifi_error_fault |= 1<<10;//0x00000400;
		if (ValBit(gu16TripFlg_Driver, Insyseepeer))//主控制EEPROM读写故障E202
			wifi_error_fault |= 1<<11;//0x00000800;
		if (ValBit(gu16TripFlg_Board, Owrtcerror))//RTC时钟读写故障E203
			wifi_error_fault |= 1<<12;//0x00001000;
		if (ValBit(gu16TripFlg_Board, Oweeperror))//键盘EEPROM读写故障E204
			wifi_error_fault |= 1<<13;//0x00002000;
		if (ValBit(gu16TripFlg_Driver, Inuarterr))//键盘与主控制通信故障E205
			wifi_error_fault |= 1<<14;//0x00004000;
		if (ValBit(gu16TripFlg_Board, Ow485error))//485通讯故障E206
			wifi_error_fault |= 1<<15;//0x00008000;
		
		if(ValBit(gu16TripFlg_Driver,Inlowflowerr))   //无水干烧保护E207
			wifi_error_fault |= 1<<16;//0x00010000; 		
		if (ValBit(gu16TripFlg_Board,Ownowaterpr))     //传感器故障E208
			wifi_error_fault |= 1<<17;//0x00020000;          
		if (ValBit(gu16TripFlg_Board,Owupwaterfail))   //自吸失败E209  
			wifi_error_fault |= 1<<18;//0x00040000;   
			
		if (ValBit(gu16TripFlg_Warn, LsHighTemp))      //高温预警降速AL01
			wifi_error_fault |= 1<<19;//0x00080000;
		if (ValBit(gu16TripFlg_Warn, Lslowvolt))//AC欠压降速AL02
			wifi_error_fault |= 1<<20;//0x00100000;
		if (ValBit(gu16TripFlg_Warn, Wnfrostpro))//防冻保护AL03
			wifi_error_fault |= 1<<21;//0x00200000;

		
    
				

		if (wifi_error_fault_buf != wifi_error_fault)
		{
			wifi_error_fault_buf = wifi_error_fault;
			mcu_dp_fault_update(DPID_FAULT, wifi_error_fault);
		}

		//////////////////////////////////////////////////////////////////////////////////功率

		//	gu16MotorWt=hgf;

		if (count2s_wifi_gu16MotorWt_up >= 40)
		{
			count2s_wifi_gu16MotorWt_up = 0;

			wifi_gu16MotorWt_value = gu16MotorWt;

			if (wifi_gu16MotorWt_value_buf != wifi_gu16MotorWt_value)
			{
				wifi_gu16MotorWt_value_buf = wifi_gu16MotorWt_value;
				mcu_dp_value_update(DPID_CUR_POWER, wifi_gu16MotorWt_value);
			}
		}

		/////////////////////////////////////////////////////////////////////////////////流量设定最大值
		if ((switch_key & 0x06) == 0) // IP20
		{
			if (water_uint_switch == 0)
				wifi_water_setrange_max_value = 20;
			if (water_uint_switch == 1)
				wifi_water_setrange_max_value = 320;
			if (water_uint_switch == 2)
				wifi_water_setrange_max_value = 70;
			if (water_uint_switch == 3)
				wifi_water_setrange_max_value = 85;
		}
		else if ((switch_key & 0x06) == 2) // IP25
		{
			if (water_uint_switch == 0)
				wifi_water_setrange_max_value = 25;
			if (water_uint_switch == 1)
				wifi_water_setrange_max_value = 420;
			if (water_uint_switch == 2)
				wifi_water_setrange_max_value = 95;
			if (water_uint_switch == 3)
				wifi_water_setrange_max_value = 110;
		}
		else if ((switch_key & 0x06) == 4) // IP30
		{
			if (water_uint_switch == 0)
				wifi_water_setrange_max_value = 30;
			if (water_uint_switch == 1)
				wifi_water_setrange_max_value = 520;
			if (water_uint_switch == 2)
				wifi_water_setrange_max_value = 120;
			if (water_uint_switch == 3)
				wifi_water_setrange_max_value = 135;
		}

		if (wifi_water_setrange_max_value_buf != wifi_water_setrange_max_value)
		{
			wifi_water_setrange_max_value_buf = wifi_water_setrange_max_value;
			mcu_dp_value_update(DPID_FLOW_RATE_MAX, wifi_water_setrange_max_value); // VALUE型数据上报;
		}

		//////////////////////////////////////////////////////////////////////////////////实时运行速率

		wifi_gu16RunSpd_real_value = gu16MotorSetSpd * 2 * 100 / u16LadderSpd[16];
		wifi_gu16RunSpd_real_value = wifi_gu16RunSpd_real_value / 5 * 5;

		if (wifi_gu16RunSpd_real_value > 100)
			wifi_gu16RunSpd_real_value = 100;

		if (wifi_gu16RunSpd_real_value_buf != wifi_gu16RunSpd_real_value)
		{
			wifi_gu16RunSpd_real_value_buf = wifi_gu16RunSpd_real_value;
			mcu_dp_value_update(DPID_SPEED_REAL, wifi_gu16RunSpd_real_value);
		}

		///////////////////////////////////////////////////////////////////////////////////模式

		//	wifi_gu8SetMode_enum=0;

		if (gu8Wash) //冲洗
		{
			if(!(fg_rest_con_ok == 1 &&fg_auto_upwater == 0 && fg_selfimbi_ok == 0  ))//非单纯自学习
			{
				wifi_gu8SetMode_enum = 0x02;
			}
			else//单纯是自学习//20220526+
			{
				if (gu8SetMode == SPEED) //循环过滤
				{
					wifi_gu8SetMode_enum = 0x01;
				}
				else if (gu8SetMode == CONTROL) //流量
				{
					wifi_gu8SetMode_enum = 0x00;
				}
				else if (gu8SetMode == TIME) //定时
				{
					if (fg_time_spd_contorl == 1)
					{
						wifi_gu8SetMode_enum = 0x00;
					}
					else
					{
						wifi_gu8SetMode_enum = 0x01;
					}
				}
				else
				{
					wifi_gu8SetMode_enum = 0x00;
				}
			}
		}
		else if (gu8SetMode == SPEED) //循环过滤
		{
			wifi_gu8SetMode_enum = 0x01;
		}
		else if (gu8SetMode == CONTROL) //流量
		{
			wifi_gu8SetMode_enum = 0x00;
		}
		else if (gu8SetMode == TIME) //定时
		{
			if (fg_time_spd_contorl == 1)
			{
				wifi_gu8SetMode_enum = 0x00;
			}
			else
			{
				wifi_gu8SetMode_enum = 0x01;
			}
		}
		else if ((gu8SetMode EQU MODE_Ctrl1) OR(SysProState == SysProState_Test)) //外部控制和自检
		{
			wifi_gu8SetMode_enum = 0x01;
		}
		else //关机
		{

			if (gu8SetMode_eep == SPEED) //循环过滤
			{
				wifi_gu8SetMode_enum = 0x01;
			}
			else if (gu8SetMode_eep == TIME) //定时
			{
				if (fg_time_spd_contorl == 1)
				{
					wifi_gu8SetMode_enum = 0x00;
				}
				else
				{
					wifi_gu8SetMode_enum = 0x01;
				}
			}
			else if (gu8SetMode_eep == CONTROL) //流量
			{
				wifi_gu8SetMode_enum = 0x00;
			}
			else //流量
			{
				wifi_gu8SetMode_enum = 0x00;
			}
		}

		if ((wifi_gu8SetMode_enum_buf != wifi_gu8SetMode_enum) || (fg_dp_download_mode == 1))
		{
			wifi_gu8SetMode_enum_buf = wifi_gu8SetMode_enum;
			mcu_dp_enum_update(DPID_MODE, wifi_gu8SetMode_enum);
		}

		///////////////////////////////////////////////////////////////////////////////////////////反冲洗设置时间

		wifi_WashTmSet_value = WashTmSet;

		if ((wifi_WashTmSet_value_buf != wifi_WashTmSet_value) || (fg_dp_download_backwash_time_set == 1))
		{
			// fg_dp_download_backwash_time_set=0;

			wifi_WashTmSet_value_buf = wifi_WashTmSet_value;
			mcu_dp_value_update(DPID_BACKWASH_TIME_SET, wifi_WashTmSet_value);
		}

		//////////////////////////////////////////////////////////////////////////////////////////	开关机

		if (gu8SetMode == 0)
			wifi_power_switch_bool = 0;
		else
			wifi_power_switch_bool = 1;

		if ((wifi_power_switch_bool_buf != wifi_power_switch_bool) || (fg_dp_download_swtich == 1))
		{

			// fg_dp_download_swtich=0;
			wifi_power_switch_bool_buf = wifi_power_switch_bool;
			mcu_dp_bool_update(DPID_SWTICH, wifi_power_switch_bool);
		}
		/////////////////////////////////////////////////////////////////////////////////////////流量设定值

		//  wifi_WaterGate_set_value =  wifi_gu16RunSpd_set_value  *  water_max_set  /  100; //根据速率设定值来

		if ((gu8SetMode == SPEED) || (gu8SetMode == CONTROL))
		{
			// wifi_WaterGate_set_value= WaterGate_set;
			if (water_uint_switch == 0) // m3/h
			{
				wifi_WaterGate_set_value = WaterGate_set;
			}
			else if (water_uint_switch == 1) // L/min
			{

			}
			else if (water_uint_switch == 2) // IMP gpm
			{

			}
			else if (water_uint_switch == 3) // US gpm
			{

			}
		}

		else if (gu8SetMode == TIME)
		{
			// wifi_WaterGate_set_value	= u8ModeTime_GateSet_Tem[gu8TimeRunNum];
			if ((gu8SetTimeTm != 0) && (gu8SetTimeTmSn >= 1))
			{
				if (water_uint_switch == 0)
				{
					wifi_WaterGate_set_value = u8ModeTime_GateSet_Tem[gu8SetTimeTmNum];
				}
				else if (water_uint_switch == 1)
				{

				}
				else if (water_uint_switch == 2)
				{
	
				}
				else if (water_uint_switch == 3)
				{

				}
			}

			else
			{
				if (water_uint_switch == 0)
				{
					wifi_WaterGate_set_value = u8ModeTime_GateSet_Tem[gu8TimeRunNum];
				}
				else if (water_uint_switch == 1)
				{

				}
				else if (water_uint_switch == 2)
				{
	
				}

				else if (water_uint_switch == 3)
				{
	
				}
			}
		}

		if ((wifi_WaterGate_set_value_buf != wifi_WaterGate_set_value) || (fg_dp_download_flow_rate_set == 1))
		{

			wifi_WaterGate_set_value_buf = wifi_WaterGate_set_value;
			mcu_dp_value_update(DPID_FLOW_RATE_SET, wifi_WaterGate_set_value);
		}

		/////////////////////////////////////////////////////////////////////////////////////////	流量设定最小值

		if (water_uint_switch == 0)
			wifi_water_setrange_min_value = 5;
		if (water_uint_switch == 1)
			wifi_water_setrange_min_value = 80;
		if (water_uint_switch == 2)
			wifi_water_setrange_min_value = 15;
		if (water_uint_switch == 3)
			wifi_water_setrange_min_value = 20;

		if (wifi_water_setrange_min_value_buf != wifi_water_setrange_min_value)
		{
			wifi_water_setrange_min_value_buf = wifi_water_setrange_min_value;
			mcu_dp_value_update(DPID_FLOW_RATE_MIN, wifi_water_setrange_min_value);
		}

		///////////////////////////////////////////////////////////////////////////////////////	当前反冲洗倒计时

		if (gu8SetWastTm > 0)
			WashTm = WashTmSet;
		wifi_WashTm_value = WashTm;

		if (wifi_WashTm_value_buf != wifi_WashTm_value)
		{
			wifi_WashTm_value_buf = wifi_WashTm_value;
			mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN, wifi_WashTm_value);
		}

		/////////////////////////////////////////////////////////////////////////////////////耗电量
		if ((count_1min_kilowatt_hour % 120) == 0)
		{
			if (fg_1min_kilowatt_hour == 0)
			{
				fg_1min_kilowatt_hour = 1;
				sum_kilowatt_hour += gu16MotorWt;
			}
		}
		else
		{
			fg_1min_kilowatt_hour = 0;
		}

		if (count_1min_kilowatt_hour >= 7200)
		{
			count_1min_kilowatt_hour = 0;
			wifi_kilowatt_hour_value = sum_kilowatt_hour / 60;
			sum_kilowatt_hour = 0;
			wifi_kilowatt_hour_value = wifi_kilowatt_hour_value / 10;
			mcu_dp_value_update(DPID_POWER_CONSUMPTION, wifi_kilowatt_hour_value);
			wifi_kilowatt_hour_value = 0; //防止重新配网再发一次//20220105
		}

		////////////////////////////////////////////////////////////////////////////////单位切换

		if (water_uint_switch == 0)
		{
			wifi_water_uint_switch_enum = 0x00;
		}
		else if (water_uint_switch == 1)
		{
			wifi_water_uint_switch_enum = 0x01;
		}
		else if (water_uint_switch == 2)
		{
			wifi_water_uint_switch_enum = 0x03;
		}
		else if (water_uint_switch == 3)
		{
			wifi_water_uint_switch_enum = 0x02;
		}

		if ((wifi_water_uint_switch_enum_buf != wifi_water_uint_switch_enum) || (fg_dp_download_uint_switch == 1))
		{

			// fg_dp_download_uint_switch=0;

			wifi_water_uint_switch_enum_buf = wifi_water_uint_switch_enum;
			mcu_dp_enum_update(DPID_UINT_SWITCH, wifi_water_uint_switch_enum);
		}

		///////////////////////////////////////////////////////	速率设定值
		if ((gu8SetMode == SPEED) || (gu8SetMode == CONTROL))
		{
			wifi_gu16RunSpd_set_value = (u8ModeSpeed_SpeedSet - 24) * 5 + 20;
			if (gu16UartSpd >= u16LadderSpd[0])
			{
				for (k = 0; k < 17; k++) // YXL-5
				{
					if (gu16RunSpd == u16LadderSpd[k])
					{
						wifi_gu16RunSpd_set_value = k * 5 + 20;
					}
				}
			}
		}
		else if (gu8SetMode == TIME)
		{
			if ((gu8SetTimeTm != 0) && (gu8SetTimeTmSn >= 1))
			{
				wifi_gu16RunSpd_set_value = u8ModeTime_SpeedSet_Tem[gu8SetTimeTmNum] * 5 + 20;
			}
			else
			{
				wifi_gu16RunSpd_set_value = u8ModeTime_SpeedSet[gu8TimeRunNum] * 5 + 20;
			}
		}
		else if ((gu8SetMode EQU MODE_Ctrl1))
		{
			for (k = 0; k < 17; k++) // YXL-5
			{
				if (gu16RunSpd == u16LadderSpd[k])
				{
					wifi_gu16RunSpd_set_value = k * 5 + 20;
				}
			}
		}
		else
		{
			wifi_gu16RunSpd_set_value = 0;
		}
		if ((wifi_gu16RunSpd_set_value_buf != wifi_gu16RunSpd_set_value) || (fg_dp_download_speed_set == 1))
		{
			// fg_dp_download_speed_set=0;
			wifi_gu16RunSpd_set_value_buf = wifi_gu16RunSpd_set_value;
			mcu_dp_value_update(DPID_SPEED_SET, wifi_gu16RunSpd_set_value);
		}

		//////////////////////////////////////////////////////////////////////////////////////////实时水流量

		wifi_WaterGate_value = WaterGate / 100;
		s = WaterGate % 100;
		if (s > 50)
			wifi_WaterGate_value += 1;

		if (wifi_WaterGate_value_buf != wifi_WaterGate_value)
		{
			wifi_WaterGate_value_buf = wifi_WaterGate_value;
			mcu_dp_value_update(DPID_WATER_REALFLOW, wifi_WaterGate_value);
		}

		/////////////////////////////////////////////////////////////////////////////////////////流量步进
		if (water_uint_switch == 0)
		{
			wifi_WaterGate_set_step_value = 1;
		}
		else if (water_uint_switch == 1)
		{
			wifi_WaterGate_set_step_value = 20;
		}
		else if (water_uint_switch == 2)
		{
			wifi_WaterGate_set_step_value = 5;
		}
		else if (water_uint_switch == 3)
		{
			wifi_WaterGate_set_step_value = 5;
		}

		if (wifi_WaterGate_set_step_value_buf != wifi_WaterGate_set_step_value)
		{
			wifi_WaterGate_set_step_value_buf = wifi_WaterGate_set_step_value;
			mcu_dp_value_update(DPID_FLOW_STEP, wifi_WaterGate_set_step_value);
		}

		////////////////////////////////////////////////////////////////////////////////////////	水压告警

		if ((fg_rest_con_ok == 1) && (!ValBit(u8FirstFunIn, LowGate)))
		{
			wifi_flow_press_warn_bool = 1;
		}
		else
		{
			wifi_flow_press_warn_bool = 0;
		}
		if (wifi_flow_press_warn_bool_buf != wifi_flow_press_warn_bool)
		{
			wifi_flow_press_warn_bool_buf = wifi_flow_press_warn_bool;
			mcu_dp_bool_update(DPID_FLOW_PRESS_WARN, wifi_flow_press_warn_bool);
		}

		/////////////////////////////////////////////////////有数值改变会发送数据，则蜂鸣器响//////////////////////////
		if ((fg_dp_download_mode == 1) || (fg_dp_download_backwash_time_set == 1) || (fg_dp_download_swtich == 1) || (fg_dp_download_flow_rate_set == 1) || (fg_dp_download_uint_switch == 1) || (fg_dp_download_speed_set == 1))
		{

			fg_dp_download_mode = 0;
			fg_dp_download_backwash_time_set = 0;
			fg_dp_download_swtich = 0;
			fg_dp_download_flow_rate_set = 0;
			fg_dp_download_uint_switch = 0;
			fg_dp_download_speed_set = 0;

			subBuzCtrl(SHORTBE);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}

void pro_all_updata(void)
{
	switch (all_updata_step)
	{
	case 0:
		break;
	case 1:
		mcu_dp_fault_update(DPID_FAULT, wifi_error_fault); //故障型数据上报;
		all_updata_step++;
		break;
	case 2:
		mcu_dp_value_update(DPID_CUR_POWER, wifi_gu16MotorWt_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 3:
		mcu_dp_value_update(DPID_FLOW_RATE_MAX, wifi_water_setrange_max_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 4:
		mcu_dp_value_update(DPID_SPEED_REAL, wifi_gu16RunSpd_real_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 5:
		mcu_dp_enum_update(DPID_MODE, wifi_gu8SetMode_enum); //枚举型数据上报;
		all_updata_step++;
		break;
	case 6:
		mcu_dp_value_update(DPID_BACKWASH_TIME_SET, wifi_WashTmSet_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 7:
		mcu_dp_bool_update(DPID_SWTICH, wifi_power_switch_bool); // BOOL型数据上报;
		all_updata_step++;
		break;
	case 8:
		mcu_dp_value_update(DPID_FLOW_RATE_SET, wifi_WaterGate_set_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 9:
		mcu_dp_value_update(DPID_FLOW_RATE_MIN, wifi_water_setrange_min_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 10:
		mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN, wifi_WashTm_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 11:
		mcu_dp_value_update(DPID_POWER_CONSUMPTION, wifi_kilowatt_hour_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 12:
		mcu_dp_enum_update(DPID_UINT_SWITCH, wifi_water_uint_switch_enum); //枚举型数据上报;
		all_updata_step++;
		break;
	case 13:
		mcu_dp_value_update(DPID_SPEED_SET, wifi_gu16RunSpd_set_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 14:
		mcu_dp_value_update(DPID_WATER_REALFLOW, wifi_WaterGate_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 15:
		mcu_dp_value_update(DPID_FLOW_STEP, wifi_WaterGate_set_step_value); // VALUE型数据上报;
		all_updata_step++;
		break;
	case 16:
		mcu_dp_bool_update(DPID_FLOW_PRESS_WARN, wifi_flow_press_warn_bool); ////BOOL型数据上报;
		all_updata_step++;
		break;

	default:
		all_updata_step = 0;
	}
}
