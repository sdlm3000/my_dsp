/*
 * main.c
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 * 用于假肢的实验台的阻尼线性化实验
 */

#include "common.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "leds.h"
#include "bluetooth.h"


//int counter_P = 0;          // 用于记录压力故障事件的次数，防止误触发
extern int state;
extern double press1, press2, press3;
extern int pwm1, pwm2;
int rt_flag = 0;    // 1代表收腿，2代表蹬腿
float pwm_counter = 1.0;

int main(void)
{
    Uint8 ch;
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

    UARTb_Init(115200);
    PWM_Init();
    LED_Init();
    ADC_Init();
    TIM0_Init(150, 5000);

    EPwm1Regs.CMPA.half.CMPA = MIN_SPEED;

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

//    CpuTimer0Regs.TCR.all = 0x4001;     // 设置TIE = 1，开启定时器0中断

    UNLOAD();
    delay_ms(1000);
    MIDDLE();
//    delay_ms(4000);         //等待时间，为蓝牙连接预留充足的时间
    for(;;)
    {
        if (bt_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (bt_unpack(ch))          /*!< 解析出有效数据包 */
            {
                btParsing(&rx_Packet);
            }
        }
        if(state == 3)
        {
            if(press1 > P_max || press2 > P_max || press3 > P_max)
            {
                motor_stop();
                UNLOAD();
                delay_ms(200);
                pwm1 = 0;
                pwm2 = 0;
                MIDDLE();
                state = 2;
            }
        }
        else if(state == 4)
        {
            if(rt_flag == 1)
            {
                if(arg > ANGLE_PULL)
                {
                    PULL();
                }
                else
                {
                    rt_flag = 2;
                }
            }
            else if(rt_flag == 2)
            {
                if(arg < ANGLE_PUSH)
                {
                    PUSH();
                }
                else
                {
                    rt_flag = 1;
                }
            }
        }
//        else if(state == 0) // 变频测试
//        {
//
//            EPwm2Regs.CMPA.half.CMPA = 0.2 * MAX_SPEED;
//            EPwm3Regs.CMPA.half.CMPA = 0.3 * MAX_SPEED;
//            EPwm5Regs.CMPA.half.CMPA = 0.8 * MAX_SPEED;
//            EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD * pwm_counter;           // Set timer period 804 TBCLKs
//            EPwm4Regs.CMPA.half.CMPA = 0.6 * EPWM4_TIMER_TBPRD * pwm_counter;
//            pwm_counter += 0.05;
//        }

    }
}
