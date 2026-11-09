#include "User_Interrupt.h"
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
#include "DMA_Interrupt.h"
#include "CurrentLoop.h"
#include "SVPWM.h"
#include "SpeedLoop.h"
#include "ADC_Init.h"

u16 TIM1_CC_Handler_Count = 0;
u16 TIM1_NON_CC_Handler_Count = 0;
u16 TIM14_CNT_10MS = 0;
u16 TIM14_CNT_500MS = 0;

void TIM14_Handler(void)
{
  TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		
	TIM14_CNT_10MS++;
	TIM14_CNT_500MS++;
	
	//MOTOR_FAULT_SET;

	/*10ms task，执行一些慢任务,10ms*/
	if(TIM14_CNT_10MS > 10)
	{
		TIM14_CNT_10MS = 0;
		
		/*MOS温度*/
		PumpDisplay.MOS_Temp = ((189*T_IPM_ADC_DATA)>>12) - (19886>>10);//GetTemperature(T_IPM_ADC_DATA);
		
		#if (OVER_TEMP_EN == 1)
			if(PumpDisplay.MOS_Temp > 85)//85℃报警
			{
				SysPara.StateMachineFlag = SysFault;
				PWMOutputs_DISABLE(); 
				debug.FaultFlag = MotorOverTemp;
			}		
		#endif	
	}

	/*判断电机是否在加速，500ms执行1次*/
	if(TIM14_CNT_500MS > 1000)
	{
		TIM14_CNT_500MS = 0;

	}

	/*主要处理速度环，弱磁，缺相保护*/
	switch(SysPara.StateMachineFlag)
	{ 
		
		case MotorStandBy:
			
			ProtectParaInit();
			OutsideLoopInit();				
			//StartWithSpeed.flag = Initial;
			break;
		
		case MotorPrePos:
			
			break;
				
		case MotorRun:
		
			Id_Calc();
			OutsideLoopCalc();	//速度环
													
			break;
				
		case SysFault:
			OutsideLoopInit();
			break;

		case IfStartMode:
			
			IfSpeedCalc();
		
			break;
									 
		default:  
			OutsideLoopInit();					
			break;         
	}
		
	//MOTOR_NO_FAULT_SET;
}

void TIM1_CC_Handler(void)
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC5);
	TIM1_CC_Handler_Count++;
}

void SysTick_Handler(void)
{
	
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_NON_CC_Handler(void) //刹车中断
{
	TIM_ClearITPendingBit(TIM1, TIM1_NON_CC_IRQn);
	TIM1_NON_CC_Handler_Count++;

	/*硬件过流保护*/
	if(TIM1->SR_b.B2IF == 1)//硬件刹车
	{
		__NOP();
		TIM_ClearITPendingBit(TIM1, 0x100);

		PWMOutputs_DISABLE();//关PWM	
		SysPara.StateMachineFlag = SysFault;		
		debug.FaultFlag = HardOCP; 

		//RELAY_OFF;

		debug.FaultDetial[0] = SysPara.CurrFirstAD;
		debug.FaultDetial[1] = SysPara.CurrSencondAD;
		debug.FaultDetial[2] = SysPara.CurrThirdAD;
		debug.FaultDetial[3] = SysPara.DcBusFiltered;
		debug.FaultDetial[4] = SysPara.MotorSpeed;
	}
	else if(TIM1->SR_b.BIF ==1)//软件手动刹车
	{

		__NOP();
		//		TIM1->BDTR |=(0<<13);  //  刹车1 BKE使能
		TIM_ClearITPendingBit(TIM1, TIM_IT_Break);		
	}
}

void UART1_Handler(void)
{
	if (UART_GetITStatus(UART1, UART_IT_TX) == SET)
	{
		UART_ClearITPendingBit(UART1, UART_IT_TX);
		
	}
	if (UART_GetITStatus(UART1, UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(UART1, UART_IT_RX);
		Pump_CommRxByte(UART_ReceiveData(UART1));
	}
}

void UART3_Handler(void)
{
 if (UART_GetITStatus(UART3, UART_IT_TX) == SET)
 {
		UART_ClearITPendingBit(UART3, UART_IT_TX);
 }
 if (UART_GetITStatus(UART3, UART_IT_RX) == SET)
 {
		//接收数据处
		UART_ClearITPendingBit(UART3, UART_IT_RX); 
 }
}
