# LCM32F039 PMSM I/F 电流环（不含 SMO）

本分支是从原 foc_1 仓库拆出的独立学习工程。master 不会被本分支修改。当前工程只保留 I/F 开环启动、Id/Iq 电流闭环及其必要外设驱动。

## 本分支实现
- 固定电气角度进行 D 轴预定位。
- TIM14 慢任务逐步增加开环电气频率和 Iq 参考。
- TIM1 触发 ADC；DMA 完成中断执行快速电流环。
- 双电阻 U/W 采样，重构 V 相：Iv = -(Iu + Iw)。
- Clarke/Park、Id/Iq 双 PI、反 Park、SVPWM 和 TIM1 CCR 更新。
- 硬件 Break 关断、软件过流和母线电压保护入口。

## 不包含
SMO/PLL、速度闭环/速度 PI、泵通信、泵业务、BEMF 观测器、NTC 显示、上位机协议。

## 目录
- Device：LCM32F039 CMSIS 和启动文件。
- Driver：本控制链需要的 RCC/GPIO/ADC/DMA/OPA/ACMP/DAC/TIM/DIV/FLASH/PWR 驱动。
- Source：板级初始化、中断和 main。
- Function：I/F、电流环、ADC 电流换算、PI、数学变换、SVPWM、保护。
- Project：Keil uVision 工程 `Project/IF_CurrentLoop.uvprojx`。

## 编译日志中问题的修复
1. `IRQn_Type`、`__NVIC_PRIO_BITS` 未定义：修正 TIM/DIV 驱动头文件顺序，必须先包含 `lcm32f039.h`，再引用 CMSIS 内核声明。`lcm32f039.h` 在定义设备 IRQ 枚举后才包含 `core_cm0.h`。
2. `ADCIN_0_B` 未定义：恢复原板 B 组实际通道 `ADCIN_11_B`，没有虚构通道。
3. `IdLoop/IqLoop` 未定义：DMA 快速中断明确包含 `PI_Regulator.h`。
4. 找不到 `lcm32f039_uart.h`：GPIO 初始化已重写，移除本工程不使用的 UART/通信依赖。
5. SysTick IRQ 符号转换警告：SysTick 是 Cortex-M 内核异常，不通过外设 NVIC_InitTypeDef 配置；由 `SysTick_Config()` 管理。
6. TIM14 未使用局部变量警告：删除未使用的 NVIC 结构体。
7. 初始化竞争：先初始化 SysPara、PI 和 PWM 软件结构，再开放 DMA 中断并启动 TIM1 计数器。

## 快速控制时序
TIM1 中心对齐 PWM（参考值 8 kHz）触发 ADC；DMA 完成中断执行：
ADC 原始值 → 偏置扣除 → U/W 相电流 → V 相重构 → Clarke/Park → Id/Iq PI
→ 反 Park → SVPWM → 更新 TIM1 CCR → I/F 角度积分（供下一周期使用）。

TIM14 约 500 Hz（96 MHz / 192 / 1001）运行慢任务，更新频率和 Iq 斜坡。

## ADC DMA 映射（按原始仓库配置裁剪）
原工程 `ADC_Init.c` 使用 A/B 同步采样，通道映射为：
- A 侧第 0 个：ADCIN_2_A（U 相电流）
- B 侧第 0 个：ADCIN_12_B（W 相电流）
- A 侧第 1 个：ADCIN_3_A（母线电压）
- B 侧第 1 个：ADCIN_11_B（直流侧电流）

精简后 DMA 缓冲区保留 4 个字，预期交错顺序为 `[A0, B0, A1, B1]`：
- `ADCConvertedRawData[0]`：U 相电流
- `ADCConvertedRawData[1]`：W 相电流
- `ADCConvertedRawData[2]`：母线电压
- `ADCConvertedRawData[3]`：直流侧电流（当前电流环不使用）

这个顺序根据原工程的 ADC A/B 同步配置与 DMA 缓冲区宏定义推导。请上板用调试器核对四个 DMA 数值和实际通道；若芯片 FIFO 实际排列不同，先修正映射再使能功率级。

## 定点单位和演示参数
保留原演示工程参数作为理解起点：系统时钟 96 MHz、PWM 8 kHz、TsPu=10294（Q17）、Kp=1151（Q10）、Ki=2488（Q14）。这些数值来自原仓库 parameters.m 的演示电机，不是你的实际电机通用参数。
- Id/Iq Ref/Fdb：Q14，16384 表示 1.0 pu。
- PI Kp：Q10；Ki：Q14；积分累加器 Ui：Q28。
- PI 输出 Vd/Vq：Q14。
- Sin/Cos：Q15。
- 电气角度：16 位整周表示，65536 计数为 360°。
- IfStart.ReferSpeed：Q14 电气频率标幺指令。
- SysPara.TsPu：Q17。

## 必须按实际硬件重新核对
- U/W 电流采样电阻、运放增益、偏置、极性和 ADC 引脚。
- ADC A/B 同步采样 DMA 排列和 PWM 采样窗口。
- DC Bus ADC 比例、欠压/过压阈值。
- 电机 Rs/Ld/Lq/极对数/磁链和基准值。
- 电流 PI Kp/Ki、输出限幅、Id/Iq 参考值和斜坡。
- TIM1 PWM 频率、死区、ADC 触发点和实际中断频率。

`ADC_CURRENT_GAIN_Q14_PER_COUNT=20`、软件过流阈值 800 ADC counts、母线阈值 1200/850、预定位 Id=1000、I/F 目标频率 2000、Iq=3000 都只是原演示工程附近的初始参考值，不能代替实际标定。

## 第一次调试顺序
1. Keil 打开 `Project/IF_CurrentLoop.uvprojx`，先编译，不连接电机功率级。
2. 检查 DMA 中断频率和 DMA[0..3] 通道对应关系。
3. 检查 `ADC_OffsetReady` 置位以及 `IuOffset/IwOffset` 稳定。
4. 用已知电流或低压注入核对 U/W 电流正负方向，验证 U+V+W 约等于 0。
5. 观察 IdFilter/IqFilter、PI 输出和三相 PWM 比较值。
6. 验证预定位角度固定、I/F 运行角度平滑递增。
7. 功率级测试必须使用限流电源、可靠硬件过流/急停、固定的无负载电机，并从低母线电压和低电流目标开始。

本分支还没有在用户本机 Keil 工具链或 LCM32F039 硬件上完成编译/运行验证；这里完成的是源代码修复与工程引用检查，不声称已上板验证。
