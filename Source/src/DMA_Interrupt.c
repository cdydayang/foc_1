#include "DMA_Interrupt.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_tim.h"

#include "function.h"
#include "ADC_Calc.h"
#include "CurrentLoop.h"
#include "IfControl.h"
#include "SysProtection.h"
#include "SVPWM.h"
#include "Math.h"

void DMAC_CH1_2_Handler(void)
{
    DMA_ClearITPendingBit(DMA1_FLAG_TC1);

    /* Hold a fixed D-axis alignment field before each alignment ADC/Park step. */
    if (SysPara.StateMachineFlag == MotorPrePos)
        IfPrePosPrepare();

    ADC_Sample_Calc();

    switch (SysPara.StateMachineFlag)
    {
        case MotorStandBy:
            /* Gate outputs remain disabled while ADC offset tracking completes. */
            break;

        case MotorPrePos:
            if (ADC_OffsetReady == 0) break;
            SysProtection();
            if (SysPara.StateMachineFlag == SysFault) break;

            InsideLoopCalc();
            ipark1.Ds = IdLoop.Out;
            ipark1.Qs = IqLoop.Out;
            ipark1.Theta = SysPara.VoltAngle;
            IPARK_MACRO1(&ipark1);

            PWM_Calc.ValfaPu = (s16)(((s32)ipark1.Alpha * PWM_Calc.PwmTBPRD) >> 14);
            PWM_Calc.VbetaPu = (s16)(((s32)ipark1.Beta * PWM_Calc.PwmTBPRD) >> 14);
            Invter_Calc();
            PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);
            Update_PWM();
            IfPrePosFinish();
            break;

        case IfStartMode:
            SysProtection();
            if (SysPara.StateMachineFlag == SysFault) break;

            InsideLoopCalc();
            ipark1.Ds = IdLoop.Out;
            ipark1.Qs = IqLoop.Out;
            ipark1.Theta = SysPara.VoltAngle;
            IPARK_MACRO1(&ipark1);

            PWM_Calc.ValfaPu = (s16)(((s32)ipark1.Alpha * PWM_Calc.PwmTBPRD) >> 14);
            PWM_Calc.VbetaPu = (s16)(((s32)ipark1.Beta * PWM_Calc.PwmTBPRD) >> 14);
            Invter_Calc();
            PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);
            Update_PWM();

            /* Prepare the next-period angle; SMO is intentionally absent. */
            IfAngleCalc();
            break;

        case SysFault:
        default:
            PWMOutputs_DISABLE();
            break;
    }
}