#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_dma.h"
#include "function.h"
#include "IfControl.h"
#include "CurrentLoop.h"
#include "SVPWM.h"
#include "ADC_Calc.h"
#include "ADC_Init.h"
#include "Timer_Init.h"
#include "GPIO_Init.h"
#include "OPA_Init.h"
#include "User_PeripheralInit.h"
#define START_MOTOR_COMMAND (GPIOF->IDR_b.IDR8)
#define STOP_MOTOR_COMMAND  (GPIOC->IDR_b.IDR13)
int main(void)
{
    SystemInit();
    SetSysClock();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ANACTRL, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC, ENABLE);
    SysTick_Config(UPDS_SYS_TICK_PERIOD);
    GPIO_Config();
    DAC_Config();
    OPA_Config();
    ADC_Config();
    ACMP_Config();
    Driver_Para_Init();
    Invter_Init();
    TIM1_Config();
    PWMOutputs_DISABLE();
    Update_PWM();
    TIM14_Config();
    NVIC_Config();
    while (1)
    {
        switch (SysPara.StateMachineFlag)
        {
            case MotorStandBy:
                PWMOutputs_DISABLE();
                GPIO_SetBits(GPIOA, GPIO_Pin_12);
                if ((START_MOTOR_COMMAND != 0) && (ADC_OffsetReady != 0))
                {
                    IfPrePosPrepare();
                    SysPara.StateMachineFlag = MotorPrePos;
                    PWMOutputs_ENABLE();
                    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
                }
                break;
            case MotorPrePos:
            case IfStartMode:
                if (STOP_MOTOR_COMMAND != 0)
                {
                    PWMOutputs_DISABLE();
                    SysPara.StateMachineFlag = MotorStandBy;
                    IfInit();
                    Inside_Loop_Init();
                }
                break;
            case SysFault:
            default:
                PWMOutputs_DISABLE();
                GPIO_ResetBits(GPIOA, GPIO_Pin_4);
                break;
        }
    }
}
