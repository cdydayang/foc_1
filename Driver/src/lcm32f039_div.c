/**
 ******************************************************************************
 * @file    lcm32f039_div.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    2021-05-21
 * @brief   This file provides all the div emulation firmware functions.
 ******************************************************************************/
/**
 ******************************************************************************
 * @attention:
 *
 *
 *
 *
 ******************************************************************************/

#include "lcm32f039_div.h"

long long Fun_64_DIV_32(long long A, int B, DIV_Result *result)
{
	A_Typedef AA;
	AA.Ans64 = A;
	DIV0->DIV64_CTRL = 1 << 24;
	DIV0->DIV64_ALO = AA.S.LOW32;
	DIV0->DIV64_AHI = AA.S.HIGH32;
	DIV0->DIV64_B = B;
	result->reliable = 1;
	if (DIV0->DIV64_CTRL != 0)
	{
		result->reliable = 0;
	}
	result->Q.S.LOW32 = DIV0->DIV64_QUOTLO;
	result->Q.S.HIGH32 = DIV0->DIV64_QUOTHI;
	result->REM = DIV0->DIV64_REM;

	return (long long)result->Q.Ans64;
}
long long Fun_32_DIV_32(int A, int B, DIV_Result *result)
{
	A_Typedef AA;
	AA.Ans64 = A;
	DIV0->DIV64_CTRL = 1 << 24 | 1 << 8;
	DIV0->DIV64_ALO = AA.S.LOW32;
	DIV0->DIV64_AHI = AA.S.HIGH32;
	DIV0->DIV64_B = B;
	result->reliable = 1;
	if (DIV0->DIV64_CTRL != (1U << 8))
	{
		result->reliable = 0;
	}
	result->Q.S.LOW32 = DIV0->DIV64_QUOTLO;
	result->Q.S.HIGH32 = DIV0->DIV64_QUOTHI;
	result->REM = DIV0->DIV64_REM;

	return (long long)result->Q.Ans64;
}

int Fun_32_DIV_16(int dividend, int divisor, result_t *result)
{
	DIV1->DIV32_CTRL_b.DIV16 = 0; //被除数32位
	DIV1->DIV32_ALO = dividend;
	DIV1->DIV32_B = divisor;
	while (DIV1->DIV32_CTRL_b.PROCCEED == RESET)
		;
	DIV1->DIV32_CTRL_b.PROCCEED = SET; //写1清0
	result->result32 = DIV1->DIV32_QUOTLO;
	result->remainder32 = DIV1->DIV32_REM;
	return result->result32;
}

int Fun_16_DIV_16(int dividend, int divisor, result_t *result)
{
	DIV1->DIV32_CTRL_b.DIV16 = 1; //被除数16位
	DIV1->DIV32_ALO = dividend;
	DIV1->DIV32_B = divisor;
	while (DIV1->DIV32_CTRL_b.PROCCEED == RESET)
		;
	DIV1->DIV32_CTRL_b.PROCCEED = SET; //写1清0
	result->result32 = DIV1->DIV32_QUOTLO;
	result->remainder32 = DIV1->DIV32_REM;
	return result->result32;
}
