#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_opa.h"
#include "OPA_Init.h"

/* OPA1 output PB13 -> ADCIN_12_B: W-current channel. */
static void OPA1_CurrentW_Init(void)
{
    GPIO_InitTypeDef gpio;
    OPA_InitTypeDef opa;

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);

    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_Speed = GPIO_Speed_Level_2;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    opa.OPA_Oppselect = OPA1_Oppselect_PA9;
    opa.OPA_Opnselect = OPA1_Opnselect_PA10;
    opa.OPA_Gain = OPA_Gain_10;
    opa.OPA_BiasVoltage = OPA_BiasVoltage_EN;
    opa.OPA_FeedbackRes = OPA_FeedbackRes_EN;
    opa.OPA_Opoto_Gpio = OPA_OPOEX_EN;
    opa.OPA_Opoto2_Gpio = OPA_OPOEX2_DISEN;
    opa.OPA_OppRES_short = OPA_OppRES_short_DISEN;
    opa.OPA_OpnRES_short = OPA_OpnRES_short_DISEN;
    opa.OPA_BiasVoltageSource = OPA_OPA_BiasVoltageSource_HALFVRH;
    OPA_Init(OPA1, &opa);
    OPA_Cmd(OPA1, ENABLE);
}

/* OPA2 output PA2 -> ADCIN_2_A: U-current channel. */
static void OPA2_CurrentU_Init(void)
{
    GPIO_InitTypeDef gpio;
    OPA_InitTypeDef opa;

    GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_2);

    gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_Speed = GPIO_Speed_Level_2;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &gpio);

    opa.OPA_Oppselect = OPA2_Oppselect_PF4;
    opa.OPA_Opnselect = OPA2_Opnselect_PF5;
    opa.OPA_Gain = OPA_Gain_10;
    opa.OPA_BiasVoltage = OPA_BiasVoltage_EN;
    opa.OPA_FeedbackRes = OPA_FeedbackRes_EN;
    opa.OPA_Opoto_Gpio = OPA_OPOEX_EN;
    opa.OPA_Opoto2_Gpio = OPA_OPOEX2_DISEN;
    opa.OPA_OppRES_short = OPA_OppRES_short_DISEN;
    opa.OPA_OpnRES_short = OPA_OpnRES_short_DISEN;
    opa.OPA_BiasVoltageSource = OPA_OPA_BiasVoltageSource_HALFVRH;
    OPA_Init(OPA2, &opa);
    OPA_Cmd(OPA2, ENABLE);
}

void OPA_Config(void)
{
    OPA1_CurrentW_Init();
    OPA2_CurrentU_Init();
}
