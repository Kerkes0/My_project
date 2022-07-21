
#include "User.h"
#include "wifi.h" //yxl-wifi
#include "Mode.h"
#include "ModeFunc.h"
#define u2cylen 150 //�������ݵĻ����С

uchar fg_wifi_ap_mode = 0;
uchar fg_wifi_smart_mode = 0;
uchar fg_wifi_led = 0;

uint16_t count_250ms_wifi_shine = 0;
uint16_t count_1500ms_wifi_shine = 0;
uint32_t sum_kilowatt_hour = 0;

uchar WifiTestOk = 0;

uint32_t wifi_error_fault = 0;				//��ǰ����
uint32_t wifi_gu16MotorWt_value = 0;		//��ǰ��ǰ����
uint32_t wifi_water_setrange_max_value = 0; //��ǰ�����趨���ֵ
uint32_t wifi_gu16RunSpd_real_value = 0;	//��ǰʵʱ��������
uint8_t wifi_gu8SetMode_enum = 0;			//��ǰģʽ
uint32_t wifi_WashTmSet_value = 0;			//��ǰ����ϴ����ʱ��
uint8_t wifi_power_switch_bool = 0;			//��ǰ���ػ�
uint32_t wifi_WaterGate_set_value = 0;		//��ǰ�����趨ֵ
uint32_t wifi_water_setrange_min_value = 0; //��ǰ�����趨��Сֵ
uint32_t wifi_WashTm_value = 0;				//��ǰ����ϴ����ʱ
uint32_t wifi_kilowatt_hour_value = 0;		//��ǰ�ĵ���
uint8_t wifi_water_uint_switch_enum = 0;	//��ǰ��λ�л�
uint32_t wifi_gu16RunSpd_set_value = 0;		//��ǰ�����趨ֵ
uint32_t wifi_WaterGate_value = 0;			//��ǰʵʱˮ����
uint32_t wifi_WaterGate_set_step_value = 0; //��ǰ��������ֵ
uint8_t wifi_flow_press_warn_bool = 0;		//ˮѹ�澯
//�ԱȻ���
uint32_t wifi_error_fault_buf = 0;//��ǰ����
uint32_t wifi_gu16MotorWt_value_buf = 0;//��ǰ��ǰ����
uint32_t wifi_water_setrange_max_value_buf = 0;//��ǰ�����趨���ֵ
uint32_t wifi_gu16RunSpd_real_value_buf = 0;//��ǰʵʱ��������
uint8_t wifi_gu8SetMode_enum_buf = 0;//��ǰģʽ
uint32_t wifi_WashTmSet_value_buf = 0;//����ϴ����ʱ��
uint8_t wifi_power_switch_bool_buf = 0;//��ǰ���ػ�
uint32_t wifi_WaterGate_set_value_buf = 0;//��ǰ�����趨ֵ
uint32_t wifi_water_setrange_min_value_buf = 0;//��ǰ�����趨��Сֵ
uint32_t wifi_WashTm_value_buf = 0;//��ǰ����ϴ����ʱ
uint32_t wifi_kilowatt_hour_value_buf = 0;	//��ǰ�ĵ���
uint8_t wifi_water_uint_switch_enum_buf = 0;//��ǰ��λ�л�
uint32_t wifi_gu16RunSpd_set_value_buf = 0;//��ǰ�����趨ֵ
uint32_t wifi_WaterGate_value_buf = 0;//��ǰʵʱˮ����
uint32_t wifi_WaterGate_set_step_value_buf = 0;//��ǰ��������ֵ
uint8_t wifi_flow_press_warn_bool_buf = 0;//ˮѹ�澯

uchar fg_dp_download_mode = 0;				//���յ�ģʽ��־
uchar fg_dp_download_backwash_time_set = 0; //���յ�����ϴʱ���־
uchar fg_dp_download_swtich = 0;			//���յ�������λ��־
uchar fg_dp_download_uint_switch = 0;		//���յ����ػ���־
uchar fg_dp_download_flow_rate_set = 0;		//���յ�����ֵ��־
uchar fg_dp_download_speed_set = 0;			//���յ�ת�ٱ�־

uchar fg_20ms_delay_ask = 0; // wifi���ͺ��20ms������ڷ��ͣ�ʵ����50ms��
uchar fg_1min_kilowatt_hour = 0;

uchar all_updata_step = 0; //����pro_all_updata();���������ݵĵڼ�����

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

//////////20220225+wifi���ڷ���
char u2overfg = 0;
uint u2oversec = 0; //�������
//��ȡ�������
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
		if (USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET) //�������ʱ
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
//////////20220225+wifi���ڷ���

//////////////////////////////////////////////////////////////////////////////
/**
 *@brief  ���������������wifi�Ĺ���ģʽ��smartģʽ��apģʽ��
 * @param  Null
 * @return Null
 */
void pro_wifi_handle(void)
{
	uchar data;

	if (fg_wifi_smart_mode == 1) //��������
	{
		fg_wifi_smart_mode = 0;
		mcu_set_wifi_mode(SMART_CONFIG);
	}

	if (fg_wifi_ap_mode == 1) // ap����
	{
		fg_wifi_ap_mode = 0;
		mcu_set_wifi_mode(AP_CONFIG);
	}

	/////////////////////////////////////////////////////////////////////////
	data = mcu_get_wifi_work_state(); //״̬��ѯ

	if (data == SMART_CONFIG_STATE) // smart����״̬
	{
		if (count_250ms_wifi_shine == 0)
		{
			count_250ms_wifi_shine = 5;
			fg_wifi_led ^= 1;
		}
	}
	else if (data == AP_STATE) // ap����״̬
	{
		if (count_1500ms_wifi_shine == 0)
		{
			count_1500ms_wifi_shine = 30;

			fg_wifi_led ^= 1;
		}
	}
	else if (data == WIFI_NOT_CONNECTED) //�����õ�δ����·����
	{
		fg_wifi_led = 0;
	}
	else if (data == WIFI_CONNECTED) //����·����
	{
		fg_wifi_led = 1;
	}
	else if (data == WIFI_CONN_CLOUD) //�����ƶ�
	{
		fg_wifi_led = 1;
	}
	else if (data == WIFI_LOW_POWER) //�͹���ģʽ
	{
		fg_wifi_led = 0;
	}
	else if (mcu_get_wifi_work_state() == SMART_AND_AP_STATE) // Wi-Fi �豸���� Smart �� AP ��������״̬
	{
		if (count_250ms_wifi_shine == 0)
		{
			count_250ms_wifi_shine = 5;
			fg_wifi_led ^= 1;
		}
	}
	else if (mcu_get_wifi_work_state() == WIFI_SATE_UNKNOW) //δ֪ģʽ
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
 * @brief  ��������
 * @param  Null
 * @return Null
 */
void pro_wifi_updata(void)
{
	uchar k, s;

	if (fg_20ms_delay_ask >= 10) // wifi���ͺ��20ms������ڷ��ͣ�ʵ����50ms��
	{
		/////////////////////////////////////////////////////////////////////////////����

		wifi_error_fault = 0;

		if (ValBit(gu16TripFlg_Driver, Involterror))//�����ѹ�쳣E001
			wifi_error_fault |= 1<<0;//0x00000001;
		if (ValBit(gu16TripFlg_Driver, Incurroverh))//�������������ֵE002
			wifi_error_fault |= 1<<1;//0x00000002;
		
		if (ValBit(gu16TripFlg_Driver, IpmTempoverh))//ɢ��������E101
			wifi_error_fault |= 1<<2;//0x00000004;
		if (ValBit(gu16TripFlg_Driver, Ipmnotemp))//ɢ��������������E102
			wifi_error_fault |= 1<<3;//0x00000008;
		if (ValBit(gu16TripFlg_Driver, Insysdriverr))//������������E103
			wifi_error_fault |= 1<<4;//0x00000010;
		if (ValBit(gu16TripFlg_Driver, Inmotmisitem))//���ȱ�ౣ��E104
			wifi_error_fault |= 1<<5;//0x00000020;
		if (ValBit(gu16TripFlg_Driver, Incurrerror))//��������������·����E105
			wifi_error_fault |= 1<<6;//0x00000040;
		if (ValBit(gu16TripFlg_Driver, Indccurrer))// DC��ѹ�쳣E106
			wifi_error_fault |= 1<<7;//0x00000080;
		if (ValBit(gu16TripFlg_Driver, Inpfcprot))// PFC����E107
			wifi_error_fault |= 1<<8;//0x00000100;
		if (ValBit(gu16TripFlg_Driver, Inmotoverload)) //������ʳ���E108
			wifi_error_fault |= 1<<9;//0x00000200;
		
		if (ValBit(gu16TripFlg_Driver, Inmotcurrerr)) //�����������·����E201
			wifi_error_fault |= 1<<10;//0x00000400;
		if (ValBit(gu16TripFlg_Driver, Insyseepeer))//������EEPROM��д����E202
			wifi_error_fault |= 1<<11;//0x00000800;
		if (ValBit(gu16TripFlg_Board, Owrtcerror))//RTCʱ�Ӷ�д����E203
			wifi_error_fault |= 1<<12;//0x00001000;
		if (ValBit(gu16TripFlg_Board, Oweeperror))//����EEPROM��д����E204
			wifi_error_fault |= 1<<13;//0x00002000;
		if (ValBit(gu16TripFlg_Driver, Inuarterr))//������������ͨ�Ź���E205
			wifi_error_fault |= 1<<14;//0x00004000;
		if (ValBit(gu16TripFlg_Board, Ow485error))//485ͨѶ����E206
			wifi_error_fault |= 1<<15;//0x00008000;
		
		if(ValBit(gu16TripFlg_Driver,Inlowflowerr))   //��ˮ���ձ���E207
			wifi_error_fault |= 1<<16;//0x00010000; 		
		if (ValBit(gu16TripFlg_Board,Ownowaterpr))     //����������E208
			wifi_error_fault |= 1<<17;//0x00020000;          
		if (ValBit(gu16TripFlg_Board,Owupwaterfail))   //����ʧ��E209  
			wifi_error_fault |= 1<<18;//0x00040000;   
			
		if (ValBit(gu16TripFlg_Warn, LsHighTemp))      //����Ԥ������AL01
			wifi_error_fault |= 1<<19;//0x00080000;
		if (ValBit(gu16TripFlg_Warn, Lslowvolt))//ACǷѹ����AL02
			wifi_error_fault |= 1<<20;//0x00100000;
		if (ValBit(gu16TripFlg_Warn, Wnfrostpro))//��������AL03
			wifi_error_fault |= 1<<21;//0x00200000;

		
    
				

		if (wifi_error_fault_buf != wifi_error_fault)
		{
			wifi_error_fault_buf = wifi_error_fault;
			mcu_dp_fault_update(DPID_FAULT, wifi_error_fault);
		}

		//////////////////////////////////////////////////////////////////////////////////����

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

		/////////////////////////////////////////////////////////////////////////////////�����趨���ֵ
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
			mcu_dp_value_update(DPID_FLOW_RATE_MAX, wifi_water_setrange_max_value); // VALUE�������ϱ�;
		}

		//////////////////////////////////////////////////////////////////////////////////ʵʱ��������

		wifi_gu16RunSpd_real_value = gu16MotorSetSpd * 2 * 100 / u16LadderSpd[16];
		wifi_gu16RunSpd_real_value = wifi_gu16RunSpd_real_value / 5 * 5;

		if (wifi_gu16RunSpd_real_value > 100)
			wifi_gu16RunSpd_real_value = 100;

		if (wifi_gu16RunSpd_real_value_buf != wifi_gu16RunSpd_real_value)
		{
			wifi_gu16RunSpd_real_value_buf = wifi_gu16RunSpd_real_value;
			mcu_dp_value_update(DPID_SPEED_REAL, wifi_gu16RunSpd_real_value);
		}

		///////////////////////////////////////////////////////////////////////////////////ģʽ

		//	wifi_gu8SetMode_enum=0;

		if (gu8Wash) //��ϴ
		{
			if(!(fg_rest_con_ok == 1 &&fg_auto_upwater == 0 && fg_selfimbi_ok == 0  ))//�ǵ�����ѧϰ
			{
				wifi_gu8SetMode_enum = 0x02;
			}
			else//��������ѧϰ//20220526+
			{
				if (gu8SetMode == SPEED) //ѭ������
				{
					wifi_gu8SetMode_enum = 0x01;
				}
				else if (gu8SetMode == CONTROL) //����
				{
					wifi_gu8SetMode_enum = 0x00;
				}
				else if (gu8SetMode == TIME) //��ʱ
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
		else if (gu8SetMode == SPEED) //ѭ������
		{
			wifi_gu8SetMode_enum = 0x01;
		}
		else if (gu8SetMode == CONTROL) //����
		{
			wifi_gu8SetMode_enum = 0x00;
		}
		else if (gu8SetMode == TIME) //��ʱ
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
		else if ((gu8SetMode EQU MODE_Ctrl1) OR(SysProState == SysProState_Test)) //�ⲿ���ƺ��Լ�
		{
			wifi_gu8SetMode_enum = 0x01;
		}
		else //�ػ�
		{

			if (gu8SetMode_eep == SPEED) //ѭ������
			{
				wifi_gu8SetMode_enum = 0x01;
			}
			else if (gu8SetMode_eep == TIME) //��ʱ
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
			else if (gu8SetMode_eep == CONTROL) //����
			{
				wifi_gu8SetMode_enum = 0x00;
			}
			else //����
			{
				wifi_gu8SetMode_enum = 0x00;
			}
		}

		if ((wifi_gu8SetMode_enum_buf != wifi_gu8SetMode_enum) || (fg_dp_download_mode == 1))
		{
			wifi_gu8SetMode_enum_buf = wifi_gu8SetMode_enum;
			mcu_dp_enum_update(DPID_MODE, wifi_gu8SetMode_enum);
		}

		///////////////////////////////////////////////////////////////////////////////////////////����ϴ����ʱ��

		wifi_WashTmSet_value = WashTmSet;

		if ((wifi_WashTmSet_value_buf != wifi_WashTmSet_value) || (fg_dp_download_backwash_time_set == 1))
		{
			// fg_dp_download_backwash_time_set=0;

			wifi_WashTmSet_value_buf = wifi_WashTmSet_value;
			mcu_dp_value_update(DPID_BACKWASH_TIME_SET, wifi_WashTmSet_value);
		}

		//////////////////////////////////////////////////////////////////////////////////////////	���ػ�

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
		/////////////////////////////////////////////////////////////////////////////////////////�����趨ֵ

		//  wifi_WaterGate_set_value =  wifi_gu16RunSpd_set_value  *  water_max_set  /  100; //���������趨ֵ��

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

		/////////////////////////////////////////////////////////////////////////////////////////	�����趨��Сֵ

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

		///////////////////////////////////////////////////////////////////////////////////////	��ǰ����ϴ����ʱ

		if (gu8SetWastTm > 0)
			WashTm = WashTmSet;
		wifi_WashTm_value = WashTm;

		if (wifi_WashTm_value_buf != wifi_WashTm_value)
		{
			wifi_WashTm_value_buf = wifi_WashTm_value;
			mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN, wifi_WashTm_value);
		}

		/////////////////////////////////////////////////////////////////////////////////////�ĵ���
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
			wifi_kilowatt_hour_value = 0; //��ֹ���������ٷ�һ��//20220105
		}

		////////////////////////////////////////////////////////////////////////////////��λ�л�

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

		///////////////////////////////////////////////////////	�����趨ֵ
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

		//////////////////////////////////////////////////////////////////////////////////////////ʵʱˮ����

		wifi_WaterGate_value = WaterGate / 100;
		s = WaterGate % 100;
		if (s > 50)
			wifi_WaterGate_value += 1;

		if (wifi_WaterGate_value_buf != wifi_WaterGate_value)
		{
			wifi_WaterGate_value_buf = wifi_WaterGate_value;
			mcu_dp_value_update(DPID_WATER_REALFLOW, wifi_WaterGate_value);
		}

		/////////////////////////////////////////////////////////////////////////////////////////��������
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

		////////////////////////////////////////////////////////////////////////////////////////	ˮѹ�澯

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

		/////////////////////////////////////////////////////����ֵ�ı�ᷢ�����ݣ����������//////////////////////////
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
		mcu_dp_fault_update(DPID_FAULT, wifi_error_fault); //�����������ϱ�;
		all_updata_step++;
		break;
	case 2:
		mcu_dp_value_update(DPID_CUR_POWER, wifi_gu16MotorWt_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 3:
		mcu_dp_value_update(DPID_FLOW_RATE_MAX, wifi_water_setrange_max_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 4:
		mcu_dp_value_update(DPID_SPEED_REAL, wifi_gu16RunSpd_real_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 5:
		mcu_dp_enum_update(DPID_MODE, wifi_gu8SetMode_enum); //ö���������ϱ�;
		all_updata_step++;
		break;
	case 6:
		mcu_dp_value_update(DPID_BACKWASH_TIME_SET, wifi_WashTmSet_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 7:
		mcu_dp_bool_update(DPID_SWTICH, wifi_power_switch_bool); // BOOL�������ϱ�;
		all_updata_step++;
		break;
	case 8:
		mcu_dp_value_update(DPID_FLOW_RATE_SET, wifi_WaterGate_set_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 9:
		mcu_dp_value_update(DPID_FLOW_RATE_MIN, wifi_water_setrange_min_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 10:
		mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN, wifi_WashTm_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 11:
		mcu_dp_value_update(DPID_POWER_CONSUMPTION, wifi_kilowatt_hour_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 12:
		mcu_dp_enum_update(DPID_UINT_SWITCH, wifi_water_uint_switch_enum); //ö���������ϱ�;
		all_updata_step++;
		break;
	case 13:
		mcu_dp_value_update(DPID_SPEED_SET, wifi_gu16RunSpd_set_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 14:
		mcu_dp_value_update(DPID_WATER_REALFLOW, wifi_WaterGate_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 15:
		mcu_dp_value_update(DPID_FLOW_STEP, wifi_WaterGate_set_step_value); // VALUE�������ϱ�;
		all_updata_step++;
		break;
	case 16:
		mcu_dp_bool_update(DPID_FLOW_PRESS_WARN, wifi_flow_press_warn_bool); ////BOOL�������ϱ�;
		all_updata_step++;
		break;

	default:
		all_updata_step = 0;
	}
}
