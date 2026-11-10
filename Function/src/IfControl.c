#include "IfControl.h"
#include "function.h"
#include "PI_Regulator.h"

IfCurr_t IfStart;

static s16 RampQ14(s16 current, s16 target, s16 step)
{
    s32 next;

    if (step <= 0) return target;

    if (current < target)
    {
        next = (s32)current + step;
        if (next > target) next = target;
    }
    else if (current > target)
    {
        next = (s32)current - step;
        if (next < target) next = target;
    }
    else
    {
        next = current;
    }

    return (s16)next;
}

void IfInit(void)
{
    IfStart.ElectAngle = IF_ALIGN_ANGLE_Q15;
    IfStart.ReferSpeed = 0;
    IfStart.IqRef = 0;
    IfStart.AlignCnt = 0;

    SysPara.IdRef = 0;
    SysPara.MotorAngle = IF_ALIGN_ANGLE_Q15;
    SysPara.VoltAngle = IF_ALIGN_ANGLE_Q15;
}

/* Called on entry and at each fast-loop alignment period. */
void IfPrePosPrepare(void)
{
    IfStart.ElectAngle = IF_ALIGN_ANGLE_Q15;
    IfStart.ReferSpeed = 0;
    IfStart.IqRef = 0;

    SysPara.MotorAngle = IF_ALIGN_ANGLE_Q15;
    SysPara.VoltAngle = IF_ALIGN_ANGLE_Q15;
    SysPara.IdRef = IF_ALIGN_ID_Q14;
}

/* Call once after the alignment PWM has been calculated and written. */
void IfPrePosFinish(void)
{
    if (IfStart.AlignCnt < IF_ALIGN_PWM_TICKS)
        IfStart.AlignCnt++;

    if (IfStart.AlignCnt >= IF_ALIGN_PWM_TICKS)
    {
        IfStart.ReferSpeed = 0;
        IfStart.IqRef = 0;
        SysPara.IdRef = IF_RUN_ID_Q14;

        IdLoop.Ui = 0;
        IqLoop.Ui = 0;

        SysPara.StateMachineFlag = IfStartMode;
    }
}

/*
 * IfStart.ReferSpeed is Q14 electrical-frequency pu.
 * TsPu is Q17. One signed 16-bit turn is 65536 angle counts.
 * 10430 ~= 32768/pi converts electrical radians to angle counts.
 * Call after PWM update so Park and inverse-Park use the same angle this cycle.
 */
void IfAngleCalc(void)
{
    s32 delta;

    delta = ((s32)IfStart.ReferSpeed * (s32)SysPara.TsPu) >> 16;
    delta = (delta * 10430) >> 15;

    IfStart.ElectAngle = (s16)((u16)IfStart.ElectAngle + (u16)delta);
    SysPara.MotorAngle = IfStart.ElectAngle;
    SysPara.VoltAngle = IfStart.ElectAngle;
}

/* TIM14 slow task: increase frequency and torque-current demand gently. */
void IfSpeedCalc(void)
{
    IfStart.ReferSpeed = RampQ14(
        IfStart.ReferSpeed, IF_TARGET_SPEED_Q14, IF_SPEED_RAMP_STEP_Q14);

    IfStart.IqRef = RampQ14(
        IfStart.IqRef, IF_TARGET_IQ_Q14, IF_IQ_RAMP_STEP_Q14);

    SysPara.IdRef = IF_RUN_ID_Q14;
}
