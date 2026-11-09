/**
  ******************************************************************************
  * @file       Pump_Function.c
  * @author     qinxingpeng
  * @version    V1.0
  * @date       2023年04月23日21点46分
  * @brief      泵应用层
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef FANFUNCTION_H
#define FANFUNCTION_H

#include <stdint.h>
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif 

	typedef struct
{
	s16 MOS_Temp;
	s16 DCcurr;
	s16 DCvolt;
	
	s16 MotorSpeedHz;
	s32 MotorSpeedRpm;

	s16 RatedSpeedHz;
	s32 RatedSpeedRpm;
	
	s16 RefSpeedHz;
	s32 RefSpeedRpm;

	s16 MotorPower;
	
	s16 MCUtemp;

	s16 WindingTemp;	
	s16 PumpTemp;

	s16 ErrorCode;

	s16 DriverOT_Flag;
	s16 MotorOT_Flag;

	s16 SysFaultBit;
}MotorDisplayType_t;

extern MotorDisplayType_t PumpDisplay;


//电机参数写
extern void Pump_SetControlType(uint16_t value);
extern void Pump_Set_CMD_Source(uint16_t value);
extern void Pump_SetCommFre(uint16_t value);
extern void Pump_SetMotorMaxFre(uint16_t value);
extern void Pump_SetFreUpLimit(uint16_t value);
extern void Pump_SetMotorType(uint16_t value);
extern void Pump_SetRatedPower(uint16_t value);
extern void Pump_SetRatedVoltage(uint16_t value);
extern void Pump_SetRatedCurrent(uint16_t value);
extern void Pump_SetRatedFre(uint16_t value);
extern void Pump_SetRatedRPM(uint16_t value);

//设置操作方式
extern void Pump_SetSpeedRef(uint16_t value);
extern void Pump_SetAcce(uint16_t value);
extern void Pump_SetDece(uint16_t value);
extern void Pump_SetSpeedKp(uint16_t value);
extern void Pump_SetSpeedKi(uint16_t value);
extern void Pump_SetMotorControl(uint16_t value);

//读取
extern uint16_t Pump_GetRegValue(uint16_t index);

extern void UART1_WriteBytes(u8 *pData,u16 size);

#ifdef __cplusplus
}
#endif 

#endif

/******************* (C) COPYRIGHT 2023 qinxingpeng *****END OF FILE****/
