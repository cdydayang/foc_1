#include "SysProtection.h"
#include "function.h"
#include "SVPWM.h"
#include "ADC_Calc.h"

/* Reference-board raw-ADC thresholds: MUST be recalibrated for the actual board. */
#define SOFTWARE_OC_RAW_LIMIT  ((s16)800)
#define DC_BUS_OV_RAW_LIMIT    ((s16)1200)
#define DC_BUS_UV_RAW_LIMIT    ((s16)850)

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
