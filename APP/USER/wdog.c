#include "User.h"

#define KR_KEY_RELOAD ((uint16_t)0xAAAA)
#define IWDG_Prescaler_32 ((uint8_t)0x03)
#define KR_KEY_ENABLE ((uint16_t)0xCCCC)

void IWDG_ReloadCounter(void)
{
  IWDG->KR = KR_KEY_RELOAD;
}

void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
  IWDG->KR = IWDG_WriteAccess;
}

void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
  IWDG->PR = IWDG_Prescaler;
}

void IWDG_SetReload(uint16_t Reload)
{
  /* Check the parameters */
  assert_param(IS_IWDG_RELOAD(Reload));
  IWDG->RLR = Reload;
}

void IWDG_Enable(void)
{
  IWDG->KR = KR_KEY_ENABLE;
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void IWDG_Config(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对寄存器IWDG_PR、IWDG_RLR的写操作
  IWDG_SetPrescaler(IWDG_Prescaler_32);         //设置IWDG预分频值：256分频最大
  IWDG_SetReload(40000 / 16);                   //设置IWDG的重装载值	:范围0~0x0FFF
  IWDG_ReloadCounter();                         //喂狗：重新装载计数器
  IWDG_Enable();                                //使能IWDG定时器
}

void WWDG_Config(void)
{

  IWDG_ReloadCounter();
}
