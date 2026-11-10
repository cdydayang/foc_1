#ifndef __ADC_INIT_H
#define __ADC_INIT_H

#include "lcm32f039.h"
#include "lcm32f039_type.h"

#define ADC_SIZE              4
#define ADC_SAMPLE_WORDS      4
#define ADC1_DR_Address       ((u32)0x4001A020)

#define PHASE_IA_ADC_DATA     ADCConvertedRawData[0] /* U: ADCIN_2_A */
#define PHASE_IC_ADC_DATA     ADCConvertedRawData[1] /* W: ADCIN_12_B */
#define VDC_ADC_DATA          ADCConvertedRawData[2] /* DC bus: ADCIN_3_A */

extern vu32 ADCConvertedRawData[ADC_SIZE];

void ADC_Config(void);

#endif
