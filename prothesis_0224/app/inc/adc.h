/*
 * adc.h
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "common.h"


//ADCµÄºê¶¨Òå
#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                        = 16 ADC clocks
#define AVG        8  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset


void ADC_Init();




#endif /* _ADC_H_ */
