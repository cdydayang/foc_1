#ifndef __SVPWM_H
#define __SVPWM_H

#include "SVPWM.h"
#include "Math.h"

/*================= SYSTEM Configuration =========*/
#define SYSTEM_FREQUENCY                   96000000

/*================== PWM Configuration ===========*/
#define PWM_SET_FREQUENCY                  (UPDS_PWM_FREQ)
#define PWM_SET_FREQUENCY_GiSTER           (SYSTEM_FREQUENCY / PWM_SET_FREQUENCY / 2  - 1)
#define PWM_PERIOD                         ((SYSTEM_FREQUENCY / PWM_SET_FREQUENCY / 2))


#define INV_SQRT3	  			4728//Q13
#define SQRT3 		   			14188//Q13

typedef struct 
{
	u16  PwmTBPRD;
	u16  Sector;
	u16  LastSector;
	
	s16  UCMP;
	s16  VCMP;
	s16  WCMP;
	s16  VbusPu;
	s16  ValfaPu;
	s16  VbetaPu;
	
	s16	VoltAlpha_q14;
	s16 VoltBeta_q14;
	s16 PwmTBPRDInv_q24;
	
	s16 CurrFirstPhase;
	s16 CurrSecondPhase;
}pwm_t;

extern pwm_t PWM_Calc;

extern void Invter_Init(void);
extern void Invter_Calc(void);

extern void PWM_Init(void);
extern void PWMOutputs_ENABLE(void);
extern void PWMOutputs_DISABLE(void);
extern void Update_PWM(void);

#endif
