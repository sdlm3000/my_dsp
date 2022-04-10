/*
 * bluetooth.h
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 * 用于接收dsp的数据
 * 并向dsp发送影响控制指令
 * 默认指令的格式为 55xx02xxxxffH
 * 1. 550102xx00ff为基本状态设置，其中 00 - 卸荷+电机停止， 01 - 卸荷， 02 - 被动， 03 - 蹬腿， 04 - 收腿， 05 - 电机运行， 06 - 电机停止
 * 2. 550202xx00ff为工作状态设置，其中 01 - 系统卸荷， 02 - 阻尼实验的被动态， 03 - 假肢状态回复, 04 - 假肢往复运动
 * 3. 550302xxxxff为开关阀pwm设置，分别设置被动态下两个开关阀的开度
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
    STATE_SET1      = 0x01,     /*!< 基本状态设置 */
    STATE_SET2      = 0x02,     /*!< 工作状态设置 */
    PWM_SET         = 0x03,     /*!< PWM设置 */
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
