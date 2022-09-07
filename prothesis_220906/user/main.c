/*
 * main.c
 *
 *  Created on: 2022年09月06日
 *      Author: sdlm
 * 用于假肢的硬件的整体测试，并且会作为之后穿戴实验的基础版本
 */
#include "common.h"
#include "uart.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "bluetooth.h"
#include "i2c.h"

int rt_flag1 = 0;    // 1代表收腿，2代表蹬腿

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
    I2CA_Init();
    PWM_Init();
    ADC_Init();


    TIM0_Init(150, 5000);   // 5ms

//    // 用于采集IMU数据
//    TIM1_Init(150, 5000);   // 5ms

    // 起始卸荷
    UNLOAD();
    delay_ms(1000);
    MIDDLE();

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

    while(1)
    {
        if (bt_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (bt_unpack(ch))          /*!< 解析出有效数据包 */
            {
                btParsing(&rx_Packet);
            }
        }

    }
}
