//
//  SpeedLoop.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "SpeedLoop.h"
#include "Math.h"
#include "function.h"
#include "Estimation.h"
#include "IfControl.h"
#include "User_PeripheralInit.h"
#include "PI_Regulator.h"
#include "ADC_Init.h"

void OutsideLoopInit(void);
void OutsideLoopCalc(void);

void Id_Calc(void);

/*******************************************************************************
* Function Name  : OutsideLoopInit
* Description    : Speed loop parameters initilization
*******************************************************************************/
void OutsideLoopInit(void)
{
	SysPara.Speed_Loop_Kp = SPEED_LOOP_KP;
	SysPara.Speed_Loop_Ki = SPEED_LOOP_KI;
	
	SpeedLoop.Kp = SysPara.Speed_Loop_Kp;
	SpeedLoop.Ki = SysPara.Speed_Loop_Ki;    
	SpeedLoop.OutMax = 3000;
	SpeedLoop.OutMin = -100; 

	SpeedLoop.Ui = 0;
	SpeedLoop.Out = 0;
	
	SpeedLoop.Ref = 0;
	SpeedLoop.Fdb = 0;
	SpeedLoop.Err = 0;
	SpeedLoop.Up = 0;
	
	SysPara.SpeedLoopCnt = 0;	
	SysPara.RefSpeed = 0;
	SysPara.RefSpeedFilter = 0;
}

/*******************************************************************************
* Function Name  : OutsideLoopCalc
* Description    : Outside loop calculation
*******************************************************************************/
void OutsideLoopCalc(void)
{	
	if(SysPara.StartDCmodeEn == 1)
	{
		SysPara.RefSpeed = 0;
		SysPara.RefSpeedFilter = 0;
		SpeedLoop.Ref = 0;
		SpeedLoop.Fdb = 0;
		SpeedLoop.Ui = 0;
		SpeedLoop.Out = 0;
	}
	else
	{	
		debug.CommandSpeed = MOTOR_SPEED_REF;
		
		if(debug.CommandSpeed < 2000)//最低1000rpm
		{
			debug.CommandSpeed = 2000;
		}
		
		debug.CommandSpeed = RangeLimit(-SPEED_LIMIT, debug.CommandSpeed, SPEED_LIMIT);	
					
		if(labs(SysPara.RefSpeed - MAXSPEED) < (Speed_Step<<1))
		{
			SysPara.RefSpeed = MAXSPEED;			
		}
		else if (SysPara.RefSpeed < MAXSPEED)
		{
			if(MAXSPEED >= 0)
			{
				SysPara.RefSpeed += Speed_Step;
			}
			else
			{
				SysPara.RefSpeed += (Speed_Step>>1);
			}
		}
		else
		{
			if(MAXSPEED <= 0)
			{
				SysPara.RefSpeed -= Speed_Step;
			}
			else
			{
				SysPara.RefSpeed -= (Speed_Step>>1);
			}
		}		

		if((labs(SpeedLoop.Fdb)) < 800)
		{
			SysPara.RefSpeed = CMPMIN2(SysPara.RefSpeed, 1500);
		}
				
		SysPara.RefSpeed = RangeLimit(-SPEED_LIMIT, SysPara.RefSpeed, SPEED_LIMIT);	
		
		SysPara.RefSpeedFilter = (SysPara.RefSpeedFilter*768 + 256*SysPara.RefSpeed)>>10;
					
		SpeedLoop.Ref = SysPara.RefSpeedFilter;	
			
		SpeedLoop.Fdb = (SpeedLoop.Fdb*157 + 99*SysPara.MotorSpeed)>>8; //200Hz,2k
						
		/*不同速度段，限制不同的Iq幅值*/
		if(SpeedLoop.Fdb < 2048)
		{
			SpeedLoop.OutMax = 2000;
		}
		else if((SpeedLoop.Fdb < 4096) && (SpeedLoop.Fdb >= 2048))
		{
			SpeedLoop.OutMax = 5000 + (((SpeedLoop.Fdb - 4096)*3000)>>11);
		}
		else
		{
			SpeedLoop.OutMax = 5000;
		}
		
		PI_Calc(&SpeedLoop);
	}  	
}

/*******************************************************************************
* Function Name  : Id_Calc
* Description    : Id reference calculation
*******************************************************************************/
void Id_Calc(void)
{	
#define IdLowSpeed	2048
#define IdHighSpeed (IdLowSpeed+1024)
	
	if(SysPara.StartDCmodeEn == 1)
	{
		if(SysPara.IdRef < NoLoadIdAMP)
		{
			SysPara.IdRef += 100;
		}
		else
		{
			SysPara.IdRef = NoLoadIdAMP;
		}
	}
	else
	{
		if(labs(SpeedLoop.Fdb) < IdLowSpeed)
		{
			SysPara.IdRef = NoLoadIdAMP;
		}
		else if(
				((labs(SpeedLoop.Fdb)) >= IdLowSpeed) &&
				((labs(SpeedLoop.Fdb)) < IdHighSpeed)
			   )
		{
			SysPara.IdRef = (IdHighSpeed - (labs(SpeedLoop.Fdb)))>>1;	
		}
		else
		{
			SysPara.IdRef = 0;
		}
	}
		
	//SysPara.IdRef = NoLoadIdAMP;//NoLoadIdAMP;//NoLoadIdAMP;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
