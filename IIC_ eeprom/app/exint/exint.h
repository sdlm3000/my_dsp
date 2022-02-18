/*
 * exint.h
 *
 *  Created on: 2021年6月3日
 *      Author: 14472
 */

#ifndef EXINT_H_
#define EXINT_H_


#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件


void EXINT1_Init(void);
interrupt void EXINT1_IRQn(void);

void EXINT2_Init(void);
interrupt void EXINT2_IRQn(void);



#endif /* EXINT_H_ */




