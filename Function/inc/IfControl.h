#ifndef __IFCONTROL_H
#define __IFCONTROL_H

#include "Math.h"

#define IfModeCurrAMP  3000
#define IfModeSwitchIqAmp 300//500

//#define IdLowSpeed	4096
//#define IdHighSpeed (IdLowSpeed+2048)

#define IfSwitchSpeed 2000//3000//2000

#define CurrSwitchStep  30

typedef struct
{
	s16 ElectAngle;
	s16 ReferSpeed;
	s16 IqRef;
}
IfCurr_t;

extern IfCurr_t IfStart;

extern void IfInit(void);
extern void IfAngleCalc(void);
extern void IfSpeedCalc(void);

#endif  /*__INITIAL_POSITION_DETECT_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
