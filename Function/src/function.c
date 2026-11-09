//
//  function.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
//  

#include "function.h"
#include "Math.h"
#include "CurrentLoop.h"
#include "Estimation.h"
#include "User_PeripheralInit.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_div.h"
#include "Pump_Function.h"
#include "SpeedLoop.h"
#include "PI_Regulator.h"
#include "ADC_Init.h"

SysPara_t SysPara;
debug_t debug;
CommDetail_t CommData;

void Driver_Para_Init(void);

void CommDataFunc(void);

void PowerOnInit(void);
	
extern vu32  ADCConvertedRawData[ADC_SIZE];

/*******************************************************************************
* Function Name  : Driver_Para_Init
* Description    : System parameters intilization
*******************************************************************************/
void Driver_Para_Init(void)
{
	SysPara.OCflag = 0;
	SysPara.DCmode_flag = 0;
	SysPara.DCmode_cnt = 0;
	
	SysPara.MotorAngle = -16384;//0;
	SysPara.VoltAngle = SysPara.MotorAngle;
	SysPara.MotorSpeed = 0;
	
	SysPara.VoltAlpha = 0;
	SysPara.VoltBeta = 0;
				
	SysPara.TsPu = 10294;//Q17
	SysPara.TsPuInv = 13038;//Q10
	
	SysPara.OC_cnt = 0;
	SysPara.OV_cnt = 0;
	
	SysPara.PrePosCnt = 0;
	
	SysPara.StartDCmodeEn = 0;
	SysPara.StartDCmodeCnt = 0;
		
	SysPara.IdRef = 0;
}

void DebugInit(void)
{
	debug.FaultFlag = 0;
	debug.FaultDetial[0] = 0;
	debug.FaultDetial[1] = 0;
	debug.FaultDetial[2] = 0;
	debug.FaultDetial[3] = 0;
	debug.FaultDetial[4] = 0;
}

void CommDataFunc(void)
{
	/*故障灯*/
	if(SysPara.StateMachineFlag == SysFault)
	{
		MOTOR_FAULT_SET;
	}
	else
	{
		MOTOR_NO_FAULT_SET;
	}

	/*手动或者自动启动模式*/
	
	/*NTC处理*/
	
	/*启动命令和参考速度处理*/
	
	/*任何时候记录上一状态，避免状态切换的错误*/
	CommData.StarLastButtonFlag = CommData.StarButtonFlag;
	
	/*外发显示参数*/
	/*直流电压*/

	/*直流电压*/
	PumpDisplay.DCvolt = (SysPara.DcBusFiltered*1386)>>13;

	/*电机速度*/
	PumpDisplay.MotorSpeedHz = (SpeedLoop.Fdb*1500)>>14;//单位1500Hz
	PumpDisplay.MotorSpeedRpm = (SpeedLoop.Fdb*5625)>>10;//防止溢出
	
	/*额定速度*/
	PumpDisplay.RatedSpeedHz = 1500;
	PumpDisplay.RatedSpeedRpm = 90000;
}

void PowerOnInit(void)
{
	SysPara.StateMachineFlag = MotorStandBy;	

	CommData.StartFlagCntEn = 0;
	CommData.u16PowerLevel = 0;
	CommData.LastPowerLevel = 0;
	CommData.u16PowerLevelCount = 0;

	debug.StartCommand = 0;
	debug.CommandSpeed = 2000;//12000;//16384;//16384代表9万转

	PumpDisplay.MOS_Temp = 0;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
