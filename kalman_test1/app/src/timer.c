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
short gyrox, gyroy, gyroz;    //������ԭʼ����
short aacx, aacy, aacz;

//�������˲������뺯��

float dt=0.001;//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��

float angle = 0.0, angle_dot = 0.0;//�ǶȺͽ��ٶ�

float P[2][2] = {{1, 0},
                  {0, 1}};

float Pdot[4] ={ 0,0,0,0};

float Q_angle=0.001, Q_gyro=0.005; //�Ƕ��������Ŷ�,���ٶ��������Ŷ�

float R_angle=0.05;

float q_bias = 0.0, angle_err, E, K_0, K_1;

float angle_m = 0.0, gyro_m = 0.0;
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
    //ʹ�����ж�
    EINT;
    ERTM;

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

    EINT;
    ERTM;

}

float Kalman_Filter(float angle_m, float gyro_m)//angleAx �� gyroGy
{
    angle += (gyro_m - q_bias) * dt;
    angle_err = angle_m - angle;
    Pdot[0] = Q_angle - P[0][1] - P[1][0];
    Pdot[1] =- P[1][1];
    Pdot[2] =- P[1][1];
    Pdot[3] = Q_gyro;
    P[0][0] += Pdot[0] * dt;
    P[0][1] += Pdot[1] * dt;
    P[1][0] += Pdot[2] * dt;
    P[1][1] += Pdot[3] * dt;
    E = R_angle + P[0][0];
    K_0 = P[0][0] / E;
    K_1 = P[1][0] / E;
//    t_0 = PCt_0;
//    t_1 = P[0][1];
    P[0][0] -= K_0 * P[0][0];
    P[0][1] -= K_0 * P[1][0];
    P[1][0] -= K_1 * P[0][0];
    P[1][1] -= K_1 * P[1][0];
    angle += K_0 * angle_err; //���ŽǶ�
//    if(angle > 180.0) angle = 180.0;
//    else if(angle < -180.0) angle = -180.0;
    q_bias += K_1 * angle_err;
    angle_dot = gyro_m-q_bias;//���Ž��ٶ�
    return angle;
}

interrupt void TIM0_IRQn(void)
{
    EALLOW;
    LED13_TOGGLE;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //�õ����ٶȴ���������
    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //�õ�����������
//    double tmp = double((aacy - 700)) /
    float angle_tmp = (float)atan2f((float)(aacy - 300.0) , (float)(aacz + 4000.0)); // 180 / 3.14

    angle_m = angle_tmp * 57.3;

    gyro_m = (float)(gyrox + 22) * 0.061037;  // 2000 / 32767

    angle = Kalman_Filter(angle_m, gyro_m);

    uart_printf("angle_m:%.2f, angle:%.2f, ax:%d, ay:%d, az:%d, gyrox:%d, gy:%.3f\r\n", angle_m, angle, aacx, aacy, aacz, gyrox, gyro_m);
    mpu6050_send_kalman((short)(angle_m * 100), (short)(angle * 100));
    PieCtrlRegs.PIEACK.bit.ACK1=1;
    EDIS;
}



interrupt void TIM1_IRQn(void)
{
    EALLOW;
    LED12_TOGGLE;
    EDIS;
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

    EINT;
    ERTM;

}

interrupt void TIM2_IRQn(void)
{
    EALLOW;
    LED11_TOGGLE;
    EDIS;

}




