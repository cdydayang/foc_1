#ifndef __CONTROLLOOP_H
#define __CONTROLLOOP_H

#include "CurrentLoop.h"
#include "Math.h"

#define MAXSPEED debug.CommandSpeed  //Q14

#define MINSPEED 0//0//50//200

#define NoLoadIdAMP 512//1024//2048

#define MAX_AMP 20000//20000//20000

#define FOC_Angle  SysPara.MotorAngle

#define SPEED_LIMIT  12000//16000//10922//×î´ó1000rpm

#define Speed_Step 20//

#define SPEED_LOOP_KP  500//1000
#define SPEED_LOOP_KI  100

extern void OutsideLoopInit(void);
extern void OutsideLoopCalc(void);

extern void Id_Calc(void);


#endif
