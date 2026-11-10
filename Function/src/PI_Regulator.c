#include "PI_Regulator.h"
#include "function.h"

PI_t IdLoop;
PI_t IqLoop;

/*
 * Ref/Fdb/Out are Q14. Kp is Q10, Ki is Q14.
 * Ui accumulates in Q28 to preserve fractional integration.
 */
void PI_Calc(PI_t *v)
{
    s32 ui_min;
    s32 ui_max;
    s32 temp;

    ui_min = ((s32)v->OutMin) << 14;
    ui_max = ((s32)v->OutMax) << 14;

    v->Err = (s32)v->Ref - (s32)v->Fdb;
    v->Err = RangeLimit(-32700, v->Err, 32700);

    v->Up = ((s32)v->Kp * v->Err) >> 10;
    v->Up = RangeLimit(-32700, v->Up, 32700);

    v->Ui += (s32)v->Ki * v->Err;
    v->Ui = RangeLimit(ui_min, v->Ui, ui_max);

    temp = v->Up + (v->Ui >> 14);
    v->Out = (s16)RangeLimit(v->OutMin, temp, v->OutMax);
}
