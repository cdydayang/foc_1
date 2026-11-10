#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_adc.h"
#include "ADC_Init.h"

/* DMA 环形缓冲区：每次触发完成后保存 4 个 32 位 ADC 数据字。 */
vu32 ADCConvertedRawData[ADC_SIZE];

/*
 * ADC 通道与 DMA 顺序（双 ADC A/B 同步采样）
 *
 * 采样序号        A 侧通道              B 侧通道
 *   0            ADCIN_2_A (U 电流)     ADCIN_12_B (W 电流)
 *   1            ADCIN_3_A (母线电压)   ADCIN_11_B (直流侧电流)
 *
 * 同步模式下，DMA 缓冲区按 A0、B0、A1、B1 交错排列：
 * [0]=U，[1]=W，[2]=Vbus，[3]=Idc。
 *
 * ADC_ChannelConfig() 的 NbrOfChannel 参数是 CHNUM 字段值，
 * 硬件通道数为 NbrOfChannel + 1。因此两组同步通道传入 1。
 */
void ADC_Config(void)
{
    ADC_InitTypeDef adc;
    DMA_InitTypeDef dma;
    uint8_t adin[16] = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);

    /* 初始化前停止旧转换和 DMA，避免配置期间写入缓冲区。 */
    ADC_StopOfConversion(ADC);
    DMA_Cmd(DMA1_Channel1, DISABLE);

    /* A 侧两个通道：U 相电流、母线电压。 */
    adin[0] = ADCIN_2_A;
    adin[1] = ADCIN_3_A;

    /* B 侧两个通道：W 相电流、直流侧电流。 */
    adin[8] = ADCIN_12_B;
    adin[9] = ADCIN_11_B;

    ADC_ChannelConfig(1, adin);

    ADC_StructInit(&adc);
    adc.ADC_ClkMode = ADC_ClockMode_SynClkDiv4;
    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_AutoWatiMode = ENABLE;
    adc.ADC_DMATRIG_LEVEL = FIFO_DMA_NOEMPTY;
    adc.ADC_DMA = ENABLE;
    adc.ADC_DiscMode = DISABLE;
    adc.ADC_SamecMode = ENABLE; /* A/B 两组同时采样，降低相电流采样时间差。 */
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

    /* DMA 外设地址固定为 ADC 数据寄存器，内存地址每字递增。 */
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

    /* DMA 搬完 4 个数据字后通知 CPU，下一次触发继续覆盖缓冲区。 */
    DMA_ITConfig(DMA1_Channel1, DMA1_FLAG_TC1, ENABLE);
    DMA_RemapConfig(DMA1_Channel1, DMA_ReqNum5, DMA_REQ_ADC);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC 使用 TIM1_TRGO 作为外部触发；这里启动 ADC 等待定时器触发。 */
    ADC_StartOfConversion(ADC);
}
