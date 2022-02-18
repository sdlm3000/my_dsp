/*
 * main_walk.c
 *
 *  Created on: 2020年10月15日
 *      Author: sdlm
 *
 * 用于假肢的实验台的行走实验
 * 读取行走过程中的内部液压压力值、踝关节角度值
 * 同时需要根据将角度传感器与实际角度进行映射
 * 并通过模型计算出每个角度值对于的缸中的压力差
 * 进行PID控制
 */

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "pro_header.h"

double volt1=0,volt2=0,volt3=0,volt4=0,volt5=0,volt6=0,volt7=0;
Uint32 sum1=0,sum2=0,sum3=0,sum4=0,sum5=0,sum6=0,sum7=0;
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

double press1=0,press2=0,press3=0;
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
    dsp_init();

    // 以下是自定义的代码, 使能相关中断:

    scia_fifo_init();               // Initialize the SCI FIFO
    scia_echoback_init();           // Initalize SCI for echoback
//    scib_fifo_init();
//    scib_echoback_init();
    EPwm1Regs.CMPA.half.CMPA = minspeed;

    InitAdc();  // 初始化ADC
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;     //采样开关的时间
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;    //时钟分频
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;           // 设置成连续运行，
    AdcRegs.ADCTRL1.bit.CPS = 0;                //预定标系数=1；
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;           //级联序列器方式
  //  AdcRegs.ADCTRL1.bit.SEQ_OVRD =1;
    AdcRegs.ADCMAXCONV.all=0x0006;              //设置5个转换//改了这里
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;      //设置ADCINA0作为第一个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;      //设置ADCINA1作为第二个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;      //设置ADCINA2作为第三个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;      //设置ADCINA3作为第四个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;      //设置ADCINA4作为第五个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;      //设置ADCINA5作为第六个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;      //设置ADCINA5作为第七个变换
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;       // Enable SEQ1 interrupt (every EOS)

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

    AdcRegs.ADCTRL2.all = 0x2000;       //软件向SEQ1位写1开启转换触发
    CpuTimer0Regs.TCR.all = 0x4001;     // 设置TIE = 1，开启定时器0中断


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


