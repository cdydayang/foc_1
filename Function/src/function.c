#include "function.h"
#include "PI_Regulator.h"
#include "IfControl.h"
#include "CurrentLoop.h"

SysPara_t SysPara;
debug_t debug;

void Driver_Para_Init(void)
{
    SysPara.StateMachineFlag = MotorStandBy;
    SysPara.MotorAngle = 0;
    SysPara.VoltAngle = 0;
    SysPara.MotorSpeed = 0;

    /* Original demo base: 8 kHz PWM and 100 Hz electrical base. Q17. */
    SysPara.TsPu = 10294;

    SysPara.IdRef = 0;
    SysPara.IdFilter = 0;
    SysPara.IqFilter = 0;
    SysPara.DcBusFiltered = 0;

    SysPara.CurrOffsetCnt = 0;
    SysPara.IuOffset = 0;
    SysPara.IwOffset = 0;
    SysPara.IuSum = 0;
    SysPara.IwSum = 0;

    SysPara.OC_cnt = 0;
    SysPara.OV_cnt = 0;

    debug.FaultFlag = NoFault;
    debug.FaultDetial[0] = 0;
    debug.FaultDetial[1] = 0;
    debug.FaultDetial[2] = 0;
    debug.FaultDetial[3] = 0;
    debug.FaultDetial[4] = 0;

    IfInit();
    Inside_Loop_Init();
}
