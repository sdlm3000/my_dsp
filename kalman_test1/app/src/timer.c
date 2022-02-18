/*
 * timer.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "timer.h"
#include "leds.h"
#include "uart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
short gyrox, gyroy, gyroz;    //陀螺仪原始数据
short aacx, aacy, aacz;

//卡尔曼滤波参数与函数

float dt=0.001;//注意：dt的取值为kalman滤波器采样时间

float angle = 0.0, angle_dot = 0.0;//角度和角速度

float P[2][2] = {{1, 0},
                  {0, 1}};

float Pdot[4] ={ 0,0,0,0};

float Q_angle=0.001, Q_gyro=0.005; //角度数据置信度,角速度数据置信度

float R_angle=0.05;

float q_bias = 0.0, angle_err, E, K_0, K_1;

float angle_m = 0.0, gyro_m = 0.0;
//定时器0初始化函数
//Freq：CPU时钟频率（150MHz）
//Period：定时周期值，单位us
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //设置定时器0的中断入口地址为中断向量表的INT0
    EALLOW;
    PieVectTable.TINT0 = &TIM0_IRQn;
    EDIS;

    //指向定时器0的寄存器地址
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    //设置定时器0的周期寄存器值
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    //设置定时器预定标计数器值为0
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    //确保定时器0为停止状态
    CpuTimer0Regs.TCR.bit.TSS = 1;
    //重载使能
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    //开始定时器功能
    CpuTimer0Regs.TCR.bit.TSS=0;
    //开启CPU第一组中断并使能第一组中断的第7个小中断，即定时器0
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    //使能总中断
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

float Kalman_Filter(float angle_m, float gyro_m)//angleAx 和 gyroGy
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
    angle += K_0 * angle_err; //最优角度
//    if(angle > 180.0) angle = 180.0;
//    else if(angle < -180.0) angle = -180.0;
    q_bias += K_1 * angle_err;
    angle_dot = gyro_m-q_bias;//最优角速度
    return angle;
}

interrupt void TIM0_IRQn(void)
{
    EALLOW;
    LED13_TOGGLE;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据
    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据
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




