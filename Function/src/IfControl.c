//
//  IfControl.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "IfControl.h"
#include "function.h"
#include "Math.h"
#include "Estimation.h"
#include "SpeedLoop.h"
#include "CurrentLoop.h"
#include "PI_Regulator.h"

IfCurr_t IfStart;

void IfInit(void);
void IfAngleCalc(void);

/******************************************************************************
函数名：IfAngleCalc
描述: I-f mode initial
输入：无
输出：无
******************************************************************************/
void IfInit(void)
{
	IfStart.ElectAngle = -16384;
	IfStart.ReferSpeed = 0;
	IfStart.IqRef = 0;
}

/******************************************************************************
函数名：IfAngleCalc
描述: Angle calculation by i-f mode
输入：无
输出：无
******************************************************************************/
void IfAngleCalc(void)
{	
	s16 temp1;
	
	temp1 = (IfStart.ReferSpeed * SysPara.TsPu)>>16; //q15
	temp1 = (temp1 * 10430)>>15;	
	temp1 += IfStart.ElectAngle;
	IfStart.ElectAngle = (s16)temp1;
	
	SysPara.VoltAngle = IfStart.ElectAngle;
	SysPara.MotorAngle = SysPara.VoltAngle;
}

void IfSpeedCalc(void)
{
	if(IfStart.ReferSpeed < IfSwitchSpeed)
	{
		IfStart.ReferSpeed += 10;
		IfStart.IqRef = IfModeCurrAMP;
		SysPara.IdRef = 0;
	}
	else
	{
		IfStart.ReferSpeed = IfSwitchSpeed;
		
		if(IfStart.IqRef > IfModeSwitchIqAmp)
		{
			IfStart.IqRef -= CurrSwitchStep;
		}
		else
		{
			IfStart.IqRef = IfModeSwitchIqAmp;
		}	

		if(SysPara.IdRef < NoLoadIdAMP)
		{
			SysPara.IdRef += CurrSwitchStep;
		}
		else
		{
			SysPara.IdRef = NoLoadIdAMP;
		}				
	}
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
