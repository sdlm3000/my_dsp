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
    DELAY_US(50000);
    MIDDLE();
    DELAY_US(1500000);          //等待时间，为蓝牙连接预留充足的时间
    for(;;)
    {
        if (bt_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (bt_unpack(ch))          /*!< 解析出有效数据包 */
            {
                btParsing(&rx_Packet);
            }
        }
        if(state == 3 && arg < 0)
        {
            motor_stop();

            MIDDLE();
        }
    }
}
