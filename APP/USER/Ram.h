/******************************************************
Ram variable
*******************************************************/
#ifndef _Ram_H_
#define _Ram_H_

extern uchar gu8MotorComTripTm;
extern uchar sndBuffer[7];
extern uchar rcvBuffer[18];
extern uchar RxDataLen;
extern uchar commCount;
extern uchar MotorComStatus;
extern uchar gu8MotorComFlg;

extern uchar gu8MotorComTm;
extern uint gu16MotorRunSpd;
extern uint gu16AcCurrent;
extern uint gu16DcVolt;
extern uint gu16PhaseCurrent;
extern uchar gu8MotorTrip;

extern uint gu16AcVolt;
// extern uchar gu8AcVoltHiTripLFT;
// extern uchar gu8AcVoltLoTripLFT;
// extern uchar gu8AcCurrHiTripLFT;
extern uchar gu8SpmTempDwTm;
extern uchar gu8MotorStopTm;
extern uchar gu8SetTimeTm;
extern uchar gu8BakLgtDlyTm;
extern uchar gu8SetTimeQueryTm;

/// extern uchar gu8E2Para[51];//yyxxll

extern uchar gu8E2Para[200]; // yyxxll

extern uchar gu8DsipNum[17];

extern uchar gu8TimeFlg;

extern uint gu16TripFlg_Board; //驱动故障
extern uint gu16TripFlg_Board_Bef;
extern uint gu16TripFlg_Driver; //显示板故障
extern uint gu16TripFlg_Driver_bef;
extern uint gu16TripFlg_Warn; //降速故障
extern uint gu16TripFlg_Warn_bef;


extern uchar gu8RTCInitCnt;
extern uchar gu8RTCWaitTm;
extern uchar gu8SetTimeFlashCnt;

extern uchar bCompOnCmd;
extern uchar gu8SetMode;
extern uchar gu8SetTimeTmNum;
extern uint gu16MotorSetSpd;

extern sint gu16SetNowTm;
extern uint gs16RunNowTm;
extern sint SetStartTm[4];
extern sint SetEndTm[4];
extern sint gs16RunStartTm[4];
extern sint gs16RunEndTm[4];

extern uchar u8ModeSpeed_SpeedSet;
extern u16 u16ModeGate_GateSet;

extern u8 fg_ModeUnit_IsChange; //流量单位是否切换了
extern uchar gu8SetTimeState;
extern uchar gu8SetTimeTmSn;

extern uint gu16RunSpd;
extern uint gu16MotorWt;
extern uchar u8SD_SoftWareVerTm;
extern uchar gu8TimeRunNum;
extern uchar gu8SetTimeQuerySn;
// extern uchar gu8SetModeE2;

extern u8 gu8KeyFlg; // YXL-5

extern u16 gu8SetTimeFlg;
extern uchar u8ModeTimeRunSpeed;
extern uint u8ModeTimeRunGate; // YXL-5
// extern uchar gu8TimeSetSpdGrade;
extern uint16_t gu8InCtrlFlg; // yxl-5

extern uchar gu8BuzzDlyTm;
extern uchar gu8SpmTempAvg;
extern uchar gu8LcdInitTm;
extern uchar gu8TripQueryTm;

extern uchar gu8SpmTempOVHCnt; //温度报警停机次数
extern uchar u8E2RdWaitTm;
extern uint16_t gu16KeyPressedTm;
extern uint16_t gu16KeyPressedTm_wifi_ap; // yxl-wifi//20211222
// extern uint  gu16NoKeyPressedTm;
// extern uchar gu8MonitorComTm;
extern uchar u8DinLFTCnt[4];
extern uchar gu8LockKeyTm;
// extern uchar gu8SpdToMotorTm;

extern uchar gu8AcVoltLmtTm;
extern uchar gu8SetFlg;
extern _stCodeDate gstCodeTime;
extern uchar u8SysTestTm;
extern uint gu16UartRunSpd;
extern uchar gu8PowerOnTm;
// extern uchar gu8TestModeBuzz;


extern uint gu16RomMinSpd;

extern uint gu16UartSpd;
extern uchar gu8MonitorTxdTm;
extern uchar gu8MonitComErrTm;
extern uchar gu8SetTimeLcdTm;
extern uchar KeyLed1;
extern uchar KeyLed2;
extern uchar KeyLed3;
extern uchar KeyLed4;
extern uchar KeyLed5;
extern uchar KeyLed6;

extern u32 u16LadderSpd_rang;
extern uint u16LadderSpd[]; // yyxxll

extern uchar gu8SetWastTm;    // yyxxll




extern uchar u8FirstFunIn; /// yyxxll
// extern uchar gs8PowerWashOTnTm;///yyxxll

extern uchar gMonitor1BtyeTm; // yyxxll
extern uchar fg_hand_close;   // yyxxll4  手动关机不能485开机

extern uint32_t WaterGate; // YXL-5
extern uint WaterGate_set; // YXL-5
extern int32_t Temp_Value;

extern uint16_t temp_cail_add;    // yxl-5
extern uint16_t temp_cail_subtra; // yxl-5

extern uchar fg_1ms_ad;           // YXL-5
extern uchar fg_light_pwm;        // YXL-5
extern uchar fg_time_spd_contorl; // yxl-5

extern unsigned char switch_key; // yxl-5
extern unsigned char switch_din; // 20211108    yxl-5
extern uint32_t u16Analog_AD;    // yxl-5


extern uchar water_max_set;    // yxl-5
extern uchar water_min_set;    // yxl-5

extern uchar Modcode[4]; // yxl-5

extern uchar fg_ageing_ok;
extern const uchar u8SpmTempTab[]; // yxl-5

extern uchar fg_host_uart_error;
extern uchar fg_slave_uart_error;

extern uchar fg_slave_tempoverhigh; // yxl-5
extern uchar fg_host_tempoverhigh;  // yxl-5

extern uchar water_set_range_max;
extern uchar water_set_range_min;
extern uint16_t WaterGate_cail_ot_preset;

extern uchar fg_air_cali;

extern uint count_05h_error_clear;





extern uint16_t Temp_Value_curr_zf10_test; //自检时，模拟控制的ad值
extern uint16_t Temp_value_test;           //自检时，压力传感器的ad值

extern uchar count_5s_dis_realwater; // yxl-5

// extern uchar  WaterPress_value_con[45];
// extern uchar fg_waterPress_value_con[45];


extern uchar fg_light_onts;

extern uchar gu8SetMode_eep; // yxl-5

extern uchar sp_2600_con_press[];
extern uint16_t sp_2600_con_flow[];

extern uchar fg_selfimbi_ok; // yxl-bash //20211210
// extern uint WashTmSet_selfimbi_buf;//yxl-bash //20211210

extern uchar fg_wifi_ap_mode;
extern uchar fg_wifi_smart_mode;
extern uchar fg_wifi_led;
extern uint16_t count_250ms_wifi_shine;
extern uint16_t count_1500ms_wifi_shine;

/////////////////wifi_and_单位切换//////////////////

extern uint32_t wifi_error_fault;              //当前故障
extern uint32_t wifi_gu16MotorWt_value;        //当前当前功率
extern uint32_t wifi_water_setrange_max_value; //当前流量设定最大值
extern uint32_t wifi_gu16RunSpd_real_value;    //当前实时运行速率
extern uint8_t wifi_gu8SetMode_enum;           //当前模式
extern uint32_t wifi_WashTmSet_value;          //当前反冲洗设置时间
extern uint8_t wifi_power_switch_bool;         //当前开关机
extern uint32_t wifi_WaterGate_set_value;      //当前流量设定值
extern uint32_t wifi_water_setrange_min_value; //当前流量设定最小值
extern uint32_t wifi_WashTm_value;             //当前反冲洗倒计时
extern uint32_t wifi_kilowatt_hour_value;      //当前耗电量
extern uint8_t wifi_water_uint_switch_enum;    //当前单位切换
extern uint32_t wifi_gu16RunSpd_set_value;     //当前速率设定值
extern uint32_t wifi_WaterGate_value;          //当前实时水流量
extern uint32_t wifi_WaterGate_set_step_value; //当前流量步进值
extern uint8_t wifi_flow_press_warn_bool;      //水压告警

//
extern uchar fg_dp_download_mode;
extern uchar fg_dp_download_backwash_time_set;
extern uchar fg_dp_download_swtich;
extern uchar fg_dp_download_uint_switch;
extern uchar fg_dp_download_flow_rate_set;
extern uchar fg_dp_download_speed_set;

extern uint16_t count_1min_kilowatt_hour;
extern uchar WifiTestOk;
extern uchar water_uint_switch;
extern uchar fg_20ms_delay_ask;
//
//
//
//





//
extern uchar count2s_wifi_gu16MotorWt_up;
/////////////////wifi_and_单位切换//////////////////

extern uchar fg_flash_rest_con; // 20220105
extern uchar fg_rest_con_wifi;  // 20220105

//////////20211206
// extern  uchar fg_gu16RunSpd_nozero;
// extern uint gu16RunSpd_nozero_buf;
//////////20211206

void DelayUs(uint u16Tm);
// extern void InitTm1729(void);
void FunInOut(void);
void FunRTC(void);
void FunMotorCom(void);

void FunWriteRam_1729(void);

// extern void FunOutput(void);
void FunE2_SysPro(void);
void FunTimeCtrl(void);
void FunMonitorCom(void);
// void IrqMonitorCom(void);
void subBuzCtrl(uchar i);

void FunMcuInit(void);

void pid(void);

void IWDG_Config(void);
void WWDG_Config(void);

#endif
