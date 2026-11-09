#ifndef __PI_REGULATOR_H
#define __PI_REGULATOR_H

#include "PI_Regulator.h"
#include "Math.h"

typedef struct
{
 	s16  Ref;	//Q14
	s16  Fdb;	//Q14
	s32  Err; //Q14
	s16  Ki;	//Q14
	s16  Kp;  //Q10
	s32  Up;	//Q14
	s32  Ui;	//Q28
	s16  OutMin;//Q14
	s16  OutMax;//Q14
	s16  Out;//Q14
}PI_t;

typedef struct
{
	s16  qKp;
	s16  qKi;
	s16  qOutMax;
	s16  qOutMin;
	s32  qRef;
	s32  qFdb;

	s32  Error;

	s16  qSpeedOut;//q14
	s16  qAngleOut;//q14
	s32  Ui;
}PLL_t;

extern PI_t IdLoop;
extern PI_t IqLoop;
extern PI_t SpeedLoop;

void PI_Calc(PI_t *v);

extern void PLL_CALC(PLL_t *v);

#endif
