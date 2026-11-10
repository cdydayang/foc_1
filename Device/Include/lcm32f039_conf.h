#ifndef __LCM32F039_CONF_H
#define __LCM32F039_CONF_H
#include "lcm32f039_rcc.h"
#include "lcm32f039_gpio.h"
#include "lcm32f039_adc.h"
#ifdef USE_FULL_ASSERT
#define uint8_t unsigned char
#define uint32_t unsigned int
#define assert_param(expr) ((expr) ? ((void)0) : (assert_failed((uint8_t *)__FILE__, __LINE__)))
void assert_failed(uint8_t *file, uint32_t line);
#undef uint8_t
#undef uint32_t
#else
#define assert_param(expr) ((void)0)
#endif
#endif
