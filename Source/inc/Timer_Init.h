#ifndef __TIMER_INIT_H
#define __TIMER_INIT_H
#include "lcm32f039.h"
#include "lcm32f039_type.h"
#define UPDS_CKTIM ((u32)96000000uL)
#define UPDS_SYS_TICK_HZ (1000)
#define UPDS_PWM_FREQ ((u16)8000)
#define UPDS_DEADTIME_NS ((u16)1500)
#define UPDS_SYS_TICK_PERIOD ((u32)(UPDS_CKTIM / UPDS_SYS_TICK_HZ))
#define UP_PWM_PRSC ((u8)0)
#define UP_REP_RATE (1)
#define UP_DEADTIME ((u16)((unsigned long long)UPDS_CKTIM / 2 * (unsigned long long)UPDS_DEADTIME_NS / 1000000000uL))
#define UP_PWM_PERIOD ((u16)(UPDS_CKTIM / (2u * UPDS_PWM_FREQ * (UP_PWM_PRSC + 1u))))
void TIM1_Config(void);
void TIM14_Config(void);
#endif
