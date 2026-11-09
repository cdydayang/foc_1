#include "DMA_Interrupt.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_type.h"
#include "function.h"
#include "CurrentLoop.h"
#include "Estimation.h"
#include "SysProtection.h"
#include "Pump_Function.h"
#include "Pump_Comm.h"
#include "lcm32f039_div.h"
#include "User_PeripheralInit.h"
#include "IfControl.h"
#include "Pump_Comm.h"
#include "User_Interrupt.h"
#include "PI_Regulator.h"
#include "CurrentLoop.h"
#include "SpeedLoop.h"
#include "SVPWM.h"
#include "ADC_Calc.h"

u16 DMAC_CH1_2_Handler_Count = 0;

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAC_CH1_2_Handler(void)
{
	DMA_ClearITPendingBit(DMA1_FLAG_TC1);	
	
	DMAC_CH1_2_Handler_Count++;
	
	//三相电流采样、母线电压采样、clark park变换
	ADC_Sample_Calc();  
	
	/*FOC状态机，处理位置估算，电流环，ADC采样处理，SVPWM，更新比较值寄存器*/
	switch(SysPara.StateMachineFlag)
	{      		
		case MotorStandBy: 
						
			/*在哪个中断执行的运算，就在哪个中断初始化*/
			Invter_Init();
			Inside_Loop_Init();
			Pos_Esti_Init();	
			Driver_Para_Init();	
			IfInit();
		
			SysPara.OC_cnt = 0;
	
		
			#if (OV_UV_PRO_EN == 1)
				UnderOrOverVoltPro();
			#endif
				
		break;
		
		case MotorPrePos:
			
			/*初始强拉暂时不用*/
			SysPara.PrePosCnt++;
		
			SysPara.IdRef = 0;
		
			if(SysPara.PrePosCnt < 150)
			{
				IfStart.IqRef = IfModeCurrAMP;
				
				SysPara.MotorAngle = 32767;
			}
			else if((SysPara.PrePosCnt >= 150)&&(SysPara.PrePosCnt < 300))
			{
				IfStart.IqRef = IfModeCurrAMP;
				SysPara.MotorAngle = -16384;
			}
			else if(SysPara.PrePosCnt >= 300)
			{
				SysPara.PrePosCnt = 0;
				
				SysPara.StateMachineFlag = IfStartMode;
			}
		
			InsideLoopCalc();
				
			ipark1.Ds = IdLoop.Out;
			ipark1.Qs = IqLoop.Out;
			ipark1.Theta = SysPara.MotorAngle;

			IPARK_MACRO1(&ipark1);
		
			PWM_Calc.ValfaPu = (ipark1.Alpha * PWM_Calc.PwmTBPRD)>>14;
			PWM_Calc.VbetaPu = (ipark1.Beta * PWM_Calc.PwmTBPRD)>>14;
					
			Invter_Calc();
					
			PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);	

			Update_PWM();	
		break;
				
		case MotorRun:
			
			/*过压过流保护*/
			#if (PROTECTION_EN == 1)

			SysProtection(); //2.5us
			#endif	
			
			/*位置估算*/
			Pos_Esti_Calc();
		
			SysPara.MotorAngle = SmoEsti.SmoPLL.qAngleOut;
			SysPara.VoltAngle = SysPara.MotorAngle;

			SysPara.MotorSpeed = SmoEsti.SmoPLL.qSpeedOut;
	
			/*电流环*/
			InsideLoopCalc();
			
			/*反变换*/
			ipark1.Ds = IdLoop.Out;
			ipark1.Qs = IqLoop.Out;
			ipark1.Theta = SysPara.VoltAngle;//FOC_Angle;

			IPARK_MACRO1(&ipark1); //q14 output

//			ipark1.Alpha = 0;
//			ipark1.Beta = 1200;			
			
			PWM_Calc.ValfaPu = (ipark1.Alpha * PWM_Calc.PwmTBPRD)>>14;
			PWM_Calc.VbetaPu = (ipark1.Beta * PWM_Calc.PwmTBPRD)>>14;
				
			/*SVPWM*/
			Invter_Calc();
						
			PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);	
			
			/*PWM寄存器*/	
			Update_PWM();	
								
		break;
				
		case SysFault:
			Invter_Init();
			Inside_Loop_Init();
			Pos_Esti_Init();	
			Driver_Para_Init();	
		break;

	case IfStartMode:
		
			/*过压过流保护*/
			#if (PROTECTION_EN == 1)
				SysProtection(); 
			#endif
	
			Pos_Esti_Calc();
						
			IfAngleCalc();

			/*电流环*/
			InsideLoopCalc();
					
			/*反变换*/
			ipark1.Ds = IdLoop.Out;
			ipark1.Qs = IqLoop.Out;
			ipark1.Theta = SysPara.VoltAngle;//FOC_Angle;
		
			IPARK_MACRO1(&ipark1); //q14 output
	
//			ipark1.Alpha = 500;
//			ipark1.Beta = 0;			
			
			PWM_Calc.ValfaPu = (ipark1.Alpha * PWM_Calc.PwmTBPRD)>>14;
			PWM_Calc.VbetaPu = (ipark1.Beta * PWM_Calc.PwmTBPRD)>>14;
				
			/*SVPWM*/
			Invter_Calc();
					
			PWM_Calc.UCMP = RangeLimit(0, PWM_Calc.UCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.VCMP = RangeLimit(0, PWM_Calc.VCMP, PWM_Calc.PwmTBPRD);
			PWM_Calc.WCMP = RangeLimit(0, PWM_Calc.WCMP, PWM_Calc.PwmTBPRD);	
		
			/*PWM寄存器*/	
			Update_PWM();

			if(
					(IfStart.IqRef == IfModeSwitchIqAmp)&&\
					(SysPara.IdRef == NoLoadIdAMP)
			  )
			{					
					SpeedLoop.Ui = IfModeSwitchIqAmp<<14;
					
					SysPara.StateMachineFlag = MotorRun;
				  SpeedLoop.Ref = IfSwitchSpeed;
					SpeedLoop.Fdb = IfSwitchSpeed;
					SysPara.MotorSpeed = IfSwitchSpeed;
				
//					//观测器切换
// 				 SmoEsti.SmoPLL.qRef = SysPara.VoltAngle;
//				 SmoEsti.SmoPLL.qSpeedOut = IfSwitchSpeed;
//				 SmoEsti.SmoPLL.Ui = IfSwitchSpeed<<14;
			}
			break;
				
		default:  
			Invter_Init();
			Inside_Loop_Init();
			Pos_Esti_Init();	
			Driver_Para_Init();	
			IfInit();
		break;         
	}
}
