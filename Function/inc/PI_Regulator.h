#ifndef __PI_REGULATOR_H
#define __PI_REGULATOR_H

#include "lcm32f039_type.h"

typedef struct
{
    s16 Ref;        /* Q14 */
    s16 Fdb;        /* Q14 */
    s32 Err;        /* Q14 */
    s16 Ki;         /* Q14 */
    s16 Kp;         /* Q10 */
    s32 Up;         /* Q14 */
    s32 Ui;         /* Q28 */
    s16 OutMin;     /* Q14 */
    s16 OutMax;     /* Q14 */
    s16 Out;        /* Q14 voltage command */
} PI_t;

extern PI_t IdLoop;
extern PI_t IqLoop;

void PI_Calc(PI_t *v);

#endif
