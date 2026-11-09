%开关频率
SwitchFrequency = 8e3;%Hz
Ts = 1/SwitchFrequency;%s

%电机参数
Np = 4;%极对数

Rs_realvalue = 6.7;%ohm,定子电阻
Ld_realvalue = 6.2e-3;%H,同步电感
Lq_realvalue = 6.2e-3;%H,同步电感
Lam_realvalue = 0.023;%Wb,磁链
Vdc = 24;%V
fn = 100;%Hz
In = 2/1.414;%A,线电流有效值
Un = Vdc/sqrt(2);%V,线电压有效值

Ib = sqrt(2)*In;%电流基值,相电流幅值
Vb = Vdc/1.732;%sqrt(2)*Un/sqrt(3);
Wb = 2*pi*fn; %角频率基值
tb = 1/Wb; %时间基值
Rb = Vb/Ib;%电阻基值
Lb = Rb*tb;%电感基值 
Pb = 1.5*Vb*Ib;%功率基值 
Tb = Pb*tb*Np;%转矩基值
fluxB = Vb*tb;%磁链基值

%标幺值
Rs = Rs_realvalue/Rb;%电阻标幺值
Ld = Ld_realvalue/Lb;%电感标幺值
Lq = Lq_realvalue/Lb;%电感标幺值
fluxF = Lam_realvalue/fluxB;%磁链标幺值
TsPu = Ts/tb;%开关周期标幺值
Jb = Np*Tb/((fn*2*pi)^2);
JPu = J_realvalue/Jb;%转动惯量标幺值

%电流环
CurrentLoopBandwidth = (SwitchFrequency/40)*2*pi/Wb;
Id_Kp = CurrentLoopBandwidth*Ld;
Id_Ki = CurrentLoopBandwidth*Rs*TsPu;

Iq_Kp = CurrentLoopBandwidth*Lq;
Iq_Ki = CurrentLoopBandwidth*Rs*TsPu;
CurrentLoopMax = 1.2;%Vdc/Vb;

%电流滤波器
LFP_BW = 1000;%Hz
LFP_Coef = LFP_BW*6.28/((LFP_BW*6.28)+SwitchFrequency);

%环路增益
PLL_Gain = 20*2*pi/Wb;
PLL_Outmax = 1.2;

PLL_Kp = PLL_Gain*2;%响应慢一点，误差精度高；%PLL_Gain*1.414,响应快一点，稳态精度差点
PLL_Ki = PLL_Gain*PLL_Gain*TsPu;

CurrGain = 1 - (Rs*TsPu/Ld);
VoltGain = TsPu/Ld;

%标幺值系统
Id_Kp_Q10 = Id_Kp*(2^10);
Id_Ki_Q14 = Iq_Ki*(2^14);

Iq_Kp_Q10 = Iq_Kp*(2^10);
Iq_Ki_Q14 = Iq_Ki*(2^14);

CurrentLoopMax_Q14 = CurrentLoopMax*(2^14);

TsPu_Q17 = TsPu*(2^17);

PLL_Kp_Q10 = PLL_Kp*(2^10);
PLL_Ki_Q14 = PLL_Ki*(2^14);
PLL_Max_Q14 = PLL_Outmax*(2^14);

CurrGain_Q14 = CurrGain*(2^14);
VoltGain_Q14 = VoltGain*(2^15);

LFP_Coef_Q8 = LFP_Coef*(2^8);
