/*
 * uart.h
 *
 *  Created on: 2021��6��24��
 *      Author: 14472
 */

#ifndef _UART_H_
#define _UART_H_

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File
#include "common.h"

#define MAX_WRITE_LEN (528)

void UARTa_Init(Uint32 baud);
void UARTb_Init(Uint32 baud);
void uart_printf(char * fmt, ...);
void uarta_send_char(Uint8 a);

Uint16 uartb_send_char(Uint8 a);

Uint16 usartb_getRxData(Uint8 *buf, Uint16 len);
Uint16 usartb_sendData(Uint8 *buf, Uint16 len);

#endif /* _UART_H_ */

