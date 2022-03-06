/*
 * uart.h
 *
 *  Created on: 2021��6��24��
 *      Author: 14472
 */

#ifndef _UART_H_
#define _UART_H_

#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�

void UARTa_Init(Uint32 baud);
void uart_printf(char * fmt, ...);

// ����
void scia_xmit(Uint16 a);
void scia_msg(char *msg);
void scia_int(int data_char);
void scia_float(double data);

#endif /* _UART_H_ */
