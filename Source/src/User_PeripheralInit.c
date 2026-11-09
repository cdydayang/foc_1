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
#include "User_PeripheralInit.h"
#include "lcm32f039_type.h"
#include "SysProtection.h"

void UART_Config(void)
{

	UART_InitTypeDef UART_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART1, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB0Periph_UART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
//	RCC_UART1CLKConfig(RCC_UART1CLK_RCH); //选择UART1时钟源，不配置默认总线时钟
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_6);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_6);
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_14 | GPIO_Pin_15;	//	PB14--UART1.Tx  PB15--UART1.Rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.UART_BaudRate = 9600;//115200;
	UART_InitStruct.UART_WordLength = UART_WordLength_8b;
	UART_InitStruct.UART_StopBits = UART_StopBits_1;
	UART_InitStruct.UART_Parity = UART_Parity_No;
	UART_InitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
	UART_InitStruct.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;   
	UART_Init(UART1, &UART_InitStruct);
	UART_ITConfig(UART1, UART_IT_TX, ENABLE); //发送中断使能
	UART_ITConfig(UART1, UART_IT_RX, ENABLE); //接收中断使能
	UART_ClearITPendingBit(UART1, UART_IT_TX);
	UART_ClearITPendingBit(UART1, UART_IT_RX);
	UART_Cmd(UART1, ENABLE);
	
	NVIC_ClearPendingIRQ(UART1_IRQn);
	NVIC_SetPriority(UART1_IRQn, 3);
	NVIC_EnableIRQ(UART1_IRQn);
	
//	RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART3, ENABLE);
//	//RCC_APB1PeriphClockCmd(RCC_APB0Periph_UART3, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
////	RCC_UART1CLKConfig(RCC_UART1CLK_RCH); //选择UART1时钟源，不配置默认总线时钟
//	
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_4);
//	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;	//	PB14--UART1.Tx  PB15--UART1.Rx
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
//	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOB, &GPIO_InitStruct);	
//	
//	UART_StructInit(&UART_InitStruct);
//	UART_InitStruct.UART_BaudRate = 9600;//115200;
//	UART_InitStruct.UART_WordLength = UART_WordLength_8b;
//	UART_InitStruct.UART_StopBits = UART_StopBits_1;
//	UART_InitStruct.UART_Parity = UART_Parity_No;
//	UART_InitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
//	UART_InitStruct.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;   
//	UART_Init(UART3, &UART_InitStruct);
//	UART_ITConfig(UART3, UART_IT_TX, ENABLE); //发送中断使能
//	UART_ITConfig(UART3, UART_IT_RX, ENABLE); //接收中断使能
//	UART_ClearITPendingBit(UART3, UART_IT_TX);
//	UART_ClearITPendingBit(UART3, UART_IT_RX);
//	UART_Cmd(UART3, ENABLE);
//	
//	NVIC_ClearPendingIRQ(UART3_IRQn);
//	NVIC_SetPriority(UART3_IRQn, 2);
//	NVIC_EnableIRQ(UART3_IRQn);

}

void DAC_Config(void)
{	
	DAC_InitTypeDef DAC_InitStruct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_ANACTRL,ENABLE);
	
	DAC_InitStruct.DAC_EXTrigger_Edge = DAC_EXTrigger_edge_dis;
	DAC_InitStruct.DAC_Trigger_Source = DAC_Trigger_Software;
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStruct.DAC_OutputBuffer = DAC0_OutputBuffer_Enable;
	DAC_InitStruct.DAC_Vref_Select = DAC_Vref_Avdd;
	DAC_InitStruct.DAC_DmaMode = DISABLE;
	DAC_InitStruct.DAC_DMAUDR_IE = DISABLE;
	DAC_Init(DAC0, &DAC_InitStruct);

	DAC_Cmd(DAC0, ENABLE);


	DAC_SetDac_10B_Data(DAC0, DAC_Align_10B_R, 840);//594 10A， 676 20A 850/1024*4等于比较值,直流设计成10A过流先
	DAC_SoftwareTriggerCmd(DAC0, ENABLE);

}



void ACMP_Config(void)
{
 
 ACMP_InitTypeDef ACMP_InitStruct;

 ACMP_InitStruct.ACMP_P_Select = CP1_PS_CPP1EXT_INPUT; //第一级外部   正端输入选择为 运放OPA1-out
 ACMP_InitStruct.ACMP_N_Select = CP1_NS_DAC0OUT;    //DAC0       负端输入选择为 DAC0-out
 ACMP_InitStruct.ACMP_Delay_time = CP_NODelay_time;   //输出延迟配置
 ACMP_InitStruct.ACMP_Blanking = CP_NOBLANKING;    //比较器消隐源选择
 ACMP_InitStruct.ACMP_Is = CP_IS_POS; // CP_IS_REV;//      //比较器输出极性
 ACMP_InitStruct.ACMP_HYSEN = ENABLE;//DISABLE;// ;//     //迟滞使能配置
 ACMP_InitStruct.ACMP_FREN = ENABLE;       //快速响应使能
 ACMP_InitStruct.ACMP_FS = DISABLE;       //输出滤波使能配置

 ACMP_InitStruct.CMP_INTENMASK = DISABLE;    //比较器连续比较中断使能屏蔽
 ACMP_InitStruct.CMP_SEQ_MODE = DISABLE;     //连续模式使能（非连续采样）
 ACMP_InitStruct.TRIG_MODE = CP_TIRG_INASOFT;   //触发模式：软件或者外部硬件触发  /软件触发
 ACMP_InitStruct.CHNL_CNT_TIME = CHNL_CNT_TIME_64CLK; //比较器通道切换时钟周期选择
 ACMP1_Init(&ACMP_InitStruct);
 ACMP1_SEQ_INPUT_SELECT(DISABLE, 5, 1); //101:，OPA1OUT;  DAC1 as negative
 ACMP1_Cmd(ENABLE);

// SATRT_OFACMP1(); //only soft trigger need

}

void UART3_Config(void)
{
	UART_InitTypeDef UART_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_4);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // PB10--UART3.Tx  PB11--UART3.Rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*选择UART3时钟源，不配置默认总线时钟
	/Select the UART3 clock source and do not configure the default bus clock*/
	//RCC_UART3CLKConfig(RCC_UART3CLK_RCH);

	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.UART_BaudRate = 9600;
	UART_InitStruct.UART_WordLength = UART_WordLength_8b;
	UART_InitStruct.UART_StopBits = UART_StopBits_1;
	UART_InitStruct.UART_Parity = UART_Parity_No;
	UART_InitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
	UART_InitStruct.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
	UART_Init(UART3, &UART_InitStruct);
	UART_ITConfig(UART3, UART_IT_TX, ENABLE); //发送中断使能 /Send the interrupt enable
	UART_ITConfig(UART3, UART_IT_RX, ENABLE); //接收中断使能 /receive interruption enable
	UART_ClearITPendingBit(UART3, UART_IT_TX);
	UART_ClearITPendingBit(UART3, UART_IT_RX);
	UART_Cmd(UART3, ENABLE);

	NVIC_ClearPendingIRQ(UART3_IRQn);
	NVIC_SetPriority(UART3_IRQn, 3);
	NVIC_EnableIRQ(UART3_IRQn);
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_NON_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_ClearPendingIRQ(TIM1_NON_CC_IRQn);
	NVIC_EnableIRQ(TIM1_NON_CC_IRQn);
	
	NVIC_ClearPendingIRQ(TIM1_CC_IRQn);	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_ClearPendingIRQ(DMAC_CH1_2_IRQn);
	NVIC_EnableIRQ(DMAC_CH1_2_IRQn);

	NVIC_ClearPendingIRQ(DMAC_CH1_2_IRQn);   
	NVIC_InitStructure.NVIC_IRQChannel = DMAC_CH1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*Open TIM1 counter*/
	TIM_Cmd(TIM1, ENABLE);

}
