/**********************************Copyright (c)**********************************
**                       ��Ȩ���� (C), 2015-2020, Ϳѻ�Ƽ�
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    protocol.c
 * @author  Ϳѻ�ۺ�Э�鿪����
 * @version v2.5.6
 * @date    2020.12.16
 * @brief
 *                       *******�ǳ���Ҫ��һ��Ҫ��Ŷ������********
 *          1. �û��ڴ��ļ���ʵ�������·�/�ϱ�����
 *          2. DP��ID/TYPE�����ݴ���������Ҫ�û�����ʵ�ʶ���ʵ��
 *          3. ����ʼĳЩ�궨�����Ҫ�û�ʵ�ִ���ĺ����ڲ���#err��ʾ,��ɺ�������ɾ����#err
 */

/****************************** �������� ������ *******************************
����MCU���ͺͱ��뻷�����ֶ��������Դ˴�������ο����û������аѿ����մ���������
Ϳѻ����MCU���ܽ������
******************************************************************************/

/******************************************************************************
                                ��ֲ��֪:
1:MCU������while��ֱ�ӵ���mcu_api.c�ڵ�wifi_uart_service()����
2:����������ʼ����ɺ�,���鲻���йش����ж�,�������ж�,���ж�ʱ������,���жϻ����𴮿����ݰ���ʧ
3:�������ж�/��ʱ���ж��ڵ����ϱ�����
******************************************************************************/

#include "wifi.h"
#include "protocol.h"
#include "Mode.h"
#include "ModeFunc.h"
#include "Adc.h"
#ifdef WEATHER_ENABLE
/**
 * @var    weather_choose
 * @brief  �������ݲ���ѡ������
 * @note   �û������Զ�����Ҫ�Ĳ�����ע�ͻ���ȡ��ע�ͼ��ɣ�ע�����
 */
const char *weather_choose[WEATHER_CHOOSE_CNT] = {
    "temp",
    "humidity",
    "condition",
    "pm25",
    /*"pressure",
    "realFeel",
    "uvi",
    "tips",
    "windDir",
    "windLevel",
    "windSpeed",
    "sunRise",
    "sunSet",
    "aqi",
    "so2 ",
    "rank",
    "pm10",
    "o3",
    "no2",
    "co",
    "conditionNum",*/
};
#endif

/******************************************************************************
                              ��һ��:��ʼ��
1:����Ҫʹ�õ�wifi����ļ����ļ���include "wifi.h"
2:��MCU��ʼ���е���mcu_api.c�ļ��е�wifi_protocol_init()����
3:��MCU���ڵ��ֽڷ��ͺ�������protocol.c�ļ���uart_transmit_output������,��ɾ��#error
4:��MCU���ڽ��պ����е���mcu_api.c�ļ��ڵ�uart_receive_input����,�������յ����ֽ���Ϊ��������
5:��Ƭ������whileѭ�������mcu_api.c�ļ��ڵ�wifi_uart_service()����
******************************************************************************/

/******************************************************************************
                        1:dp���ݵ��������Ͷ��ձ�
          **��Ϊ�Զ����ɴ���,���ڿ���ƽ̨������޸�����������MCU_SDK**
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
    {
        {DPID_FAULT, DP_TYPE_BITMAP},
        {DPID_CUR_POWER, DP_TYPE_VALUE},
        {DPID_FLOW_RATE_MAX, DP_TYPE_VALUE},
        {DPID_SPEED_REAL, DP_TYPE_VALUE},
        {DPID_MODE, DP_TYPE_ENUM},
        {DPID_BACKWASH_TIME_SET, DP_TYPE_VALUE},
        {DPID_SWTICH, DP_TYPE_BOOL},
        {DPID_FLOW_RATE_SET, DP_TYPE_VALUE},
        {DPID_FLOW_RATE_MIN, DP_TYPE_VALUE},
        {DPID_BACKWASH_COUNTDOWN, DP_TYPE_VALUE},
        {DPID_POWER_CONSUMPTION, DP_TYPE_VALUE},
        {DPID_UINT_SWITCH, DP_TYPE_ENUM},
        {DPID_SPEED_SET, DP_TYPE_VALUE},
        {DPID_WATER_REALFLOW, DP_TYPE_VALUE},
        {DPID_FLOW_STEP, DP_TYPE_VALUE},

        {DPID_FLOW_PRESS_WARN, DP_TYPE_BOOL},

};


/******************************************************************************
                           2:���ڵ��ֽڷ��ͺ���
�뽫MCU���ڷ��ͺ�������ú�����,�������յ���������Ϊ�������봮�ڷ��ͺ���
******************************************************************************/

/**
 * @brief  ���ڷ�������
 * @param[in] {value} ����Ҫ���͵�1�ֽ�����
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    // #error "�뽫MCU���ڷ��ͺ�������ú���,��ɾ������"//yxl-wifi

    /*
        extern void Uart_PutChar(unsigned char value);
        Uart_PutChar(value);	                                //���ڷ��ͺ���
    */

    pro_wifiuart_tx(value); ////yxl-wifi
}

/******************************************************************************
                           �ڶ���:ʵ�־����û�����
1:APP�·����ݴ���
2:�����ϱ�����
******************************************************************************/

/******************************************************************************
                            1:���������ϱ�����
��ǰ��������ȫ�������ϱ�(�������·�/���ϱ���ֻ�ϱ�)
  ��Ҫ�û�����ʵ�����ʵ��:
  1:��Ҫʵ�ֿ��·�/���ϱ����ݵ��ϱ�
  2:��Ҫʵ��ֻ�ϱ����ݵ��ϱ�
�˺���ΪMCU�ڲ��������
�û�Ҳ�ɵ��ô˺���ʵ��ȫ�������ϱ�
******************************************************************************/

//�Զ������������ϱ�����

/**
 * @brief  ϵͳ����dp����Ϣ�ϴ�,ʵ��APP��muc����ͬ��
 * @param  Null
 * @return Null
 * @note   �˺���SDK�ڲ�����ã�MCU����ʵ�ָú����������ϱ����ܣ�����ֻ�ϱ��Ϳ��ϱ����·�������
 */
void all_data_update(void)
{
    all_updata_step = 1; // 20220314+����all���ݵĵڼ����裬Ϊ1ʱ����Wifi.c�ļ��е�void pro_all_updata()����

    //  #error "���ڴ˴�����·����ϱ����ݼ�ֻ�ϱ�����ʾ��,������ɺ�ɾ������"//yxl-wif
    // /*
    //�˴���Ϊƽ̨�Զ����ɣ��밴��ʵ�������޸�ÿ�����·����ϱ�������ֻ�ϱ�����
    //    mcu_dp_fault_update(DPID_FAULT,��ǰ����); //�����������ϱ�;
    //    mcu_dp_value_update(DPID_CUR_POWER,��ǰ��ǰ����); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_FLOW_RATE_MAX,��ǰ�����趨���ֵ); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_SPEED_REAL,��ǰʵʱ��������); //VALUE�������ϱ�;
    //    mcu_dp_enum_update(DPID_MODE,��ǰģʽ); //ö���������ϱ�;
    //    mcu_dp_value_update(DPID_BACKWASH_TIME_SET,��ǰ����ϴ����ʱ��); //VALUE�������ϱ�;
    //    mcu_dp_bool_update(DPID_SWTICH,��ǰ���ػ�); //BOOL�������ϱ�;
    //    mcu_dp_value_update(DPID_FLOW_RATE_SET,��ǰ�����趨ֵ); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_FLOW_RATE_MIN,��ǰ�����趨��Сֵ); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN,��ǰ����ϴ����ʱ); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_POWER_CONSUMPTION,��ǰ�ĵ���); //VALUE�������ϱ�;
    //    mcu_dp_enum_update(DPID_UINT_SWITCH,��ǰ��λ�л�); //ö���������ϱ�;
    //    mcu_dp_value_update(DPID_SPEED_SET,��ǰ�����趨ֵ); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_WATER_REALFLOW,��ǰʵʱˮ����); //VALUE�������ϱ�;
    //    mcu_dp_value_update(DPID_FLOW_STEP,��ǰ��������ֵ); //VALUE�������ϱ�;
    //	 mcu_dp_bool_update(DPID_FLOW_PRESS_WARN,ѹ���澯); ////BOOL�������ϱ�;

    // */

    // 20220314-���ڷ��͵Ļ������ƣ��ʲ�ȡ�������͵ķ������ο�Wifi.c�ļ���void pro_all_updata(void)������
    //	  mcu_dp_fault_update(DPID_FAULT,wifi_error_fault); //�����������ϱ�;
    //     mcu_dp_value_update(DPID_CUR_POWER,wifi_gu16MotorWt_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_FLOW_RATE_MAX,wifi_water_setrange_max_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_SPEED_REAL,wifi_gu16RunSpd_real_value); //VALUE�������ϱ�;
    //     mcu_dp_enum_update(DPID_MODE,wifi_gu8SetMode_enum); //ö���������ϱ�;
    //     mcu_dp_value_update(DPID_BACKWASH_TIME_SET,wifi_WashTmSet_value); //VALUE�������ϱ�;
    //     mcu_dp_bool_update(DPID_SWTICH,wifi_power_switch_bool); //BOOL�������ϱ�;
    //     mcu_dp_value_update(DPID_FLOW_RATE_SET,wifi_WaterGate_set_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_FLOW_RATE_MIN,wifi_water_setrange_min_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_BACKWASH_COUNTDOWN,wifi_WashTm_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_POWER_CONSUMPTION,wifi_kilowatt_hour_value); //VALUE�������ϱ�;
    //     mcu_dp_enum_update(DPID_UINT_SWITCH,wifi_water_uint_switch_enum); //ö���������ϱ�;
    //     mcu_dp_value_update(DPID_SPEED_SET,wifi_gu16RunSpd_set_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_WATER_REALFLOW,wifi_WaterGate_value); //VALUE�������ϱ�;
    //     mcu_dp_value_update(DPID_FLOW_STEP,wifi_WaterGate_set_step_value); //VALUE�������ϱ�;
    //	  mcu_dp_bool_update(DPID_FLOW_PRESS_WARN,wifi_flow_press_warn_bool); ////BOOL�������ϱ�;
    // 20220314-
}

/******************************************************************************
                                WARNING!!!
                            2:���������ϱ�����
�Զ�������ģ�庯��,�������û�����ʵ�����ݴ���
******************************************************************************/
/*****************************************************************************
�������� : dp_download_mode_handle
�������� : ���DPID_MODE�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     ��ģʽ
*****************************************************************************/
static unsigned char dp_download_mode_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪENUM
    unsigned char ret;
    unsigned char mode;

    mode = mcu_get_dp_download_enum(value, length);
    switch (mode)
    {
    case 0: //���յ�����ģʽָ��
        //��ѧϰ�����ڳ�Ƶ�����У���������ˮ
        if (fg_rest_con_ok == 1 && fg_selfimbi_ok == 0 && fg_auto_upwater == 0)
        {
            gu8SetMode = CONTROL;	
        }
				//������ˮ
				if(fg_auto_upwater == 1)
				{
					;
				}
        //��Ƶ��������δ��ѧϰʱ��
        else if (fg_selfimbi_ok == 1 && fg_rest_con_ok == 0)
        {
            fg_selfimbi_ok = 0;   //�˳���Ƶ����
            //fg_rest_con_wifi = 1; // 20220105
            fg_rest_con_ok = 1;u8WashState = WashState_SelfStudy; //�˳�����,������ѧϰ;   //������ѧϰ
        }
        //��Ƶ�������Ѿ���ѧϰʱ��
        else if (fg_selfimbi_ok == 1 && fg_rest_con_ok == 1)
        {

        }
        //�ػ�ʱ
        else if(gu8SetMode == 0)
        {
          gu8SetMode = CONTROL;           
        }
				//�����������ʱ
				else
				{
					gu8Wash = 0;
          gu8SetMode = CONTROL;				
				}
        break; 
    case 1: //���յ�ת��ģʽָ��
        //��ѧϰ�����ڳ�Ƶ�����У���������ˮ
        if (fg_rest_con_ok == 1 && fg_selfimbi_ok == 0 && fg_auto_upwater == 0)
        {
            gu8SetMode = CONTROL;	
        }
				//������ˮ
				if(fg_auto_upwater == 1)
				{
					;
				}
        //��Ƶ��������δ��ѧϰʱ��
        else if (fg_selfimbi_ok == 1 && fg_rest_con_ok == 0)
        {
            fg_selfimbi_ok = 0;   //�˳���Ƶ����
            //fg_rest_con_wifi = 1; // 20220105
            fg_rest_con_ok = 1;u8WashState = WashState_SelfStudy; //�˳�����,������ѧϰ;   //������ѧϰ
        }
        //��Ƶ�������Ѿ���ѧϰʱ��
        else if (fg_selfimbi_ok == 1 && fg_rest_con_ok == 1)
        {

        }
        //�ػ�ʱ
        else if(gu8SetMode == 0)
        {
          gu8SetMode = SPEED;           
        }
				//�����������ʱ
        else
        {
            gu8SetMode = SPEED;
            gu8Wash = 0;
        }
        break;
    case 2: //���յ�����ϴģʽָ��
        //��ѧϰ �� ������ˮ �� ��Ƶ����ʱ
        if ((fg_rest_con_ok == 1) || (fg_auto_upwater == 1) || (fg_selfimbi_ok == 1)) // yxl-xue  �״β��ܵ���180  ��ѧϰ�ڼ䲻�ɲ���
        {
        }
        else
        {
            if (((gu8SetMode EQU SPEED)OR(gu8SetMode EQU TIME) OR(gu8SetMode EQU CONTROL))AND(gu8PowerOnTm > 10))
            {
                if (!gu8Wash) // 20211228+
                {
                    gu8KeyFlg= bWash; /// yyxxll
                }
            }
        }
        break;
    default:

        break;
    }

    //������DP���ݺ�Ӧ�з���
    // subBuzCtrl(SHORTBE);
    fg_dp_download_mode = 1;
    fg_20ms_delay_ask = 0;

    ret = mcu_dp_enum_update(DPID_MODE, mode);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_backwash_time_set_handle
�������� : ���DPID_BACKWASH_TIME_SET�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     ������ϴʱ�䣬��ѧϰ�ͳ�Ƶ������������ˮ
*****************************************************************************/
static unsigned char dp_download_backwash_time_set_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long backwash_time_set;

    backwash_time_set = mcu_get_dp_download_value(value, length);
    /*
    //VALUE�������ݴ���
    */

    //������ѧϰ�����ǳ�Ƶ���������ǿ�����ˮʱ��ֻ�Ƿ���ϴʱ
    if ((gu8Wash == 1) && (fg_rest_con_ok == 0) && (fg_selfimbi_ok == 0) && (fg_auto_upwater == 0))
    {
				WashTmSet = backwash_time_set;
        WashTmSet_Normal = backwash_time_set;
    }
    else if ((fg_rest_con_ok == 1) && (fg_selfimbi_ok == 0) && (fg_auto_upwater == 0)) //������ѧϰ
    {
        //            WashTmSet = backwash_time_set;
        //            if (WashTmSet <= 180)
        //                WashTmSet = 180;
    }
    else if ((fg_selfimbi_ok == 1)&& (fg_auto_upwater == 0)) //��Ƶ����ʱ
    {
        WashTmSet = backwash_time_set;
        if (WashTmSet <= 600)
            WashTmSet = 600;
    }
    else if (fg_auto_upwater == 1) //��ˮʱ
    {
    }
    //������DP���ݺ�Ӧ�з���
    fg_dp_download_backwash_time_set = 1;
    fg_20ms_delay_ask = 0;

    ret = mcu_dp_value_update(DPID_BACKWASH_TIME_SET, backwash_time_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_swtich_handle
�������� : ���DPID_SWTICH�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     �����ػ�
*****************************************************************************/
static unsigned char dp_download_swtich_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪBOOL
    unsigned char ret;
    // 0:��/1:��
    unsigned char swtich;

    swtich = mcu_get_dp_download_bool(value, length);
    if (swtich == 0)
    {
        //����ʱ����
        if (gu8SetMode != 0)
        {
            gu8KeyFlg = bPower;
            if (gu8SetMode == SPEED) // yxl-5  �ػ�������
            {
                gu8SetMode_eep = SPEED;
            }
            else if (gu8SetMode == TIME)
            {
                gu8SetMode_eep = TIME;
            }
            else
            {
                gu8SetMode_eep = CONTROL;
            }

            // 20220105
            if (fg_selfimbi_ok == 1) //���ڳ�Ƶ����
            {
                fg_rest_con_wifi = 1;
            }
        }
    }
    else
    {
        //�ػ�ʱ����
        if (gu8SetMode == 0)
        {
            gu8KeyFlg = bPower;
        }
    }

    //������DP���ݺ�Ӧ�з���
    // subBuzCtrl(SHORTBE);
    fg_dp_download_swtich = 1;
    fg_20ms_delay_ask = 0;
    ret = mcu_dp_bool_update(DPID_SWTICH, swtich);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_flow_rate_set_handle
�������� : ���DPID_FLOW_RATE_SET�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     ������ֵ
*****************************************************************************/
static unsigned char dp_download_flow_rate_set_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long flow_rate_set;

    flow_rate_set = mcu_get_dp_download_value(value, length);
    /*
    //VALUE�������ݴ���

    */

    count_5s_dis_realwater = 0;
		if (water_uint_switch == 0)
		{
				WaterGate_set = flow_rate_set;
		}
		else if (water_uint_switch == 1)
		{
				WaterGate_set = (flow_rate_set - 80) / 20 + 8;
		}
		else if (water_uint_switch == 2)
		{
				WaterGate_set = (flow_rate_set - 15) / 5 + 9;
		}
		else if (water_uint_switch == 3)
		{
				WaterGate_set = (flow_rate_set - 20) / 5 + 7;
		}

			//�����ⲿ���ơ�485����ʱ
    if (u8OutCtrlState)
    {
    }
    else
    {			
			gu8SetMode = CONTROL; //��ʱֱ��תΪ����			
    }
		
    if (fg_selfimbi_ok == 1) // 20211230�޸��˳�Ƶ����ʱ��ʾ�����յ�app������ģʽָ��󲻻���ѧϰ������
    {
        fg_rest_con_ok = 1;u8WashState = WashState_SelfStudy; //�˳�����,������ѧϰ;
    }

    //������DP���ݺ�Ӧ�з���
    // subBuzCtrl(SHORTBE);
    fg_dp_download_flow_rate_set = 1;
    fg_20ms_delay_ask = 0;
    ret = mcu_dp_value_update(DPID_FLOW_RATE_SET, flow_rate_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_uint_switch_handle
�������� : ���DPID_UINT_SWITCH�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     ��������λ
*****************************************************************************/
static unsigned char dp_download_uint_switch_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪENUM
    unsigned char ret;
    unsigned char uint_switch;

    uint_switch = mcu_get_dp_download_enum(value, length);
    switch (uint_switch)
    {
    case 0:
        water_uint_switch = 0x00;
        break;
    case 1:
        water_uint_switch = 0x01;
        break;
    case 2:
        water_uint_switch = 0x03;
        break;
    case 3:
        water_uint_switch = 0x02;
        break;
    default:
        break;
    }

    //������DP���ݺ�Ӧ�з���
    fg_dp_download_uint_switch = 1;
    fg_20ms_delay_ask = 0;
    ret = mcu_dp_enum_update(DPID_UINT_SWITCH, uint_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
�������� : dp_download_speed_set_handle
�������� : ���DPID_SPEED_SET�Ĵ�����
������� : value:����Դ����
        : length:���ݳ���
���ز��� : �ɹ�����:SUCCESS/ʧ�ܷ���:ERROR
ʹ��˵�� : ���·����ϱ�����,��Ҫ�ڴ��������ݺ��ϱ���������app
��ע     ��ת��
*****************************************************************************/
static unsigned char dp_download_speed_set_handle(const unsigned char value[], unsigned short length)
{
    //ʾ��:��ǰDP����ΪVALUE
    unsigned char ret;
    unsigned long speed_set;

    speed_set = mcu_get_dp_download_value(value, length);
    /*
    //VALUE�������ݴ���

    */
		u8ModeSpeed_SpeedSet = (speed_set - 20) / 5 + 24;

	//�����ⲿ���ơ�485����ʱ
    if (u8OutCtrlState)
    {
    }
    else
    {			
      
			gu8SetMode = SPEED;
			
    }
    if (fg_selfimbi_ok == 1) // 20211230�޸��˳�Ƶ����ʱ��ʾ�����յ�app��ת��ģʽָ��󲻻���ѧϰ������
    {
        fg_rest_con_ok = 1;u8WashState = WashState_SelfStudy; //�˳�����,������ѧϰ;
    }
    //������DP���ݺ�Ӧ�з���
    fg_dp_download_speed_set = 1;
    fg_20ms_delay_ask = 0;
    ret = mcu_dp_value_update(DPID_SPEED_SET, speed_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/******************************************************************************--------------------------------------------------------------------------------------------
                                WARNING!!!
�˲��ֺ����û������޸�!!
******************************************************************************/

/**
 * @brief  dp�·�������
 * @param[in] {dpid} dpid ���
 * @param[in] {value} dp���ݻ�������ַ
 * @param[in] {length} dp���ݳ���
 * @return dp������
 * -           0(ERROR): ʧ��
 * -           1(SUCCESS): �ɹ�
 * @note   �ú����û������޸�
 */
unsigned char dp_download_handle(unsigned char dpid, const unsigned char value[], unsigned short length)
{
    /*********************************
    ��ǰ����������·�/���ϱ����ݵ���
    ���庯������Ҫʵ���·����ݴ���
    �������Ҫ��������������APP��,����APP����Ϊ�·�ʧ��
    ***********************************/
    unsigned char ret;
    switch (dpid)
    {
    case DPID_MODE:
        //ģʽ������
        ret = dp_download_mode_handle(value, length);
        break;
    case DPID_BACKWASH_TIME_SET:
        //����ϴ����ʱ�䴦����
        ret = dp_download_backwash_time_set_handle(value, length);
        break;
    case DPID_SWTICH:
        //���ػ�������
        ret = dp_download_swtich_handle(value, length);
        break;
    case DPID_FLOW_RATE_SET:
        //�����趨ֵ������
        ret = dp_download_flow_rate_set_handle(value, length);
        break;
    case DPID_UINT_SWITCH:
        //��λ�л�������
        ret = dp_download_uint_switch_handle(value, length);
        break;
    case DPID_SPEED_SET:
        //�����趨ֵ������
        ret = dp_download_speed_set_handle(value, length);
        break;

    default:
        break;
    }
    return ret;
}

/**
 * @brief  ��ȡ����dp�����ܺ�
 * @param[in] Null
 * @return �·������ܺ�
 * @note   �ú����û������޸�
 */
unsigned char get_download_cmd_total(void)
{
    return (sizeof(download_cmd) / sizeof(download_cmd[0]));
}

/******************************************************************************
                                WARNING!!!
�˴���ΪSDK�ڲ�����,�밴��ʵ��dp����ʵ�ֺ����ڲ�����
******************************************************************************/

#ifdef SUPPORT_MCU_FIRM_UPDATE
/**
 * @brief  ��������Сѡ��
 * @param[in] {package_sz} ��������С
 * @ref           0x00: 256byte (Ĭ��)
 * @ref           0x01: 512byte
 * @ref           0x02: 1024byte
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void upgrade_package_choose(unsigned char package_sz)
{
#error "������ʵ��������ʵ����������Сѡ�����,��ɺ���ɾ������"
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len, package_sz);
    wifi_uart_write_frame(UPDATE_START_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  MCU����̼�����ģʽ
 * @param[in] {value} �̼�������
 * @param[in] {position} ��ǰ���ݰ����ڹ̼�λ��
 * @param[in] {length} ��ǰ�̼�������(�̼�������Ϊ0ʱ,��ʾ�̼����������)
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
unsigned char mcu_firm_update_handle(const unsigned char value[], unsigned long position, unsigned short length)
{
#error "���������MCU�̼���������,��ɺ���ɾ������"
    if (length == 0)
    {
        //�̼����ݷ������
    }
    else
    {
        //�̼����ݴ���
    }

    return SUCCESS;
}
#endif

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  ��ȡ���ĸ���ʱ��
 * @param[in] {time} ��ȡ���ĸ���ʱ������
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void mcu_get_greentime(unsigned char time[])
{
#error "�����������ش���,��ɾ������"
    /*
    time[0] Ϊ�Ƿ��ȡʱ��ɹ���־��Ϊ 0 ��ʾʧ�ܣ�Ϊ 1��ʾ�ɹ�
    time[1] Ϊ��ݣ�0x00 ��ʾ 2000 ��
    time[2] Ϊ�·ݣ��� 1 ��ʼ��12 ����
    time[3] Ϊ���ڣ��� 1 ��ʼ��31 ����
    time[4] Ϊʱ�ӣ��� 0 ��ʼ��23 ����
    time[5] Ϊ���ӣ��� 0 ��ʼ��59 ����
    time[6] Ϊ���ӣ��� 0 ��ʼ��59 ����
    */
    if (time[0] == 1)
    {
        //��ȷ���յ�wifiģ�鷵�صĸ�������
    }
    else
    {
        //��ȡ����ʱ�����,�п����ǵ�ǰwifiģ��δ����
    }
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCUУ�Ա���RTCʱ��
 * @param[in] {time} ��ȡ���ĸ���ʱ������
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void mcu_write_rtctime(unsigned char time[])
{
#error "���������RTCʱ��д�����,��ɾ������"
    /*
    Time[0] Ϊ�Ƿ��ȡʱ��ɹ���־��Ϊ 0 ��ʾʧ�ܣ�Ϊ 1��ʾ�ɹ�
    Time[1] Ϊ��ݣ�0x00 ��ʾ 2000 ��
    Time[2] Ϊ�·ݣ��� 1 ��ʼ��12 ����
    Time[3] Ϊ���ڣ��� 1 ��ʼ��31 ����
    Time[4] Ϊʱ�ӣ��� 0 ��ʼ��23 ����
    Time[5] Ϊ���ӣ��� 0 ��ʼ��59 ����
    Time[6] Ϊ���ӣ��� 0 ��ʼ��59 ����
    Time[7] Ϊ���ڣ��� 1 ��ʼ�� 7 ������1��������һ
   */
    if (time[0] == 1)
    {
        //��ȷ���յ�wifiģ�鷵�صı���ʱ������
    }
    else
    {
        //��ȡ����ʱ�����ݳ���,�п����ǵ�ǰwifiģ��δ����
    }
}
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  wifi���ܲ��Է���
 * @param[in] {result} wifi���ܲ��Խ��
 * @ref       0: ʧ��
 * @ref       1: �ɹ�
 * @param[in] {rssi} ���Գɹ���ʾwifi�ź�ǿ��/����ʧ�ܱ�ʾ��������
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void wifi_test_result(unsigned char result, unsigned char rssi)
{
    //#error "������ʵ��wifi���ܲ��Գɹ�/ʧ�ܴ���,��ɺ���ɾ������"//yxl-wifi
    if (result == 0)
    {
        //����ʧ��
        if (rssi == 0x00)
        {
            //δɨ�赽����Ϊtuya_mdev_test·����,����
        }
        else if (rssi == 0x01)
        {
            //ģ��δ��Ȩ
        }

        WifiTestOk = 0;
    }
    else
    {
        //���Գɹ�
        // rssiΪ�ź�ǿ��(0-100, 0�ź���100�ź���ǿ)
        // 20220105
        if (rssi >= 60)
        {
            WifiTestOk = 1;
        }
        else
        {
            WifiTestOk = 0;
        }
    }
}
#endif

#ifdef WEATHER_ENABLE
/**
 * @brief  mcu����������
 * @param  Null
 * @return Null
 */
void mcu_open_weather(void)
{
    int i = 0;
    char buffer[13] = {0};
    unsigned char weather_len = 0;
    unsigned short send_len = 0;

    weather_len = sizeof(weather_choose) / sizeof(weather_choose[0]);

    for (i = 0; i < weather_len; i++)
    {
        buffer[0] = sprintf(buffer + 1, "w.%s", weather_choose[i]);
        send_len = set_wifi_uart_buffer(send_len, (unsigned char *)buffer, buffer[0] + 1);
    }

#error "�������ʾ���������ƴ�����������룬��ɺ���ɾ������"
    /*
    //����ȡ�Ĳ����к�ʱ���йصĲ���ʱ(��:�ճ�����)����Ҫ����t.unix����t.localʹ�ã���Ҫ��ȡ�Ĳ��������ǰ��ո���ʱ�仹�Ǳ���ʱ��
    buffer[0] = sprintf(buffer+1,"t.unix"); //����ʱ��   ��ʹ��  buffer[0] = sprintf(buffer+1,"t.local"); //����ʱ��
    send_len = set_wifi_uart_buffer(send_len, (unsigned char *)buffer, buffer[0]+1);
    */

    buffer[0] = sprintf(buffer + 1, "w.date.%d", WEATHER_FORECAST_DAYS_NUM);
    send_len = set_wifi_uart_buffer(send_len, (unsigned char *)buffer, buffer[0] + 1);

    wifi_uart_write_frame(WEATHER_OPEN_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  ���������ܷ����û��Դ�����
 * @param[in] {res} ���������ܷ��ؽ��
 * @ref       0: ʧ��
 * @ref       1: �ɹ�
 * @param[in] {err} ������
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void weather_open_return_handle(unsigned char res, unsigned char err)
{
#error "��������ɴ��������ܷ������ݴ������,��ɺ���ɾ������"
    unsigned char err_num = 0;

    if (res == 1)
    {
        //���������سɹ�
    }
    else if (res == 0)
    {
        //����������ʧ��
        //��ȡ������
        err_num = err;
    }
}

/**
 * @brief  ���������û��Դ�����
 * @param[in] {name} ������
 * @param[in] {type} ��������
 * @ref       0: int ��
 * @ref       1: string ��
 * @param[in] {data} ����ֵ�ĵ�ַ
 * @param[in] {day} ��һ�������  0:��ʾ���� ȡֵ��Χ: 0~6
 * @ref       0: ����
 * @ref       1: ����
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void weather_data_user_handle(char *name, unsigned char type, const unsigned char *data, char day)
{
#error "���������ʾ���������������������ݴ������,��ɺ���ɾ������"
    int value_int;
    char value_string[50]; //�����еĲ������ݽ϶࣬����Ĭ��Ϊ50�������Ը��ݶ���Ĳ����������ʵ����ٸ�ֵ

    my_memset(value_string, '\0', 50);

    //���Ȼ�ȡ��������
    if (type == 0)
    { //������INT��
        value_int = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    }
    else if (type == 1)
    {
        my_strcpy(value_string, data);
    }

    //ע��Ҫ������ѡ������������ò���ֵ������
    if (my_strcmp(name, "temp") == 0)
    {
        printf("day:%d temp value is:%d\r\n", day, value_int); // int ��
    }
    else if (my_strcmp(name, "humidity") == 0)
    {
        printf("day:%d humidity value is:%d\r\n", day, value_int); // int ��
    }
    else if (my_strcmp(name, "pm25") == 0)
    {
        printf("day:%d pm25 value is:%d\r\n", day, value_int); // int ��
    }
    else if (my_strcmp(name, "condition") == 0)
    {
        printf("day:%d condition value is:%s\r\n", day, value_string); // string ��
    }
}
#endif

#ifdef MCU_DP_UPLOAD_SYN
/**
 * @brief  ״̬ͬ���ϱ����
 * @param[in] {result} ���
 * @ref       0: ʧ��
 * @ref       1: �ɹ�
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void get_upload_syn_result(unsigned char result)
{
#error "���������״̬ͬ���ϱ��������,��ɾ������"

    if (result == 0)
    {
        //ͬ���ϱ�����
    }
    else
    {
        //ͬ���ϱ��ɹ�
    }
}
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  ��ȡ WIFI ״̬���
 * @param[in] {result} ָʾ WIFI ����״̬
 * @ref       0x00: wifi״̬ 1 smartconfig ����״̬
 * @ref       0x01: wifi״̬ 2 AP ����״̬
 * @ref       0x02: wifi״̬ 3 WIFI �����õ�δ����·����
 * @ref       0x03: wifi״̬ 4 WIFI ������������·����
 * @ref       0x04: wifi״̬ 5 ������·���������ӵ��ƶ�
 * @ref       0x05: wifi״̬ 6 WIFI �豸���ڵ͹���ģʽ
 * @ref       0x06: wifi״̬ 7 WIFI �豸����smartconfig&AP����״̬
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void get_wifi_status(unsigned char result)
{
#error "��������ɻ�ȡ WIFI ״̬�������,��ɾ������"

    switch (result)
    {
    case 0:
        // wifi����״̬1
        break;

    case 1:
        // wifi����״̬2
        break;

    case 2:
        // wifi����״̬3
        break;

    case 3:
        // wifi����״̬4
        break;

    case 4:
        // wifi����״̬5
        break;

    case 5:
        // wifi����״̬6
        break;

    case 6:
        // wifi����״̬7
        break;

    default:
        break;
    }
}
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  �������ͽ��
 * @param[in] {result} ���
 * @ref       0x00: �ɹ�
 * @ref       0x01: ��������δ����
 * @ref       0x02: ��������δ���ӳɹ�
 * @ref       0x03: �������ͳ�ʱ
 * @ref       0x04: ��������ݳ��ȴ���
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void stream_trans_send_result(unsigned char result)
{
#error "���������ʾ���������������������ͽ���������,��ɺ���ɾ������"
    switch (result)
    {
    case 0x00:
        //�ɹ�
        break;

    case 0x01:
        //��������δ����
        break;

    case 0x02:
        //��������δ���ӳɹ�
        break;

    case 0x03:
        //�������ͳ�ʱ
        break;

    case 0x04:
        //��������ݳ��ȴ���
        break;

    default:
        break;
    }
}

/**
 * @brief  ���ͼ�������ͽ��
 * @param[in] {result} ���
 * @ref       0x00: �ɹ�
 * @ref       0x01: ʧ��
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void maps_stream_trans_send_result(unsigned char result)
{
#error "���������ʾ�������������ƶ��ͼ�������ͽ���������,��ɺ���ɾ������"
    switch (result)
    {
    case 0x00:
        //�ɹ�
        break;

    case 0x01:
        //ʧ��
        break;

    default:
        break;
    }
}
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
/**
 * @brief  ·����Ϣ���ս��֪ͨ
 * @param[in] {result} ģ���Ƿ�ɹ����յ���ȷ��·����Ϣ
 * @ref       0x00: ʧ��
 * @ref       0x01: �ɹ�
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void wifi_connect_test_result(unsigned char result)
{
    // #error "������ʵ��wifi���ܲ��Գɹ�/ʧ�ܴ���,��ɺ���ɾ������"
    if (result == 0)
    {
        //·����Ϣ����ʧ�ܣ����鷢����·����Ϣ���Ƿ���������JSON���ݰ�
    }
    else
    {

        //·����Ϣ���ճɹ�����������ע��WIFI_STATE_CMDָ���wifi����״̬
    }
}
#endif

#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  ��ȡģ��mac���
 * @param[in] {mac} ģ�� MAC ����
 * @ref       mac[0]: Ϊ�Ƿ��ȡmac�ɹ���־��0x00 ��ʾ�ɹ���0x01 ��ʾʧ��
 * @ref       mac[1]~mac[6]: ����ȡ MAC��ַ��־λ���mac[0]Ϊ�ɹ������ʾģ����Ч��MAC��ַ
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void mcu_get_mac(unsigned char mac[])
{
#error "���������mac��ȡ����,��ɾ������"
    /*
    mac[0]Ϊ�Ƿ��ȡmac�ɹ���־��0x00 ��ʾ�ɹ���Ϊ0x01��ʾʧ��
    mac[1]~mac[6]:����ȡ MAC��ַ��־λ���mac[0]Ϊ�ɹ������ʾģ����Ч��MAC��ַ
   */

    if (mac[0] == 1)
    {
        //��ȡmac����
    }
    else
    {
        //��ȷ���յ�wifiģ�鷵�ص�mac��ַ
    }
}
#endif

#ifdef GET_IR_STATUS_ENABLE
/**
 * @brief  ��ȡ����״̬���
 * @param[in] {result} ָʾ����״̬
 * @ref       0x00: ����״̬ 1 ���ڷ��ͺ�����
 * @ref       0x01: ����״̬ 2 ���ͺ��������
 * @ref       0x02: ����״̬ 3 ����ѧϰ��ʼ
 * @ref       0x03: ����״̬ 4 ����ѧϰ����
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void get_ir_status(unsigned char result)
{
#error "��������ɺ���״̬����,��ɾ������"
    switch (result)
    {
    case 0:
        //����״̬ 1
        break;

    case 1:
        //����״̬ 2
        break;

    case 2:
        //����״̬ 3
        break;

    case 3:
        //����״̬ 4
        break;

    default:
        break;
    }

    wifi_uart_write_frame(GET_IR_STATUS_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef IR_TX_RX_TEST_ENABLE
/**
 * @brief  ��������շ�������֪ͨ
 * @param[in] {result} ģ���Ƿ�ɹ����յ���ȷ����Ϣ
 * @ref       0x00: ʧ��
 * @ref       0x01: �ɹ�
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void ir_tx_rx_test_result(unsigned char result)
{
#error "������ʵ�ֺ�������շ����⹦�ܲ��Գɹ�/ʧ�ܴ���,��ɺ���ɾ������"
    if (result == 0)
    {
        //��������շ�����ɹ�
    }
    else
    {
        //��������շ�����ʧ�ܣ����鷢�������ݰ�
    }
}
#endif

#ifdef FILE_DOWNLOAD_ENABLE
/**
 * @brief  �ļ����ذ���Сѡ��
 * @param[in] {package_sz} �ļ����ذ���С
 * @ref       0x00: 256 byte (Ĭ��)
 * @ref       0x01: 512 byte
 * @ref       0x02: 1024 byte
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void file_download_package_choose(unsigned char package_sz)
{
#error "������ʵ��������ʵ���ļ����ذ���Сѡ�����,��ɺ���ɾ������"
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len, package_sz);
    wifi_uart_write_frame(FILE_DOWNLOAD_START_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  �ļ�������ģʽ
 * @param[in] {value} ���ݻ�����
 * @param[in] {position} ��ǰ���ݰ������ļ�λ��
 * @param[in] {length} ��ǰ�ļ�������(����Ϊ0ʱ,��ʾ�ļ����������)
 * @return ���ݴ�����
 * -           0(ERROR): ʧ��
 * -           1(SUCCESS): �ɹ�
 * @note   MCU��Ҫ����ʵ�ָù���
 */
unsigned char file_download_handle(const unsigned char value[], unsigned long position, unsigned short length)
{
#error "����������ļ������ش���,��ɺ���ɾ������"
    if (length == 0)
    {
        //�ļ������ݷ������
    }
    else
    {
        //�ļ������ݴ���
    }

    return SUCCESS;
}
#endif

#ifdef MODULE_EXPANDING_SERVICE_ENABLE
/**
 * @brief  ��ģ��ʱ�����֪ͨ���
 * @param[in] {value} ���ݻ�����
 * @param[in] {length} ���ݳ���
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void open_module_time_serve_result(const unsigned char value[], unsigned short length)
{
#error "������ʵ��ģ��ʱ�����֪ͨ�������,��ɺ���ɾ������"
    unsigned char sub_cmd = value[0];

    switch (sub_cmd)
    {
    case 0x01:
    { //������  ��ģ��ʱ�����֪ͨ
        if (0x02 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        if (value[1] == 0)
        {
            //�������ɹ�
        }
        else
        {
            //������ʧ��
        }
    }
    break;

    case 0x02:
    { //������  ģ��ʱ�����֪ͨ
        if (0x09 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        unsigned char time_type = value[1]; // 0x00:����ʱ��  0x01:����ʱ��
        unsigned char time_data[7];

        my_memcpy(time_data, value + 2, length - 2);
        /*
        Data[0]Ϊ���, 0x00��ʾ2000��
        Data[1]Ϊ�·ݣ���1��ʼ��12����
        Data[2]Ϊ���ڣ���1��ʼ��31����
        Data[3]Ϊʱ�ӣ���0��ʼ��23����
        Data[4]Ϊ���ӣ���0��ʼ��59����
        Data[5]Ϊ���ӣ���0��ʼ��15����
        Data[6]Ϊ���ڣ���1��ʼ��7������1��������һ
        */

        //�ڴ˴����ʱ�����ݴ�����룬time_typeΪʱ������

        unsigned short send_len = 0;
        send_len = set_wifi_uart_byte(send_len, sub_cmd);
        wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
    }
    break;

    case 0x03:
    { //������  ��������������������
        if (0x02 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        if (value[1] == 0)
        {
            //�ɹ�
        }
        else
        {
            //ʧ��
        }
    }
    break;

    case 0x04:
    { //������  ��ģ������״̬֪ͨ
        if (0x02 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        if (value[1] == 0)
        {
            //�ɹ�
        }
        else
        {
            //ʧ��
        }
    }
    break;

    case 0x05:
    { //������  ģ������״̬֪ͨ
        if (0x02 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        switch (value[1])
        {
        case 0x00:
            //ģ�鱾������

            break;
        case 0x01:
            // APPԶ������

            break;
        case 0x02:
            // APP�ָ���������

            break;
        default:
            break;
        }

        unsigned short send_len = 0;
        send_len = set_wifi_uart_byte(send_len, sub_cmd);
        wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
    }
    break;

    default:
        break;
    }
}
#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
/**
 * @brief  ���������Բ��Խ��
 * @param[in] {value} ���ݻ�����
 * @param[in] {length} ���ݳ���
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void BLE_test_result(const unsigned char value[], unsigned short length)
{
#error "������ʵ�����������Բ��Խ������,��ɺ���ɾ������"
    unsigned char sub_cmd = value[0];

    if (0x03 != length)
    {
        //���ݳ��ȴ���
        return;
    }

    if (0x01 != sub_cmd)
    {
        //���������
        return;
    }

    unsigned char result = value[1];
    unsigned char rssi = value[2];

    if (result == 0)
    {
        //����ʧ��
        if (rssi == 0x00)
        {
            //δɨ�赽����Ϊ ty_mdev�����ű�,����
        }
        else if (rssi == 0x01)
        {
            //ģ��δ��Ȩ
        }
    }
    else if (result == 0x01)
    {
        //���Գɹ�
        // rssiΪ�ź�ǿ��(0-100, 0�ź���100�ź���ǿ)
    }
}
#endif

#ifdef VOICE_MODULE_PROTOCOL_ENABLE
/**
 * @brief  ��ȡ����״̬����
 * @param[in] {result} ����״̬��
 * @ref       0x00: ����
 * @ref       0x01: mic����״̬
 * @ref       0x02: ����
 * @ref       0x03: ����¼��
 * @ref       0x04: ����ʶ��
 * @ref       0x05: ʶ��ɹ�
 * @ref       0x06: ʶ��ʧ��
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void get_voice_state_result(unsigned char result)
{
#error "������ʵ�ֻ�ȡ����״̬�����������,��ɺ���ɾ������"
    switch (result)
    {
    case 0:
        //����
        break;

    case 1:
        // mic����״̬
        break;

    case 2:
        //����
        break;

    case 3:
        //����¼��
        break;

    case 4:
        //����ʶ��
        break;

    case 5:
        //ʶ��ɹ�
        break;

    case 6:
        //ʶ��ʧ��
        break;

    default:
        break;
    }
}

/**
 * @brief  MIC�������ý��
 * @param[in] {result} ����״̬��
 * @ref       0x00: mic ����
 * @ref       0x01: mic ����
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void set_voice_MIC_silence_result(unsigned char result)
{
#error "������ʵ��MIC�������ô������,��ɺ���ɾ������"
    if (result == 0)
    {
        // mic ����
    }
    else
    {
        // mic ����
    }
}

/**
 * @brief  speaker�������ý��
 * @param[in] {result} ����ֵ
 * @ref       0~10: ������Χ
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void set_speaker_voice_result(unsigned char result)
{
#error "������ʵ��speaker�������ý���������,��ɺ���ɾ������"
}

/**
 * @brief  ��Ƶ������
 * @param[in] {result} ��Ƶ����״̬
 * @ref       0x00: �ر���Ƶ����
 * @ref       0x01: mic1��Ƶ��·����
 * @ref       0x02: mic2��Ƶ��·����
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void voice_test_result(unsigned char result)
{
#error "������ʵ����Ƶ�������������,��ɺ���ɾ������"
    if (result == 0x00)
    {
        //�ر���Ƶ����
    }
    else if (result == 0x01)
    {
        // mic1��Ƶ��·����
    }
    else if (result == 0x02)
    {
        // mic2��Ƶ��·����
    }
}

/**
 * @brief  ���Ѳ�����
 * @param[in] {result} ���ѷ���ֵ
 * @ref       0x00: ���ѳɹ�
 * @ref       0x01: ����ʧ��(10s��ʱʧ��)
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void voice_awaken_test_result(unsigned char result)
{
#error "������ʵ�ֻ��Ѳ������������,��ɺ���ɾ������"
    if (result == 0x00)
    {
        //���ѳɹ�
    }
    else if (result == 0x01)
    {
        //����ʧ��
    }
}

/**
 * @brief  ����ģ����չ����
 * @param[in] {value} ���ݻ�����
 * @param[in] {length} ���ݳ���
 * @return Null
 * @note   MCU��Ҫ����ʵ�ָù���
 */
void voice_module_extend_fun(const unsigned char value[], unsigned short length)
{
    unsigned char sub_cmd = value[0];
    unsigned char play;
    unsigned char bt_play;
    unsigned short send_len = 0;

    switch (sub_cmd)
    {
    case 0x00:
    { //������  MCU��������
        if (0x02 != length)
        {
            //���ݳ��ȴ���
            return;
        }

        if (value[1] == 0)
        {
            //�ɹ�
        }
        else
        {
            //ʧ��
        }
    }
    break;

    case 0x01:
    { //������  ״̬֪ͨ
        if (0x02 > length)
        {
            //���ݳ��ȴ���
            return;
        }

        unsigned char play = 0xff;
        unsigned char bt_play = 0xff;

        const char *str_buff = (const char *)&value[1];
        const char *str_result = NULL;

        str_result = strstr(str_buff, "play") + my_strlen("play") + 2;
        if (NULL == str_result)
        {
            //���ݴ���
            goto ERR_EXTI;
        }

        if (0 == memcmp(str_result, "true", my_strlen("true")))
        {
            play = 1;
        }
        else if (0 == memcmp(str_result, "false", my_strlen("false")))
        {
            play = 0;
        }
        else
        {
            //���ݴ���
            goto ERR_EXTI;
        }

        str_result = strstr(str_buff, "bt_play") + my_strlen("bt_play") + 2;
        if (NULL == str_result)
        {
            //���ݴ���
            goto ERR_EXTI;
        }

        if (0 == memcmp(str_result, "true", my_strlen("true")))
        {
            bt_play = 1;
        }
        else if (0 == memcmp(str_result, "false", my_strlen("false")))
        {
            bt_play = 0;
        }
        else
        {
            //���ݴ���
            goto ERR_EXTI;
        }

#error "������ʵ������ģ��״̬֪ͨ�������,��ɺ���ɾ������"
        // MCU�����ݽ�֧�֡�����/��ͣ�� ���������ء�
        // play    ����/��ͣ����  1(����) / 0(��ͣ)
        // bt_play �������ع���   1(��)   / 0(��)

        send_len = 0;
        send_len = set_wifi_uart_byte(send_len, sub_cmd);
        send_len = set_wifi_uart_byte(send_len, 0x00);
        wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
    }
    break;

    default:
        break;
    }

    return;

ERR_EXTI:
    send_len = 0;
    send_len = set_wifi_uart_byte(send_len, sub_cmd);
    send_len = set_wifi_uart_byte(send_len, 0x01);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
    return;
}
#endif
