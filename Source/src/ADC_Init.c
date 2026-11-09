#include "lcm32f039_conf.h"
#include "lcm32f039.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_opa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcm32f039_tim.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_dma.h"
#include "lcm32f039_uart.h"
#include "ADC_Init.h"
#include "lcm32f039_type.h"
#include "SysProtection.h"

vu32  ADCConvertedRawData[ADC_SIZE];

void ADC_Config(void)
{
	ADC_InitTypeDef	ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/*每个元素表示对应CHx采样通道对应的输入ADIN，
	正常模式：CH0-CH15取值范围0-15对应ADIN0-15*/
	uint8_t ADIN_BUFF[16] = {0}; // CH0-CH15
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
	ADC_StopOfConversion(ADC);
	DMA_Cmd(DMA1_Channel1, DISABLE);
	//		ADC_VrefintSelect(ADC,4);	//选择参考电平 1：3.3V  2：5.0V  3:2.5V  4:4.0V  5:VDD  6: 外部IO输入参考电压  7：VRH输出			

	ADC_StructInit(&ADC_InitStructure);	


	ADIN_BUFF[0] = ADCIN_2_A;//Iu; 

	ADIN_BUFF[1] = ADCIN_3_A; //BUS

	ADIN_BUFF[2] = ADCIN_1_A; //旋钮
	ADIN_BUFF[3] = ADCIN_6_A; //NTC

	ADIN_BUFF[8] = ADCIN_12_B;  //Iw
	ADIN_BUFF[9] = ADCIN_11_B;  //Ibus
	ADIN_BUFF[10] = ADCIN_9_B;  //UBEMF
	ADIN_BUFF[11] = ADCIN_8_B;  //VBEMF
	ADIN_BUFF[12] = ADCIN_10_B;  //WBEMF


	ADC_ChannelConfig(4, ADIN_BUFF); //采样通道个数(内部+1)，输入通道,输入通道可随意组合，具体可参考用户手册

	ADC_InitStructure.ADC_ClkMode = ADC_ClockMode_SynClkDiv4; //	ADC->CFGR2->(0X01<<27)	SYS_CLK/2 时钟分频系数
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;		//	ADC->CFGR1->0x00
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;							//转换模式0=单次;1=连续;表示触发一次转换一个通道或是一个序列
	ADC_InitStructure.ADC_AutoWatiMode = ENABLE; 							//	ADC->CFGR1->bit13 0:自动延迟转换模式关闭  1: 自动延迟转换模式开启
	ADC_InitStructure.ADC_DMATRIG_LEVEL = FIFO_DMA_NOEMPTY;	
	ADC_InitStructure.ADC_DMA = ENABLE; 											//	ADC->CFGR1->bit0	0：DMA disabled 				1： DMA enabled

	ADC_InitStructure.ADC_DiscMode 	 = DISABLE; 								//	ADC->CFGR1->bit16 0： 断续模式禁止				1： 断续模式开启
	ADC_InitStructure.ADC_SamecMode  = ENABLE;//;//	DISABLE; //			 								//	ADC->CFGR2->bit0	0：连续采样模式  				1：同时采样
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//ADC_ExternalTrigConvEdge_Falling;	//ADC->EXTCFG->bit0&1->10：外部下降沿触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;					//ADC->EXTCFG   TIM1_TRGO触发采样
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;													//	ADC->CFGR1->bit5		0: 右对齐		1: 左对齐
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;								// 	ADC->CFGR1->bit2 		0： 向前扫描 (从 CHSEL0 到 CHSEL15 )	1： 向后扫描 (从 CHSEL15 到 CHSEL0 )
	ADC_InitStructure.ADC_Vref = ADC_Vref_Internal_5V;//ADC_Vref_Internal_3V3;//;//;//;//;//ADC_Vref_Internal_5V;//;	//ADC_Vref_Externa_Vrh4V ;//	

	ADC_InitStructure.ADC_SampleTime = 6;//4;		//取值范围0-7;AD 采样信号脉冲宽度。默认值为 010，脉冲宽度=(x+1)*CLK
	ADC_InitStructure.ADC_SampleDelayTime = 0;	//取值范围0-15;AD 采样延迟。同时采样模式下，一次采样周期内两次采样的延迟;
	ADC_InitStructure.ADC_AdjustTime = 4;		//取值范围0-7;AD 上电准备阶段脉冲宽度。默认值为 010，脉冲宽度=(x+1)*CLK
	ADC_InitStructure.ADC_ConvertTime = 0;//10;//10;//初始0;		//取值范围0、15;采样周期
	ADC_InitStructure.ADC_ConvertPulseTime = 0; //取值范围0-1;启动转换脉冲宽度。默认值为 0，脉冲宽度=(x+3)*CLK
	ADC_Init(ADC, &ADC_InitStructure);
	ADC->EXTCFG |=1<<2;
	ADC_Cmd(ADC, ENABLE);				    


	//{//DMA搬运ADC数据，ADC采样后存储在DR寄存器，所以设置外设基地址为DR寄存器，搬运至内存基地址，即自定义数组。
	//DMA搬运ADC数据，ADC采样后存储在DR寄存器，所以设置外设基地址为DR寄存器，搬运至内存基地址，即自定义数组。
		

	/*DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;				//用来设置DMA传输的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedRawData;	//内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//设置数据传输方向，即决定是从外设读取数据到内存还是从内存读数据到外设，这里我们是从外设ADC_DR寄存器读数据到内存
	DMA_InitStructure.DMA_BufferSize = ADC_SAMPLE_NUMF;							//设置一次传输数据量的大小
	DMA_InitStructure.DMA_MSIZE = DMA_MSIZE_1;								//突发事务传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//设置传输数据的时候 （外设） 地址是不变还是递增。
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//设置传输数据的时候 （内存 ）地址是不变还是递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; //设置外设的数据长度是为字节传输（8bits）还是半字（16bits）还是字传输（32bits）
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;			//设置内存的数据长度，同上。
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//设置是否进行循环采集
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//设置优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //设置是否是存储器到存储器模式传输
	/* Enable DMA1 channel1 TC interupt*/
	DMA_ITConfig(DMA1_Channel1, DMA1_FLAG_TC1, ENABLE); //配置DMA发送完成后中断
	DMA_RemapConfig(DMA1_Channel1, DMA_ReqNum5, DMA_REQ_ADC);
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_StartOfConversion(ADC);	
}
