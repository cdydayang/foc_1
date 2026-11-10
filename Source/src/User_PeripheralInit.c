#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_tim.h"
#include "User_PeripheralInit.h"

/*
 * ACMP1 负责硬件快速过流比较：
 * 电流检测放大器输出接比较器正端，DAC0 输出作为比较阈值。
 * DAC 阈值必须结合实际分流电阻、运放增益和板级保护设计重新校准。
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

    /* 原参考板的 DAC 10 位阈值；未经实测前不能视为真实安培保护值。 */
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

/*
 * 配置使用到的外部中断优先级：
 * TIM1 硬件 Break 最高，ADC DMA 快速电流环第二，TIM14 慢任务第三。
 *
 * SysTick_IRQn 是 Cortex-M 内核异常，不是普通外设 IRQ，
 * 不要塞进 NVIC_InitTypeDef（其 IRQChannel 字段是无符号数，会产生 -1 转换警告）。
 * SysTick_Config() 自己会将 SysTick 优先级设为最低级。
 */
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

    /* TIM1 计数器开始运行，后续 PWM 比较事件触发 ADC。 */
    TIM_Cmd(TIM1, ENABLE);
}
