#ifndef __ADC_CALC_H
#define __ADC_CALC_H

#include "ADC_Calc.h"
#include "Math.h"

#define INV_SQRT3	  			4728//Q13
#define SQRT3 		   			14188//Q13

#define SAMPLE_CURR_LPF_EN		1//0//1//0//1

extern void ADC_Sample_Calc(void);

extern void PowerOnInit(void);

#endif
