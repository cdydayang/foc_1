#include "User_Interrupt.h"
#include "lcm32f039_tim.h"
#include "function.h"
#include "IfControl.h"
#include "SVPWM.h"

void TIM14_Handler(void)
{
    static u16 slow_tick = 0;

    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
    slow_tick++;

    /* TIM14 is about 500 Hz with the reference clock settings. */
    if (SysPara.StateMachineFlag == IfStartMode)
        IfSpeedCalc();

    (void)slow_tick;
}

void TIM1_CC_Handler(void)
{
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC5);
}

void TIM1_NON_CC_Handler(void)
{
    TIM_ClearITPendingBit(TIM1, TIM1_NON_CC_IRQn);

    /* Hardware comparator break: force outputs inactive and latch the fault. */
    if (TIM1->SR_b.B2IF == 1)
    {
        TIM_ClearITPendingBit(TIM1, 0x100);
        PWMOutputs_DISABLE();
        SysPara.StateMachineFlag = SysFault;
        debug.FaultFlag = HardOCP;
        debug.FaultDetial[0] = SysPara.CurrFirstAD;
        debug.FaultDetial[1] = SysPara.CurrSencondAD;
        debug.FaultDetial[2] = SysPara.CurrThirdAD;
        debug.FaultDetial[3] = SysPara.DcBusAD;
        debug.FaultDetial[4] = SysPara.MotorAngle;
    }
    else if (TIM1->SR_b.BIF == 1)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    }
}

void SysTick_Handler(void)
{
    /* Reserved for future 1 ms tasks; no motor control is executed here. */
}
