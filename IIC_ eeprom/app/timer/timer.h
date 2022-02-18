/*
 * timer.h
 *
 *  Created on: 2021年6月4日
 *      Author: 14472
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件



void TIM0_Init(float Freq, float Period);
interrupt void TIM0_IRQn(void);

void TIM1_Init(float Freq, float Period);
interrupt void TIM1_IRQn(void);

void TIM2_Init(float Freq, float Period);
interrupt void TIM2_IRQn(void);


#endif /* TIMER_H_ */





