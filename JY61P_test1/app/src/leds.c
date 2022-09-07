/*
 * leds.c
 *
 *  Created on: 2021��5��19��
 *      Author: 14472
 */

#include  "leds.h"


void  LED_Init(void)
{

EALLOW;

//LED14 �˿�����
GpioCtrlRegs.GPCMUX1.bit.GPIO68=0;
GpioCtrlRegs.GPCDIR.bit.GPIO68=1;
GpioCtrlRegs.GPCPUD.bit.GPIO68=0;

//LED13 �˿�����
GpioCtrlRegs.GPCMUX1.bit.GPIO67=0;
GpioCtrlRegs.GPCDIR.bit.GPIO67=1;
GpioCtrlRegs.GPCPUD.bit.GPIO67=0;

//LED12 �˿�����
GpioCtrlRegs.GPCMUX1.bit.GPIO66=0;
GpioCtrlRegs.GPCDIR.bit.GPIO66=1;
GpioCtrlRegs.GPCPUD.bit.GPIO66=0;

//LED11 �˿�����
GpioCtrlRegs.GPCMUX1.bit.GPIO65=0;
GpioCtrlRegs.GPCDIR.bit.GPIO65=1;
GpioCtrlRegs.GPCPUD.bit.GPIO65=0;

//LED10 �˿�����
GpioCtrlRegs.GPCMUX1.bit.GPIO64=0;
GpioCtrlRegs.GPCDIR.bit.GPIO64=1;
GpioCtrlRegs.GPCPUD.bit.GPIO64=0;

//LED9�˿�����
GpioCtrlRegs.GPBMUX2.bit.GPIO61=0;
GpioCtrlRegs.GPBDIR.bit.GPIO61=1;
GpioCtrlRegs.GPBPUD.bit.GPIO61=0;

//LED8 �˿�����
GpioCtrlRegs.GPBMUX2.bit.GPIO60=0;
GpioCtrlRegs.GPBDIR.bit.GPIO60=1;
GpioCtrlRegs.GPBPUD.bit.GPIO60=0;

GpioDataRegs.GPCSET.bit.GPIO68=1;
GpioDataRegs.GPCSET.bit.GPIO67=1;
GpioDataRegs.GPCSET.bit.GPIO66=1;
GpioDataRegs.GPCSET.bit.GPIO65=1;
GpioDataRegs.GPCSET.bit.GPIO64=1;
GpioDataRegs.GPBSET.bit.GPIO61=1;
GpioDataRegs.GPBSET.bit.GPIO60=1;


EDIS;
}
