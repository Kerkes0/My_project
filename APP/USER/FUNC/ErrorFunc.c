#include "ErrorFunc.h"
#include "User.h"
#include "LCD.h"
#include "ModeFunc.h"
#include "Mode.h"
#include "Adc.h"
#include "Uart.h"

uchar gu8MotorTripBef = 0;

//----------------------------------故障在一定时间和次数内单独清除
uint32_t count_05h_error_clear_Inmotmisitem = 0;     //电机缺相
uint32_t count_05h_error_clear_Insysdriverr = 0;     //驱动故障
uint32_t count_05h_error_clear_Indccurrer = 0;       // DC电压异常
uint32_t count_05h_error_clear_Incurroverh = 0;      //输入电流超过限值
uint32_t count_05h_error_clear_Inpfcprot = 0;        // pfc保护
uint32_t count_05h_error_clear_IpmTempoverh = 0;     //散热器过热
uint32_t count_05h_error_clear_gu8SpmTempOVHCnt = 0; //温度报警停机
uint32_t count_05h_error_clear_Inmotoverload = 0;    //电机功率超载
uint32_t count_05h_error_clear_Innowater = 0;        //干烧保护

uchar count_Inmotmisitem_tm = 0;  //电机缺相次数，可恢复，半小时内3次则锁定
uchar count_Insysdriverr_tm = 0;  //驱动故障次数，可恢复，半小时内3次则锁定
uchar count_Indccurrer_tm = 0;    // DC电压异常次数，可恢复，半小时内3次则锁定
uchar count_Incurroverh_tm = 0;   //输入电流超过限值次数，可恢复，半小时内3次则锁定
uchar count_Inpfcprot_tm = 0;     // pfc保护次数，可恢复，半小时内3次则锁定
uchar count_IpmTempoverh_tm = 0;  //散热器过热次数，可恢复，半小时内3次则锁定
uchar gu8SpmTempOVHCnt = 0;       //温度报警停机次数，可恢复，半小时内3次则锁定
uchar count_Inmotoverload_tm = 0; //电机功率超载次数，可恢复，半小时内3次则锁定
uchar count_Innowater_tm = 0;     //干烧保护次数
//----------------------------------故障在一定时间和次数内单独清除

//故障清除相关
//需放在10ms的时基中
void subMotorTrip(void)
{
    //-----------自吸失败故障恢复
    if (ValBit(gu16TripFlg_Board, Owupwaterfail) // E209
        && count_upwater_iserror == 1             //第一次失败
        && gu16upwateriserrorTm == 0              // 2分钟时间
    )
    {
        ClrBit(gu16TripFlg_Board, Owupwaterfail); //清除自吸失败E209
        u8WashState = 0;                           //重新进上水自吸
        FunModeToUpWash();                         //重新进上水自吸
    }

    //----------------------------------故障在一定时间和次数内单独清除
    if ((count_Inmotmisitem_tm > 0) && (count_Inmotmisitem_tm < 3)) //电机缺相
    {
        count_05h_error_clear_Inmotmisitem++;
        if (count_05h_error_clear_Inmotmisitem > 180000) // 0.01s*180000=1800s=30min
        {
            count_05h_error_clear_Inmotmisitem = 0;
            count_Inmotmisitem_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Inmotmisitem = 0;
    }
    //==================================================================================
    if ((count_Insysdriverr_tm > 0) && (count_Insysdriverr_tm < 3)) //驱动故障
    {
        count_05h_error_clear_Insysdriverr++;
        if (count_05h_error_clear_Insysdriverr > 180000)
        {
            count_05h_error_clear_Insysdriverr = 0;
            count_Insysdriverr_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Insysdriverr = 0;
    }
    //======================================================================================
    if ((count_Indccurrer_tm > 0) && (count_Indccurrer_tm < 3)) // DC电压异常
    {
        count_05h_error_clear_Indccurrer++;
        if (count_05h_error_clear_Indccurrer > 180000)
        {
            count_05h_error_clear_Indccurrer = 0;
            count_Indccurrer_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Indccurrer = 0;
    }
    //====================================================================================
    if ((count_Incurroverh_tm > 0) && (count_Incurroverh_tm < 3)) //输入电流超过限值
    {
        count_05h_error_clear_Incurroverh++;
        if (count_05h_error_clear_Incurroverh > 180000)
        {
            count_05h_error_clear_Incurroverh = 0;
            count_Incurroverh_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Incurroverh = 0;
    }
    //==========================================================================================
    if ((count_Inpfcprot_tm > 0) && (count_Inpfcprot_tm < 3)) // pfc保护
    {
        count_05h_error_clear_Inpfcprot++;
        if (count_05h_error_clear_Inpfcprot > 180000)
        {
            count_05h_error_clear_Inpfcprot = 0;
            count_Inpfcprot_tm = 0; // pfc保护
        }
    }
    else
    {
        count_05h_error_clear_Inpfcprot = 0;
    }

    //===========================================================================================
    if ((count_IpmTempoverh_tm > 0) && (count_IpmTempoverh_tm < 3)) //散热器过热
    {
        count_05h_error_clear_IpmTempoverh++;
        if (count_05h_error_clear_IpmTempoverh > 180000)
        {
            count_05h_error_clear_IpmTempoverh = 0;
            count_IpmTempoverh_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_IpmTempoverh = 0;
    }
    //=================================================================================================
    if ((count_Inmotoverload_tm > 0) && (count_Inmotoverload_tm < 3)) //电机功率超载
    {
        count_05h_error_clear_Inmotoverload++;
        if (count_05h_error_clear_Inmotoverload > 180000)
        {
            count_05h_error_clear_Inmotoverload = 0;
            count_Inmotoverload_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Inmotoverload = 0;
    }
    //============================================================================
    if ((gu8SpmTempOVHCnt > 0) && (gu8SpmTempOVHCnt < 3)) //温度
    {
        count_05h_error_clear_gu8SpmTempOVHCnt++;
        if (count_05h_error_clear_gu8SpmTempOVHCnt > 180000)
        {
            count_05h_error_clear_gu8SpmTempOVHCnt = 0;
            gu8SpmTempOVHCnt = 0;
        }
    }
    else
    {
        count_05h_error_clear_gu8SpmTempOVHCnt = 0;
    }
    //==============================================================================
    if ((count_Innowater_tm > 0) && (count_Innowater_tm < 2)) //干烧保护
    {
        count_05h_error_clear_Innowater++;
        if (count_05h_error_clear_Innowater > 180000)
        {
            count_05h_error_clear_Innowater = 0;
            count_Innowater_tm = 0;
        }
    }
    else
    {
        count_05h_error_clear_Innowater = 0;
    }

    //关机后，可恢复故障次数全部清除
    if (gu8SetMode == 0)
    {
        count_Inmotmisitem_tm = 0;  //电机缺相保护
        count_Insysdriverr_tm = 0;  //驱动故障
        count_Indccurrer_tm = 0;    // DC电压异常
        count_Incurroverh_tm = 0;   //输入电流超过限值
        count_Inpfcprot_tm = 0;     // pfc保护
        count_IpmTempoverh_tm = 0;  //散热器过热
        gu8SpmTempOVHCnt = 0;       //温度报警停机
        count_Inmotoverload_tm = 0; //电机功率超载
        // count_Ownowaterpr_tm = 0;
        count_Innowater_tm = 0; //干烧保护
    }
    //----------------------------------故障在一定时间和次数内单独清除

    //------------------------------------------正常时突然报故障，

    // yxl-5

    //不可运行时，判断故障是否还存在，不存在则故障标准清除
    //相应的确定故障不存在时，对应的生效故障一直清零
    if (!bCompOnCmd)
    {
        if (gu8MotorTrip != 1) // 电机电流检测故障  不可恢复
        {
        }

        if ((gu8MotorTrip != 2) && (gu8MotorTrip != 3) && (count_Inmotmisitem_tm < 3)) //电机缺相保护 3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Inmotmisitem);
        }

        if ((gu8MotorTrip != 4) && (gu8MotorTrip != 11) && (gu8MotorTrip != 12) && (gu8MotorTrip != 22) && (count_Insysdriverr_tm < 3)) //主控驱动故障3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Insysdriverr);
        }

        if (gu8MotorTrip != 5) // 交流电流采样检测故障  不可恢复
        {
        }

        if (gu8MotorTrip != 6) //  主控eep读写故障 不可恢复
        {
        }

        if ((gu8MotorTrip != 7) && (count_Indccurrer_tm < 3)) // DC电压异常 3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Indccurrer);
        }

        if (gu8MotorTrip != 8) //  输入电压异常 可恢复
        {
            ClrBit(gu16TripFlg_Driver, Involterror);
        }

        if ((gu8MotorTrip != 9) && (gu8MotorTrip != 10) && (count_Incurroverh_tm < 3)) //输入电流超过限值  3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Incurroverh);
        }

        if ((gu8MotorTrip != 13) && (gu8MotorTrip != 14) && (gu8MotorTrip != 15) && (gu8MotorTrip != 16) && (count_Inpfcprot_tm < 3)) // pfc保护 3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Inpfcprot);
        }

        if (gu8MotorTrip != 17) //  键盘与主控通信故障 可恢复
        {
            //	ClrBit(gu16TripFlg_Driver,Inuarterr );  //上面计算
            fg_slave_uart_error = 0;
        }

        if (gu8MotorTrip != 18) //  散热器传感器故障 不可恢复
        {
        }

        if ((gu8MotorTrip != 19) && (count_IpmTempoverh_tm < 3)) //散热器过热 3次不可恢复
        {
            fg_slave_tempoverhigh = 0;
            // ClrBit(gu16TripFlg_Driver,IpmTempoverh ); //yxl-5上位机根据温度自己判断
        }

        if ((gu8MotorTrip != 20) && (count_Inmotoverload_tm < 3)) //电机功率超载 3次不可恢复
        {
            ClrBit(gu16TripFlg_Driver, Inmotoverload);
        }

        if ((gu8MotorTrip != 23) && (count_Innowater_tm < 2)) //  干烧 2次不可恢复  yxl-e208
        {
            ClrBit(gu16TripFlg_Driver, Inlowflowerr);
        }
    }
    //变频器发送线性故障过来，gu8MotorTripBuf -> gu8MotorTrip -> gu16TripFlg_Driver（注意：gu8MotorTripBuf和gu8MotorTripBef不同）
    //当确定有线性故障 并且 当前一次确定无线性故障 时，即线性故障生效了一次。（gu16TripFlg_Driver为生效线性故障）
    //同时有是否可恢复的次数自加
    //与下面代码关联，类似与if（a>1&&b=0）{??};  b=a;
    if ((gu8MotorTrip > 0) AND(gu8MotorTripBef == 0))
    {
        //---------------------------------------------yxl-5

        if (gu8MotorTrip == 1) // 电机电流检测故障  不可恢复
        {
            SetBit(gu16TripFlg_Driver, Inmotcurrerr);
        }
        
        else if ((gu8MotorTrip == 2) || (gu8MotorTrip == 3)) //电机缺相保护  3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Inmotmisitem))
            {
                SetBit(gu16TripFlg_Driver, Inmotmisitem);
                count_Inmotmisitem_tm++;
            }
        }

        else if ((gu8MotorTrip == 4) || (gu8MotorTrip == 11) || (gu8MotorTrip == 12) || (gu8MotorTrip == 22)) //主控驱动故障 3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Insysdriverr))
            {
                SetBit(gu16TripFlg_Driver, Insysdriverr);
                count_Insysdriverr_tm++; //驱动故障
            }
        }

        else if (gu8MotorTrip == 5) // 交流电流采样检测故障  不可恢复
        {
            SetBit(gu16TripFlg_Driver, Incurrerror);
        }

        else if (gu8MotorTrip == 6) //  主控eep读写故障 不可恢复
        {
            SetBit(gu16TripFlg_Driver, Insyseepeer);
        }

        else if (gu8MotorTrip == 7) // DC电压异常 3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Indccurrer))
            {
                SetBit(gu16TripFlg_Driver, Indccurrer);
                count_Indccurrer_tm++; // DC电压异常
            }
        }

        else if (gu8MotorTrip == 8) //  a输入电压异常 可恢复
        {
            SetBit(gu16TripFlg_Driver, Involterror);
        }

        else if ((gu8MotorTrip == 9) || (gu8MotorTrip == 10)) //输入电流超过限值  3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Incurroverh))
            {
                SetBit(gu16TripFlg_Driver, Incurroverh);
                count_Incurroverh_tm++; //输入电流超过限值
            }
        }

        else if ((gu8MotorTrip == 13) || (gu8MotorTrip == 14) || (gu8MotorTrip == 15) || (gu8MotorTrip == 16)) // pfc保护 3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Inpfcprot))
            {
                SetBit(gu16TripFlg_Driver, Inpfcprot);
                count_Inpfcprot_tm++; // pfc保护
            }
        }

        else if (gu8MotorTrip == 17) //  键盘与主机通信故障 可恢复
        {
            // SetBit(gu16TripFlg_Driver,Inuarterr ); //上面计算
            fg_slave_uart_error = 1;
        }

        else if (gu8MotorTrip == 18) //  散热器传感器故障 不可恢复
        {
            SetBit(gu16TripFlg_Driver, Ipmnotemp);
        }

        else if (gu8MotorTrip == 19) //散热器过热 3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, IpmTempoverh))
            {
                // SetBit(gu16TripFlg_Driver,IpmTempoverh ); //yxl-5上位机根据温度自己判断
                fg_slave_tempoverhigh = 1;
                count_IpmTempoverh_tm++; //散热器过热
            }
        }

        else if (gu8MotorTrip == 20) //电机功率超载 3次不可恢复
        {
            if (!ValBit(gu16TripFlg_Driver, Inmotoverload))
            {
                SetBit(gu16TripFlg_Driver, Inmotoverload);
                count_Inmotoverload_tm++; //电机功率超载
            }
        }
        else if (gu8MotorTrip == 23) //干烧保护//yxl-e208
        {
            if (!ValBit(gu16TripFlg_Driver, Inlowflowerr))
            {
                SetBit(gu16TripFlg_Driver, Inlowflowerr);
                count_Innowater_tm++; //干烧保护
            }
        }
    }

    ///////////////////////////////////////yxl-5

    //从机散热器或主机散热器过热时报线性故障
    if ((fg_slave_tempoverhigh == 1) || (fg_host_tempoverhigh == 1))
    {
        SetBit(gu16TripFlg_Driver, IpmTempoverh);
    }
    else
    {
        ClrBit(gu16TripFlg_Driver, IpmTempoverh);
    }

    ///////////////////////////////////////yxl-5



    gu8MotorTripBef = gu8MotorTrip; //前一次的确定线性故障 = 确定线性故障，类似与if（a>0&&b=0）{??};  b=a; _|-
}

//系统故障记忆
void FunErrorRemeber(void)
{ //------------------------------------------正常时突然报故障，立即进行存储
    if ((gu16TripFlg_Board_Bef == 0) AND(gu16TripFlg_Board > 0))
    {
        SetBit(gu8SetFlg, bTripSave);
    }
    if ((gu16TripFlg_Driver_bef == 0) AND(gu16TripFlg_Driver > 0)) // yxl-5
    {
        SetBit(gu8SetFlg, bTripSave);
    }
    if ((gu16TripFlg_Warn_bef == 0) AND(gu16TripFlg_Warn > 0)) // yxl-5
    {
        SetBit(gu8SetFlg, bTripSave);
    }
    gu16TripFlg_Board_Bef = gu16TripFlg_Board;
    gu16TripFlg_Driver_bef = gu16TripFlg_Driver;
    gu16TripFlg_Warn_bef = gu16TripFlg_Warn;
}

