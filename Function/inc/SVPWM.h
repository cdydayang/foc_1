#ifndef __SVPWM_H
#define __SVPWM_H
#include "lcm32f039_type.h"
#include "Timer_Init.h"
#define SYSTEM_FREQUENCY (UPDS_CKTIM)
#define PWM_PERIOD (UP_PWM_PERIOD)
typedef struct
{
    u16 PwmTBPRD;
    u16 Sector;
    u16 LastSector;
    s16 UCMP;
    s16 VCMP;
    s16 WCMP;
    s16 VbusPu;
    s16 ValfaPu;
    s16 VbetaPu;
    s16 VoltAlpha_q14;
    s16 VoltBeta_q14;
    s16 PwmTBPRDInv_q24;
} pwm_t;
extern pwm_t PWM_Calc;
void Invter_Init(void);
void Invter_Calc(void);
void PWMOutputs_ENABLE(void);
void PWMOutputs_DISABLE(void);
void Update_PWM(void);
#endif
