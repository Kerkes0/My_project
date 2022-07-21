#include "ErrorFunc.h"
#include "User.h"
#include "LCD.h"
#include "ModeFunc.h"
#include "Mode.h"
#include "Adc.h"
#include "Uart.h"

uchar gu8MotorTripBef = 0;

//----------------------------------������һ��ʱ��ʹ����ڵ������
uint32_t count_05h_error_clear_Inmotmisitem = 0;     //���ȱ��
uint32_t count_05h_error_clear_Insysdriverr = 0;     //��������
uint32_t count_05h_error_clear_Indccurrer = 0;       // DC��ѹ�쳣
uint32_t count_05h_error_clear_Incurroverh = 0;      //�������������ֵ
uint32_t count_05h_error_clear_Inpfcprot = 0;        // pfc����
uint32_t count_05h_error_clear_IpmTempoverh = 0;     //ɢ��������
uint32_t count_05h_error_clear_gu8SpmTempOVHCnt = 0; //�¶ȱ���ͣ��
uint32_t count_05h_error_clear_Inmotoverload = 0;    //������ʳ���
uint32_t count_05h_error_clear_Innowater = 0;        //���ձ���

uchar count_Inmotmisitem_tm = 0;  //���ȱ��������ɻָ�����Сʱ��3��������
uchar count_Insysdriverr_tm = 0;  //�������ϴ������ɻָ�����Сʱ��3��������
uchar count_Indccurrer_tm = 0;    // DC��ѹ�쳣�������ɻָ�����Сʱ��3��������
uchar count_Incurroverh_tm = 0;   //�������������ֵ�������ɻָ�����Сʱ��3��������
uchar count_Inpfcprot_tm = 0;     // pfc�����������ɻָ�����Сʱ��3��������
uchar count_IpmTempoverh_tm = 0;  //ɢ�������ȴ������ɻָ�����Сʱ��3��������
uchar gu8SpmTempOVHCnt = 0;       //�¶ȱ���ͣ���������ɻָ�����Сʱ��3��������
uchar count_Inmotoverload_tm = 0; //������ʳ��ش������ɻָ�����Сʱ��3��������
uchar count_Innowater_tm = 0;     //���ձ�������
//----------------------------------������һ��ʱ��ʹ����ڵ������

//����������
//�����10ms��ʱ����
void subMotorTrip(void)
{
    //-----------����ʧ�ܹ��ϻָ�
    if (ValBit(gu16TripFlg_Board, Owupwaterfail) // E209
        && count_upwater_iserror == 1             //��һ��ʧ��
        && gu16upwateriserrorTm == 0              // 2����ʱ��
    )
    {
        ClrBit(gu16TripFlg_Board, Owupwaterfail); //�������ʧ��E209
        u8WashState = 0;                           //���½���ˮ����
        FunModeToUpWash();                         //���½���ˮ����
    }

    //----------------------------------������һ��ʱ��ʹ����ڵ������
    if ((count_Inmotmisitem_tm > 0) && (count_Inmotmisitem_tm < 3)) //���ȱ��
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
    if ((count_Insysdriverr_tm > 0) && (count_Insysdriverr_tm < 3)) //��������
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
    if ((count_Indccurrer_tm > 0) && (count_Indccurrer_tm < 3)) // DC��ѹ�쳣
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
    if ((count_Incurroverh_tm > 0) && (count_Incurroverh_tm < 3)) //�������������ֵ
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
    if ((count_Inpfcprot_tm > 0) && (count_Inpfcprot_tm < 3)) // pfc����
    {
        count_05h_error_clear_Inpfcprot++;
        if (count_05h_error_clear_Inpfcprot > 180000)
        {
            count_05h_error_clear_Inpfcprot = 0;
            count_Inpfcprot_tm = 0; // pfc����
        }
    }
    else
    {
        count_05h_error_clear_Inpfcprot = 0;
    }

    //===========================================================================================
    if ((count_IpmTempoverh_tm > 0) && (count_IpmTempoverh_tm < 3)) //ɢ��������
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
    if ((count_Inmotoverload_tm > 0) && (count_Inmotoverload_tm < 3)) //������ʳ���
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
    if ((gu8SpmTempOVHCnt > 0) && (gu8SpmTempOVHCnt < 3)) //�¶�
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
    if ((count_Innowater_tm > 0) && (count_Innowater_tm < 2)) //���ձ���
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

    //�ػ��󣬿ɻָ����ϴ���ȫ�����
    if (gu8SetMode == 0)
    {
        count_Inmotmisitem_tm = 0;  //���ȱ�ౣ��
        count_Insysdriverr_tm = 0;  //��������
        count_Indccurrer_tm = 0;    // DC��ѹ�쳣
        count_Incurroverh_tm = 0;   //�������������ֵ
        count_Inpfcprot_tm = 0;     // pfc����
        count_IpmTempoverh_tm = 0;  //ɢ��������
        gu8SpmTempOVHCnt = 0;       //�¶ȱ���ͣ��
        count_Inmotoverload_tm = 0; //������ʳ���
        // count_Ownowaterpr_tm = 0;
        count_Innowater_tm = 0; //���ձ���
    }
    //----------------------------------������һ��ʱ��ʹ����ڵ������

    //------------------------------------------����ʱͻȻ�����ϣ�

    // yxl-5

    //��������ʱ���жϹ����Ƿ񻹴��ڣ�����������ϱ�׼���
    //��Ӧ��ȷ�����ϲ�����ʱ����Ӧ����Ч����һֱ����
    if (!bCompOnCmd)
    {
        if (gu8MotorTrip != 1) // �������������  ���ɻָ�
        {
        }

        if ((gu8MotorTrip != 2) && (gu8MotorTrip != 3) && (count_Inmotmisitem_tm < 3)) //���ȱ�ౣ�� 3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Inmotmisitem);
        }

        if ((gu8MotorTrip != 4) && (gu8MotorTrip != 11) && (gu8MotorTrip != 12) && (gu8MotorTrip != 22) && (count_Insysdriverr_tm < 3)) //������������3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Insysdriverr);
        }

        if (gu8MotorTrip != 5) // ������������������  ���ɻָ�
        {
        }

        if (gu8MotorTrip != 6) //  ����eep��д���� ���ɻָ�
        {
        }

        if ((gu8MotorTrip != 7) && (count_Indccurrer_tm < 3)) // DC��ѹ�쳣 3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Indccurrer);
        }

        if (gu8MotorTrip != 8) //  �����ѹ�쳣 �ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Involterror);
        }

        if ((gu8MotorTrip != 9) && (gu8MotorTrip != 10) && (count_Incurroverh_tm < 3)) //�������������ֵ  3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Incurroverh);
        }

        if ((gu8MotorTrip != 13) && (gu8MotorTrip != 14) && (gu8MotorTrip != 15) && (gu8MotorTrip != 16) && (count_Inpfcprot_tm < 3)) // pfc���� 3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Inpfcprot);
        }

        if (gu8MotorTrip != 17) //  ����������ͨ�Ź��� �ɻָ�
        {
            //	ClrBit(gu16TripFlg_Driver,Inuarterr );  //�������
            fg_slave_uart_error = 0;
        }

        if (gu8MotorTrip != 18) //  ɢ�������������� ���ɻָ�
        {
        }

        if ((gu8MotorTrip != 19) && (count_IpmTempoverh_tm < 3)) //ɢ�������� 3�β��ɻָ�
        {
            fg_slave_tempoverhigh = 0;
            // ClrBit(gu16TripFlg_Driver,IpmTempoverh ); //yxl-5��λ�������¶��Լ��ж�
        }

        if ((gu8MotorTrip != 20) && (count_Inmotoverload_tm < 3)) //������ʳ��� 3�β��ɻָ�
        {
            ClrBit(gu16TripFlg_Driver, Inmotoverload);
        }

        if ((gu8MotorTrip != 23) && (count_Innowater_tm < 2)) //  ���� 2�β��ɻָ�  yxl-e208
        {
            ClrBit(gu16TripFlg_Driver, Inlowflowerr);
        }
    }
    //��Ƶ���������Թ��Ϲ�����gu8MotorTripBuf -> gu8MotorTrip -> gu16TripFlg_Driver��ע�⣺gu8MotorTripBuf��gu8MotorTripBef��ͬ��
    //��ȷ�������Թ��� ���� ��ǰһ��ȷ�������Թ��� ʱ�������Թ�����Ч��һ�Ρ���gu16TripFlg_DriverΪ��Ч���Թ��ϣ�
    //ͬʱ���Ƿ�ɻָ��Ĵ����Լ�
    //��������������������if��a>1&&b=0��{??};  b=a;
    if ((gu8MotorTrip > 0) AND(gu8MotorTripBef == 0))
    {
        //---------------------------------------------yxl-5

        if (gu8MotorTrip == 1) // �������������  ���ɻָ�
        {
            SetBit(gu16TripFlg_Driver, Inmotcurrerr);
        }
        
        else if ((gu8MotorTrip == 2) || (gu8MotorTrip == 3)) //���ȱ�ౣ��  3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Inmotmisitem))
            {
                SetBit(gu16TripFlg_Driver, Inmotmisitem);
                count_Inmotmisitem_tm++;
            }
        }

        else if ((gu8MotorTrip == 4) || (gu8MotorTrip == 11) || (gu8MotorTrip == 12) || (gu8MotorTrip == 22)) //������������ 3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Insysdriverr))
            {
                SetBit(gu16TripFlg_Driver, Insysdriverr);
                count_Insysdriverr_tm++; //��������
            }
        }

        else if (gu8MotorTrip == 5) // ������������������  ���ɻָ�
        {
            SetBit(gu16TripFlg_Driver, Incurrerror);
        }

        else if (gu8MotorTrip == 6) //  ����eep��д���� ���ɻָ�
        {
            SetBit(gu16TripFlg_Driver, Insyseepeer);
        }

        else if (gu8MotorTrip == 7) // DC��ѹ�쳣 3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Indccurrer))
            {
                SetBit(gu16TripFlg_Driver, Indccurrer);
                count_Indccurrer_tm++; // DC��ѹ�쳣
            }
        }

        else if (gu8MotorTrip == 8) //  a�����ѹ�쳣 �ɻָ�
        {
            SetBit(gu16TripFlg_Driver, Involterror);
        }

        else if ((gu8MotorTrip == 9) || (gu8MotorTrip == 10)) //�������������ֵ  3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Incurroverh))
            {
                SetBit(gu16TripFlg_Driver, Incurroverh);
                count_Incurroverh_tm++; //�������������ֵ
            }
        }

        else if ((gu8MotorTrip == 13) || (gu8MotorTrip == 14) || (gu8MotorTrip == 15) || (gu8MotorTrip == 16)) // pfc���� 3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Inpfcprot))
            {
                SetBit(gu16TripFlg_Driver, Inpfcprot);
                count_Inpfcprot_tm++; // pfc����
            }
        }

        else if (gu8MotorTrip == 17) //  ����������ͨ�Ź��� �ɻָ�
        {
            // SetBit(gu16TripFlg_Driver,Inuarterr ); //�������
            fg_slave_uart_error = 1;
        }

        else if (gu8MotorTrip == 18) //  ɢ�������������� ���ɻָ�
        {
            SetBit(gu16TripFlg_Driver, Ipmnotemp);
        }

        else if (gu8MotorTrip == 19) //ɢ�������� 3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, IpmTempoverh))
            {
                // SetBit(gu16TripFlg_Driver,IpmTempoverh ); //yxl-5��λ�������¶��Լ��ж�
                fg_slave_tempoverhigh = 1;
                count_IpmTempoverh_tm++; //ɢ��������
            }
        }

        else if (gu8MotorTrip == 20) //������ʳ��� 3�β��ɻָ�
        {
            if (!ValBit(gu16TripFlg_Driver, Inmotoverload))
            {
                SetBit(gu16TripFlg_Driver, Inmotoverload);
                count_Inmotoverload_tm++; //������ʳ���
            }
        }
        else if (gu8MotorTrip == 23) //���ձ���//yxl-e208
        {
            if (!ValBit(gu16TripFlg_Driver, Inlowflowerr))
            {
                SetBit(gu16TripFlg_Driver, Inlowflowerr);
                count_Innowater_tm++; //���ձ���
            }
        }
    }

    ///////////////////////////////////////yxl-5

    //�ӻ�ɢ����������ɢ��������ʱ�����Թ���
    if ((fg_slave_tempoverhigh == 1) || (fg_host_tempoverhigh == 1))
    {
        SetBit(gu16TripFlg_Driver, IpmTempoverh);
    }
    else
    {
        ClrBit(gu16TripFlg_Driver, IpmTempoverh);
    }

    ///////////////////////////////////////yxl-5



    gu8MotorTripBef = gu8MotorTrip; //ǰһ�ε�ȷ�����Թ��� = ȷ�����Թ��ϣ�������if��a>0&&b=0��{??};  b=a; _|-
}

//ϵͳ���ϼ���
void FunErrorRemeber(void)
{ //------------------------------------------����ʱͻȻ�����ϣ��������д洢
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

