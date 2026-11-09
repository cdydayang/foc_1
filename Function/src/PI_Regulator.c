//
//  PI_Regulator.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "PI_Regulator.h"
#include "Math.h"
#include "function.h"
#include "Estimation.h"
#include "IfControl.h"
#include "User_PeripheralInit.h"

PI_t IdLoop;
PI_t IqLoop;

PI_t SpeedLoop;

void PI_Calc(PI_t *v);

/*******************************************************************************
* Function Name  : PI_Calc
* Description    : PI regulator calculation
*******************************************************************************/
void PI_Calc(PI_t *v)
{	
	s32 temp;        
	s32 temp1, temp2;
        
	temp1 = (v->OutMin<<14);  //Q28
	temp2 = (v->OutMax<<14);	//Q28
	
	v->Err = v->Ref - v->Fdb;//Q14
        
	v->Err = RangeLimit(-32700, v->Err, 32700);   //(-2,err,2)
	
	v->Up = (v->Kp * v->Err)>>10;//Q14
        
	v->Up = RangeLimit(-32700, v->Up, 32700);//Q14
	
	v->Ui += (v->Ki* v->Err);//Q28
	
	v->Ui = RangeLimit(temp1, v->Ui, temp2);//Q28
		
	temp = v->Up + (v->Ui>>14);//Q14
        				
	v->Out = RangeLimit(v->OutMin, temp, v->OutMax);//Q14
}

void PLL_CALC(PLL_t *v)
{
	s32  Up;
	s16  temp;        
	s32  temp1, temp2;
	s32  temp3;

	temp1 = (s32)(v->qOutMin<<14);
	temp2 = (s32)(v->qOutMax<<14);
	
	//Calc proportional
	Up = ((s32)(v->qKp * v->Error))>>10;//q14
	
	Up = RangeLimit(-32700, Up, 32700);//Q14
	
	//Calc integral
	v->Ui += ((s32)((v->qKi* v->Error)));//q28
	
	//Limit ui 
	v->Ui = RangeLimit(temp1, v->Ui, temp2);//q28
		
	//Out = Up + Ui 
	temp3 = Up + (v->Ui>>14);//q14
				
	//Limit speed output
	v->qSpeedOut = RangeLimit(v->qOutMin, temp3, v->qOutMax);//q14
	
	temp = (((v->qSpeedOut * SysPara.TsPu)>>16)*10430)>>15;//-32768~32767 defines -pi~pi
	temp += v->qAngleOut;
	v->qAngleOut = (s16)temp;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
