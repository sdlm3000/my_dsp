/*
 * bluetooth.h
 *
 *  Created on: 2022年2月24日
 *      Author: sdlm
 * 用于接收dsp的数据
 * 并向dsp发送影响控制指令
 * 默认指令的格式为 55xx02xxxxffH
 * 1. 550102xx00ff为基本状态设置，其中 00 - 卸荷+电机停止， 01 - 卸荷， 02 - 蹬腿， 03 - 被动， 04 - 收腿， 05 - 电机状态转变， 06 - 电机停止
 * 2. 550202xx00ff为设置控制率是否输出，00 -> 不输出控制率，电机停止， 01 -> 正常输出控制率， 02 -> 不输出控制率，电机停止，卸荷，然后进入被动态
 * 3. 550304xxxxxxxxff为假肢的触发角度阈值设置
 * 4. 550402xxxxff为假肢的相关压力阈值设置
 * 5. 550503xxxxxxff为假肢的valve的pwm最大值
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
    ANGLE_SET       = 0x03,     /*!< 角度相关阈值设置 */
    PRESSURE_SET    = 0x04,     /*!< 角度相关阈值设置 */
    VALVE_PWM_SET   = 0x05,     /*!< VALVE2 PWM阈值设置 */
    VALUE_GET       = 0x10,     /*!< 获取可修改变量 */
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
