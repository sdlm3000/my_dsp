/*
 * bluetooth.c
 *
 *  Created on: 2022��2��24��
 *      Author: sdlm
 */

#include "bluetooth.h"


/* ���ڽ��ս����ɹ������ݰ� */
bt_t rx_Packet;

/**
  * @brief  ���մ������ݽ������
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
  * @brief  bluetoothģ�鴮�����ݽ��������ڽ��յ�ÿһ�������贫�봦��
  * @note   �˺�����Ҫʵʱ����
  * @param  ch: ���ڽ��յĵ�������
  * @retval Uint8: 0 �ް� 1 ��Ч��
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
                rxState = (ch > 0) ? waitForData : waitForChksum1;  /*ch=0,���ݳ���Ϊ0��У��1*/
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
            if (cksum == ch)    /*!< У׼��ȷ����1 */
            {
                rx_Packet.checkSum = cksum;
                rxState = waitForStartByte1;
                return 1;
            }
            else    /*!< У����� */
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
  * @brief  bluetooth���ݰ�����
  * @param  packet: bt���ݰ�
  * @retval None
  */
void btParsing(bt_t *packet)
{
    /* ��̬�� */
    if (packet->msgID == STATE_SET)
    {
        uart_printf("state_set\r\n");
    }
    else if (packet->msgID == PWM_SET)
    {
        uart_printf("pwm_state\r\n");
    }
}

