#include "User.h"
#include <string.h>
#include <stdio.h>

typedef struct PID
{

    double SetPoint; //  设定目标 Desired Value

    double Proportion; //  比例常数 Proportional Const
    double Integral;   //  积分常数 Integral Const
    double Derivative; //  微分常数 Derivative Const

    double LastError; //  Error[-1]（上一次误差）
    double PrevError; //  Error[-2]（上上次误差）
    double SumError;  //  Sums of Errors

} PID;

// PID计算部分

double PIDCalc(PID *pp, double NextPoint)
{
    double dError, Error;

    Error = pp->SetPoint - NextPoint;       // 偏差
    pp->SumError += Error;                  // 积分
    dError = pp->LastError - pp->PrevError; // 当前微分
    pp->PrevError = pp->LastError;          //把上一次误差赋给上上次误差
    pp->LastError = Error;                  //当前误差赋给上一次误差
    return (pp->Proportion * Error          // 比例项
            + pp->Integral * pp->SumError   // 积分项
            + pp->Derivative * dError       // 微分项
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
    sPID.Proportion = 0.5; // Set PID Coefficients（KP,KI,KD系数设定）
    sPID.Integral = 0.5;
    sPID.Derivative = 0.0;
    // sPID.SetPoint   = 100.0;           //  Set PID Setpoint

    sPID.SetPoint = WaterGate_set * 100;

    // for (;;) {                         //  Mock Up of PID Processing(PID处理示例)

    rIn = sensor();             //  Read Input
    rOut = PIDCalc(&sPID, rIn); //  Perform PID Integration
    actuator(rOut);             //  Effect Needed Changes
                                //  }
}
