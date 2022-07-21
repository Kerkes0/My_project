#include "User.h"
#include <string.h>
#include <stdio.h>

typedef struct PID
{

    double SetPoint; //  �趨Ŀ�� Desired Value

    double Proportion; //  �������� Proportional Const
    double Integral;   //  ���ֳ��� Integral Const
    double Derivative; //  ΢�ֳ��� Derivative Const

    double LastError; //  Error[-1]����һ����
    double PrevError; //  Error[-2]�����ϴ���
    double SumError;  //  Sums of Errors

} PID;

// PID���㲿��

double PIDCalc(PID *pp, double NextPoint)
{
    double dError, Error;

    Error = pp->SetPoint - NextPoint;       // ƫ��
    pp->SumError += Error;                  // ����
    dError = pp->LastError - pp->PrevError; // ��ǰ΢��
    pp->PrevError = pp->LastError;          //����һ���������ϴ����
    pp->LastError = Error;                  //��ǰ������һ�����
    return (pp->Proportion * Error          // ������
            + pp->Integral * pp->SumError   // ������
            + pp->Derivative * dError       // ΢����
    );
}

// Initialize PID Structure

void PIDInit(PID *pp)
{
    memset(pp, 0, sizeof(PID));
}

// Main Program

double sensor(void) //  Dummy Sensor Function
{
    return 100.0;

}

void actuator(double rDelta) //  Dummy Actuator Function
{
}

void pid(void)
{
    PID sPID;    //  PID Control Structure
    double rOut; //  PID Response (Output)
    double rIn;  //  PID Feedback (Input)

    PIDInit(&sPID);        //  Initialize Structure
    sPID.Proportion = 0.5; // Set PID Coefficients��KP,KI,KDϵ���趨��
    sPID.Integral = 0.5;
    sPID.Derivative = 0.0;
    // sPID.SetPoint   = 100.0;           //  Set PID Setpoint

    sPID.SetPoint = WaterGate_set * 100;

    // for (;;) {                         //  Mock Up of PID Processing(PID����ʾ��)

    rIn = sensor();             //  Read Input
    rOut = PIDCalc(&sPID, rIn); //  Perform PID Integration
    actuator(rOut);             //  Effect Needed Changes
                                //  }
}
