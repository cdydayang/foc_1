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
#include "Timer_Init.h"
#include "lcm32f039_type.h"
#include "SysProtection.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the TIM1 Pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_Config(void)
{
		/* TIM1 clock enable */

		TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure;
		GPIO_InitTypeDef 					GPIO_InitStructure;

		TIM_OCInitTypeDef  				TIM_OCInitStructure;
		TIM_BDTRInitTypeDef				TIM_BDTRInitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1 , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM15 , ENABLE);
		/* Time Base configuration */
		TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2;
		TIM_TimeBaseStructure.TIM_Period = UP_PWM_PERIOD;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;

		// Initial condition is REP=0 to set the UPDATE only on the underflow
		TIM_TimeBaseStructure.TIM_RepetitionCounter = UP_REP_RATE;
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;                  
		TIM_OCInitStructure.TIM_Pulse = 0x505*2/3; //dummy value
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;          

		TIM_OC1Init(TIM1, &TIM_OCInitStructure); 
		TIM_OC2Init(TIM1, &TIM_OCInitStructure);
		TIM_OC3Init(TIM1, &TIM_OCInitStructure);


		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
		TIM_OCInitStructure.TIM_Pulse = 1;//0;//1;//UP_PWM_PERIOD-1; //dummy value
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		TIM_OC4Init(TIM1, &TIM_OCInitStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
		TIM_OCInitStructure.TIM_Pulse = 1;//UP_PWM_PERIOD-1; //dummy value
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		TIM_OC5Init(TIM1, &TIM_OCInitStructure);


		/* Enables the TIM1 Preload on CC1 Register */
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
		/* Enables the TIM1 Preload on CC2 Register */
		TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
		/* Enables the TIM1 Preload on CC3 Register */
		TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
		/* Enables the TIM1 Preload on CC4 Register */
		TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
		/* Enables the TIM1 Preload on CC5 Register */
		TIM_OC5PreloadConfig(TIM1, TIM_OCPreload_Enable);

		//设置CMP刹车
		//		sysctrl_access();
		//		SYSCTRL->EDU_CFG4 |= (1 << 9); //比较器ACMP1作为刹车源, 触发break1
		//		__dekey()

		sysctrl_access();
		SYSCTRL->EDU_CFG4 |= (1 << 17); //比较器ACMP1作为刹车源，触发break2
		__dekey()


		/* Automatic Output enable, Break, dead time and lock configuration	 */
		TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
		TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
		TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;//TIM_LOCKLevel_1; //
		TIM_BDTRInitStructure.TIM_DeadTime = UP_DEADTIME;
		TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
		TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
		TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

		TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

		//	TIM1->BDTR |=(3<<24);

		//		#if(Hardware_OCP_EN == 1)
		//		TIM1->BDTR |=(1<<25);  //  配置刹车极性为高电平有效
		//		TIM1->BDTR |=(0<<26);  //  AOE2不使能
		//		TIM1->BDTR |=(1<<24);  //  刹车2 BK2E使能
		//		#endif

		/* Main Output Enable */
		TIM_CtrlPWMOutputs(TIM1, ENABLE);

		/*使能预装载*/
		TIM_ARRPreloadConfig(TIM1, ENABLE);

		//  	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

		TIM1->CR2 |= TIM_MMS_OC4;

		TIM_ITConfig(TIM1, TIM_DIER_BIE, ENABLE); //刹车中断BIE使能
		//  	/* TIM1 counter enable */
		//  	TIM_Cmd(TIM1, ENABLE);

		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Enable);
		TIM_CCxCmd(TIM1, TIM_Channel_5, TIM_CCx_Enable);

		TIM_ClearITPendingBit(TIM1, TIM_IT_CC5);//开中断前先清标志位
		TIM_ITConfig(TIM1, TIM_IT_CC5, ENABLE); //使能比较中断	

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_5); // TIM1_CH1   	pB7    
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_3); // TIM1_CH2   	PB5   
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_4); // TIM1_CH3 		PB3    

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_6); //	TIM1_CH1N   pB6
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_4); //	TIM1_CH2N   PB4    
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_7); //TIM1_CH3N   PA15   

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_3 |GPIO_Pin_5  | GPIO_Pin_4 | GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		


		/******  TEST  *******/		
		//	  TIM1->CCR1 = 200;
		//	  TIM1->CCR2 = 200;
		//	  TIM1->CCR3 = 200;

}

void BreakEnable(void)
{
	TIM1->BDTR |= (TIM_Break_Enable);//Bit 12 BKE: Break enable;必须打开，才能软件或硬件关闭PWM
	//清除TIM1的中断待处理位
  TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
	
	//使能指定的TIM中断
  TIM_ITConfig(TIM1, TIM_IT_Break,ENABLE);
}

void BreakDisable(void)
{
	TIM1->BDTR &= (~TIM_Break_Enable);//Bit 12 BKE: Break enable;必须打开，才能软件或硬件关闭PWM
}

void TIMCounterStart(void)
{
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
}

void TIMCounterStop(void)
{
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, DISABLE);
}


void TIM3Configuration(void)
{

	
}

void TIM14_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

//	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	RCC_APB0PeriphClockCmd(RCC_APB0Periph_TIM14, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = 191; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 1000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM14, ENABLE);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM14, ENABLE);
}
