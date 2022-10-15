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
#include "anglePred.h"
#include "stdio.h"



extern float pelvic_acc_z[N], t[N];
extern float rshank_euler_y[N], rthigh_euler_y[N];

int point_num = 0, predict_flag = 0;

static float a_pel[3], ang_pel[3];
static float ang_rsh[3];
static float ang_rth[3];
static float a_rfo[3], ang_rfo[3];
static float ang_lsh[3];
static float ang_lth[3];
static float a_lfo[3], ang_lfo[3];
static int walk_state = 0, begin_flag = 0, index = 0;
static float ang_rsh_off = 0.0, ang_rth_off = 0.0;


//static int walk_cnt = 0;
//Uint8 buf[32] = {};
//Uint16 buf_len = 0;



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

// IMU 数据接收中断
interrupt void TIM0_IRQn(void)
{
    unsigned char chrTemp[30];
    float angle_tmp;
    // 轮训接收各个IMU的数据
    // 7个IMU，但是为了整体周期性，按10个一次轮训
    scia_msg("1 ");
    if(I2C_ReadData(PELVIC, &chrTemp[0], AX, 24) == 0) {
        a_pel[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_pel[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_pel[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_pel[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_pel[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_pel[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scia_float(a_pel[0]);
        scia_xmit(' ');
        scia_float(a_pel[1]);
        scia_xmit(' ');
        scia_float(a_pel[2]);
        scia_xmit(' ');
        scia_float(ang_pel[0]);
        scia_xmit(' ');
        scia_float(ang_pel[1]);
        scia_xmit(' ');
        scia_float(ang_pel[2]);
        scia_xmit('\n');
    }
    // DELAY_US(100);

    scia_msg("2 ");
    if(I2C_ReadData(RTHIGH, &chrTemp[0], AX, 24) == 0) {
        ang_rth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scia_float(ang_rth[1]);
        scia_xmit('\n');
    }
    // DELAY_US(100);
    scia_msg("3 ");
    if(I2C_ReadData(RSHANK, &chrTemp[0], AX, 24) == 0) {
        ang_rsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scia_float(ang_rsh[1]);
        scia_xmit('\n');
    }
    // DELAY_US(100);
    scia_msg("4 ");
    if(I2C_ReadData(RFOOT, &chrTemp[0], AX, 24) == 0) {
        a_rfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_rfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_rfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_rfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_rfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_rfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scia_float(a_rfo[0]);
        scia_xmit(' ');
        scia_float(a_rfo[1]);
        scia_xmit(' ');
        scia_float(a_rfo[2]);
        scia_xmit(' ');
        scia_float(ang_rfo[0]);
        scia_xmit(' ');
        scia_float(ang_rfo[1]);
        scia_xmit(' ');
        scia_float(ang_rfo[2]);
        scia_xmit('\n');
    }
    // DELAY_US(100);
    scia_msg("5 ");
    if(I2C_ReadData(LTHIGH, &chrTemp[0], AX, 24) == 0) {
        ang_lth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scia_float(ang_lth[1]);
        scia_xmit('\n');
    }
    // DELAY_US(100);
    scia_msg("6 ");
    if(I2C_ReadData(LSHANK, &chrTemp[0], AX, 24) == 0) {
        ang_lsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scia_float(ang_lsh[1]);
        scia_xmit('\n');
    }
    // DELAY_US(100);
    scia_msg("7 ");
    if(I2C_ReadData(LFOOT, &chrTemp[0], AX, 24) == 0) {
        a_lfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_lfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_lfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_lfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_lfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_lfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scia_float(a_lfo[0]);
        scia_xmit(' ');
        scia_float(a_lfo[1]);
        scia_xmit(' ');
        scia_float(a_lfo[2]);
        scia_xmit(' ');
        scia_float(ang_lfo[0]);
        scia_xmit(' ');
        scia_float(ang_lfo[1]);
        scia_xmit(' ');
        scia_float(ang_lfo[2]);
        scia_xmit('\n');
    }


    // 寻找步态周期的起始和结束，同时存放预测算法需要的数据
    // 默认每次都能正常获取到IMU数据
    if(predict_flag == 0)
    {
        if(walk_state == 0) {
            // 起始静置200个周期后，得到一些角度的偏移量
            begin_flag++;
            if(begin_flag > 50) {
                ang_rth_off = ang_rth[1];
                ang_rsh_off = ang_rsh[1];
                walk_state = 1;
            }
        }
        else if(walk_state == 1 || walk_state == 4) {  
            // 利用右小腿，来判断步态的起始和终止
            angle_tmp = ang_rsh[1] - ang_rsh_off;
            if((ang_rsh[1] > (ang_rsh_off + 1)) || walk_state == 4) {
                walk_state = 2;
                rshank_euler_y[index] = angle_tmp * DEG2RAD;
                rthigh_euler_y[index] = (ang_rth[1] - ang_rth_off) * DEG2RAD;
                pelvic_acc_z[index] = transCoods(a_pel, ang_pel);
                if(index < N - 1) index++;
            }
        }
        else if(walk_state == 2 || walk_state == 3) {
            rshank_euler_y[index] = (ang_rsh[1] - ang_rsh_off) * DEG2RAD;
            rthigh_euler_y[index] = (ang_rth[1] - ang_rth_off) * DEG2RAD;
            pelvic_acc_z[index] = transCoods(a_pel, ang_pel);
            if(walk_state == 2 && (ang_rsh[1] < (ang_rsh_off - 1))) walk_state = 3;
            else if((walk_state == 3 && ang_rsh[1] > (ang_rsh_off + 1))) {
                walk_state = 4;
                point_num = index + 1;
                index = 0;
                predict_flag = 1;
            }
            if(index != 0 && index < N - 1) index++;
        }
    }
    scia_msg("w ");
    scia_int(walk_state);
    scia_msg(" ");
    scia_int(index);
    scia_msg("\n");

    EALLOW;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}


int cnt1 = 0;
interrupt void TIM1_IRQn(void)
{
    cnt1++;
    if(cnt1 > 10)
    {
//        scia_msg("tim1 ");
//        scia_int(cnt1);
//        scia_msg("\n");
        cnt1 = 0;
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

int tim2_cnt = 0;
interrupt void TIM2_IRQn(void)
{
    EALLOW;
//    LED11_TOGGLE;
    tim2_cnt++;
    EDIS;

}




