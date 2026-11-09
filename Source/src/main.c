//
//  main.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
//  

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
#include "User_PeripheralInit.h"
#include "lcm32f039_type.h"
#include "function.h"
#include "lcm32f039_div.h"
#include "IfControl.h"
#include "Pump_Comm.h"
#include "Pump_Function.h"
#include "CurrentLoop.h"
#include "Estimation.h"
#include "SVPWM.h"
#include "ADC_Init.h"
#include "Timer_Init.h"
#include "GPIO_Init.h"
#include "OPA_Init.h"

u16 MainLoop_Count = 0;
u8 u8_MainLoop_Count = 0;

int main()
{
	SystemInit();
	SetSysClock(); //÷˜∆µ≈‰÷√
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ANACTRL, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC, ENABLE);
	
	SysTick_Config(UPDS_SYS_TICK_PERIOD);
	GPIO_Config();
	DAC_Config();	

	OPA_Config();		
	ADC_Config();

	ACMP_Config();	
	
	TIM1_Config();
	
	PWMOutputs_DISABLE();
	
	TIM14_Config();
	
	NVIC_Config();
	//UART_Config();
	UART3_Config();
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DIV, ENABLE);
	
	UART_SendData(UART3,0x55);
	
	Invter_Init();
		
	Update_PWM();	

	PowerOnInit();
	
	SysPara.StateMachineFlag = MotorStandBy;
	
	Pump_CommInit(UART1_WriteBytes,0x01);
	
	while (1)
	{		
		MainLoop_Count++;

#if 0		
		if(MainLoop_Count == 0)
		{
			u8_MainLoop_Count++;
			UART_SendData(UART1, u8_MainLoop_Count);
		}
#endif	
		
		CommDataFunc();

		switch(SysPara.StateMachineFlag)
		{    			
			case MotorStandBy:
				
				MOTOR_STANDBY_FLAG;
			
				if(START_MOTOR_COMMAND == 1)
				{	
					SysPara.StartDCmodeEn = 0;			
					PWMOutputs_ENABLE();
					MOTOR_RUN_FLAG;	
					SysPara.StateMachineFlag = MotorPrePos;//IfStartMode;//MotorRun;						
				}
				else
				{						
					PWMOutputs_DISABLE();
					MOTOR_STANDBY_FLAG;
					MOTOR_NO_FAULT_SET;
				}
				break;
					
			case MotorPrePos:
				if(STOP_MOTOR_COMMAND == 1)
				{
					SysPara.StateMachineFlag = MotorStandBy;
					PWMOutputs_DISABLE();
				}
											
				break;
									
			case MotorRun:
				
			  MOTOR_RUN_FLAG;
			
				if(STOP_MOTOR_COMMAND == 1)
				{
					SysPara.StateMachineFlag = MotorStandBy;
					PWMOutputs_DISABLE();
				}
				
				break;
					
			case SysFault:
				MOTOR_FAULT_SET;
				break;
				
			case IfStartMode:
				
				if(STOP_MOTOR_COMMAND == 1)
				{
					SysPara.StateMachineFlag = MotorStandBy;
					PWMOutputs_DISABLE();
				}
				break;
										 
			default: 
				SysPara.StateMachineFlag = MotorStandBy;
				PWMOutputs_DISABLE();				
				break;         
		} 
	}	
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/

