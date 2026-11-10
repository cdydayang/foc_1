#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_adc.h"
#include "ADC_Init.h"

vu32 ADCConvertedRawData[ADC_SIZE];

void ADC_Config(void)
{
    ADC_InitTypeDef adc;
    DMA_InitTypeDef dma;
    uint8_t adin[16] = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);

    ADC_StopOfConversion(ADC);
    DMA_Cmd(DMA1_Channel1, DISABLE);

    /*
     * Simultaneous A/B samples:
     * pair 0 = U-current (A2) + W-current (B12)
     * pair 1 = DC-bus voltage (A3) + unused B0
     * ADC DMA packing order must be confirmed on the real silicon/board.
     */
    adin[0] = ADCIN_2_A;
    adin[8] = ADCIN_12_B;
    adin[1] = ADCIN_3_A;
    adin[9] = ADCIN_0_B;

    ADC_ChannelConfig(1, adin); /* CHNUM is sequence length minus one. */

    ADC_StructInit(&adc);
    adc.ADC_ClkMode = ADC_ClockMode_SynClkDiv4;
    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_AutoWatiMode = ENABLE;
    adc.ADC_DMATRIG_LEVEL = FIFO_DMA_NOEMPTY;
    adc.ADC_DMA = ENABLE;
    adc.ADC_DiscMode = DISABLE;
    adc.ADC_SamecMode = ENABLE;
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_ScanDirection = ADC_ScanDirection_Upward;
    adc.ADC_Vref = ADC_Vref_Internal_5V;
    adc.ADC_SampleTime = 6;
    adc.ADC_SampleDelayTime = 0;
    adc.ADC_AdjustTime = 4;
    adc.ADC_ConvertTime = 0;
    adc.ADC_ConvertPulseTime = 0;

    ADC_Init(ADC, &adc);
    ADC->EXTCFG |= (1u << 2);
    ADC_Cmd(ADC, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    dma.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    dma.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedRawData[0];
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = ADC_SAMPLE_WORDS;
    dma.DMA_MSIZE = DMA_MSIZE_1;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma);

    DMA_ITConfig(DMA1_Channel1, DMA1_FLAG_TC1, ENABLE);
    DMA_RemapConfig(DMA1_Channel1, DMA_ReqNum5, DMA_REQ_ADC);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_StartOfConversion(ADC);
}
