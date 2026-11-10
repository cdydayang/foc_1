#include "IfControl.h"
#include "function.h"
#include "PI_Regulator.h"

IfCurr_t IfStart;

/*
 * Q14 斜坡工具：让一个有符号定点数逐步靠近目标值。
 * current、target、step 都使用相同 Q 格式；这里 I/F 速度和 Iq 均为 Q14。
 * 每次调用只改变 step，避免启动瞬间的频率/转矩指令突变。
 */
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

/*
 * I/F 状态初始化：
 * ElectAngle = 开环电气角度；ReferSpeed = 开环电气频率命令（Q14）；
 * IqRef = Q 轴转矩电流命令（Q14）；AlignCnt = 预定位 PWM 周期计数。
 */
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

/*
 * 预定位：固定一个电气角度，给 D 轴电流 Id，Iq 保持 0。
 * 此阶段只建立一个静止定子磁场，不做角度积分，不做速度估算。
 */
void IfPrePosPrepare(void)
{
    IfStart.ElectAngle = IF_ALIGN_ANGLE_Q15;
    IfStart.ReferSpeed = 0;
    IfStart.IqRef = 0;

    SysPara.MotorAngle = IF_ALIGN_ANGLE_Q15;
    SysPara.VoltAngle = IF_ALIGN_ANGLE_Q15;
    SysPara.IdRef = IF_ALIGN_ID_Q14;
}

/*
 * 每执行完一个预定位 PWM 周期调用一次。
 * IF_ALIGN_PWM_TICKS = 预定位时间 × PWM 频率；
 * 默认 1200 × 125 us = 150 ms（前提是快速中断确实为 8 kHz）。
 */
void IfPrePosFinish(void)
{
    if (IfStart.AlignCnt < IF_ALIGN_PWM_TICKS)
        IfStart.AlignCnt++;

    if (IfStart.AlignCnt >= IF_ALIGN_PWM_TICKS)
    {
        IfStart.ReferSpeed = 0;
        IfStart.IqRef = 0;
        SysPara.IdRef = IF_RUN_ID_Q14;

        /* 清除预定位阶段的 PI 积分，避免切入 I/F 时输出跳变。 */
        IdLoop.Ui = 0;
        IqLoop.Ui = 0;

        SysPara.StateMachineFlag = IfStartMode;
    }
}

/*
 * 开环角度积分：theta(k+1) = theta(k) + omega_e * Ts。
 * ReferSpeed 为 Q14，TsPu 为 Q17，乘积先右移 16 位以得到中间定点结果。
 * 10430 ≈ 32768/pi，把电气弧度增量换算为一圈 65536 计数的角度增量。
 * 这里使用 unsigned 16-bit 加法再转回 s16，允许角度自然绕回。
 * 注意：本函数在 PWM 更新之后调用，计算的是下一控制周期要用的角度。
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

/*
 * TIM14 慢任务：频率斜坡和 Iq 斜坡。
 * 频率决定旋转磁场转速；Iq 决定电磁转矩。两者必须逐渐增加，
 * 否则转子可能跟不上开环旋转磁场而失步。
 */
void IfSpeedCalc(void)
{
    IfStart.ReferSpeed = RampQ14(
        IfStart.ReferSpeed, IF_TARGET_SPEED_Q14, IF_SPEED_RAMP_STEP_Q14);

    IfStart.IqRef = RampQ14(
        IfStart.IqRef, IF_TARGET_IQ_Q14, IF_IQ_RAMP_STEP_Q14);

    /* 表贴 PMSM 的第一版先使用 Id=0；实际电机需按参数确认。 */
    SysPara.IdRef = IF_RUN_ID_Q14;
}
