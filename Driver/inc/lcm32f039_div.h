/*****************************************************
**  @brief div moudle driver
**	@author zhang
**	@data 2020-10-13
**	@function  A/B = Q......Rem
*******************************************************/

#ifndef LCM32F039_DIV_H
#define LCM32F039_DIV_H
#include "lcm32f039.h"
#include <stdint.h>
#include <core_cm0.h>

typedef union
{
	struct
	{
		uint32_t LOW32;
		uint32_t HIGH32;
	} S;
	uint64_t Ans64;
} A_Typedef;
typedef struct
{
	A_Typedef Q;
	__IO uint32_t REM;
	__IO uint32_t reliable;
} DIV_Result;

typedef struct
{
	uint32_t result32;	  //ษฬ
	uint32_t remainder32; //ำเสý
} result_t;

long long Fun_32_DIV_32(int A, int B, DIV_Result *result);
long long Fun_64_DIV_32(long long A, int B, DIV_Result *result);

int Fun_32_DIV_16(int dividend, int divisor, result_t *result);
int Fun_16_DIV_16(int dividend, int divisor, result_t *result);
#endif
