#include "DMA_Interrupt.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_tim.h"

#include "function.h"
#include "ADC_Calc.h"
#include "CurrentLoop.h"
#include "IfControl.h"
#include "PI_Regulator.h"
#include "SysProtection.h"
#include "SVPWM.h"
#include "Math.h"

/*
 * 这是 FOC 的快速中断，每次 ADC DMA 完成后执行一次。
 * 默认 PWM/电流环频率为 8 kHz，单次控制运算必须在下一次采样前结束。
 *
 * 本文件有意不包含 Estimation.h，也不调用 Pos_Esti_Calc()：
 * 当前角度完全由 I/F 开环积分产生，不做 SMO/PLL 位置估算。
 */
void DMAC_CH1_2_Handler(void)
{
    /* 清除 DMA 传输完成标志，否则中断会重复进入。 */
    DMA_ClearITPendingBit(DMA1_FLAG_TC1);

    /*
     * 1. ADC 处理：
     *    扣除 U/W 电流零点，重构 V 相，执行 Clarke/Park，
     *    更新 IdFilter/IqFilter 和母线电压反馈。
     */
    if (SysPara.StateMachineFlag == MotorPrePos)
    {
        /* 预定位期间始终固定在同一个电气角度，防止角度被积分改变。 */
        IfPrePosPrepare();
    }

    ADC_Sample_Calc();

    switch (SysPara.StateMachineFlag)
    {
        case MotorStandBy:
            /* 待机期间只采 ADC 零点；功率输出由主状态机保持关闭。 */
            break;

        case MotorPrePos:
            if (ADC_OffsetReady == 0)
            {
                break;
            }

            /* 2. 预定位阶段同样需要软件过流/母线保护。 */
            SysProtection();
            if (SysPara.StateMachineFlag == SysFault)
            {
                break;
            }

            /* 3. 双 PI：IdRef/IqRef 与 IdFilter/IqFilter 比较，输出 Vd/Vq。 */
            InsideLoopCalc();

            /* 4. 反 Park：将旋转坐标系 Vd/Vq 转换为静止坐标系 Vα/Vβ。 */
            ipark1.Ds = IdLoop.Out;
            ipark1.Qs = IqLoop.Out;
            ipark1.Theta = SysPara.VoltAngle;
            IPARK_MACRO1(&ipark1);

            /* 5. 以 Q14 电压指令乘以 PWM 周期，换算到 SVPWM 计数尺度。 */
            PWM_Calc.ValfaPu = (s16)(((s32)ipark1.Alpha * PWM_Calc.PwmTBPRD) >> 14);
            PWM_Calc.VbetaPu = (s16)(((s32)ipark1.Beta * PWM_Calc.PwmTBPRD) >> 14);

            /* 6. SVPWM 计算扇区和三相比较值，再做最终边界限制。 */
            Invter_Calc();
            PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
            PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);
            Update_PWM();

            /* 7. 本次 PWM 更新后累计预定位时间，到时切入 I/F 运行态。 */
            IfPrePosFinish();
            break;

        case IfStartMode:
            /* 运行态每拍先检查过流、过压/欠压；故障时立即禁止后续计算。 */
            SysProtection();
            if (SysPara.StateMachineFlag == SysFault)
            {
                break;
            }

            /* 双电流环：Id 目标通常为 0，Iq 目标由 I/F 慢速斜坡给出。 */
            InsideLoopCalc();

            /* 使用当前采样对应的同一个角度执行反 Park。 */
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

            /*
             * 当前周期的 Park 与反 Park 都使用旧角度；
             * PWM 写入后再积分，得到下一周期要用的新角度。
             */
            IfAngleCalc();
            break;

        case SysFault:
        default:
            /* 任意未知状态都按故障处理：不继续更新调制量。 */
            PWMOutputs_DISABLE();
            break;
    }
}
