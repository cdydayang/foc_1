#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include "function.h"
#include "Math.h"

/*================= SYSTEM Configuration =========*/
#define SYSTEM_FREQUENCY                   96000000

/*================== PWM Configuration ===========*/
#define PWM_SET_FREQUENCY                  (UPDS_PWM_FREQ)
#define PWM_SET_FREQUENCY_GiSTER           (SYSTEM_FREQUENCY / PWM_SET_FREQUENCY / 2  - 1)
#define PWM_PERIOD                         ((SYSTEM_FREQUENCY / PWM_SET_FREQUENCY / 2))


#define INV_SQRT3	  			4728//Q13
#define SQRT3 		   			14188//Q13

#define MAX_CURR_AMP 			16384
#define RangeLimit(min,x,max)  (((x) >= (max))?(max):(((x)>= (min))?(x):(min)))
#define CMPMAX2(a,b)  (((a) >= (b))? (a):(b))
#define CMPMIN2(a,b)  (((a) <= (b))? (a):(b))

#define MotorStandBy	(s16)1
#define MotorRun 	    (s16)2
#define SysFault        (s16)3
#define MotorPrePos     (s16)4
#define IfStartMode         (s16)5

#define CompareMax3(X,Y,Z)  (((X) > (Y))?(((X) > (Z))? (X):(Z)):(((Y) > (Z))? (Y):(Z)))
#define CompareMin3(X,Y,Z)  (((X) > (Y))?(((Y) > (Z))? (Z):(Y)):(((X) > (Z))? (Z):(X)))

#define SAMPLE_CURR_LPF_EN		1//0//1//0//1

#define OVER_MOD_EN				0

#define BUS_Detect_En	    1//0

#define __IO    volatile             /*!< Defines 'read / write' permissions */

//#define DIV0 ((DIV_TypeDef *) (0x48010000+(4UL<<12)))

#define MOTOR_RUN_FLAG		GPIO_ResetBits(GPIOA, GPIO_Pin_12)
#define MOTOR_STANDBY_FLAG		GPIO_SetBits(GPIOA, GPIO_Pin_12)

#define MOTOR_FAULT_SET		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define MOTOR_NO_FAULT_SET  	GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define START_MOTOR_COMMAND GPIOF->IDR_b.IDR8
#define STOP_MOTOR_COMMAND GPIOC->IDR_b.IDR13

#define MOTOR_SPEED_REF (REF_SPEED_DATA<<2)

#define labs(x)  (((x)>0)? (x): (-x))

typedef struct
{
	s16 RefSpeed;
	s16 ComSpeed;
	s16 RefSpeedFilter;
	s16 DirChangeFlag;

	s16 OCflag;
	s16 DCmode_flag;
	s16 DCmode_cnt;

	s16 MotorAngle;
	s16 MotorSpeed;
	s16 VoltAngle;
		
	s16 VoltAlpha;
	s16 VoltBeta;
	
	s16 TsPu;
	s16 TsPuInv;

	s16 FluxPu;
	
	s16 IdRef;
	
	s16 IdFilter;
	s16 IqFilter;
	
	s16 StateMachineFlag;
	
	s16 BUS_Coef;

	s16 CurrFirstAD;
	s16 CurrSencondAD;
	s16 CurrThirdAD;
	s16 IdcAD;
	s16 DcBusAD;
	
	s16 Ialpha;
	s16 Ibeta;
	
	s16 DcBusFiltered; //Q13
	s16 DcCurrFiltered;
	
	s16 FluxInvPu;
	
	s16 debug[10];
			
	s16 CurrUphase;
	s16 CurrVphase;
	s16 CurrWphase;
		
	s16 CurrOffsetCnt;
	
	s16 IuOffset;
	s16 IwOffset;
	s16 IDC_Offset;
	
	s32 IuSum;
	s32 IwSum;
	s32 IDC_Sum;
	
	s16 OC_cnt;
	s16 OV_cnt;
	
	s16 PrePosCnt;
	
	s16 StartDCmodeEn;
	s32 StartDCmodeCnt;
	
	s16 SpeedLoopCnt;
	
	s16 DriverPower;

	s16 Speed_Loop_Kp;
	s16 Speed_Loop_Ki;
}
SysPara_t;

typedef struct
{
	s16 StartCommand;
	s16 CommandSpeed;
	s16 FaultFlag;
	s16 FaultDetial[5];
}debug_t;

typedef struct
{
    __IO uint32_t DIV_CTRL;
    __IO uint32_t DIV_ALO;
    __IO uint32_t DIV_AHI;
    __IO uint32_t DIV_B;
    __IO uint32_t DIV_QUOTLO;
    __IO uint32_t DIV_QUOTHI;
    __IO uint32_t DIV_REM;
} DIV_TypeDef;

typedef struct
{
	s16 u16PowerLevel;
	s16 LastPowerLevel;
	s16 StartFlagCntEn;
	s16 PowerOnDelay;

	s16 u16PowerLevelCount;
	s16 StarButtonFlag;
	s16 StarLastButtonFlag;

	s16 AutoStartFlag;
	s16 ManualStartFlag;

	s16 ManualStartEn;
	s16 ManualStopEn;
	
	s16 RemoteOp;
}
CommDetail_t;

enum
{
	NoFault,
	OverCurrent,
	OverVolt,
	DriverOverTemp,
	MotorOverTemp,
	MotorEstiFault,
	HardOCP,
	OtherFault
};

enum
{
	PumpNoFault,
	PowerFault,
	NoPumpFault,
	SysOverTemp,
	InterFault,
	SysOtherFault
};

extern SysPara_t SysPara;
extern debug_t debug;
extern CommDetail_t CommData;

extern void UnderOrOverVoltPro(void);

extern void Driver_Para_Init(void);

extern void DebugInit(void);

extern void CommDataFunc(void);

extern void PowerOnInit(void);
#endif
