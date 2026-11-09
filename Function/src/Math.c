//
//  Math.c
//
//  Created by PMSM_RFO on 2023-09-06
//  Copyright (C) 2023-2033 RFC Tech. All rights reserved.
//  

#include "Math.h"
#include "CurrentLoop.h"
#include "function.h"

IPARK_T ipark1 = IPARK_DEFAULTS;
PARK_T park1 = PARK_DEFAULTS;
PARK_T park2 = PARK_DEFAULTS;
CLARKE_T clarke1 = CLARKE_DEFAULTS;

void IPARK_MACRO1(IPARK_T *v);
void PARK_MACRO1(PARK_T *v);
void CLARKE_MACRO1(CLARKE_T *v);

/*--------------------Private macro ------------------*/
#define SIN_COS_TABLE {\
0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,\
0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,\
0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,\
0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,\
0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,\
0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,\
0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,\
0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,\
0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,\
0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,\
0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,\
0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,\
0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,\
0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,\
0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,\
0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,\
0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,\
0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,\
0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,\
0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,\
0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,\
0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,\
0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,\
0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,\
0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,\
0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,\
0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,\
0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,\
0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,\
0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,\
0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,\
0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE}

/*--------------------274:99℃  3901：-30℃ ------------------*/
#define TEMPERATURE_TABLE {\
274,282,289,297,305,313,322,331,340,350,\
359,369,380,390,401,413,424,436,449,461,\
475,488,502,517,531,547,562,579,595,613,\
630,649,667,687,707,727,748,770,792,815,\
839,863,888,913,940,966,994,1022,1051,1081,\
1112,1143,1175,1207,1241,1275,1310,1345,1382,1419,\
1456,1495,1534,1574,1614,1655,1696,1739,1781,1825,\
1868,1912,1957,2002,2047,2093,2139,2185,2231,2278,\
2324,2371,2417,2464,2510,2556,2602,2648,2693,2738,\
2783,2827,2870,2913,2955,2997,3038,3078,3118,3157,\
3195,3232,3268,3303,3338,3371,3404,3435,3466,3496,\
3524,3552,3579,3605,3630,3654,3677,3699,3720,3741,\
3760,3779,3797,3814,3830,3846,3861,3875,3888,3901}

#define SIN_MASK        0x0300u
#define U0_90           0x0200u
#define U90_180         0x0300u
#define U180_270        0x0000u
#define U270_360        0x0100u

/*------------------- Private variables ---------------*/
const int16_t hSin_Cos_Table[256] = SIN_COS_TABLE;
const int16_t hTEMPERATURE_TABLE[130] = TEMPERATURE_TABLE;
/*------------------ Private functions ----------------*/
Trig_Components MCM_Trig_Functions(int16_t hAngle);
s32 MCM_Sqrt(s32 wInput);
int16_t arctan2(int16_t y1,int16_t x1);
s16 GetTemperature(vu32 ADCvalue);

/*************************************
	函数名：GetTemperature
	描述：计算温度
	输入：ADCvalue---12位ADC采样值
	输出：Temperature---温度值，单位℃
**************************************/
s16 GetTemperature(vu32 ADCvalue)
{
	u16 high,low,middle;
	s16 Temperature;
	high = 129;
	low = 0;
	while(1)
	{
   	middle=(high+low)>>1;
		if(ADCvalue>hTEMPERATURE_TABLE[middle])
		{
			low = (high+low)>>1;
		}
		else if (ADCvalue<hTEMPERATURE_TABLE[middle])
		{
			high = (high+low)>>1;
		}
		else
		{
			Temperature = 99-middle;
			break;
		}
		if(high == low+1)
		{
			Temperature = 99-middle;
			break;
		}
	}
	
	return Temperature;	
}

/*************************************
	函数名：MCM_Trig_Functions
	描述：角度查表正余弦
	输入：hAngle---[-32768,32767]
	输出：Trig_Components---正余弦结构体
**************************************/
Trig_Components MCM_Trig_Functions(int16_t hAngle)
{
  s32 shindex;
  u16 uhindex;
  
  Trig_Components Local_Components;
  
  /* 10 bit index computation  */  
  shindex =((s32)32768 + (s32)hAngle); 
  uhindex = (u16)shindex;
  uhindex /= (u16)64;      
  
  switch ((u16)(uhindex) & SIN_MASK) 
  {
		case U0_90:
			Local_Components.hSin = hSin_Cos_Table[(u8)(uhindex)];
			Local_Components.hCos = hSin_Cos_Table[(u8)(0xFFu-(u8)(uhindex))];
			break;
		
		case U90_180:  
			 Local_Components.hSin = hSin_Cos_Table[(u8)(0xFFu-(u8)(uhindex))];
			 Local_Components.hCos = -hSin_Cos_Table[(u8)(uhindex)];
			break;
		
		case U180_270:
			 Local_Components.hSin = -hSin_Cos_Table[(u8)(uhindex)];
			 Local_Components.hCos = -hSin_Cos_Table[(u8)(0xFFu-(u8)(uhindex))];
			break;
		
		case U270_360:
			 Local_Components.hSin =  -hSin_Cos_Table[(u8)(0xFFu-(u8)(uhindex))];
			 Local_Components.hCos =  hSin_Cos_Table[(u8)(uhindex)]; 
			break;
		default:
			break;
  }
  return (Local_Components);
}


/*************************************
	函数名：MCM_Sqrt
	描述：平方根计算
	输入：wInput---计算值
	输出：wtemprootnew---平方根
**************************************/
s32 MCM_Sqrt(s32 wInput)
{
  u8 biter = 0u;
  s32 wtemproot;
  s32 wtemprootnew;

  if (wInput > 0)
  {
    
    if (wInput <= (s32)2097152)
    {
      wtemproot = (s32)128;
    }
    else
    {
      wtemproot = (s32)8192;
    }
    
    do
    {
      wtemprootnew = (wtemproot + wInput/wtemproot)/(s32)2;
      if (wtemprootnew == wtemproot)
      {
        biter = 6u;
      }
      else
      {
        biter ++;
        wtemproot = wtemprootnew;
      }
    }
    while (biter < 6u);
  }
  else
  {
    wtemprootnew = (s32)0;
  }
  
  return (wtemprootnew); 
}

/*************************************
	函数名：arctan2
	描述：反正切函数
	输入：y1---正弦值
				x1---余弦值
        输入16位有符号型变量
	输出：result---反正切值
        输出-32768~32767对应-pi~pi
**************************************/
s16 arctan2(s16 y1,s16 x1)
{
	s32 a;
	s32 ay,ax;
	s32 x;
	s32 result;
	s32 sw;
	
	sw = 0;
	
	if(y1<0)
	{
		ay = -y1;
	}
	else
	{
		ay = y1;
	}
	
	if(x1<0)
	{
		ax = -x1;
	}
	else
	{
		ax = x1;
	}
	
	if(ay>ax)
	{
		a = ay;
		ay = ax;
		ax = a;
		sw = 1;
	}
	
	if(ax == 0)
	{
		return 0;
	}
	else
	{
		x = ((ay<<15)/ax);
		
		if(x == 0x8000)
		{
			result = 0x2000;
		}
		else
		{
			a = ((0x118b<<13) - x*0x96)>>13;
			a = (-(0x4306<<14) + a*x)>>16;
			a = ((0x364<<12) + a*x)>>15;
			a = ((0x5179<<14) + a*x)>>15;
			
			result = (a*x)>>15;
		}
		
		if(sw == 1)
		{
			result = 0x4000 - result;
		}
		
		if(x1>=0)
		{
			if(y1<0)	
			{
				result = -result;
			}
		}
		else
		{
			if(y1>=0)
			{
				result = 0x8000 - result;
			}
			else
			{
				result = -(0x8000 - result);
			}
		}
		
		return result;
	}
}

void IPARK_MACRO1(IPARK_T *v)											
{			
	Trig_Components Local_Vector_Components;
	
	Local_Vector_Components = MCM_Trig_Functions(v->Theta);
	v->Sin = Local_Vector_Components.hSin;
	v->Cos = Local_Vector_Components.hCos;

	v->Alpha = (v->Ds * v->Cos - v->Qs * v->Sin) >> 15;		
	v->Beta  = (v->Qs * v->Cos + v->Ds * v->Sin) >> 15;
}

void PARK_MACRO1(PARK_T *v)
{
	Trig_Components Local_Vector_Components;
	
	Local_Vector_Components = MCM_Trig_Functions(v->Theta);
	v->Sin = Local_Vector_Components.hSin;
	v->Cos = Local_Vector_Components.hCos;

	v->Ds = (v->Alpha * v->Cos + v->Beta * v->Sin) >> 15;	
	v->Qs = (v->Beta * v->Cos - v->Alpha * v->Sin) >> 15;	
}

void CLARKE_MACRO1(CLARKE_T *v)
{
	s16 TempA;
	s16 TempB;

	TempA = v->As;																		//Ialpha = IA
	TempB = (INV_SQRT3 * (v->As + (v->Bs<<1)))>>13;	//Ibeta = (IA + 2*IB)/sqrt(3)
	
	v->Alpha = TempA;
	v->Beta	= TempB;
}

/******************* (C) COPYRIGHT 2023-2033 by RFC Tech *****END OF FILE****/
