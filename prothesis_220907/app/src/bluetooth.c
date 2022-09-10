/*
 * bluetooth.c
 *
 *  Created on: 2022��2��24��
 *      Author: sdlm
 */

#include "bluetooth.h"
#include "prothesis.h"
#include "pwm.h"
#include "stdio.h"


/* ���ڽ��ս����ɹ������ݰ� */
bt_t rx_Packet;

static int state = 0;
static int motor_flag = 0;
static Uint8 buf[100] = {};
Uint16 dataLen = 0;

extern int DATA_SEND;


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
            // ȡ������ͣ�Ĭ������ֵΪ0xff
            cksum = 0x00ff;
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
    /* ��֫״̬������ */
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
        // ���ÿ������Ƿ����
        // 0 -> ����������ʣ����ֹͣ
        // 1 -> �������������
        // 2 -> ����������ʣ����ֹͣ��ж�ɣ�Ȼ����뱻��̬
        CONTROL_FLAG = state;
        dataLen = sprintf(buf, "\r\nCONTROL_VALUE: %d\r\n", CONTROL_FLAG);
        usartb_sendData(buf, dataLen);

    }
    else if (packet->msgID == ANGLE_SET)
    {
        // ��֫�Ĵ����Ƕ���ֵ����
        // data[0] Ϊ ANGLE_PUSH �ĽǶ�
        // data[1] Ϊ ANGLE_PULL �ĽǶȣ�Ĭ��ȡ��ֵ
        ANGLE_PULL_MIDDLE = packet->data[0] / 16 * 10 + packet->data[0] % 16;
        ANGLE_ZHI_BEI = -((float)(packet->data[1] / 16 * 10 + packet->data[1] % 16));
        ANGLE_MIDDLE_PUSH = packet->data[2] / 16 * 10 + packet->data[2] % 16;
        ANGLE_PUAH_PULL = -((float)(packet->data[3] / 16 * 10 + packet->data[3] % 16));
        dataLen = sprintf(buf, "\r\nANGLE_VALUE: %.1f  %.1f  %.1f  %.1f\r\n", ANGLE_PULL_MIDDLE, ANGLE_ZHI_BEI, ANGLE_MIDDLE_PUSH, ANGLE_PUAH_PULL);
        usartb_sendData(buf, dataLen);
    }
    else if (packet->msgID == PRESSURE_SET)
    {
        // ��֫�����ѹ����ֵ����
        // data[0] Ϊ P_MAX �ĸ�λ����С��λ��Ĭ���ټ�10��ȡֵ��ΧΪ 10.0-19.9
        // data[1] Ϊ P_HIGH �ĸ�λ����С��λ��Ĭ���ټ�10��ȡֵ��ΧΪ 10.0-19.9
        P_MAX = (float)(packet->data[0] / 16) + packet->data[0] % 16 * 0.1 + 10.0;
        P_HIGH = (float)(packet->data[1] / 16) + packet->data[1] % 16 * 0.1 + 10.0;
        dataLen = sprintf(buf, "\r\nPRESSURE_VALUE: %.1f  %.1f\r\n", P_MAX, P_HIGH);
        usartb_sendData(buf, dataLen);
    }
    else if (packet->msgID == VALVE_PWM_SET)
    {
        // ��֫��valve��pwm���ֵ
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

