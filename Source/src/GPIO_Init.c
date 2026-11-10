#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "GPIO_Init.h"

/*
 * 这里只初始化 I/F 最小工程实际使用的 GPIO：
 * - PF8：启动按键输入
 * - PC13：停止按键输入
 * - PA3：母线电压 ADC 模拟输入
 * - PA4、PA12：状态/故障 LED
 * - 三相 PWM 引脚由 TIM1_Config() 设置为复用功能
 * - 两路电流模拟前端引脚由 OPA_Config() 配置
 */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_GPIOA |
        RCC_AHBPeriph_GPIOB |
        RCC_AHBPeriph_GPIOC |
        RCC_AHBPeriph_GPIOF,
        ENABLE);

    /* 在切换到定时器复用功能前，先把六路桥臂引脚拉到低电平。 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_3);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);

    /* 母线电压：PA3 / ADCIN_3_A，必须配置为模拟输入。 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 启动按键 PF8：输入模式。按键有效电平需要按实际板卡确认。 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* 停止按键 PC13：输入模式。 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* LED 输出：初始置高，具体亮灭电平由板卡 LED 接法决定。 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_12);
}
