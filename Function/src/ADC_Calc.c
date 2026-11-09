//
//  ADC_Calc.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "ADC_Calc.h"
#include "Math.h"
#include "CurrentLoop.h"
#include "Estimation.h"
#include "User_PeripheralInit.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_div.h"
#include "Pump_Function.h"
#include "SpeedLoop.h"
#include "PI_Regulator.h"
#include "function.h"
#include "ADC_Init.h"


void ADC_Sample_Calc(void);

extern vu32  ADCConvertedRawData[ADC_SIZE];

extern void ReadAdcResult(void);

//测量电压和三相电流
void ReadAdcResult(void)
{
	s16 temp1, temp2, temp3;

	temp1 = PHASE_IA_ADC_DATA;//U phase
	temp2 = PHASE_IC_ADC_DATA;//V phase
	
	if(SysPara.StateMachineFlag == MotorStandBy)
	{
		SysPara.CurrOffsetCnt++;

		if(SysPara.CurrOffsetCnt > 32)
		{
			SysPara.IuOffset = SysPara.IuSum>>5;
			SysPara.IwOffset = SysPara.IwSum>>5;
			SysPara.IDC_Offset = SysPara.IDC_Sum>>5;

			SysPara.CurrOffsetCnt = 0;

			SysPara.IuSum = 0;
			SysPara.IwSum = 0;
			SysPara.IDC_Sum = 0;
		}	
		else
		{
			SysPara.IuSum += temp1;
			SysPara.IwSum += temp2;
			SysPara.IDC_Sum += temp3;
		}	
	}
	else
	{
		SysPara.CurrOffsetCnt = 0;

		SysPara.IuSum = 0;
		SysPara.IwSum = 0;
		SysPara.IDC_Sum = 0;
	}
	
	SysPara.CurrFirstAD = -(temp1 - SysPara.IuOffset);//U phase
	SysPara.CurrThirdAD = -(temp2 - SysPara.IwOffset);//V phase
	SysPara.CurrSencondAD = -SysPara.CurrFirstAD - SysPara.CurrThirdAD;

	/*电池电压*/
	SysPara.DcBusAD = VDC_ADC_DATA;
}

void ADC_Sample_Calc(void)
{
	s16 DcBusPu;

	ReadAdcResult();
	
	/*电流采样接口，霍尔接反了*/			
	SysPara.CurrUphase = SysPara.CurrFirstAD;
	SysPara.CurrVphase = SysPara.CurrSencondAD;
	SysPara.CurrWphase = -SysPara.CurrUphase - SysPara.CurrVphase;//
	
	//2A对应1.0V对应819 ADC值，标幺值2A
	clarke1.As = SysPara.CurrUphase*20;
	clarke1.Bs = SysPara.CurrVphase*20;
	clarke1.Cs = SysPara.CurrWphase*20;
	
	CLARKE_MACRO1(&clarke1);
	
	//PARK变换
	park1.Alpha = clarke1.Alpha;
	park1.Beta = clarke1.Beta;
	park1.Theta = FOC_Angle;
	PARK_MACRO1(&park1);

	#if (SAMPLE_CURR_LPF_EN == 1)
	SysPara.IdFilter = (144*SysPara.IdFilter + 112*park1.Ds)>>8;//8k,1000Hz 	
	SysPara.IqFilter = (144*SysPara.IqFilter + 112*park1.Qs)>>8;//8k,1000Hz 
	#else
	SysPara.IdFilter = park1.Ds;//不滤波
	SysPara.IqFilter = park1.Qs;
	#endif
	
	DcBusPu = ((SysPara.DcBusAD*15522)>>10);//24V对应936
	SysPara.DcBusFiltered = (227*SysPara.DcBusFiltered + 29*DcBusPu)>>8;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
