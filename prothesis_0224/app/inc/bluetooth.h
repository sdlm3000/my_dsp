/*
 * bluetooth.h
 *
 *  Created on: 2022��2��24��
 *      Author: sdlm
 * ���ڽ���dsp������
 * ����dsp����Ӱ�����ָ��
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "common.h"
#include "uart.h"

#define bt_uart_receive(data, len)  usartb_getRxData(data, len) /*!< ���ڻ�ȡ��������API */
#define bt_uart_send(data, len)     usartb_sendData(data, len)  /*!< ����i��������API */

/*����֡ͷ*/
#define UP_BYTE1        0x55

/* ����ָ��ID */
enum controlmsgid
{
    STATE_SET       = 0x01,     /*!< ״̬���� */
    PWM_SET         = 0x02,     /*!< PWM���� */
};

#define BT_MAX_DATA_SIZE 28

/*ͨѶ���ݽṹ*/
typedef struct
{
    Uint8 startByte1;
    Uint8 msgID;
    Uint8 dataLen;
    Uint8 data[BT_MAX_DATA_SIZE];
    Uint8 checkSum;
} bt_t;


/* ���ڽ��ս����ɹ������ݰ� */
extern bt_t   rx_Packet;

Uint8 bt_unpack(Uint8 ch);
void btParsing(bt_t *packet);

#endif /* _BLUETOOTH_H_ */