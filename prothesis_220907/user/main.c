/*
 * main.c
 *
 *  Created on: 2022年09月07日
 *      Author: sdlm
 * 2022.09.10  用于假肢的步行实验，主要使用人体质心预测算法和自适应鲁棒控制
 * 2022.10.12  这个版本主要用于假肢步行实验的测试，不跑新算法了
 */
#include "common.h"
#include "uart.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "bluetooth.h"
#include "i2c.h"

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
        // 用于接收蓝牙数据，并解析数据
        if (bt_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (bt_unpack(ch))          /*!< 解析出有效数据包 */
            {
                btParsing(&rx_Packet);
            }
        }
        // 假肢主要控制逻辑
        if(CONTROL_FLAG == NO_CONTROL_OUT)
        {
            motor_stop();
        }
        else if(CONTROL_FLAG == CONTROL_OUT)
        {
            // TODO: 控制率
            if(proState == MIDDLE_ZHI_STATE)
            {
                if(angle <= ANGLE_ZHI_BEI)
                {
                    proState = MIDDLE_BEI_STATE;
                }
            }
            else if(proState == MIDDLE_BEI_STATE)
            {
                if(angle >= ANGLE_MIDDLE_PUSH)
                {
                    proState = PUSH_STATE;
                }
            }
            else if(proState == PUSH_STATE)
            {
                if(angle <= ANGLE_PUAH_PULL)
                {
                    proState = PULL_STATE;
                }
            }
            else if(proState == PULL_STATE)
            {
                if(angle >= ANGLE_PULL_MIDDLE)
                {
                    proState = MIDDLE_ZHI_STATE;
                }
            }

            switch(proState)
            {
                case MIDDLE_ZHI_STATE:
                    MIDDLE_zhi();
                    break;
                case MIDDLE_BEI_STATE:
                    MIDDLE_bei();
                    break;
                case PUSH_STATE:
                    PUSH_control();
                    break;
                case PULL_STATE:
                    PULL();
                    valve_pwm_middle_zhi = MIN_SPEED;
                    valve_pwm_middle_bei = MIN_SPEED;
                    valve_pwm_push = MAX_SPEED;
                    middle_zhi_controlFlag = 0;
                    break;
                default:
                    break;
            }

            if(press1 < (P_HIGH - 0.5) && press1 > 0)
            {
                motor_run();
            }
            else if(press1>(P_HIGH + 0.5))
            {
                motor_stop();
            }

        }
        else    // 默认都是 CONTROL_RESET
        {
            // 回到初始状态
            motor_stop();
            UNLOAD();
            delay_ms(1000);
            MIDDLE();
            proState = MIDDLE_ZHI_STATE;
        }


    }
}
