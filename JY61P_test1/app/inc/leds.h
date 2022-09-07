/*
 * leds.h
 *
 *  Created on: 2021年5月19日
 *      Author: 14472
 */

#ifndef _LEDS_H_
#define _LEDS_H_



#include  "DSP2833x_Device.h"	//  DSP2833x  头文件
#include  "DSP2833x_Examples.h"	//  DSP2833x  例子相关头文件



#define  LED14_OFF	(GpioDataRegs.GPCSET.bit.GPIO68=1)
#define  LED14_ON	(GpioDataRegs.GPCCLEAR.bit.GPIO68=1)
#define  LED14_TOGGLE	(GpioDataRegs.GPCTOGGLE.bit.GPIO68=1)



#define  LED13_OFF	(GpioDataRegs.GPCSET.bit.GPIO67=1)
#define  LED13_ON	(GpioDataRegs.GPCCLEAR.bit.GPIO67=1)
#define  LED13_TOGGLE	(GpioDataRegs.GPCTOGGLE.bit.GPIO67=1)



#define  LED12_OFF	(GpioDataRegs.GPCSET.bit.GPIO66=1)
#define  LED12_ON	(GpioDataRegs.GPCCLEAR.bit.GPIO66=1)
#define  LED12_TOGGLE	(GpioDataRegs.GPCTOGGLE.bit.GPIO66=1)



#define  LED11_OFF	(GpioDataRegs.GPCSET.bit.GPIO65=1)
#define  LED11_ON	(GpioDataRegs.GPCCLEAR.bit.GPIO65=1)
#define  LED11_TOGGLE	(GpioDataRegs.GPCTOGGLE.bit.GPIO65=1)



#define  LED10_OFF	(GpioDataRegs.GPCSET.bit.GPIO64=1)
#define  LED10_ON	(GpioDataRegs.GPCCLEAR.bit.GPIO64=1)
#define  LED10_TOGGLE	(GpioDataRegs.GPCTOGGLE.bit.GPIO64=1)



#define  LED9_OFF	(GpioDataRegs.GPBSET.bit.GPIO61=1)
#define  LED9_ON	(GpioDataRegs.GPBCLEAR.bit.GPIO61=1)
#define  LED9_TOGGLE	(GpioDataRegs.GPBTOGGLE.bit.GPIO61=1)


#define  LED8_OFF	(GpioDataRegs.GPBSET.bit.GPIO60=1)
#define  LED8_ON	(GpioDataRegs.GPBCLEAR.bit.GPIO60=1)
#define  LED8_TOGGLE	(GpioDataRegs.GPBTOGGLE.bit.GPIO60=1)



void  LED_Init(void);




#endif /* _LEDS_H_ */
