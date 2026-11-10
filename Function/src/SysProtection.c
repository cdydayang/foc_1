#include "SysProtection.h"
#include "function.h"
#include "SVPWM.h"
#include "ADC_Calc.h"

/*
 * 以下为原参考板的 ADC count 初始阈值，不是通用保护值。
 * 实际阈值必须结合分流电阻、放大器增益、母线分压比和 ADC 参考电压标定。
 */
#define SOFTWARE_OC_RAW_LIMIT  ((s16)800)
#define DC_BUS_OV_RAW_LIMIT    ((s16)1200)
#define DC_BUS_UV_RAW_LIMIT    ((s16)850)

/* 统一故障锁存：先禁止 PWM，再记录故障代码与现场调试数据。 */
static void LatchFault(s16 fault)
{
    PWMOutputs_DISABLE();
    SysPara.StateMachineFlag = SysFault;
    debug.FaultFlag = fault;
    debug.FaultDetial[0] = SysPara.CurrFirstAD;
    debug.FaultDetial[1] = SysPara.CurrSencondAD;
    debug.FaultDetial[2] = SysPara.CurrThirdAD;
    debug.FaultDetial[3] = SysPara.DcBusAD;
    debug.FaultDetial[4] = SysPara.MotorAngle;
}

/* 快速中断内执行的软件过流和母线过压/欠压保护。 */
void SysProtection(void)
{
    if (ADC_OffsetReady == 0) return;

    if ((SysPara.CurrFirstAD > SOFTWARE_OC_RAW_LIMIT) ||
        (SysPara.CurrFirstAD < -SOFTWARE_OC_RAW_LIMIT) ||
        (SysPara.CurrSencondAD > SOFTWARE_OC_RAW_LIMIT) ||
        (SysPara.CurrSencondAD < -SOFTWARE_OC_RAW_LIMIT) ||
        (SysPara.CurrThirdAD > SOFTWARE_OC_RAW_LIMIT) ||
        (SysPara.CurrThirdAD < -SOFTWARE_OC_RAW_LIMIT))
    {
        LatchFault(OverCurrent);
        return;
    }

    if ((SysPara.DcBusAD > DC_BUS_OV_RAW_LIMIT) ||
        ((SysPara.DcBusAD < DC_BUS_UV_RAW_LIMIT) &&
         (SysPara.StateMachineFlag != MotorStandBy)))
    {
        LatchFault(OverVolt);
        return;
    }
}
