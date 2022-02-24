/*
 * bluetooth.h
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 * 用于接收dsp的数据
 * 并向dsp发送影响控制指令
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "common.h"
#include "uart.h"

#define bt_uart_receive(data, len)  usartb_getRxData(data, len) /*!< 串口获取接收数据API */
#define bt_uart_send(data, len)     usartb_sendData(data, len)  /*!< 串口i发送数据API */

/*上行帧头*/
#define UP_BYTE1        0x55

/* 控制指令ID */
enum controlmsgid
{
    STATE_SET       = 0x01,     /*!< 状态设置 */
    PWM_SET         = 0x02,     /*!< PWM设置 */
};

#define BT_MAX_DATA_SIZE 28

/*通讯数据结构*/
typedef struct
{
    Uint8 startByte1;
    Uint8 msgID;
    Uint8 dataLen;
    Uint8 data[BT_MAX_DATA_SIZE];
    Uint8 checkSum;
} bt_t;


/* 串口接收解析成功的数据包 */
extern bt_t   rx_Packet;

Uint8 bt_unpack(Uint8 ch);
void btParsing(bt_t *packet);

#endif /* _BLUETOOTH_H_ */
