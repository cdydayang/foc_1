#include "lcm32f039.h"
#include "lcm32f039_tim.h"
#include "User_Interrupt.h"

#include "function.h"
#include "IfControl.h"
#include "SVPWM.h"

/*
 * TIM14 慢速任务：
 * 当前定时器配置为 96 MHz / (191+1) / (1000+1) ≈ 499.5 Hz。
 * I/F 频率与 Iq 参考斜坡在这里更新，不放进 8 kHz 电流环中断。
 */
void TIM14_Handler(void)
{
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);

    if (SysPara.StateMachineFlag == IfStartMode)
    {
        IfSpeedCalc();
    }
}

/* TIM1_CH5 事件仅用于 ADC 触发同步，本中断不做电机算法。 */
void TIM1_CC_Handler(void)
{
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC5);
}

/* TIM1 非捕获比较中断：处理硬件 Break 保护。 */
void TIM1_NON_CC_Handler(void)
{
    /*
     * 注意：TIM_ClearITPendingBit() 的第二个参数是定时器状态标志，
     * 不是 NVIC IRQ 编号，因此不能传 TIM1_NON_CC_IRQn。
     */
    if (TIM1->SR_b.B2IF == 1)
    {
        /* B2IF 对应硬件 Break2 标志，参考芯片原工程使用 0x100 清除。 */
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
        /* 软件 Break 标志。 */
        TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    }
}

/* 当前 I/F 学习工程暂时不需要 1 ms 软件任务，保留 CMSIS 中断入口。 */
void SysTick_Handler(void)
{
}
