/*
 * spi.h
 *
 *  Created on: 2021��6��25��
 *      Author: 14472
 */

#ifndef SPI_H_
#define SPI_H_


#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�


void SPIA_Init(void);
Uint16 SPIA_SendReciveData(Uint16 dat);


#endif /* SPI_H_ */
