/*
 * timer.c
 *
 *  Created on: 2022��1��25��
 *      Author: sdlm
 */

#include "timer.h"
#include "uart.h"
#include "math.h"
#include "prothesis.h"
#include "jy61p.h"
#include "stdio.h"
#include "adc.h"

extern int middle_zhi_controlFlag;
extern double Beta, Beta_last;
extern double W_BD;
extern double Alpha, Alpha_cos, Alpha_sin;
extern double F_A;
extern double T;
extern double delta_P;
extern double delta_P0;
extern double dd_P;

extern int valve_pwm_middle_zhi;
extern int valve_pwm_middle_bei;
extern int valve_pwm_push;

// �������ݱ�־λ
int DATA_SEND = 1;

// ADC�������ȡƽ��ֵ����
static Uint32 sum1 = 0, sum2 = 0,sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
static Uint16 SampleTable1[BUF_SIZE];
static Uint16 SampleTable2[BUF_SIZE];
static Uint16 SampleTable3[BUF_SIZE];
static Uint16 SampleTable4[BUF_SIZE];
static Uint16 SampleTable5[BUF_SIZE];
static Uint16 SampleTable6[BUF_SIZE];
static Uint16 SampleTable7[BUF_SIZE];

// ѹ�����ߵĴ�������������һ�������Ż������ѹ��������ֹ�󴥷�
static int counter_P = 0;

// �׹ؽڽǶ�ƽ��ֵ
static double angle_ave[5] = {0.0};


//��ʱ��0��ʼ������
//Freq��CPUʱ��Ƶ�ʣ�150MHz��
//Period����ʱ����ֵ����λus
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //���ö�ʱ��0���ж���ڵ�ַΪ�ж��������INT0
    EALLOW;
    PieVectTable.TINT0 = &TIM0_IRQn;
    EDIS;

    //ָ��ʱ��0�ļĴ�����ַ
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    //���ö�ʱ��0�����ڼĴ���ֵ
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    //���ö�ʱ��Ԥ���������ֵΪ0
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    //ȷ����ʱ��0Ϊֹͣ״̬
    CpuTimer0Regs.TCR.bit.TSS = 1;
    //����ʹ��
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    //��ʼ��ʱ������
    CpuTimer0Regs.TCR.bit.TSS=0;
    //����CPU��һ���жϲ�ʹ�ܵ�һ���жϵĵ�7��С�жϣ�����ʱ��0
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

}

void TIM1_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
    EDIS;

    EALLOW;
    PieVectTable.XINT13 = &TIM1_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    // Initialize timer period to maximum:
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer1Regs.TPR.all  = 0;
    CpuTimer1Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer1Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer1Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer1.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer1, Freq, Period);

    CpuTimer1Regs.TCR.bit.TSS=0;

    IER |= M_INT13;

}

interrupt void TIM0_IRQn(void)
{
    int i;
    double temp;
    Uint8 buf[100] = {};

    CpuTimer0.InterruptCount++;
    AdcRegs.ADCTRL2.all = 0x2000;   // �����SEQ1λд1����ת������
    // ADC��ѹת����10�β���ȡƽ��ֵ
    for (i = 0; i < AVG; i++) // AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {}  // Wait for interrupt��η����ж�
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable1[i] = (AdcRegs.ADCRESULT0 >> 4);
        SampleTable2[i] = (AdcRegs.ADCRESULT1 >> 4);
        SampleTable3[i] = (AdcRegs.ADCRESULT2 >> 4);
        SampleTable4[i] = (AdcRegs.ADCRESULT3 >> 4);
        SampleTable5[i] = (AdcRegs.ADCRESULT4 >> 4);
        SampleTable6[i] = (AdcRegs.ADCRESULT5 >> 4);
        SampleTable7[i] = (AdcRegs.ADCRESULT6 >> 4);
        sum1 = sum1 + SampleTable1[i];
        sum2 = sum2 + SampleTable2[i];
        sum3 = sum3 + SampleTable3[i];
        sum4 = sum4 + SampleTable4[i];
        sum5 = sum5 + SampleTable5[i];
        sum6 = sum6 + SampleTable6[i];
        sum7 = sum7 + SampleTable7[i];
    }

    // ת�����Һѹѹ��
    press1 = (((double)sum1) * 3 / 4096 / AVG) * K_GAIN1 + B_GAIN;
    press2 = (((double)sum2) * 3 / 4096 / AVG) * K_GAIN2 + B_GAIN;
    press3 = (((double)sum3) * 3 / 4096 / AVG) * K_GAIN3 + B_GAIN;

    // �ڲ�Һѹѹ����λ
    if(press1 >= P_MAX || press1 <= 0)
    {
        counter_P++;
        if(counter_P >= 15)
        {
            motor_stop();
            counter_P = 0;
        }
    }

    // ת������׹ؽڽǶ�
    angle = ((((double)sum4) * 3 / 4096 / AVG) * 100);//������ʵ�ʵ�ѹ 0-3v
    // 20220910  ���ȼ��� -> -12.70  ���ȼ��� -> 24.6
    angle = -186.4 + angle;
    temp = 0;
    for(i = 0; i < 4; i++)
    {
        angle_ave[i] = angle_ave[i + 1];
        temp = temp + angle_ave[i];
    }
    angle_ave[4] = angle;
    angle= (temp + angle_ave[4]) / 5; // 5�㻬��ƽ��

    // ת��������ѹ��
    P_foot1 = ((double)sum5) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v
    P_foot2 = ((double)sum6) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v
    P_foot3 = ((double)sum7) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v

    Beta_last = Beta;
    Beta = angle;

//    W_BD = (Beta - Beta_last) * PI / 180.0 / 0.005;   // ��ʱ����Ϊ5ms
    W_BD = (Beta - Beta_last) / 0.005;   // ��ʱ����Ϊ5ms
    delta_P0 = press2 - press3;
    if(CpuTimer0.InterruptCount >= 10)
    {
        Alpha_cos = (L_BD * cos(Beta * PI / 180.0) - d_x) / L_AB;
        Alpha_sin = sqrt(1 - Alpha_cos * Alpha_cos);
        // ����4�����T��F_A
        if(proState == MIDDLE_ZHI_STATE)
        {
            temp = Beta - xita_cp;
            T = (k1_cp * temp + k2_cp * temp * temp + b_cp * W_BD) * T_GAIN;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90.0 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;


            if(P_foot3 > 0.5)
            {
                middle_zhi_controlFlag = 1;
            }
            if(middle_zhi_controlFlag == 1)
            {
                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
                if(valve_pwm_middle_zhi < MIN_SPEED)
                {
                    valve_pwm_middle_zhi = MIN_SPEED;
                }
                else if(valve_pwm_middle_zhi > MAX_VALVE_4)
                {
                    valve_pwm_middle_zhi = MAX_VALVE_4;
                }
            }
//            valve_pwm_middle_zhi = max_valve4;
        }
        else if(proState == MIDDLE_ZHI_STATE)
        {
            temp = Beta - xita_cd;
            T = (k1_cd * temp + k2_cd * temp * temp + b_cd * W_BD) * T_GAIN;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;
            valve_pwm_middle_zhi = 0;
            valve_pwm_middle_bei = (angle + 4) / 3 * MAX_VALVE_3;
            if(valve_pwm_middle_bei < MIN_SPEED)
            {
                valve_pwm_middle_bei = MIN_SPEED;

            }
            else if(valve_pwm_middle_bei > MAX_VALVE_3)
            {
                valve_pwm_middle_bei = MAX_VALVE_3;
            }
        }
        else if(proState == PUSH_STATE)
        {
            temp = Beta - xita_pp;
            T = (k1_pp * temp + k2_pp * temp * temp + b_pp * W_BD) * T_GAIN;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;

            valve_pwm_push = valve_pwm_push + (int)(Kp * dd_P);
            if(valve_pwm_push < MIN_VALVE_2)
            {
                valve_pwm_push = MIN_VALVE_2;
            }
            else if(valve_pwm_push > MAX_SPEED)
            {
                valve_pwm_push = MAX_SPEED;
            }
            valve_pwm_push = MIN_VALVE_2;
        }
    }

    // ������������λ��
    if(CpuTimer0.InterruptCount % 2 == 0 && DATA_SEND == 1)
    {
        Uint16 dataLen = sprintf(buf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %d %d %d %d %d\r\n", angle, press1, press2, press3,
                                P_foot1, P_foot2, P_foot3, proState, motorState, valve_pwm_middle_zhi, valve_pwm_middle_bei, valve_pwm_push);
        usartb_sendData(buf, dataLen);
    }

    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    sum5 = 0;
    sum6 = 0;
    sum7 = 0;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    EDIS;
}

interrupt void TIM1_IRQn(void)
{
    Uint8 flag = 0x4;
    if(jy61p_GetData(flag, 0x50, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //�õ�����������
    }
    DELAY_US(100);
    if(jy61p_GetData(flag, 0x51, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //�õ�����������
    }
    DELAY_US(100);
    if(jy61p_GetData(flag, 0x52, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //�õ�����������
    }
//    jy61p_SendData(flag, a, w, Angle);    //�õ�����������
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}



void TIM2_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2
    EDIS;

    EALLOW;
    PieVectTable.TINT2 = &TIM2_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    // Initialize timer period to maximum:
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer2.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer2, Freq, Period);

    CpuTimer2Regs.TCR.bit.TSS=0;

    IER |= M_INT14;

}

interrupt void TIM2_IRQn(void)
{


}




