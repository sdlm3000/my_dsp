/*
 * bluetooth.c
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 */

#include "bluetooth.h"
#include "prothesis.h"
#include "pwm.h"

/* 串口接收解析成功的数据包 */
bt_t rx_Packet;
int state = 0;
int pwm1 = 0, pwm2 = 0;
float pwm_f = 1.0;

extern double press1, press2, press3;
extern double arg;
extern int rt_flag1;
int motor_flag = 0;

/**
  * @brief  接收串口数据解包流程
  */
static enum
{
    waitForStartByte1,
    waitForMsgID,
    waitForDataLength,
    waitForData,
    waitForChksum1,
} rxState = waitForStartByte1;

/**
  * @brief  bluetooth模块串口数据解析（串口接收的每一个数据需传入处理）
  * @note   此函数需要实时调用
  * @param  ch: 串口接收的单个数据
  * @retval Uint8: 0 无包 1 有效包
  */
Uint8 bt_unpack(Uint8 ch)
{
    static Uint8 cksum = 0, dataIndex = 0;
//    uart_printf("%d %d\n", ch, rxState);
    switch (rxState)
    {
        case waitForStartByte1:
            if (ch == UP_BYTE1)
            {
                rxState = waitForMsgID;
                rx_Packet.startByte1 = ch;
            }

            cksum = ch;
            break;

        case waitForMsgID:
            rx_Packet.msgID = ch;
            rxState = waitForDataLength;
            cksum += ch;
            break;

        case waitForDataLength:
            if (ch <= BT_MAX_DATA_SIZE)
            {
                rx_Packet.dataLen = ch;
                dataIndex = 0;
                rxState = (ch > 0) ? waitForData : waitForChksum1;  /*ch=0,数据长度为0，校验1*/
                cksum += ch;
            }
            else
            {
                rxState = waitForStartByte1;
            }
            break;

        case waitForData:
            rx_Packet.data[dataIndex] = ch;
            dataIndex++;
            cksum += ch;

            if (dataIndex == rx_Packet.dataLen)
            {
                rxState = waitForChksum1;
            }

            break;

        case waitForChksum1:
            cksum = cksum & 0x00ff;
            // 取消检验和，默认最后的值为0xff
            cksum = 0x00ff;
            if (cksum == ch)    /*!< 校准正确返回1 */
            {
                rx_Packet.checkSum = cksum;
                rxState = waitForStartByte1;
                return 1;
            }
            else    /*!< 校验错误 */
            {
                rxState = waitForStartByte1;
            }

            break;

        default:
            rxState = waitForStartByte1;
            break;
    }

    return 0;
}

/**
  * @brief  bluetooth数据包解析
  * @param  packet: bt数据包
  * @retval None
  */
void btParsing(bt_t *packet)
{
    /* 假肢状态的设置 */
    if(packet->msgID == STATE_SET1)
    {
         state = packet->data[0];
         uart_printf("state_set: %d\r\n", state);
         if(state == 0)
         {
             motor_stop();
             UNLOAD();
         }
         else if(state == 1)
         {
             UNLOAD();
         }
         else if(state == 3)
         {
             MIDDLE();
         }
         else if(state == 2)
         {
             PUSH();
         }
         else if(state == 4)
         {
             PULL();
         }
         else if(state == 5)
         {
             if (motor_flag == 0)
             {
                 motor_flag = 1;
                 motor_run();
             }
             else if(motor_flag == 1)
             {
                 motor_flag = 0;
                 motor_stop();
             }

         }
         else if(state == 6)
         {
             motor_stop();
         }
    }
    else if (packet->msgID == STATE_SET2)
    {
        state = packet->data[0];
//        uart_printf("state_set: %d\r\n", state);
        if(state == 0)
        {
            state = 3;
        }
        if(state == 1)      // 卸荷
        {
            motor_stop();
            UNLOAD();
            delay_ms(500);
            MIDDLE();
        }
        else if(state == 2) // 被动态实验状态
        {
            valve_pwm_middle_zhi = 0;
            valve_pwm_middle_bei = 0;
            MIDDLE_bei();
        }
        else if(state == 4) // 假肢往复运动
        {
            if(rt_flag1 == 0)
            {
                UNLOAD();
                rt_flag1 = 1;
                motor_run();
            }
            else if(rt_flag1 != 0)
            {
                rt_flag1 = 0;
                motor_stop();
                MIDDLE();
            }
        }
        else if(state == 3) // 假肢状态回复
        {
//            if(press1 < P_max && press2 < P_max && press3 < P_max &&
//               press1 > P_min && press2 > P_min && press3 > P_min &&
//               arg < 1)
//            {
//                PUSH();
//                motor_run();
//            }
            UNLOAD();
            delay_ms(100);
            PUSH();
            motor_run();
        }

    }
    /* 假肢阀的pwm设置 */
    else if (packet->msgID == PWM_SET1)
    {
        pwm1 = packet->data[0] / 16 * 10 + packet->data[0] % 16;
        pwm2 = packet->data[1] / 16 * 10 + packet->data[1] % 16;
        valve_pwm_middle_zhi = pwm1 * 0.01 * MAX_SPEED;
        valve_pwm_middle_bei = pwm2 * 0.01 * MAX_SPEED;
        if(state == 2)
        {
            MIDDLE_bei();
        }
    }
    else if (packet->msgID == PWM_SET2)
    {
        pwm_f = 50.0 / (packet->data[0] / 16 * 10 + packet->data[0] % 16);
        pwm2 = packet->data[1] / 16 * 10 + packet->data[1] % 16;
        valve_pwm_middle_zhi = 0;
        valve_pwm_middle_bei = pwm2 * 0.01 * EPWM4_TIMER_TBPRD * pwm_f;
        if(state == 2)
        {
            EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD * pwm_f;
            MIDDLE_bei();
        }

    }
}

