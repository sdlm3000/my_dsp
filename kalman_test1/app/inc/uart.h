/*
 * uart.h
 *
 *  Created on: 2021年6月24日
 *      Author: 14472
 */

#ifndef _UART_H_
#define _UART_H_

#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件
#include "common.h"

#define MAX_WRITE_LEN (528)

void UARTa_Init(Uint32 baud);
void uart_printf(char * fmt, ...);
void usart1_send_char(Uint8 a);

#endif /* _UART_H_ */

