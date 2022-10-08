/*
 * timer.c
 *
 *  Created on: 2022��1��25��
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

short gyroxa, gyroya, gyroza;    //������ԭʼ����
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

int counter = 0;
// IMU ���ݽ����ж�
interrupt void TIM0_IRQn(void)
{
    unsigned char chrTemp[30];
    counter++;
    // ��ѵ���ո���IMU������
    // 7��IMU������Ϊ�����������ԣ���10��һ����ѵ
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

    // Ѱ�Ҳ�̬���ڵ���ʼ�ͽ�����ͬʱ���Ԥ���㷨��Ҫ������
    // Ĭ��ÿ�ζ���������ȡ��IMU����
    if(counter == 0)
    {
        if(walk_state == 0) {
            // ��ʼ����10�����ں󣬵õ�һЩ�Ƕȵ�ƫ����
            begin_flag++;
            if(begin_flag > 10) {
                ang_rth_off = ang_rth[1];
                ang_rsh_off = ang_rsh[1];
                walk_state = 1;
            }
        }
        else if(walk_state == 1 || walk_state == 4) {  
            // ������С�ȣ����жϲ�̬����ʼ����ֹ
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




