#include "CurrentLoop.h"
#include "function.h"
#include "IfControl.h"
#include "PI_Regulator.h"

/*
 * 以下增益来自原仓库 parameters.m 中的演示电机和其标幺基值：
 * Kp=1151(Q10)，Ki=2488(Q14)，输出限幅 ±20000(Q14)。
 * 你的电机 Rs/Ld/Lq 或电流/电压基值不同，必须重新计算，不能直接照搬。
 */
#define CURRENT_PI_KP_Q10       ((s16)1151)
#define CURRENT_PI_KI_Q14       ((s16)2488)
#define CURRENT_VOLT_LIMIT_Q14  ((s16)20000)

/* 电流参考值限幅，避免错误命令超过当前学习工程设定的范围。 */
static s16 ClampCurrentRef(s32 x)
{
    if (x > IF_CURRENT_REF_LIMIT_Q14) return IF_CURRENT_REF_LIMIT_Q14;
    if (x < -(s32)IF_CURRENT_REF_LIMIT_Q14) return (s16)-IF_CURRENT_REF_LIMIT_Q14;
    return (s16)x;
}

/* 初始化 Id/Iq 两个 PI 的参数、积分值和输出范围。 */
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

/*
 * 每个 ADC DMA 快速中断调用一次：
 * 1) IdLoop：IdRef 与 IdFilter 比较，PI 输出 Vd；
 * 2) IqLoop：I/F 的 IqRef 与 IqFilter 比较，PI 输出 Vq。
 * Ref/Fdb 是电流（Q14），Out 是电压指令（Q14），不要把 Out 当成电流。
 */
void InsideLoopCalc(void)
{
    IdLoop.Ref = ClampCurrentRef(SysPara.IdRef);
    IdLoop.Fdb = SysPara.IdFilter;
    PI_Calc(&IdLoop);

    /* 当前工程没有速度环：Iq 参考始终来自 I/F 启动状态机。 */
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
