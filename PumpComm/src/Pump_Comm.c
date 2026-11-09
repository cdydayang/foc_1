//
//  Pump_Comm.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "Pump_Comm.h"
#include "Pump_Function.h"
#include <string.h>
#include "function.h"

typedef struct
{
	uint8_t Addr;
	uint8_t RxCnt;
	uint8_t RxBuf[10];
	uint8_t TxBuf[30];
	uint8_t RxFlag;
	fpPump_CommTx Tx;
}tPump_Comm;

static tPump_Comm Pump_Comm;

void Pump_CommRxByte(uint8_t byte)
{
	if(Pump_Comm.RxFlag == 1)
	{
		//上一帧数据还没有处理
		return;
	}

	if((Pump_Comm.RxCnt == 0) && (byte == Pump_Comm.Addr))
	{
		Pump_Comm.RxBuf[Pump_Comm.RxCnt++] = byte;
	}
	else if(Pump_Comm.RxCnt == 1)
	{
		if((byte == 0x03) || (byte == 0x06))
		{
			//只接收03和06相关指令
			Pump_Comm.RxBuf[Pump_Comm.RxCnt++] = byte;
		}
		else
		{
			Pump_Comm.RxCnt = 0;
		}
	}
	else if((Pump_Comm.RxCnt > 1) && (Pump_Comm.RxCnt < 8))
	{
		Pump_Comm.RxBuf[Pump_Comm.RxCnt++] = byte;

		if(Pump_Comm.RxCnt == 8)
		{
			//最大只接收8个字节的数据
			Pump_Comm.RxFlag = 1;
			Pump_Comm.RxCnt = 0;
		}
	}
	else
	{
		Pump_Comm.RxCnt = 0;
	}
}

void Pump_CommInit(fpPump_CommTx fp,uint8_t addr)
{
	Pump_Comm.Tx = fp;
	Pump_Comm.Addr = addr;
	Pump_Comm.RxCnt = 0;
	Pump_Comm.RxFlag = 0;
}

static uint16_t Pump_CommCalcCRC(uint8_t *pData,uint16_t size)
{
	uint16_t temp = 0xFFFF;
	uint16_t i = 0,n = 0;

	for(n = 0;n < size;n++)
	{
		temp = pData[n] ^ temp;
		for (i = 0; i < 8; i++)
		{
			if (temp & 0x01)
			{
				temp = temp >> 1;
				temp = temp ^ 0xa001;
			}
			else
			{
				temp = temp >> 1;
			}
		}
	}
	return temp;
}

static void Pump_ReadDeviceValue(void)
{
	uint16_t startReg = 0;
	uint16_t stopReg = 0;
	uint16_t i = 0;
	uint16_t temp[11] = {0};
	uint16_t crc = 0;

	startReg = ((Pump_Comm.RxBuf[2] << 8) | Pump_Comm.RxBuf[3]);
	stopReg = startReg + ((Pump_Comm.RxBuf[4] << 8) | Pump_Comm.RxBuf[5]);

	if((stopReg <= startReg) || ((stopReg - startReg) > 11))
	{
		//协议错误
		return;
	}

	Pump_Comm.TxBuf[0] = Pump_Comm.RxBuf[0];
	Pump_Comm.TxBuf[1] = Pump_Comm.RxBuf[1];
	Pump_Comm.TxBuf[2] = (stopReg - startReg) << 1;

	for(i = startReg;i < stopReg;i++)
	{
		temp[i] = Pump_GetRegValue(i);
		Pump_Comm.TxBuf[3 + i * 2] = (uint8_t)(temp[i] >> 8);
		Pump_Comm.TxBuf[4 + i * 2] = (uint8_t)(temp[i]);
	}
	crc = Pump_CommCalcCRC(&Pump_Comm.TxBuf[0],Pump_Comm.TxBuf[2] + 3);

	Pump_Comm.TxBuf[3 + Pump_Comm.TxBuf[2]] = (uint8_t)crc;
	Pump_Comm.TxBuf[4 + Pump_Comm.TxBuf[2]] = (uint8_t)(crc >> 8);

	if(Pump_Comm.Tx != 0)
	{
		Pump_Comm.Tx(&Pump_Comm.TxBuf[0],Pump_Comm.TxBuf[2] + 5);
	}
}

static void Pump_WriteDeviceValue(void)
{
	uint16_t cmd = (Pump_Comm.RxBuf[2] << 8) | Pump_Comm.RxBuf[3];
	uint16_t value = (Pump_Comm.RxBuf[4] << 8) | Pump_Comm.RxBuf[5];

	switch(cmd)
	{
		case 0xF001://电机控制方式
			Pump_SetControlType(value);
			if(Pump_Comm.Tx != 0)
			{
				Pump_Comm.Tx(&Pump_Comm.RxBuf[0],8);
			}
		break;
	}
}

static void Pump_CommProcess(void)
{
	if(Pump_Comm.RxBuf[1] == 0x03)
	{
		//读设备
		Pump_ReadDeviceValue();
	}
	else if(Pump_Comm.RxBuf[1] == 0x06)
	{
		//写设备
		Pump_WriteDeviceValue();
	}
}

void Pump_CommTaskEntry(void)
{
	if(Pump_Comm.RxFlag == 1)
	{
		//接收完成，开始处理
		if(Pump_CommCalcCRC(&Pump_Comm.RxBuf[0],6) == ((Pump_Comm.RxBuf[7] << 8) | Pump_Comm.RxBuf[6]))
		{
			//CRC校验通过
			Pump_CommProcess();
		}

		Pump_Comm.RxFlag = 0;
	}
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
