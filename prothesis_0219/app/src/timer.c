/*
 * timer.c
 *
 *  Created on: 2022��1��25��
 *      Author: sdlm
 */

#include "timer.h"
#include "leds.h"
#include "uart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#include "imu901.h"
#include "prothesis.h"

short gyroxa, gyroya, gyroza;    //������ԭʼ����
float pitcha, rolla, yawa;
int num1 = 0;
int num2 = 0;


extern double press1,press2,press3;
extern float arg;
extern double arg_ave[5];
extern Uint16 press_plante1;
extern Uint16 press_plante2;
extern Uint16 press_plante3;

extern int flag_state1;

extern double volt1,volt2,volt3,volt4,volt5,volt6,volt7;
extern Uint32 sum1,sum2,sum3,sum4,sum5,sum6,sum7;
extern Uint16 SampleTable1[BUF_SIZE];
extern Uint16 SampleTable2[BUF_SIZE];
extern Uint16 SampleTable3[BUF_SIZE];
extern Uint16 SampleTable4[BUF_SIZE];
extern Uint16 SampleTable5[BUF_SIZE];
extern Uint16 SampleTable6[BUF_SIZE];
extern Uint16 SampleTable7[BUF_SIZE];


extern int valve_pwm_middle_zhi;
extern int valve_pwm_middle_bei;
extern int valve_pwm_push;
extern int switch_flag;
extern int state_motor;
extern int state;

extern int counter_time;
extern int counter;

extern int flag_run;

extern int counter_P;

extern double Beta, Beta_last;   // �Ƕ���

extern double W_BD;

extern double Alpha, Alpha_cos, Alpha_sin;

extern double T, F_A;

extern double delta_P, delta_P0;

extern double dd_P;


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
//    //ʹ�����ж�
//    EINT;
//    ERTM;

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
    double temp,temp2;
    CpuTimer0.InterruptCount++;
    EALLOW;
    AdcRegs.ADCTRL2.all = 0x2000;//�����SEQ1λд1����ת������
    for (i = 0;i < AVG;i++)//AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {} // Wait for interrupt��η����ж�
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
    press1 = ((((double)sum1) * 3 / 4096 / AVG) * gain1 - 4) * 1.25;
    scia_float(press1);
    press2 = ((((double)sum2) * 3 / 4096 / AVG) * gain2 - 4) * 1.25;
    scia_xmit(' ');
    scia_float(press2);
    press3 = ((((double)sum3) * 3 / 4096 / AVG) * gain3 - 4) * 1.25;
    scia_xmit(' ');
    scia_float(press3);

    if(press1 >= P_max || press1 <= 0)
    {
        counter_P++;
        if(counter_P >= 15)
        {
            motor_stop();
            flag_run = 0;
            UNLOAD();
            counter_P = 0;
            state = 0;
        }
    }
    arg = ((((double)sum4) * 3 / 4096 / AVG) * 100);//������ʵ�ʵ�ѹ 0-3v
    arg = 66.5 - arg;     // 70Ϊ��λ�ĽǶȴ�����ֵ
    temp2 = 0;
    for(i=0;i<4;i++)
    {
        arg_ave[i] = arg_ave[i+1];
        temp2 = temp2 + arg_ave[i];
    }
    arg_ave[4] = arg;
    arg = (temp2 + arg_ave[4]) / 5; // 10�㻬��ƽ��
    scia_xmit(' ');
    scia_float(arg);

    volt5 = ((double)sum5) * 3 / 4096 / AVG;//������ʵ�ʵ�ѹ 0-3v
    volt6 = ((double)sum6) * 3 / 4096 / AVG;//������ʵ�ʵ�ѹ 0-3v
    volt7 = ((double)sum7) * 3 / 4096 / AVG;//������ʵ�ʵ�ѹ 0-3v
    Beta_last = Beta;
    Beta = arg;

//    W_BD = (Beta - Beta_last) * PI / 180.0 / 0.005;   // ��ʱ����Ϊ5ms
    W_BD = (Beta - Beta_last) / 0.005;   // ��ʱ����Ϊ5ms
    delta_P0 = press2 - press3;
    if(CpuTimer0.InterruptCount >= 10)
    {
        Alpha_cos = (L_BD * cos(Beta * PI / 180.0) - d_x) / L_AB;
        Alpha_sin = sqrt(1 - Alpha_cos * Alpha_cos);
        // ����4�����T��F_A
        if(state == 1)
        {
            temp = Beta - xita_cp;
            T = (k1_cp * temp + k2_cp * temp * temp + b_cp * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;


            if(volt7 > 0.5)
            {
                flag_state1 = 1;

            }
            if(flag_state1 == 1)
            {
                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
                if(valve_pwm_middle_zhi < minspeed)
                {
                    valve_pwm_middle_zhi = minspeed;
                }
                else if(valve_pwm_middle_zhi > max_valve4)
                {
                    valve_pwm_middle_zhi = max_valve4;
                }
            }
//            valve_pwm_middle_zhi = max_valve4;
        }
        else if(state == 2)
        {
            temp = Beta - xita_cd;
            T = (k1_cd * temp + k2_cd * temp * temp + b_cd * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;
            valve_pwm_middle_zhi = 0;
            valve_pwm_middle_bei = (arg + 4) / 3 * max_valve3;
            if(valve_pwm_middle_bei < minspeed)
            {
                valve_pwm_middle_bei = minspeed;

            }
            else if(valve_pwm_middle_bei > max_valve3)
            {
                valve_pwm_middle_bei = max_valve3;
            }
//            valve_pwm_middle_zhi = 0;
//            valve_pwm_middle_bei = max_valve3;

//            if(delta_P>=0 && valve_pwm_middle_zhi>0)
//            {
//                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
//                if(valve_pwm_middle_zhi<minspeed)
//                {
//                    valve_pwm_middle_zhi = minspeed;
//                }
//                else if(valve_pwm_middle_zhi>=max_valve4)
//                {
//                    valve_pwm_middle_zhi = max_valve4;
//                }
//            }
//            else
//            {
//                valve_pwm_middle_bei = valve_pwm_middle_bei + (int)(Kp_2 * dd_P);
//                if(valve_pwm_middle_bei<minspeed)
//                {
//                    valve_pwm_middle_bei = minspeed;
//
//                }
//                else if(valve_pwm_middle_bei>max_valve3)
//                {
//                    valve_pwm_middle_bei = max_valve3;
//                }
//            }
//            valve_pwm_middle_zhi = 0;
//            valve_pwm_middle_bei = 1800;
        }
        else if(state == 3)
        {
            temp = Beta - xita_pp;
            T = (k1_pp * temp + k2_pp * temp * temp + b_pp * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // ���delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;

            valve_pwm_push = valve_pwm_push + (int)(Kp * dd_P);
            if(valve_pwm_push < min_valve2)
            {
                valve_pwm_push = min_valve2;
            }
            else if(valve_pwm_push > maxspeed)
            {
                valve_pwm_push = maxspeed;
            }
            valve_pwm_push = min_valve2;
        }
    }
    if(state == 2 && arg > 10.0)
    {
        counter++;
    }
    else if(state == 4)
    {
        counter_time++;
    }
    scia_xmit(' ');
    scia_float(delta_P);
    scia_xmit(' ');
    if(state == 1)
    {
        scia_int(valve_pwm_middle_zhi);
        scia_xmit(' ');
    }
    else if(state == 2)
    {
        if(delta_P>=0)
        {
            scia_int(valve_pwm_middle_zhi);
            scia_xmit(' ');
        }
        else
        {
            scia_int(valve_pwm_middle_bei);
            scia_xmit(' ');
        }
    }
    else
    {
        scia_int(valve_pwm_push);
        scia_xmit(' ');
    }



    scia_xmit(state_motor+0x30);


    scia_xmit(' ');
    scia_float(volt5);
    scia_xmit(' ');
    scia_float(volt6);
    scia_xmit(' ');
    scia_float(volt7);

    scia_xmit(' ');
    scia_xmit(state+0x30);

    scia_xmit('\r');
    scia_xmit('\n');

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
    EALLOW;
    num2++;
    if(num2 % 100 == 0)
    {
        LED12_TOGGLE;
        num2 = 0;
    }

    EDIS;
    if(mpu_dmp_get_data(&pitcha, &rolla, &yawa) == 0)
    {
        MPU_Get_Gyroscope(&gyroxa, &gyroya, &gyroza);    //�õ�����������
//        mpu6050_send_data((int)(rolla * 100), (int)(pitcha * 100), (int)(yawa * 10), gyroxa, gyroya , gyroza);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
    }

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
    EALLOW;
    LED11_TOGGLE;
    EDIS;

}




