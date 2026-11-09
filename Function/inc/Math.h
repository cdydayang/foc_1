#ifndef _MATH_H
#define _MATH_H

#include "stdint.h"
#include "lcm32f039_type.h"

//typedef   int8_t   s8;
//typedef   int16_t  s16;
//typedef   int32_t  s32;

//typedef   uint8_t  u8;
//typedef   uint16_t u16;
//typedef   uint32_t u32;

typedef struct
{
	s16 hCos;
	s16 hSin;
} 
Trig_Components;

extern Trig_Components MCM_Trig_Functions(s16 hAngle);
extern s32 MCM_Sqrt(s32 wInput);
extern s16 arctan2(s16 y1,s16 x1);
extern s16 GetTemperature(vu32 ADCvalue);
typedef struct
{  
	int16_t  As;  		// Input: phase-a stator variable
	int16_t  Bs;			// Input: phase-b stator variable
	int16_t  Cs;			// Input: phase-c stator variable  
	int16_t  Alpha;		// Output: stationary d-axis stator variable 
	int16_t  Beta;		// Output: stationary q-axis stator variable
}CLARKE_T;	            
                 
#define CLARKE_DEFAULTS {0, 0, 0, 0, 0} 

extern CLARKE_T clarke1;											
extern void CLARKE_MACRO1(CLARKE_T *u1);

typedef volatile struct 
{  
	int16_t  Alpha;		
	int16_t  Beta;		
	int16_t  Ds;		
	int16_t  Qs;	
	
	int16_t  Theta;		
	int16_t  Sin;		
	int16_t  Cos;		
} IPARK_T;	            
                
#define IPARK_DEFAULTS {0, 0, 0, 0, 0, 0, 0}
				   
extern IPARK_T ipark1;
										
void IPARK_MACRO1(volatile IPARK_T *v);

typedef struct 
{  
	int16_t  Alpha;		
	int16_t  Beta;		
	int16_t  Ds;			
	int16_t  Qs;			

	s32  Theta;		
	s32  Sin;			
	s32  Cos;			 
}PARK_T;	            
                  
#define PARK_DEFAULTS {0, 0, 0, 0, 0, 0, 0}

extern PARK_T park1;
extern PARK_T park2;

extern void PARK_MACRO1(PARK_T *u1);

#endif
