/*
 * buzzer.h
 *
 *  Created on: 2021年5月21日
 *      Author: 14472
 */

#ifndef BUZZER_H_
#define BUZZER_H_






#include  "DSP2833x_Device.h"	//  DSP2833x  头文件
#include  "DSP2833x_Examples.h"	//  DSP2833x  例子相关头文件






#define  BUZZER_ON	(GpioDataRegs.GPASET.bit.GPIO7=1)
#define  BUZZER_OFF	(GpioDataRegs.GPACLEAR.bit.GPIO7=1)
#define  BUZZER_TOGGLE	(GpioDataRegs.GPATOGGLE.bit.GPIO7=1)


void  BUZZER_Init(void);




#endif  /* BUZZER_H_ */
