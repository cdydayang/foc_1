#ifndef __TIMER_INIT_H
#define __TIMER_INIT_H
#include "lcm32f039.h"
#include "lcm32f039_type.h"

/*************************************************************************************/
//系统时钟
#define UPDS_CKTIM									((u32)96000000uL)								// 系统主频
//Systick中断频率
#define UPDS_SYS_TICK_HZ						(1000)		
//PWM开关频率（主中断）
#define UPDS_PWM_FREQ 							((u16)8000) 										//Unit: Hz 
//PWM死区时间,单位ns，0~3500ns
#define UPDS_DEADTIME_NS						((u16)1500)//2.0us

#define UPDS_SYS_TICK_PERIOD				(u32)(UPDS_CKTIM/UPDS_SYS_TICK_HZ)
#define UP_PWM_PRSC ((u8)0)
#define UP_REP_RATE (1)  // (N.b): Internal current loop is performed every 

#define UP_DEADTIME  (u16)((unsigned long long)UPDS_CKTIM/2 \
				*(unsigned long long)UPDS_DEADTIME_NS/1000000000uL) 
#define UP_PWM_PERIOD 	((u16) (UPDS_CKTIM / (u32)(2 * UPDS_PWM_FREQ *(UP_PWM_PRSC+1)))) 	

/*************************************************************************************/
				
void TIM1_Config(void);
void TIM14_Config(void);

#endif


