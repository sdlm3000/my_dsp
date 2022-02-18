/*
 * timer.h
 *
 *  Created on: 2021��6��4��
 *      Author: 14472
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�



void TIM0_Init(float Freq, float Period);
interrupt void TIM0_IRQn(void);

void TIM1_Init(float Freq, float Period);
interrupt void TIM1_IRQn(void);

void TIM2_Init(float Freq, float Period);
interrupt void TIM2_IRQn(void);


#endif /* TIMER_H_ */





