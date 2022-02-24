/*
 * bluetooth.c
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 */

#include "bluetooth.h"


/* 串口接收解析成功的数据包 */
bt_t rx_Packet;

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
    /* 姿态角 */
    if (packet->msgID == STATE_SET)
    {
        uart_printf("state_set\r\n");
    }
    else if (packet->msgID == PWM_SET)
    {
        uart_printf("pwm_state\r\n");
    }
}

