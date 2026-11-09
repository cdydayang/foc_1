#ifndef __SYSPROTECTION_H
#define __SYSPROTECTION_H

#include "function.h"
#include "Math.h"

#define PROTECTION_EN		1//1：调用过压和过流保护函数

#define FAST_OCP_EN			0//0：快速软件过流 1：软件过流加滤波

#define Hardware_OCP_EN     1//1：打开硬件过流
#define SOFTWARE_OCP_EN     1//1: 打开软件过流

#define OV_UV_PRO_EN        1//0//1: 使能过压欠压保护

#define OVER_TEMP_EN        0//1//1：过温保护使能

#define EstiFaultRestartEn  0//1:使能估算错误，失步后自动重启功能

extern void UnderOrOverVoltPro(void);

extern void SysProtection(void);
extern void SoftwareOC(void);

extern void ProtectParaInit(void);

#endif
