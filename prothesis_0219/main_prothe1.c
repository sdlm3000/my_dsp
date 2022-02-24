/*
 * main.c
 *
 *  Created on: 2022年2月19日
 *      Author: sdlm
 * 用于假肢的实验台的行走实验
 * 读取行走过程中的内部液压压力值、踝关节角度值
 * 同时需要根据将角度传感器与实际角度进行映射
 * 并通过模型计算出每个角度值对于的缸中的压力差
 * 进行PID控制
 */

#include "common.h"
#include "prothesis.h"
#include "pwm.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"




double volt1=0, volt2=0, volt3=0, volt4=0, volt5=0, volt6=0, volt7=0;
Uint32 sum1=0, sum2=0,sum3=0, sum4=0, sum5=0, sum6=0, sum7=0;
Uint16 SampleTable1[BUF_SIZE];
Uint16 SampleTable2[BUF_SIZE];
Uint16 SampleTable3[BUF_SIZE];
Uint16 SampleTable4[BUF_SIZE];
Uint16 SampleTable5[BUF_SIZE];
Uint16 SampleTable6[BUF_SIZE];
Uint16 SampleTable7[BUF_SIZE];

int i=0;
int flag_chong = 0;

double arg_ave[5] = {0.0};

double press1 = 0, press2 = 0, press3 = 0;
double arg = 0.0;

double Beta = 0.0, Beta_last = 0.0;   // 角度制

double W_BD = 0.0;

double Alpha, Alpha_cos, Alpha_sin;

double T = 0.0, F_A = 0.0;



// 主要用于被动态，对阀的PWM进行PID控制，实现假肢的阻抗对生物踝关节的阻抗模拟
double delta_P = 0.0;       // 理想压差，P2-P3
double delta_P0 = 0.0;      // 实际压差
double dd_P = 0.0;          // 实际压差与理想压差的差值，delta_P0 - delta_P



Uint16 press_plante1 = 0;
Uint16 press_plante2 = 0;
Uint16 press_plante3 = 0;

// 状态：0-卸荷态，1-被动背屈，2-主动跖屈，3-主动背屈，4-被动跖屈
// 其中1和4都对应值被动态，可视为一个
int state = 0;               //用于表示假肢的状态
int state0 = 0;               //用于表示假肢的上一个状态

int state_motor = 0;        //用于表示电机的运行状态，0 -> 运行,1 -> 停止

int switch_flag = 0;

int valve_pwm_middle_zhi = 0;   //被动状态下，用于控制阀的PWM值
int valve_pwm_middle_bei = 0;   //被动状态下，用于控制阀的PWM值
int valve_pwm_push = 0;     //蹬腿状态下，用于控制阀的PWM值

int flag_run = 0;
int flag_circu = 0;         // 纯周期运动的标志位
int flag_state1 = 0;        // state1开始调整PWM的标志位

int counter_time = 0;       // 主动跖曲和主动背曲之间的延时计数器
int counter = 0;            // 用于记录触发事件的次数，防止误触发
int counter_P = 0;          // 用于记录压力故障事件的次数，防止误触发

long my_debug = 0;

int counter_circu = 0;

double my_temp1,my_temp2;




int main(void)
{
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();

    //完成DSP的初始化
    // 步骤 1. 初始化系统控制:
    InitSysCtrl();

    // 步骤3. 清除所有中断,初始化中断向量表
    DINT;
    // 初始化PIE控制寄存器到他们的默认状态.
    InitPieCtrl();
    // 禁止CPU中断和清除所有CPU中断标志
    IER = 0x0000;
    IFR = 0x0000;
    //初始化PIE中断向量表，并使其指向中断服务子程序（ISR）
    InitPieVectTable();

    UARTa_Init(115200);
    PWM_Init();
    TIM0_Init(150, 5000);

    EPwm1Regs.CMPA.half.CMPA = minspeed;

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

//    CpuTimer0Regs.TCR.all = 0x4001;     // 设置TIE = 1，开启定时器0中断

    UNLOAD();
    DELAY_US(50000);
    MIDDLE();
    DELAY_US(1500000);          //等待时间，为蓝牙连接预留充足的时间
    state = 1;
    flag_run = 1;
    flag_circu = 0;
    valve_pwm_middle_zhi = minspeed;
    valve_pwm_middle_bei = minspeed;
    valve_pwm_push = maxspeed;

    while(1)
    {
        if(flag_run==1)
        {
            my_debug++;
            if(state==1)
            {
                if(arg<=-3)
                {
                    state = 2;
                }
            }
            if(state==2)
            {
                counter++;
                if(arg>=5)//if(volt7 - volt6 <= 0.07)// if(arg>=4.5)
                {
//                    if(counter>=10)
//                    {
                    state = 3;
                    counter = 0;
//                    }
                }
            }
            if(state==3)
            {
                if(arg<=-11.5)  // && press_plante2<=0.1)
                {
                    state = 4;
                }
            }
//            if(state==4)
//            {
//                if(counter_time>=30)
//                {
//                   state = 5;
//                   counter_time = 0;
//                }
//            }
            if(state==4)
            {
                if(arg>=0 || volt7>=0.8)
                {
                    state = 1;
                }
            }
            switch(state)
            {
                case 1:
                    MIDDLE_zhi();
                    motor_stop();
                    flag_chong = 0;
                    break;
                case 2:
                    MIDDLE_bei();
                    motor_stop();
                    flag_chong = 0;
                    break;
                case 3:
                    PUSH_control();
                    flag_chong = 1;
                    break;
//                case 4:
//                    motor_stop();
//                    break;
                case 4:
                    PULL();
                    motor_run();
                    flag_chong = 1;
                    valve_pwm_middle_zhi = minspeed;
                    valve_pwm_middle_bei = minspeed;
                    valve_pwm_push = maxspeed;
                    flag_state1 = 0;
                    break;
                default:
                    break;
            }
//            if(flag_chong==1)
//            {
            if(press1<(P_high-0.2))
            {
                motor_run();
            }
            else if(press1>(P_high+0.2))
            {
                motor_stop();
            }
//            }

        }

//        // 假肢纯循环作动
//        if(flag_circu == 1)
//        {
//            if(state == 1)
//            {
//                if(arg>=16)
//                {
//                    state = 2;
//                    counter_circu++;
//                }
//            }
//            if(state == 2)
//            {
//                if(arg<=-5)
//                {
//                    state = 1;
//                }
//            }
//            switch(state)
//            {
//                case 1:
//                    PULL();
//                    break;
//                case 2:
//                    PUSH();
//                    break;
//
//                default:
//                    break;
//            }
//            if(press1>12 || press2>12 || press3>12 || counter_circu>6)
//            {
//
//                motor_stop();
//                UNLOAD();
//                DELAY_US(1000);
//                MIDDLE();
//                flag_circu = 0;
//            }
//            else
//            {
//                motor_run();
//
//            }
//        }

    }
}
