#include "ADC_Calc.h"
#include "ADC_Init.h"
#include "function.h"
#include "Math.h"

/* 零点标定完成标志：1 表示 U/W 两路采样偏置已经计算完成。 */
volatile u8 ADC_OffsetReady = 0;

#define ADC_OFFSET_SAMPLE_COUNT          ((s16)32)
#define ADC_CURRENT_GAIN_Q14_PER_COUNT   ((s16)20)    /* 原演示板量程系数，实际必须标定 */
#define ADC_BUS_GAIN_Q13                ((s16)15522) /* 原演示板母线电压比例，实际必须标定 */

/* 将 32 位中间计算结果限制到 16 位有符号数范围。 */
static s16 SaturateS16(s32 x)
{
    if (x > 32767) return 32767;
    if (x < -32768) return -32768;
    return (s16)x;
}

/*
 * 待机期间采集 32 组 U/W ADC 原始值并求平均。
 * 电流检测放大器通常在零电流时输出一个中点偏置；
 * 后续每次采样必须减去该偏置，才得到正负方向的相电流变化量。
 */
static void CurrentOffsetTrack(void)
{
    s16 iu = (s16)PHASE_IA_ADC_DATA;
    s16 iw = (s16)PHASE_IC_ADC_DATA;

    if ((SysPara.StateMachineFlag == MotorStandBy) && (ADC_OffsetReady == 0))
    {
        SysPara.IuSum += iu;
        SysPara.IwSum += iw;
        SysPara.CurrOffsetCnt++;

        if (SysPara.CurrOffsetCnt >= ADC_OFFSET_SAMPLE_COUNT)
        {
            SysPara.IuOffset = (s16)(SysPara.IuSum / ADC_OFFSET_SAMPLE_COUNT);
            SysPara.IwOffset = (s16)(SysPara.IwSum / ADC_OFFSET_SAMPLE_COUNT);
            SysPara.CurrOffsetCnt = 0;
            SysPara.IuSum = 0;
            SysPara.IwSum = 0;
            ADC_OffsetReady = 1;
        }
    }
}

/*
 * 快速 ADC 处理，每次 DMA 搬运完成调用：
 * 1) 扣除 U/W 零点；
 * 2) 按双电阻采样关系重构第三相：Iv = -(Iu + Iw)；
 * 3) ADC count 转成 Q14 电流；
 * 4) Clarke 得到 Ialpha/Ibeta，再用开环角度 Park 得到 Id/Iq；
 * 5) 低通滤波后交给两个 PI；
 * 6) 计算直流母线电压标幺值，供 SVPWM 做母线补偿。
 */
void ADC_Sample_Calc(void)
{
    s16 iu_raw;
    s16 iw_raw;
    s16 dc_raw;
    s16 dc_bus_pu;
    s32 scaled;

    CurrentOffsetTrack();

    iu_raw = (s16)PHASE_IA_ADC_DATA;
    iw_raw = (s16)PHASE_IC_ADC_DATA;
    dc_raw = (s16)VDC_ADC_DATA;

    if (ADC_OffsetReady == 0)
    {
        /* 零点尚未完成：禁止把未标定的电流送入 PI。 */
        SysPara.CurrFirstAD = 0;
        SysPara.CurrThirdAD = 0;
        SysPara.CurrSencondAD = 0;
        SysPara.IdFilter = 0;
        SysPara.IqFilter = 0;
        SysPara.DcBusAD = dc_raw;
        return;
    }

    /* 原参考板的电流模拟前端极性为反相，这里保留负号；上板必须验证。 */
    SysPara.CurrFirstAD = (s16)-(iu_raw - SysPara.IuOffset); /* U 相 ADC count */
    SysPara.CurrThirdAD = (s16)-(iw_raw - SysPara.IwOffset); /* W 相 ADC count */
    SysPara.CurrSencondAD = (s16)(-(s32)SysPara.CurrFirstAD - SysPara.CurrThirdAD); /* V 相重构 */

    SysPara.CurrUphase = SysPara.CurrFirstAD;
    SysPara.CurrVphase = SysPara.CurrSencondAD;
    SysPara.CurrWphase = SysPara.CurrThirdAD;

    /* 将 ADC count 转为 Q14：20 是原参考板每 count 的软件标定乘数。 */
    clarke1.As = SaturateS16((s32)SysPara.CurrUphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    clarke1.Bs = SaturateS16((s32)SysPara.CurrVphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    clarke1.Cs = SaturateS16((s32)SysPara.CurrWphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    CLARKE_MACRO1(&clarke1);

    /* Park 的角度采用本周期 I/F 开环电气角度。 */
    park1.Alpha = clarke1.Alpha;
    park1.Beta = clarke1.Beta;
    park1.Theta = SysPara.MotorAngle;
    PARK_MACRO1(&park1);

    /* 8 kHz 采样的一阶低通滤波：144/256 旧值 + 112/256 新值。 */
    SysPara.IdFilter = (s16)((144L * SysPara.IdFilter + 112L * park1.Ds) >> 8);
    SysPara.IqFilter = (s16)((144L * SysPara.IqFilter + 112L * park1.Qs) >> 8);

    /* Vbus ADC 转换成以 Vb 为基值的 Q13 标幺值，供 SVPWM 计算补偿。 */
    SysPara.DcBusAD = dc_raw;
    scaled = ((s32)dc_raw * ADC_BUS_GAIN_Q13) >> 10;
    dc_bus_pu = SaturateS16(scaled);
    SysPara.DcBusFiltered = (s16)((227L * SysPara.DcBusFiltered + 29L * dc_bus_pu) >> 8);
}
