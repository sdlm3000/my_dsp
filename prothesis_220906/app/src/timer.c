/*
 * timer.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "timer.h"
#include "uart.h"
#include "math.h"
#include "prothesis.h"
#include "jy61p.h"
#include "stdio.h"
#include "ringbuffer.h"

// 陀螺仪数据获取
float a[3], w[3], Angle[3];

double tmp1, tmp2, tmp3;
int num1 = 0;
int num2 = 0;

// 电压数据
static double P_foot1 = 0.0, P_foot2 = 0.0, P_foot3 = 0.0;

// ADC采样相关取平均值数组
static Uint32 sum1 = 0, sum2 = 0,sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
static Uint16 SampleTable1[BUF_SIZE];
static Uint16 SampleTable2[BUF_SIZE];
static Uint16 SampleTable3[BUF_SIZE];
static Uint16 SampleTable4[BUF_SIZE];
static Uint16 SampleTable5[BUF_SIZE];
static Uint16 SampleTable6[BUF_SIZE];
static Uint16 SampleTable7[BUF_SIZE];

// 踝关节角度平均值
static double arg_ave[5] = {0.0};

// 
double press1 = 0, press2 = 0, press3 = 0;
double arg = 0.0;

int counter_P = 0;



extern int pwm1, pwm2;


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

interrupt void TIM0_IRQn(void)
{
    int i;
    double temp;
    char buf[100] = {};

    CpuTimer0.InterruptCount++;
    AdcRegs.ADCTRL2.all = 0x2000;   // 软件向SEQ1位写1开启转换触发
    // ADC电压转换，10次采样取平均值
    for (i = 0; i < AVG; i++) // AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {}  // Wait for interrupt如何发起中断
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

    // 转换获得液压压力
    tmp1 = ((double)sum1) * 3 / 4096 / AVG;
    tmp2 = ((double)sum2) * 3 / 4096 / AVG;
    tmp3 = ((double)sum3) * 3 / 4096 / AVG;
    press1 = (((double)sum1) * 3 / 4096 / AVG) * K_GAIN1 + B_GAIN;
    press2 = (((double)sum2) * 3 / 4096 / AVG) * K_GAIN2 + B_GAIN;
    press3 = (((double)sum3) * 3 / 4096 / AVG) * K_GAIN3 + B_GAIN;

    // 内部液压压力限位
    if(press1 >= P_max || press1 <= 0)
    {
        counter_P++;
        if(counter_P >= 15)
        {
            motor_stop();
//            UNLOAD();
            counter_P = 0;
        }
    }

    // 转换获得踝关节角度
    arg = ((((double)sum4) * 3 / 4096 / AVG) * 100);//采样的实际电压 0-3v
    arg = 196.4 - arg;     // 70为零位的角度传感器值
    temp = 0;
    for(i = 0; i < 4; i++)
    {
        arg_ave[i] = arg_ave[i+1];
        temp = temp + arg_ave[i];
    }
    arg_ave[4] = arg;
    arg = (temp + arg_ave[4]) / 5; // 5点滑动平均

    // 转换获得足底压力
    P_foot1 = ((double)sum5) * 3 / 4096 / AVG; //采样的实际电压 0-3v
    P_foot2 = ((double)sum6) * 3 / 4096 / AVG; //采样的实际电压 0-3v
    P_foot3 = ((double)sum7) * 3 / 4096 / AVG; //采样的实际电压 0-3v

    // 发送数据至上位机
    if(CpuTimer0.InterruptCount % 2 == 0)
    {
        Uint8 strLen = sprintf(buf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f\r\n", arg, press1, press2, press3, P_foot1, P_foot2, P_foot3);
//        uart_printf("%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f\r\n", aa, arg, press1, press2, press3, P_foot1, P_foot2, P_foot3);

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


Uint8 f1,f2,f3;
interrupt void TIM1_IRQn(void)
{
    Uint8 flag = 0x4;
    if(jy61p_GetData(flag, 0x50, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //得到陀螺仪数据
    }
    DELAY_US(100);
    if(jy61p_GetData(flag, 0x51, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //得到陀螺仪数据
    }
    DELAY_US(100);
    if(jy61p_GetData(flag, 0x52, a, w, Angle) == 0)
    {
        jy61p_SendData(flag, a, w, Angle);    //得到陀螺仪数据
    }
//    jy61p_SendData(flag, a, w, Angle);    //得到陀螺仪数据
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




