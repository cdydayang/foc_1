//
//  Pump_Function.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "Pump_Function.h"
#include "function.h"
#include "lcm32f039_uart.h"

MotorDisplayType_t PumpDisplay;

int8_t txBuf[8] = {0,1,2,3,4,5,6,7};

void UART3_WriteBytes(u8 *pData,u16 size)
{
	u16 i = 0;
	
	for(i = 0;i < size;i++)
	{
		UART_SendData(UART3,pData[i]);
		delay10us(120);
	}		
}

void UART1_WriteBytes(u8 *pData,u16 size)
{
	u16 i = 0;
	
	for(i = 0;i < size;i++)
	{
		UART_SendData(UART1,pData[i]);
		delay10us(120);
	}		
}

//static uint16_t Pump_GetControlType(void)
//{
//	return 0x0;//电机控制方式
//}

//static uint16_t Pump_Get_CMD_Source(void)
//{
//	return 0x2;//命令源选择
//}

//static uint16_t Pump_GetCommFre(void)
//{
//	return 0x9;//频率选择
//}

//static uint16_t Pump_GetMotorMaxFre(void)
//{
//	return 0x0001;//最大频率
//}

//static uint16_t Pump_GetFreUpLimit(void)
//{
//	return 0x6667;//频率上限
//}

//static uint16_t Pump_GetMotorType(void)
//{
//	return 0x0514;//电机类型
//}

//static uint16_t Pump_GetRatedPower(void)
//{
//	return 0x012C;//额定功率
//}

//static uint16_t Pump_GetRatedVoltage(void)
//{
//	return 0x02BC;//额定电压
//}

//static uint16_t Pump_GetRatedCurrent(void)
//{
//	return 0x0001;//额定电流
//}

//static uint16_t Pump_GetRatedFre(void)
//{
//	return 0x0100;//额定频率
//}

//static uint16_t Pump_GetRatedRPM(void)
//{
//	return 0x03E8;//额定转速
//}

//static uint16_t Pump_GetSpeedRef(void)
//{
//	return 0x03E8;//速度给定
//}

//static uint16_t Pump_GetAcce(void)
//{
//	return 0x03E8;//加速时间
//}

//static uint16_t Pump_GetDece(void)
//{
//	return 0x03E8;//加速时间
//}

//static uint16_t Pump_GetSpeedKp(void)
//{
//	return 0x03E8;//速度增益
//}

//static uint16_t Pump_GetSpeedKi(void)
//{
//	return 0x03E8;//速度积分
//}

//static uint16_t Pump_GetMotorControl(void)
//{
//	return 0x03E8;//速度积分
//}

//static uint16_t Pump_GetMotorCurrentFre(void)
//{
//	return 0x03E8;//电机运行频率
//}

//static uint16_t Pump_GetMotorSetFre(void)
//{
//	return 0x03E8;//电机设定频率
//}

//static uint16_t Pump_GetBusVoltage(void)
//{
//	return 0x03E8;//母线电压
//}

//static uint16_t Pump_GetOutputVoltage(void)
//{
//	return 0x03E8;//输出电压
//}

//static uint16_t Pump_GetOutputCurrent(void)
//{
//	return 0x03E8;//输出电流
//}

//static uint16_t Pump_GetOutputPower(void)
//{
//	return 0x03E8;//输出功率
//}

//static uint16_t Pump_GetOutputRPM(void)
//{
//	return 0x03E8;//输出转速
//}

//static uint16_t Pump_GetError(void)
//{
//	return 0x03E8;//故障码
//}

uint16_t Pump_GetRegValue(uint16_t index)
{
	uint16_t result = 0;

	return result;
}

void Pump_SetControlType(uint16_t value)
{
	//控制方式
}


void Pump_Set_CMD_Source(uint16_t value)
{
	//命令源
}

void Pump_SetCommFre(uint16_t value)
{
	//通讯频率
}

void Pump_SetMotorMaxFre(uint16_t value)
{
	//电机最大频率
}

void Pump_SetFreUpLimit(uint16_t value)
{
	//频率上限
}

void Pump_SetMotorType(uint16_t value)
{
	//电机类型
}

void Pump_SetRatedPower(uint16_t value)
{
	//额定功率
}

void Pump_SetRatedVoltage(uint16_t value)
{
	//额定电压
}

void Pump_SetRatedCurrent(uint16_t value)
{
	//额定电流
}

void Pump_SetRatedFre(uint16_t value)
{
	//额定频率
}

void Pump_SetRatedRPM(uint16_t value)
{
	//debug.CommandSpeed = value;//额定转速
}

void Pump_SetSpeedRef(uint16_t value)
{
	//速度给定
	debug.CommandSpeed = (value*4473)>>12;//额定转速
	//debug.CommandSpeed = (value*4920)>>12;//额定转速
}

void Pump_SetAcce(uint16_t value)
{
	//加速时间
}

void Pump_SetDece(uint16_t value)
{
	//减速时间
}

void Pump_SetSpeedKp(uint16_t value)
{
	//速度环增益
}

void Pump_SetSpeedKi(uint16_t value)
{
	//速度环积分
}

void Pump_SetMotorControl(uint16_t value)
{	
	if(value == 1)
	{
		debug.StartCommand = 200;
	}
	else if(value == 6)
	{
		debug.StartCommand = 0;
	}
	//电机控制
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
