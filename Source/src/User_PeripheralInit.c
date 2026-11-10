#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_dma.h"
#include "User_PeripheralInit.h"

/*
 * ACMP1 compares the current-sense amplifier output against DAC0.
 * Confirm comparator input routing, polarity and threshold on the actual PCB.
 */
void DAC_Config(void)
{
    DAC_InitTypeDef dac;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ANACTRL, ENABLE);

    dac.DAC_EXTrigger_Edge = DAC_EXTrigger_edge_dis;
    dac.DAC_Trigger_Source = DAC_Trigger_Software;
    dac.DAC_WaveGeneration = DAC_WaveGeneration_None;
    dac.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    dac.DAC_OutputBuffer = DAC0_OutputBuffer_Enable;
    dac.DAC_Vref_Select = DAC_Vref_Avdd;
    dac.DAC_DmaMode = DISABLE;
    dac.DAC_DMAUDR_IE = DISABLE;
    DAC_Init(DAC0, &dac);
    DAC_Cmd(DAC0, ENABLE);

    /* Original board's initial comparator trip threshold. Validate before use. */
    DAC_SetDac_10B_Data(DAC0, DAC_Align_10B_R, 840);
    DAC_SoftwareTriggerCmd(DAC0, ENABLE);
}

void ACMP_Config(void)
{
    ACMP_InitTypeDef cmp;

    cmp.ACMP_P_Select = CP1_PS_CPP1EXT_INPUT;
    cmp.ACMP_N_Select = CP1_NS_DAC0OUT;
    cmp.ACMP_Delay_time = CP_NODelay_time;
    cmp.ACMP_Blanking = CP_NOBLANKING;
    cmp.ACMP_Is = CP_IS_POS;
    cmp.ACMP_HYSEN = ENABLE;
    cmp.ACMP_FREN = ENABLE;
    cmp.ACMP_FS = DISABLE;
    cmp.CMP_INTENMASK = DISABLE;
    cmp.CMP_SEQ_MODE = DISABLE;
    cmp.TRIG_MODE = CP_TIRG_INASOFT;
    cmp.CHNL_CNT_TIME = CHNL_CNT_TIME_64CLK;

    ACMP1_Init(&cmp);
    ACMP1_SEQ_INPUT_SELECT(DISABLE, 5, 1);
    ACMP1_Cmd(ENABLE);
}

void NVIC_Config(void)
{
    NVIC_InitTypeDef nvic;

    NVIC_ClearPendingIRQ(TIM1_NON_CC_IRQn);
    nvic.NVIC_IRQChannel = TIM1_NON_CC_IRQn;
    nvic.NVIC_IRQChannelPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    NVIC_ClearPendingIRQ(DMAC_CH1_2_IRQn);
    nvic.NVIC_IRQChannel = DMAC_CH1_2_IRQn;
    nvic.NVIC_IRQChannelPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    NVIC_ClearPendingIRQ(TIM14_IRQn);
    nvic.NVIC_IRQChannel = TIM14_IRQn;
    nvic.NVIC_IRQChannelPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    NVIC_ClearPendingIRQ(SysTick_IRQn);
    nvic.NVIC_IRQChannel = SysTick_IRQn;
    nvic.NVIC_IRQChannelPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    TIM_Cmd(TIM1, ENABLE);
}
