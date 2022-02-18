/*
 * spi.h
 *
 *  Created on: 2021年6月25日
 *      Author: 14472
 */

#ifndef SPI_H_
#define SPI_H_


#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件


void SPIA_Init(void);
Uint16 SPIA_SendReciveData(Uint16 dat);


#endif /* SPI_H_ */
