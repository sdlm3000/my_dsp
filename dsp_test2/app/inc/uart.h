/*
 * uart.h
 *
 *  Created on: 2021年6月24日
 *      Author: 14472
 */

#ifndef UART_H_
#define UART_H_

#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件


void UARTa_Init(Uint32 baud);
void UARTa_SendByte(int a);
void UARTa_SendString(char * msg);

void uart_printf(char * fmt, ...);
void scia_xmit(Uint32 a);
void scia_msg(char *msg);
void scia_int(int data_char);
void scia_float(double data);

#endif /* UART_H_ */

