/*
 * timer.h
 *
 *  Created on: 2022Äê1ÔÂ25ÈÕ
 *      Author: sdlm
 */

#ifndef _TIMER_H_
#define _TIMER_H_


#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File
#include "common.h"




void TIM0_Init(float Freq, float Period);
interrupt void TIM0_IRQn(void);

void TIM1_Init(float Freq, float Period);
interrupt void TIM1_IRQn(void);

void TIM2_Init(float Freq, float Period);
interrupt void TIM2_IRQn(void);


#endif /* _TIMER_H_ */
