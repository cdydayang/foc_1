#ifndef _MATH_H
#define _MATH_H

#include "lcm32f039_type.h"
#include <stdint.h>

/* 正弦/余弦查表结果：Q15，+1.0 对应约 32767。 */
typedef struct
{
    s16 hCos;
    s16 hSin;
} Trig_Components;

/* 输入角度：16 位有符号数，一整圈共 65536 个计数。 */
extern Trig_Components MCM_Trig_Functions(s16 hAngle);

/* Clarke 变换输入/输出使用 Q14 电流。 */
typedef struct
{
    int16_t As;       /* U 相电流 */
    int16_t Bs;       /* V 相电流 */
    int16_t Cs;       /* W 相电流（保留用于观察，当前公式不直接使用） */
    int16_t Alpha;    /* 静止坐标系 Ialpha */
    int16_t Beta;     /* 静止坐标系 Ibeta */
} CLARKE_T;
#define CLARKE_DEFAULTS {0, 0, 0, 0, 0}
extern CLARKE_T clarke1;
extern void CLARKE_MACRO1(CLARKE_T *v);

/* 反 Park：Vd/Vq -> Valpha/Vbeta。电压分量 Q14，Sin/Cos Q15。 */
typedef volatile struct
{
    int16_t Alpha;
    int16_t Beta;
    int16_t Ds;
    int16_t Qs;
    int16_t Theta;
    int16_t Sin;
    int16_t Cos;
} IPARK_T;
#define IPARK_DEFAULTS {0, 0, 0, 0, 0, 0, 0}
extern IPARK_T ipark1;
extern void IPARK_MACRO1(volatile IPARK_T *v);

/* Park：Ialpha/Ibeta -> Id/Iq。角度单位与 I/F 电气角度一致。 */
typedef struct
{
    int16_t Alpha;
    int16_t Beta;
    int16_t Ds;
    int16_t Qs;
    s32 Theta;
    s32 Sin;
    s32 Cos;
} PARK_T;
#define PARK_DEFAULTS {0, 0, 0, 0, 0, 0, 0}
extern PARK_T park1;
extern PARK_T park2;
extern void PARK_MACRO1(PARK_T *v);

#endif
