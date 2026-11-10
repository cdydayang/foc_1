//
//  SVPWM.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
// 

#include "SVPWM.h"
#include "Math.h"
#include "function.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_div.h"
#include "Timer_Init.h"

pwm_t PWM_Calc;

void Invter_Init(void);
void Invter_Calc(void);
void PWMOutputs_ENABLE(void);
void PWMOutputs_DISABLE(void);
void Update_PWM(void);

void Invter_Init(void)
{
	PWM_Calc.PwmTBPRD = PWM_PERIOD;
	PWM_Calc.PwmTBPRDInv_q24 = (s16)((1<<24)/PWM_Calc.PwmTBPRD);
	
	PWM_Calc.UCMP = PWM_Calc.PwmTBPRD>>3;
	PWM_Calc.VCMP = PWM_Calc.PwmTBPRD>>3;
	PWM_Calc.WCMP = PWM_Calc.PwmTBPRD>>3;
	
	PWM_Calc.Sector = 0;
	PWM_Calc.LastSector = 0;
}

/*
 * SVPWM 空间矢量调制：
 * 先用 Vα/Vβ 判断所在扇区，再计算相邻两个有效矢量作用时间 T1/T2，
 * 剩余时间 T0 平分到 PWM 周期两端，最后换算成 U/V/W 三相比较值。
 * Vbus 标幺值用于母线电压补偿；母线读数无效时不做除零运算。
 */
void Invter_Calc(void)
{
#define c0To60Degree 	    6
#define c60To120Degree		4
#define c120To180Degree 	5
#define c180To240Degree 	1
#define c240To300Degree		3
#define c300To360Degree		2
#define MIN_PULSE_WIDTH 	500//输出侧采样，没有最小脉宽
	
	s16 A,B,C;
	
	s16 V1,V2,V3;
	s16 T0,T1,T2;
	
	s32 temp1, temp2;

	DIV_Result result0, result1;

	#if (BUS_Detect_En == 1)	
	temp2 = SQRT3 << 10;

    /*
     * 直流母线标幺电压为 0 时不能做除法。
     * 这通常出现在上电、欠压或 ADC 尚未稳定的阶段；
     * 先使用单位补偿系数，欠压保护会阻止电机进入正常运行。
     */
    if (SysPara.DcBusFiltered <= 0)
    {
        temp1 = 1024;
    }
    else
    {
        temp1 = (s32)Fun_32_DIV_32(temp2, SysPara.DcBusFiltered, &result0);
    }

	//temp1 = (s16)((SQRT3<<10)/SysPara.DcBusFiltered);//q10
	#else
	temp1 = 1024;//(s16)((SQRT3<<10)/SysPara.DcBusFiltered)
	#endif
	
	V1 = (PWM_Calc.VbetaPu*temp1)>>10;
	
	//0.866*alpha - 0.5*beta
	V2 = (14188 * PWM_Calc.ValfaPu)>>14;
	V2 -= (PWM_Calc.VbetaPu>>1);
	V2 = (V2*temp1)>>10;
	
	//-0.866*alpha - 0.5*beta
	V3 = (-(14188 * PWM_Calc.ValfaPu))>>14;
	V3 -= (PWM_Calc.VbetaPu>>1);
	V3 = (V3*temp1)>>10;
	
	A = (V1 > 0)?1:0;
	B = (V2 > 0)?1:0;
	C = (V3 > 0)?1:0;
	
	PWM_Calc.Sector = A * 4 + 2 * B + C;
		
	switch(PWM_Calc.Sector)
	{
		case c0To60Degree:
		
		T1 = V2;
		T2 = V1;
		
		break;
		
		case c60To120Degree:
		
			T1 = -V2;
			T2 = -V3;
		
		break;
		
		case c120To180Degree:
		
			T1 = V1;
			T2 = V3;
		
		break;
		
		case c180To240Degree:
		
			T1 = -V1;
			T2 = -V2;
		
		break;
		
		case c240To300Degree:
		
			T1 = V3;
			T2 = V2;
		
		break;		
		
		case c300To360Degree:
		
			T1 = -V3;
			T2 = -V1;
		
		break;
		
		default:
		
			T1 = 0;
			T2 = 0;
		
		break;
	}
	
	if((T1 + T2) > (PWM_Calc.PwmTBPRD))
	{
#if (OVER_MOD_EN == 1)
		if(
			(T1 > T2) && \
			(T1 > PWM_Calc.PwmTBPRD)
		  )
		{
			T1 = PWM_Calc.PwmTBPRD;
			T2 = 0;
		}
		else if(T2 > PWM_Calc.PwmTBPRD)
		{
			T1 = 0;
			T2 = v->PwmTBPRD;
		}
		else
		{
			T1 = (s16)((T1 * PWM_Calc.PwmTBPRD) / (T1 + T2));
			T2 = (PWM_Calc.PwmTBPRD) - T1;
		}
#else	
	temp2 = T1 * PWM_Calc.PwmTBPRD;
	temp1 = T1 + T2;
	T1 = Fun_32_DIV_32(temp2, temp1, &result1);
	
	//T1 = (s16)((T1 * v->PwmTBPRD) / (T1 + T2));

	T2 = (PWM_Calc.PwmTBPRD) - T1;
#endif
	}
	
	T0 = (PWM_Calc.PwmTBPRD - T1 - T2)>>1;
	
	if(T0 < MIN_PULSE_WIDTH)
	{
		T0 = MIN_PULSE_WIDTH;
	}
	
	switch(PWM_Calc.Sector)
	{
		case c0To60Degree:
		
			PWM_Calc.UCMP = T0;
			PWM_Calc.VCMP = T1+T0;
			PWM_Calc.WCMP = T1+T2+T0;
		
		break;
		
		case c60To120Degree:
		
			PWM_Calc.UCMP = T1+T0;
			PWM_Calc.VCMP = T0;
			PWM_Calc.WCMP = T1+T2+T0;
		
		break;
		
		case c120To180Degree:
		
			PWM_Calc.UCMP = T1+T2+T0;
			PWM_Calc.VCMP = T0;
			PWM_Calc.WCMP = T1+T0;
		
		break;
		
		case c180To240Degree:
		
			PWM_Calc.UCMP = T1+T2+T0;
			PWM_Calc.VCMP = T1+T0;
			PWM_Calc.WCMP = T0;
		
		break;
		
		case c240To300Degree:
		
			PWM_Calc.UCMP = T1+T0;
			PWM_Calc.VCMP = T1+T2+T0;
			PWM_Calc.WCMP = T0;
		
		break;
		
		case c300To360Degree:
		
			PWM_Calc.UCMP = T0;
			PWM_Calc.VCMP = T1+T2+T0;
			PWM_Calc.WCMP = T1+T0;
		
		break;
		
		default:
		
			PWM_Calc.UCMP = T0;
			PWM_Calc.VCMP = T0;
			PWM_Calc.WCMP = T0;
		
		break;	
	}
}

/*
 * 将三相 SVPWM 比较值写入 TIM1 预装载寄存器。
 * CCR4 固定在原参考工程的采样触发位置，用于 TIM1_TRGO 同步 ADC。
 */
void Update_PWM(void)
{
	TIM1->CCR1 = PWM_Calc.UCMP;//大小直接代表下桥的占空比	
	TIM1->CCR2 = PWM_Calc.VCMP;//*3/2
	TIM1->CCR3 = PWM_Calc.WCMP;//*3/2
	
	TIM1->CCR4 = 200;//1;//PWM_Calc.PwmTBPRD - 100;//1;//PWM_Calc.PwmTBPRD - 100;//*3/2 // To Syncronyze the ADC
}

/* 打开三相上下桥输出。只有 offset/calibration ready 且状态机允许时调用。 */
void PWMOutputs_ENABLE(void)
{
//	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
//	TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
//	TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

	TIM1->EGR&= ~0x0080;
	TIM1->BDTR|= 1<<15;
	
	TIM1->CCER = 0X1555;//全开1555，只开下1CCC，千万区分1555和1CCC；
}

/* 通过 TIM1 Break 事件关闭主输出使能，作为软件关断路径。 */
void PWMOutputs_DISABLE(void)
{
//	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
//	TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
//	TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
//	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

	TIM1->EGR|= 0x0080;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
