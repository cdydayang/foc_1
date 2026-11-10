#ifndef __ADC_INIT_H
#define __ADC_INIT_H

#include "lcm32f039.h"
#include "lcm32f039_type.h"

/*
 * I/F 电流环只需要两个同步采样对：
 *   第 1 对：U 相电流 + W 相电流
 *   第 2 对：母线电压 + 直流侧电流（当前控制算法不使用 IDC）
 *
 * 同步采样 DMA 的数据按原参考工程的排列方式交错存放：
 * [0]=U 相，[1]=W 相，[2]=母线电压，[3]=直流侧电流。
 */
#define ADC_SIZE              4
#define ADC_SAMPLE_WORDS      4
#define ADC1_DR_Address       ((u32)0x4001A020)

#define PHASE_IA_ADC_DATA     ADCConvertedRawData[0] /* U 相，ADCIN_2_A */
#define PHASE_IC_ADC_DATA     ADCConvertedRawData[1] /* W 相，ADCIN_12_B */
#define VDC_ADC_DATA          ADCConvertedRawData[2] /* 母线电压，ADCIN_3_A */
#define IDC_ADC_DATA          ADCConvertedRawData[3] /* 直流侧电流，ADCIN_11_B */

extern vu32 ADCConvertedRawData[ADC_SIZE];

void ADC_Config(void);

#endif
