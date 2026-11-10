#ifndef __ADC_CALC_H
#define __ADC_CALC_H

#include "lcm32f039_type.h"

extern volatile u8 ADC_OffsetReady;

void ADC_Sample_Calc(void);

#endif
