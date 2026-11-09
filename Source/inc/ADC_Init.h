#ifndef _ADC_INIT_H
#define _ADC_INIT_H

#include "lcm32f039.h"
#include "lcm32f039_type.h"

/*************************************************************************************/
#define VDC_ADC_DATA     		ADCConvertedRawData[2]	 	
#define PHASE_IA_ADC_DATA    		ADCConvertedRawData[0]

#define PHASE_IC_ADC_DATA    			ADCConvertedRawData[1]

#define T_IPM_ADC_DATA   	ADCConvertedRawData[6]

#define REF_SPEED_DATA 		ADCConvertedRawData[4]

#define  IDC_ADC_DATA   			ADCConvertedRawData[3]	

#define BEMF_W_PHASE_ADC_DATA    	ADCConvertedRawData[9]

#define BEMF_V_PHASE_ADC_DATA    			ADCConvertedRawData[7]

#define BEMF_U_PHASE_ADC_DATA    			ADCConvertedRawData[5]



/*************************************************************************************/
#define DMA_ENABLE 1
#define ADC1_DR_Address    ((u32)0x4001A020)
#define ADC_SIZE				14
#define ADC_SAMPLE_NUMF 10//

/*************************************************************************************/
extern vu32  ADCConvertedRawData[ADC_SIZE];

void ADC_Config(void);

#endif


