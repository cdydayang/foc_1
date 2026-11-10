#include "lcm32f039.h"
#include "lcm32f039_conf.h"
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_tim.h"
#include "lcm32f039_acmp.h"
#include "lcm32f039_dac.h"
#include "lcm32f039_dma.h"

#include "function.h"
#include "IfControl.h"
#include "CurrentLoop.h"
#include "SVPWM.h"
#include "ADC_Calc.h"
#include "ADC_Init.h"
#include "Timer_Init.h"
#include "GPIO_Init.h"
#include "OPA_Init.h"
#include "User_PeripheralInit.h"

/* 启动按键 PF8、停止按键 PC13：有效电平应按实际板卡电路确认。 */
#define START_MOTOR_COMMAND   (GPIOF->IDR_b.IDR8)
#define STOP_MOTOR_COMMAND    (GPIOC->IDR_b.IDR13)

int main(void)
{
    /* 第 1 步：配置芯片系统时钟。参考工程运行频率为 96 MHz。 */
    SystemInit();
    SetSysClock();

    /* 打开本工程会用到的 GPIO、ADC、模拟模块和 TIM1 外设时钟。 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ANACTRL, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ADC, ENABLE);
    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB |
        RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF, ENABLE);

    /*
     * 第 2 步：先初始化所有软件控制变量。
     * 特别注意：必须在启动 TIM1 计数器和开放 DMA 中断之前完成。
     * 否则第一次 ADC DMA 中断可能先于 SysPara/PI/PWM_Calc 初始化。
     * Driver_Para_Init() 内部会初始化 SysPara、IfStart 和 Id/Iq PI。
     */
    Driver_Para_Init();
    Invter_Init();

    /* SysTick 只保留系统节拍入口，不在里面执行电流环运算。 */
    SysTick_Config(UPDS_SYS_TICK_PERIOD);

    /* 第 3 步：板级 IO、模拟前端、ADC 通道和硬件比较器。 */
    GPIO_Config();
    DAC_Config();
    OPA_Config();
    ADC_Config();
    ACMP_Config();

    /*
     * 第 4 步：配置 TIM1 三相互补 PWM。
     * 先确保 PWM 输出关闭，并写入一组安全的初始比较值。
     */
    TIM1_Config();
    PWMOutputs_DISABLE();
    Update_PWM();

    /* 第 5 步：启动 TIM14 慢任务定时器，更新 I/F 频率和 Iq 斜坡。 */
    TIM14_Config();

    /*
     * 第 6 步：最后配置中断并启动 TIM1 计数器。
     * 从这一步开始 ADC DMA 中断可能到来，前面的软件数据已经全部就绪。
     */
    NVIC_Config();

    while (1)
    {
        switch (SysPara.StateMachineFlag)
        {
            case MotorStandBy:
                /* 待机：功率输出关闭，等待 32 组 ADC 零点采样完成。 */
                PWMOutputs_DISABLE();
                GPIO_SetBits(GPIOA, GPIO_Pin_12);

                if ((START_MOTOR_COMMAND != 0) && (ADC_OffsetReady != 0))
                {
                    /* 固定电气角度，准备 D 轴预定位，然后允许 PWM 输出。 */
                    IfPrePosPrepare();
                    SysPara.StateMachineFlag = MotorPrePos;
                    PWMOutputs_ENABLE();
                    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
                }
                break;

            case MotorPrePos:
            case IfStartMode:
                /* 运行中检测停止命令：关 PWM、回到待机并清除 PI/I-F 状态。 */
                if (STOP_MOTOR_COMMAND != 0)
                {
                    PWMOutputs_DISABLE();
                    SysPara.StateMachineFlag = MotorStandBy;
                    IfInit();
                    Inside_Loop_Init();
                }
                break;

            case SysFault:
            default:
                /* 故障状态只允许保持关断，排查原因后再复位重启。 */
                PWMOutputs_DISABLE();
                GPIO_ResetBits(GPIOA, GPIO_Pin_4);
                break;
        }
    }
}
