#include "CurrentLoop.h"
#include "function.h"
#include "IfControl.h"
#include "PI_Regulator.h"

/* Demo motor gains from the original parameters.m: not universal values. */
#define CURRENT_PI_KP_Q10       ((s16)1151)
#define CURRENT_PI_KI_Q14       ((s16)2488)
#define CURRENT_VOLT_LIMIT_Q14  ((s16)20000)

static s16 ClampCurrentRef(s32 x)
{
    if (x > IF_CURRENT_REF_LIMIT_Q14) return IF_CURRENT_REF_LIMIT_Q14;
    if (x < -(s32)IF_CURRENT_REF_LIMIT_Q14) return (s16)-IF_CURRENT_REF_LIMIT_Q14;
    return (s16)x;
}

void Inside_Loop_Init(void)
{
    IdLoop.Kp = CURRENT_PI_KP_Q10;
    IdLoop.Ki = CURRENT_PI_KI_Q14;
    IdLoop.OutMax = CURRENT_VOLT_LIMIT_Q14;
    IdLoop.OutMin = (s16)-CURRENT_VOLT_LIMIT_Q14;
    IdLoop.Ui = 0;
    IdLoop.Out = 0;
    IdLoop.Ref = 0;
    IdLoop.Fdb = 0;
    IdLoop.Err = 0;
    IdLoop.Up = 0;

    IqLoop.Kp = CURRENT_PI_KP_Q10;
    IqLoop.Ki = CURRENT_PI_KI_Q14;
    IqLoop.OutMax = CURRENT_VOLT_LIMIT_Q14;
    IqLoop.OutMin = (s16)-CURRENT_VOLT_LIMIT_Q14;
    IqLoop.Ui = 0;
    IqLoop.Out = 0;
    IqLoop.Ref = 0;
    IqLoop.Fdb = 0;
    IqLoop.Err = 0;
    IqLoop.Up = 0;
}

void InsideLoopCalc(void)
{
    IdLoop.Ref = ClampCurrentRef(SysPara.IdRef);
    IdLoop.Fdb = SysPara.IdFilter;
    PI_Calc(&IdLoop);

    /* This branch contains no speed controller: Iq is always the I/F command. */
    if ((SysPara.StateMachineFlag == MotorPrePos) ||
        (SysPara.StateMachineFlag == IfStartMode))
    {
        IqLoop.Ref = ClampCurrentRef(IfStart.IqRef);
    }
    else
    {
        IqLoop.Ref = 0;
    }

    IqLoop.Fdb = SysPara.IqFilter;
    PI_Calc(&IqLoop);
}
