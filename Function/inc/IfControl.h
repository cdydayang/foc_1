#ifndef __IF_CONTROL_H
#define __IF_CONTROL_H

#include "lcm32f039_type.h"

/* Default learning values: recalibrate for the real motor and current scale. */
#define IF_ALIGN_ANGLE_Q15        ((s16)0)
#define IF_ALIGN_ID_Q14           ((s16)1000)
#define IF_ALIGN_PWM_TICKS        ((u16)1200) /* 150 ms at 8 kHz */

#define IF_TARGET_SPEED_Q14       ((s16)2000)
#define IF_SPEED_RAMP_STEP_Q14    ((s16)10)

#define IF_TARGET_IQ_Q14          ((s16)3000)
#define IF_IQ_RAMP_STEP_Q14       ((s16)30)

#define IF_CURRENT_REF_LIMIT_Q14  ((s16)6000)
#define IF_RUN_ID_Q14             ((s16)0)

typedef struct
{
    s16 ElectAngle;     /* signed 16-bit electrical angle; one turn = 65536 */
    s16 ReferSpeed;     /* Q14 electrical-frequency command */
    s16 IqRef;          /* Q14 current reference */
    u16 AlignCnt;       /* fast-loop alignment counter */
} IfCurr_t;

extern IfCurr_t IfStart;

void IfInit(void);
void IfPrePosPrepare(void);
void IfPrePosFinish(void);
void IfSpeedCalc(void);
void IfAngleCalc(void);

#endif
