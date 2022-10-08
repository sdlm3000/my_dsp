/*
 * timer.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "timer.h"
#include "uart.h"
#include "mpu6050.h"
#include "math.h"
#include "imu901.h"

#include "jy61p.h"
#include "i2c.h"

#define N       100

short gyroxa, gyroya, gyroza;    //陀螺仪原始数据
float pitcha, rolla, yawa;
int num1 = 0;


float a_pel[3], w_pel[3], ang_pel[3];
float a_rsh[3], w_rsh[3], ang_rsh[3];
float a_rth[3], w_rth[3], ang_rth[3];
float a_rfo[3], w_rfo[3], ang_rfo[3];
float a_lsh[3], w_lsh[3], ang_lsh[3];
float a_lth[3], w_lth[3], ang_lth[3];
float a_lfo[3], w_lfo[3], ang_lfo[3];

float ang_rsh_off = 0.0, ang_rth_off = 0.0;
float rshank_euler_y[N] = {0.0}, rthigh_euler_y[N] = {0.0};
float rshank_angvel_y[N] = {0.0}, rthigh_angvel_y[N] = {0.0};
float pelvic_acc_x[N] = {0.0}, pelvic_acc_y[N] = {0.0}, pelvic_acc_z[N] = {0.0};
float pelvic_euler_x[N] = {0.0}, pelvic_euler_y[N] = {0.0}, pelvic_euler_z[N] = {0.0};

static int walk_state = 0, begin_flag = 0, index = 0;
int point_num = 0, predict_flag = 0;


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
//    //使能总中断
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

int counter = 0;
// IMU 数据接收中断
interrupt void TIM0_IRQn(void)
{
    unsigned char chrTemp[30];
    counter++;
    // 轮训接收各个IMU的数据
    // 7个IMU，但是为了整体周期性，按10个一次轮训
    if(counter == 1)
    {
        scia_msg("1 ");
        if(I2C_ReadData(PELVIC, &chrTemp[0], AX, 24) == 0) {
            a_pel[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
            a_pel[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
            a_pel[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
            ang_pel[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
            ang_pel[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
            ang_pel[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        }
    }
    else if(counter == 2)
    {
        scia_msg("2 ");
        if(I2C_ReadData(RTHIGH, &chrTemp[0], AX, 24) == 0) {
            w_rth[1] = (float)CharToShort(&chrTemp[8]) / W_TRANS;
            ang_rth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        }
    }
    else if(counter == 3)
    {
        scia_msg("3 ");
        if(I2C_ReadData(RSHANK, &chrTemp[0], AX, 24) == 0) {
            w_rsh[1] = (float)CharToShort(&chrTemp[8]) / W_TRANS;
            ang_rsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        }
    }
    else if(counter == 4)
    {
        scia_msg("4 ");
        if(I2C_ReadData(RFOOT, &chrTemp[0], AX, 24) == 0) {
            a_rfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
            a_rfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
            a_rfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
            ang_rfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
            ang_rfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
            ang_rfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        }
    }
    else if(counter == 5)
    {
        scia_msg("5 ");
        if(I2C_ReadData(LTHIGH, &chrTemp[0], AX, 24) == 0) {
            w_lth[1] = (float)CharToShort(&chrTemp[8]) / W_TRANS;
            ang_lth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        }
    }
    else if(counter == 6)
    {
        scia_msg("6 ");
        if(I2C_ReadData(LSHANK, &chrTemp[0], AX, 24) == 0) {
            w_lsh[1] = (float)CharToShort(&chrTemp[8]) / W_TRANS;
            ang_lsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        }
    }
    else if(counter == 7)
    {
        scia_msg("7 ");
        if(I2C_ReadData(LFOOT, &chrTemp[0], AX, 24) == 0) {
            a_lfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
            a_lfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
            a_lfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
            ang_lfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
            ang_lfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
            ang_lfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        }
    }
    else if(counter == 10) counter = 0;

    // 寻找步态周期的起始和结束，同时存放预测算法需要的数据
    // 默认每次都能正常获取到IMU数据
    if(counter == 0)
    {
        if(walk_state == 0) {
            // 起始静置10个周期后，得到一些角度的偏移量
            begin_flag++;
            if(begin_flag > 10) {
                ang_rth_off = ang_rth[1];
                ang_rsh_off = ang_rsh[1];
                walk_state = 1;
            }
        }
        else if(walk_state == 1 || walk_state == 4) {  
            // 利用右小腿，来判断步态的起始和终止
            rshank_euler_y[index] = ang_rsh[1] - ang_rsh_off;
            if(rshank_euler_y[index] > 1 || walk_state == 4) {
                walk_state = 2;
                rthigh_euler_y[index] = ang_rth[1] - ang_rth_off;
                rshank_angvel_y[index] = w_rsh[1];
                rthigh_angvel_y[index] = w_rth[1];
                pelvic_acc_x[index] = a_pel[0];
                pelvic_acc_y[index] = a_pel[1];
                pelvic_acc_z[index] = a_pel[2];
                pelvic_euler_x[index] = ang_pel[0];
                pelvic_euler_y[index] = ang_pel[1];
                pelvic_euler_z[index] = ang_pel[2];
                index++;
            }
        }
        else if(walk_state == 2 && walk_state == 3) {
            rshank_euler_y[index] = ang_rsh[1] - ang_rsh_off;
            rthigh_euler_y[index] = ang_rth[1] - ang_rth_off;
            rshank_angvel_y[index] = w_rsh[1];
            rthigh_angvel_y[index] = w_rth[1];
            pelvic_acc_x[index] = a_pel[0];
            pelvic_acc_y[index] = a_pel[1];
            pelvic_acc_z[index] = a_pel[2];
            pelvic_euler_x[index] = ang_pel[0];
            pelvic_euler_y[index] = ang_pel[1];
            pelvic_euler_z[index] = ang_pel[2];
            index++;
            if(walk_state == 2 && rshank_euler_y[index - 1] < -1) walk_state = 3;
            else if(walk_state == 3 && rshank_euler_y[index - 1] > 1) {
                walk_state = 4;
                point_num = index;
                index = 0;
                predict_flag = 1;
            }
            
        }
    }


    EALLOW;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}



interrupt void TIM1_IRQn(void)
{


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
//    LED11_TOGGLE;
    EDIS;

}




