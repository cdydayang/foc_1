//
//  Estimation.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "CurrentLoop.h"
#include "Math.h"
#include "function.h"
#include "Estimation.h"
#include "IfControl.h"
#include "PI_Regulator.h"

Observer_t SmoEsti;

void Pos_Esti_Init(void);
void Pos_Esti_Calc(void);

/*******************************************************************************
* Function Name  : Pos_Esti_Init
* Description    : Observer initilization
*******************************************************************************/
void Pos_Esti_Init(void)
{
#define PLL_BW      0.2//10Hz
	
	SmoEsti.SmoPLL.qRef = 0;
	SmoEsti.SmoPLL.qFdb = 0;
	SmoEsti.SmoPLL.qAngleOut = 0;
	SmoEsti.SmoPLL.qSpeedOut = 0;
	SmoEsti.SmoPLL.Ui = 0;
	
	SmoEsti.SmoPLL.qOutMax = 20000;
	SmoEsti.SmoPLL.qOutMin = -20000;
	
	SmoEsti.SmoPLL.qKp = (s16)(PLL_BW*1024*2);//Q10;
	SmoEsti.SmoPLL.qKi = ((s32)(PLL_BW*PLL_BW*16384*SysPara.TsPu))>>17;//Q14
	
	SmoEsti.EstiIalpha = 0;
 	SmoEsti.EstiIbeta = 0;

	SmoEsti.preUalpha = 0;
	SmoEsti.preUbeta = 0;
	
	SmoEsti.preIalpha = 0;
	SmoEsti.preIbeta = 0;
	
	SmoEsti.BEMF_Alpha = 0;
	SmoEsti.BEMF_Beta = 0;
	SmoEsti.BEMF_1st_Alpha = 0;
	SmoEsti.BEMF_1st_Beta = 0;
	SmoEsti.BEMF_2nd_Alpha = 0;
	SmoEsti.BEMF_2nd_Beta = 0;
	
	SmoEsti.CurrGain = 14171;//Q14

	SmoEsti.VoltGain = 4577;//Q15
	
	SmoEsti.SgnGain = 500;//Q10
	
	SmoEsti.BEMF_SinValue = 0;
	SmoEsti.BEMF_CosValue = -32767;
	SmoEsti.Angle_SinValue = 0;
	SmoEsti.Angle_CosValue = -32767;
}

/*******************************************************************************
* Function Name  : Pos_Esti_Calc
* Description    : Observer calculation
*******************************************************************************/
void Pos_Esti_Calc(void)
{
#define LPF_COFE 72
	
	s32 temp1, temp2, temp3;
	
	Trig_Components Local_Vector_Components;

	temp1 = SmoEsti.preUalpha - SmoEsti.BEMF_1st_Alpha;//Q14
	temp2 = (temp1*SmoEsti.VoltGain)>>15;//Q14
	
	temp1 = (SmoEsti.preIalpha*SmoEsti.CurrGain)>>14;//Q14
	SmoEsti.EstiIalpha = temp1 + temp2;
	SmoEsti.BEMF_Alpha = ((SmoEsti.EstiIalpha - ipark1.Alpha)*SmoEsti.SgnGain)>>10;//Q14
	
	temp1 = SmoEsti.preUbeta - SmoEsti.BEMF_1st_Beta;//Q14
	temp2 = (temp1*SmoEsti.VoltGain)>>15;//Q14
	
	temp1 = (SmoEsti.preIbeta*SmoEsti.CurrGain)>>14;//Q14
	SmoEsti.EstiIbeta = temp1 + temp2;
	SmoEsti.BEMF_Beta = ((SmoEsti.EstiIbeta - ipark1.Beta)*SmoEsti.SgnGain)>>10;//Q14
	
	SmoEsti.BEMF_Alpha = RangeLimit((-20000), SmoEsti.BEMF_Alpha, 20000);//Q14
	SmoEsti.BEMF_Beta = RangeLimit((-20000), SmoEsti.BEMF_Beta, 20000);//Q14
	
	SmoEsti.BEMF_1st_Alpha = (SmoEsti.BEMF_1st_Alpha*(256-LPF_COFE) + LPF_COFE*SmoEsti.BEMF_Alpha)>>8;//Q14
	SmoEsti.BEMF_1st_Beta = (SmoEsti.BEMF_1st_Beta*(256-LPF_COFE) + LPF_COFE*SmoEsti.BEMF_Beta)>>8;//Q14
	SmoEsti.BEMF_2nd_Alpha = (SmoEsti.BEMF_2nd_Alpha*(256-LPF_COFE) + LPF_COFE*SmoEsti.BEMF_1st_Alpha)>>8;//Q14
	SmoEsti.BEMF_2nd_Beta = (SmoEsti.BEMF_2nd_Beta*(256-LPF_COFE) + LPF_COFE*SmoEsti.BEMF_1st_Beta)>>8;//Q14
	
	temp1 = SmoEsti.BEMF_2nd_Alpha>>1;//Q13
	temp2 = SmoEsti.BEMF_2nd_Beta>>1;//Q13
		
  temp3 = (temp1*temp1) + (temp2*temp2);//Q26
	
	temp1 = MCM_Sqrt(temp3);//Q13
	
	SmoEsti.BEMF_SinValue = (s16)((SmoEsti.BEMF_2nd_Alpha<<13)/temp1);//Q13
	SmoEsti.BEMF_CosValue = (s16)((SmoEsti.BEMF_2nd_Beta<<13)/temp1);//Q13
	
	temp1 = -((SmoEsti.BEMF_SinValue*SmoEsti.Angle_CosValue)>>13);//Q15
	temp2 = (SmoEsti.BEMF_CosValue*SmoEsti.Angle_SinValue)>>13;//q15
	temp1 -= temp2;
		
	SmoEsti.SmoPLL.Error = -temp1;
	
	PLL_CALC(&SmoEsti.SmoPLL);
	
	Local_Vector_Components = MCM_Trig_Functions(SmoEsti.SmoPLL.qAngleOut);
	SmoEsti.Angle_SinValue = Local_Vector_Components.hSin;
	SmoEsti.Angle_CosValue = Local_Vector_Components.hCos;
	
//	SysPara.MotorAngle = SmoEsti.SmoPLL.qAngleOut;
//	SysPara.MotorSpeed = SmoEsti.SmoPLL.qSpeedOut;
//	
//	SysPara.VoltAngle = SysPara.MotorAngle;
	
	SmoEsti.preUalpha = ipark1.Alpha;
	SmoEsti.preUbeta = ipark1.Beta;
	SmoEsti.preIalpha = clarke1.Alpha;
	SmoEsti.preIbeta = clarke1.Beta;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
