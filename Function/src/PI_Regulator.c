#include "PI_Regulator.h"
#include "function.h"

PI_t IdLoop;
PI_t IqLoop;

/*
 * Q 格式：Ref/Fdb/Err/Out 为 Q14；Kp 为 Q10；Ki 为 Q14；Ui 为 Q28。
 * Up = Kp(Q10) × Err(Q14) >> 10，回到 Q14；
 * Ui 累加 Ki(Q14) × Err(Q14)，因此保留为 Q28；
 * 最后 Ui>>14 回到 Q14，与比例项相加。
 */
void PI_Calc(PI_t *v)
{
    s32 ui_min;
    s32 ui_max;
    s32 temp;

    /* 用乘法换算 Q14 输出限幅到 Q28，避免对负数做左移。 */
    ui_min = (s32)v->OutMin * 16384L;
    ui_max = (s32)v->OutMax * 16384L;

    /* 电流误差 = 目标电流 - 实际电流。 */
    v->Err = (s32)v->Ref - (s32)v->Fdb;
    v->Err = RangeLimit(-32700, v->Err, 32700);

    /* 比例环节：Q10 × Q14 >> 10 = Q14。 */
    v->Up = ((s32)v->Kp * v->Err) >> 10;
    v->Up = RangeLimit(-32700, v->Up, 32700);

    /* 积分环节：Q14 × Q14 = Q28，积分状态也限制在输出范围内。 */
    v->Ui += (s32)v->Ki * v->Err;
    v->Ui = RangeLimit(ui_min, v->Ui, ui_max);

    /* 比例 + 积分，输出为电压矢量分量（Q14）。 */
    temp = v->Up + (v->Ui >> 14);
    v->Out = (s16)RangeLimit(v->OutMin, temp, v->OutMax);
}
