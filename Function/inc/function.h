#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H
#include "lcm32f039_type.h"
#include "Math.h"
#define RangeLimit(min, x, max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))
#define INV_SQRT3 ((s16)4728)
#define SQRT3 ((s16)14188)
#define MAX_CURR_Q14 ((s16)16384)
#define FOC_VOLT_LIMIT_Q14 ((s16)20000)
#define MotorStandBy ((s16)1)
#define MotorPrePos ((s16)4)
#define IfStartMode ((s16)5)
#define SysFault ((s16)3)
#define BUS_Detect_En 1
#define OVER_MOD_EN 0
#define FOC_Angle SysPara.MotorAngle
typedef struct
{
    s16 StateMachineFlag;
    s16 MotorAngle;
    s16 VoltAngle;
    s16 MotorSpeed;
    s16 TsPu;
    s16 IdRef;
    s16 IdFilter;
    s16 IqFilter;
    s16 CurrFirstAD;
    s16 CurrSencondAD;
    s16 CurrThirdAD;
    s16 CurrUphase;
    s16 CurrVphase;
    s16 CurrWphase;
    s16 IuOffset;
    s16 IwOffset;
    s32 IuSum;
    s32 IwSum;
    s16 CurrOffsetCnt;
    s16 DcBusAD;
    s16 DcBusFiltered;
    s16 OC_cnt;
    s16 OV_cnt;
} SysPara_t;
typedef struct
{
    s16 FaultFlag;
    s16 FaultDetial[5];
} debug_t;
enum { NoFault = 0, OverCurrent, OverVolt, HardOCP };
extern SysPara_t SysPara;
extern debug_t debug;
void Driver_Para_Init(void);
#endif
