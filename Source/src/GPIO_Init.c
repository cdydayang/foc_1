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
#include "GPIO_Init.h"
#include "lcm32f039_type.h"
#include "SysProtection.h"

void GPIO_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStruct;
	  /* 第1步：打开 GPIO 的时钟 */	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF , ENABLE);

//UVW 上下桥控制
	GPIO_ResetBits(GPIOB, GPIO_Pin_7 );
	GPIO_ResetBits(GPIOB, GPIO_Pin_5 );
	GPIO_ResetBits(GPIOB, GPIO_Pin_3 );
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_6 );
	GPIO_ResetBits(GPIOB, GPIO_Pin_4 );
	GPIO_ResetBits(GPIOA, GPIO_Pin_15 );

/* PA3 analog input: DC-bus voltage ADC channel. */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
    gpio.GPIO_Pin = GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_Speed = GPIO_Speed_Level_2;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    //PF8 start key
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF,&GPIO_InitStruct);

//PC13  stop key
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

//PA1  旋钮  AD1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1, GPIO_AF_1);  
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

//PB0  BEMFV  AD8
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0, GPIO_AF_1);  
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

//PA8  BEMFW  AD10
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8, GPIO_AF_2);  
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
//PA6  MOS NTC  AD6
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6, GPIO_AF_1);  
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	//PA3  BUS  AD3
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_1);  
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//PA4  LED
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//PA12  LED
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
