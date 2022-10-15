/*
 * bluetooth.c
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 */

#include "bluetooth.h"
#include "prothesis.h"
#include "pwm.h"
#include "stdio.h"


/* 串口接收解析成功的数据包 */
bt_t rx_Packet;

static int state = 0;
static int motor_flag = 0;
static Uint8 buf[100] = {};
Uint16 dataLen = 0;

extern int DATA_SEND;


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
        // 设置控制率是否输出
        // 0 -> 不输出控制率，电机停止
        // 1 -> 正常输出控制率
        // 2 -> 不输出控制率，电机停止，卸荷，然后进入被动态
        CONTROL_FLAG = state;
        dataLen = sprintf(buf, "\r\nCONTROL_VALUE: %d\r\n", CONTROL_FLAG);
        usartb_sendData(buf, dataLen);

    }
    else if (packet->msgID == ANGLE_SET)
    {
        // 假肢的触发角度阈值设置
        // data[0] 为 ANGLE_PUSH 的角度
        // data[1] 为 ANGLE_PULL 的角度，默认取负值
        ANGLE_PULL_MIDDLE = packet->data[0] / 16 * 10 + packet->data[0] % 16;
        ANGLE_ZHI_BEI = -((float)(packet->data[1] / 16 * 10 + packet->data[1] % 16));
        ANGLE_MIDDLE_PUSH = packet->data[2] / 16 * 10 + packet->data[2] % 16;
        ANGLE_PUAH_PULL = -((float)(packet->data[3] / 16 * 10 + packet->data[3] % 16));
        dataLen = sprintf(buf, "\r\nANGLE_VALUE: %.1f  %.1f  %.1f  %.1f\r\n", ANGLE_PULL_MIDDLE, ANGLE_ZHI_BEI, ANGLE_MIDDLE_PUSH, ANGLE_PUAH_PULL);
        usartb_sendData(buf, dataLen);
    }
    else if (packet->msgID == PRESSURE_SET)
    {
        // 假肢的相关压力阈值设置
        // data[0] 为 P_MAX 的个位数和小数位，默认再加10，取值范围为 10.0-19.9
        // data[1] 为 P_HIGH 的个位数和小数位，默认再加10，取值范围为 10.0-19.9
        P_MAX = (float)(packet->data[0] / 16) + packet->data[0] % 16 * 0.1 + 10.0;
        P_HIGH = (float)(packet->data[1] / 16) + packet->data[1] % 16 * 0.1 + 10.0;
        dataLen = sprintf(buf, "\r\nPRESSURE_VALUE: %.1f  %.1f\r\n", P_MAX, P_HIGH);
        usartb_sendData(buf, dataLen);
    }
    else if (packet->msgID == VALVE_PWM_SET)
    {
        // 假肢的valve的pwm最大值
        state = packet->data[0];
        int tmp = (packet->data[1] / 16) * 1000 + (packet->data[1] % 16) * 100 + (packet->data[2] / 16) * 10 + packet->data[2] % 16;
        if(state == 2)
        {
            MIN_VALVE_2 = tmp;
            dataLen = sprintf(buf, "\r\nVALVE_%d_VALUE: %d\r\n", state, MIN_VALVE_2);
        }
        else if(state == 3)
        {
            MAX_VALVE_3 = tmp;
            dataLen = sprintf(buf, "\r\nVALVE_%d_VALUE: %d\r\n", state, MAX_VALVE_3);
        }
        else if(state == 4)
        {
            MAX_VALVE_4 = tmp;
            dataLen = sprintf(buf, "\r\nVALVE_%d_VALUE: %d\r\n", state, MAX_VALVE_4);
        }
        usartb_sendData(buf, dataLen);
    }
    else if(packet->msgID == VALUE_GET)
    {
        DATA_SEND = ~DATA_SEND;
        dataLen = sprintf(buf, "\r\nALL_VALUE: %d  %.1f  %.1f  %.1f  %.1f  %.1f  %.1f  %d  %d  %d\r\n", CONTROL_FLAG, ANGLE_PULL_MIDDLE, 
                            ANGLE_ZHI_BEI, ANGLE_MIDDLE_PUSH, ANGLE_PUAH_PULL, P_MAX, P_HIGH, MIN_VALVE_2, MAX_VALVE_3, MAX_VALVE_4);
        usartb_sendData(buf, dataLen);
    }
    

}

