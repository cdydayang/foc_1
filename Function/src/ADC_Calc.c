#include "ADC_Calc.h"
#include "ADC_Init.h"
#include "function.h"
#include "Math.h"

volatile u8 ADC_OffsetReady = 0;

#define ADC_OFFSET_SAMPLE_COUNT   ((s16)32)
#define ADC_CURRENT_GAIN_Q14_PER_COUNT ((s16)20) /* Original demo calibration; verify on actual board. */
#define ADC_BUS_GAIN_Q13           ((s16)15522)

static s16 SaturateS16(s32 x)
{
    if (x > 32767) return 32767;
    if (x < -32768) return -32768;
    return (s16)x;
}

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
        SysPara.CurrFirstAD = 0;
        SysPara.CurrThirdAD = 0;
        SysPara.CurrSencondAD = 0;
        SysPara.IdFilter = 0;
        SysPara.IqFilter = 0;
        SysPara.DcBusAD = dc_raw;
        return;
    }

    /* The original front end has inverted current polarity. Keep it explicit. */
    SysPara.CurrFirstAD = (s16)-(iu_raw - SysPara.IuOffset); /* U */
    SysPara.CurrThirdAD = (s16)-(iw_raw - SysPara.IwOffset); /* W */
    SysPara.CurrSencondAD = (s16)(-(s32)SysPara.CurrFirstAD - SysPara.CurrThirdAD); /* V */

    SysPara.CurrUphase = SysPara.CurrFirstAD;
    SysPara.CurrVphase = SysPara.CurrSencondAD;
    SysPara.CurrWphase = SysPara.CurrThirdAD;

    clarke1.As = SaturateS16((s32)SysPara.CurrUphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    clarke1.Bs = SaturateS16((s32)SysPara.CurrVphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    clarke1.Cs = SaturateS16((s32)SysPara.CurrWphase * ADC_CURRENT_GAIN_Q14_PER_COUNT);
    CLARKE_MACRO1(&clarke1);

    park1.Alpha = clarke1.Alpha;
    park1.Beta = clarke1.Beta;
    park1.Theta = SysPara.MotorAngle;
    PARK_MACRO1(&park1);

    /* 8 kHz sampled first-order LPF, same coefficients as the reference project. */
    SysPara.IdFilter = (s16)((144L * SysPara.IdFilter + 112L * park1.Ds) >> 8);
    SysPara.IqFilter = (s16)((144L * SysPara.IqFilter + 112L * park1.Qs) >> 8);

    SysPara.DcBusAD = dc_raw;
    scaled = ((s32)dc_raw * ADC_BUS_GAIN_Q13) >> 10;
    dc_bus_pu = SaturateS16(scaled);
    SysPara.DcBusFiltered = (s16)((227L * SysPara.DcBusFiltered + 29L * dc_bus_pu) >> 8);
}
