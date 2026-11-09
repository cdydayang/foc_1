#ifndef __ESTIMATION_H
#define __ESTIMATION_H

#include "CurrentLoop.h"
#include "Math.h"
#include "PI_Regulator.h"

#define MAX_SPEED		24576//19660//(s16)(16384*1.2f)

typedef struct
{
	PLL_t SmoPLL;

	s16 CurrGain;	
	s16 VoltGain; 	
	s16 SgnGain;
	
	s16 preUalpha;
	s16 preUbeta;
	s16 preIalpha;
	s16 preIbeta;
	
	s16 EstiIalpha;
	s16 EstiIbeta;
	
	s16 BEMF_Alpha;
	s16 BEMF_Beta;
	
	s16 BEMF_1st_Alpha;
	s16 BEMF_1st_Beta;
	
	s16 BEMF_2nd_Alpha;
	s16 BEMF_2nd_Beta;
	
	s16 BEMF_SinValue;
	s16 BEMF_CosValue;
	s16 Angle_SinValue;
	s16 Angle_CosValue;
}
Observer_t;

extern Observer_t SmoEsti;

extern void Pos_Esti_Init(void);
extern void Pos_Esti_Calc(void);
#endif
