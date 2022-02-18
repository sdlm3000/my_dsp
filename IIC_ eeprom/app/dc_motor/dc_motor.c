/*
 * dc_motor.c
 *
 *  Created on: 2021��5��29��
 *      Author: 14472
 */

#include "dc_motor.h"


void DC_Motor_Init(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;// ����GPIOʱ��
	//DC_Motor �˿�����
	GpioCtrlRegs.GPAMUX1.bit.GPIO0=0;
	GpioCtrlRegs.GPADIR.bit.GPIO0=1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO1=0;
	GpioCtrlRegs.GPADIR.bit.GPIO1=1;


	EDIS;

	GpioDataRegs.GPACLEAR.bit.GPIO0=1;
	GpioDataRegs.GPACLEAR.bit.GPIO1=1;

}




