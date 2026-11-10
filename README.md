# LCM32F039 PMSM I/F 电流环（不含 SMO）

本分支是从原 foc_1 仓库拆出的独立学习工程。master 不会被本分支修改。本分支只保留 I/F 开环启动 + Id/Iq 电流闭环所需文件。

## 本分支实现
- 固定电气角度进行 D 轴预定位。
- TIM14 慢任务逐步增加开环电气频率和 Iq 电流参考。
- TIM1 触发 ADC；DMA 完成中断执行快速电流环。
- 双电阻 U/W 采样，重构 V 相：Iv = -(Iu + Iw)。
- Clarke/Park、Id/Iq 双 PI、反 Park、SVPWM 和 TIM1 CCR 更新。
- 硬件 Break 关断、软件过流和直流母线电压保护入口。

## 明确剔除
SMO/PLL、转速闭环、速度环、泵通信、泵业务状态、BEMF 观测器采样、NTC 显示业务、上位机协议和原 demo 里未使用的外设驱动。

## 目录
- Device：LCM32F039 CMSIS、启动文件。
- Driver：本控制链需要的 RCC/GPIO/ADC/DMA/OPA/ACMP/DAC/TIM/DIV/FLASH/PWR 驱动。
- Source：板级初始化、中断和 main。
- Function：I/F、电流环、ADC 电流换算、PI、数学变换、SVPWM、保护。
- Project：Keil uVision 工程。

## 快速控制时序
TIM1 中心对齐 PWM（参考设置 8 kHz）触发 ADC；ADC DMA 完成中断执行：
ADC 原始值 → 偏置扣除 → U/W 电流 → V 相重构 → Clarke/Park → Id/Iq PI
→ 反 Park → SVPWM → 更新 TIM1 CCR → I/F 角度积分（供下一采样周期使用）。

TIM14 是慢任务，负责 I/F 频率和 Iq 参考斜坡，不在 8 kHz 快速中断里做斜坡。

## 定点单位和演示参数
保留原演示工程参数作为学习起点：系统时钟 96 MHz、PWM 8 kHz、TsPu=10294（Q17）、Kp=1151（Q10）、Ki=2488（Q14）。这些值来自原仓库 parameters.m 的演示电机，并非你的实际电机通用参数。
- 电流 Ref/Fdb：Q14，16384 表示 1.0 pu。
- PI Kp：Q10；Ki：Q14；积分累加器 Ui：Q28。
- PI 输出 Vd/Vq：Q14。
- Sin/Cos：Q15。
- 电气角度：16 位整周表示，65536 计数为 360°。
- IfStart.ReferSpeed：Q14 电气频率标幺指令。
- SysPara.TsPu：Q17。

## ADC 通道布局假设
本分支将同步采样序列精简为两个 A/B 采样对：
- DMA[0]：U 相电流（ADCIN_2_A，OPA2 输出）。
- DMA[1]：W 相电流（ADCIN_12_B，OPA1 输出）。
- DMA[2]：直流母线电压（ADCIN_3_A）。
- DMA[3]：未使用的 B 侧同步通道（ADCIN_0_B），仅用于保持第二采样对的数据布局。

这是依照原仓库 A/B 同步采样 DMA 排列方式裁剪出来的预期布局。必须在真实 LCM32F039 板上通过调试器核对四个 DMA 数值与实际信号；若芯片 DMA/FIFO 打包规则或板卡走线不同，应先修正数组映射，不能直接使能功率输出。

## 初次调试顺序
1. 用 Keil 打开 Project/IF_CurrentLoop.uvprojx，先编译，不接通功率级。
2. 检查 DMA 中断频率、DMA[0..3] 数据顺序。
3. 检查 ADC_OffsetReady 置位，IuOffset/IwOffset 稳定。
4. 验证重构的 U/V/W 电流之和接近零。
5. 观察 IdFilter/IqFilter、PI 输出和 U/V/W PWM 比较值。
6. 验证预定位固定角度，以及 I/F 角度平滑递增。
7. 功率级测试必须使用限流电源、可靠硬件过流/急停、固定的无负载电机，并从低母线电压和低电流目标开始。

## 必须按实际硬件重新核对
- U/W 电流采样电阻、运放增益、偏置、极性和 ADC 引脚。
- ADC A/B 同步采样数据排列和采样窗口。
- DC Bus ADC 比例、欠压/过压阈值。
- 电机 Rs/Ld/Lq/极对数/磁链及基准值。
- 电流 PI Kp/Ki、输出限幅、Id/Iq 参考值、启动斜坡。
- TIM1 PWM 频率、死区、ADC 触发点和实际中断频率。

ADC_CURRENT_GAIN_Q14_PER_COUNT=20、软件过流阈值 800 ADC counts、母线阈值 1200/850、预定位 Id=1000、I/F 目标频率 2000、Iq=3000 都只是原演示工程附近的初始参考值，不能代替实际标定。

## 已知限制
本分支没有转子位置反馈，也没有 SMO；I/F 电气频率指令不等于真实转子速度。频率斜坡过快、负载过大或电流采样方向错误都可能导致失步或过流。没有完成采样标定和保护验证之前，不要使能功率输出。

本分支尚未在实际 LCM32F039 硬件和 Keil 工具链上编译/运行验证；它是清理后的学习工程，不声称可直接用于量产。