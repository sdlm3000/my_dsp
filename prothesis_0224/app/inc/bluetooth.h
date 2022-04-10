/*
 * bluetooth.h
 *
 *  Created on: 2022��2��24��
 *      Author: sdlm
 * ���ڽ���dsp������
 * ����dsp����Ӱ�����ָ��
 * Ĭ��ָ��ĸ�ʽΪ 55xx02xxxxffH
 * 1. 550102xx00ffΪ����״̬���ã����� 00 - ж��+���ֹͣ�� 01 - ж�ɣ� 02 - ������ 03 - ���ȣ� 04 - ���ȣ� 05 - ������У� 06 - ���ֹͣ
 * 2. 550202xx00ffΪ����״̬���ã����� 01 - ϵͳж�ɣ� 02 - ����ʵ��ı���̬�� 03 - ��֫״̬�ظ�, 04 - ��֫�����˶�
 * 3. 550302xxxxffΪ���ط�pwm���ã��ֱ����ñ���̬���������ط��Ŀ���
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
    STATE_SET1      = 0x01,     /*!< ����״̬���� */
    STATE_SET2      = 0x02,     /*!< ����״̬���� */
    PWM_SET         = 0x03,     /*!< PWM���� */
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
