/*
 * dc_motor.h
 *
 *  Created on: 2021年5月29日
 *      Author: 14472
 */


#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_


#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件


#define DC_MOTOR_INA_SETH			(GpioDataRegs.GPASET.bit.GPIO0=1)
#define DC_MOTOR_INA_SETL			(GpioDataRegs.GPACLEAR.bit.GPIO0=1)
#define DC_MOTOR_INB_SETH			(GpioDataRegs.GPASET.bit.GPIO1=1)
#define DC_MOTOR_INB_SETL			(GpioDataRegs.GPACLEAR.bit.GPIO1=1)


void DC_Motor_Init(void);

#endif /* DC_MOTOR_H_ */




