#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_opa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcm32f039_tim.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_uart.h"
#include "OPA_Init.h"
#include "lcm32f039_type.h"
#include "SysProtection.h"

void OPA_Config(void);
void OPA0_Config(void);
void OPA1_Config(void);
void OPA2_Config(void);
	
void OPA_Config(void)
{
	OPA0_Config();//直流电流
	OPA1_Config();
	OPA2_Config();
}

void OPA0_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct; 
	OPA_InitTypeDef 	OPA_InitStruct;

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_2);    //OPA0 -P   PF7
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_2);   //OPA0 -N    PF6
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStruct);	

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
		
	OPA_InitStruct.OPA_Oppselect = OPA0_Oppselect_PF7;
	OPA_InitStruct.OPA_Opnselect =  OPA0_Opnselect_PF6;
	OPA_InitStruct.OPA_Gain = OPA_Gain_10;
	OPA_InitStruct.OPA_BiasVoltage = OPA_BiasVoltage_EN;
	OPA_InitStruct.OPA_FeedbackRes = OPA_FeedbackRes_EN;
	OPA_InitStruct.OPA_Opoto_Gpio = OPA_OPOEX_EN;//PB12 AD11
	OPA_InitStruct.OPA_Opoto2_Gpio = OPA_OPOEX2_DISEN;//
	OPA_InitStruct.OPA_OppRES_short = OPA_OppRES_short_DISEN;
	OPA_InitStruct.OPA_OpnRES_short = OPA_OpnRES_short_DISEN;
	OPA_InitStruct.OPA_BiasVoltageSource = OPA_OPA_BiasVoltageSource_HALFVRH;
	OPA_Init(OPA0, &OPA_InitStruct);
	OPA_Cmd(OPA0, ENABLE);
}

void OPA1_Config()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	OPA_InitTypeDef 	OPA_InitStruct;

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);
	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_1); 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	OPA_InitStruct.OPA_Oppselect = OPA1_Oppselect_PA9;
	OPA_InitStruct.OPA_Opnselect = OPA1_Opnselect_PA10;
	OPA_InitStruct.OPA_Gain = OPA_Gain_10;          
	OPA_InitStruct.OPA_BiasVoltage = OPA_BiasVoltage_EN;
	OPA_InitStruct.OPA_FeedbackRes = OPA_FeedbackRes_EN;
	OPA_InitStruct.OPA_Opoto_Gpio = OPA_OPOEX_EN;//PB13 AD12
	OPA_InitStruct.OPA_Opoto2_Gpio = OPA_OPOEX2_DISEN;	
	OPA_InitStruct.OPA_OppRES_short = OPA_OppRES_short_DISEN;
	OPA_InitStruct.OPA_OpnRES_short = OPA_OpnRES_short_DISEN;
	OPA_InitStruct.OPA_BiasVoltageSource = OPA_OPA_BiasVoltageSource_HALFVRH;
	OPA_Init(OPA1, &OPA_InitStruct);
	OPA_Cmd(OPA1, ENABLE);
}

void OPA2_Config()
{
	GPIO_InitTypeDef    GPIO_InitStruct;
	OPA_InitTypeDef 		OPA_InitStruct;

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4,  GPIO_AF_2);  //PF4
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5,  GPIO_AF_2);  //PF5

	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
		
	OPA_InitStruct.OPA_Oppselect = OPA2_Oppselect_PF4;
	OPA_InitStruct.OPA_Opnselect = OPA2_Opnselect_PF5;
	OPA_InitStruct.OPA_Gain = OPA_Gain_10;//OPA_Gain_20;
	OPA_InitStruct.OPA_BiasVoltage = OPA_BiasVoltage_EN;
	OPA_InitStruct.OPA_FeedbackRes = OPA_FeedbackRes_EN;
	OPA_InitStruct.OPA_Opoto_Gpio = OPA_OPOEX_EN;//PA2 AD2
	OPA_InitStruct.OPA_Opoto2_Gpio = OPA_OPOEX2_DISEN;	
	OPA_InitStruct.OPA_OppRES_short = OPA_OppRES_short_DISEN;
	OPA_InitStruct.OPA_OpnRES_short = OPA_OpnRES_short_DISEN;
	OPA_InitStruct.OPA_BiasVoltageSource = OPA_OPA_BiasVoltageSource_HALFVRH;
	OPA_Init(OPA2, &OPA_InitStruct);
	OPA_Cmd(OPA2, ENABLE);
}
