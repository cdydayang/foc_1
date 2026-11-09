//
//  CurrentLoop.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "CurrentLoop.h"
#include "Math.h"
#include "function.h"
#include "Estimation.h"
#include "IfControl.h"
#include "User_PeripheralInit.h"
#include "PI_Regulator.h"
#include "SpeedLoop.h"

void Inside_Loop_Init(void);
void InsideLoopCalc(void);

/*******************************************************************************
* Function Name  : Inside_Loop_Init
* Description    : Current loop parameters initilization
*******************************************************************************/
void Inside_Loop_Init(void)
{		
	IdLoop.Kp = 1151;
	IdLoop.Ki = 2488;

	IqLoop.Kp = 1151;
	IqLoop.Ki = 2488;
	
	IdLoop.OutMax = MAX_AMP;
	IdLoop.OutMin = -MAX_AMP;

	IdLoop.Ui = 0;
	IdLoop.Out = 0;	
	
	IdLoop.Ref = 0;
	IdLoop.Fdb = 0;
	IdLoop.Err = 0;
	IdLoop.Up = 0;
	
	IqLoop.OutMax = MAX_AMP;
	IqLoop.OutMin = -MAX_AMP;

	IqLoop.Ui = 0;
	IqLoop.Out = 0;  
	
	IqLoop.Ref = 0;
	IqLoop.Fdb = 0;
	IqLoop.Err = 0;
	IqLoop.Up = 0;
}

/*******************************************************************************
* Function Name  : InsideLoopCalc
* Description    : Current loop calculation
*******************************************************************************/
void InsideLoopCalc(void)
{	
	IdLoop.OutMax = MAX_AMP;
	IdLoop.OutMin = -IdLoop.OutMax;//Q14
	
	IqLoop.OutMax = MAX_AMP;
	IqLoop.OutMin = -IqLoop.OutMax;
	
	IdLoop.Ref = SysPara.IdRef;//Q14
	IdLoop.Fdb = SysPara.IdFilter;//Q14
	PI_Calc(&IdLoop);
	
	if((SysPara.StateMachineFlag == MotorPrePos)||(SysPara.StateMachineFlag == IfStartMode))
	{
		IqLoop.Ref = IfStart.IqRef;//Q14
	}
	else
	{
		IqLoop.Ref = SpeedLoop.Out;//Q14
	}
	
	//IqLoop.Ref = 0; 

	IqLoop.Fdb = SysPara.IqFilter;//Q14
	PI_Calc(&IqLoop);
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
