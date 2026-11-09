//
//  SysProtection.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "function.h"
#include "Math.h"
#include "SpeedLoop.h"
#include "Estimation.h"
#include "User_PeripheralInit.h"
#include "lcm32f039_tim.h"
#include "SysProtection.h"
#include "Pump_Comm.h"
#include "Pump_Function.h"
#include "SVPWM.h"
#include "ADC_Init.h"


void SysProtection(void);
void SoftwareOC(void);

void UnderOrOverVoltPro(void);
void StallProtection(void);
void OutputPhasePro(void);
void OverPowerPro(void);

void ProtectParaInit(void);

extern vu32  ADCConvertedRawData[ADC_SIZE];

void SysProtection(void)
{
	#if (SOFTWARE_OCP_EN == 1)
		SoftwareOC();
	#endif
	
	#if (OV_UV_PRO_EN == 1)
		UnderOrOverVoltPro();
	#endif
}

void ProtectParaInit(void)//过压一直执行，不允许初始化,不执行保护才初始化
{
	
}

void SoftwareOC(void)
{
#define OC_AD_VALUE  800//500//1638代表30A，目前满载相电流峰值9A左右
	
	if(
			(SysPara.CurrFirstAD > OC_AD_VALUE) || \
			(SysPara.CurrFirstAD < -OC_AD_VALUE) || \
			(SysPara.CurrSencondAD > OC_AD_VALUE) || \
			(SysPara.CurrSencondAD < -OC_AD_VALUE) || \
			(SysPara.CurrThirdAD > OC_AD_VALUE) || \
			(SysPara.CurrThirdAD < -OC_AD_VALUE)
		)
	{
		#if (FAST_OCP_EN == 0)
			SysPara.OC_cnt++;
			
			if(SysPara.OC_cnt > 0)  //8k  4对应0.5ms
			{
				SysPara.OC_cnt = 0;
				
				PWMOutputs_DISABLE();//关PWM	
				SysPara.StateMachineFlag = SysFault;		
				debug.FaultFlag = OverCurrent;
				
				//RELAY_OFF;

				PumpDisplay.SysFaultBit = PowerFault;
				
				debug.FaultDetial[0] = SysPara.CurrFirstAD;
				debug.FaultDetial[1] = SysPara.CurrSencondAD;
				debug.FaultDetial[2] = SysPara.CurrThirdAD;
				debug.FaultDetial[3] = SysPara.DcBusFiltered;
				debug.FaultDetial[4] = SysPara.MotorSpeed;		
			}
		#else
			PWMOutputs_DISABLE();//关PWM	
			SysPara.StateMachineFlag = SysFault;		
			debug.FaultFlag = OverCurrent; 
			
			//RELAY_OFF;
			
			debug.FaultDetial[0] = SysPara.CurrFirstAD;
			debug.FaultDetial[1] = SysPara.CurrSencondAD;
			debug.FaultDetial[2] = SysPara.CurrThirdAD;
			debug.FaultDetial[3] = SysPara.DcBusFiltered;
			debug.FaultDetial[4] = SysPara.DcBusAD;
		#endif
	}
	else
	{
		SysPara.OC_cnt = 0;
	}
}

void UnderOrOverVoltPro(void)
{	
#define OV_AD_VALUE  (1200)//935对应24V 
#define UV_AD_VALUE  (850)      

	
	if(
		(SysPara.DcBusAD > OV_AD_VALUE) || \
		((SysPara.DcBusAD < UV_AD_VALUE) && (SysPara.StateMachineFlag == MotorRun))
	  )
	{
		SysPara.OV_cnt++;
		
		if(SysPara.OV_cnt > 0)  //8k  4对应0.5ms
		{
			SysPara.OV_cnt = 0;
			
			PWMOutputs_DISABLE();   //关PWM	
			SysPara.StateMachineFlag = SysFault;	

			PumpDisplay.SysFaultBit = PowerFault;
			
			//RELAY_OFF;
			
			debug.FaultFlag = OverVolt;
			debug.FaultDetial[0] = SysPara.DcBusAD;
			debug.FaultDetial[1] = SysPara.MotorSpeed;
			debug.FaultDetial[2] = SysPara.IdFilter;
			debug.FaultDetial[3] = SysPara.IqFilter;
			debug.FaultDetial[4] = SysPara.DcBusAD;	
		}
	}
	else
	{
		SysPara.OV_cnt = 0;
	}
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
