/*
 * buzzer.c
 *
 *  Created on: 2021��5��21��
 *      Author: 14472
 */

#include  "buzzer.h"


void  BUZZER_Init(void)
{

  EALLOW;



  //BUZZER �˿�����
  GpioCtrlRegs.GPAMUX1.bit.GPIO7=0; GpioCtrlRegs.GPADIR.bit.GPIO7=1; GpioCtrlRegs.GPAPUD.bit.GPIO7=0;



  EDIS;



  GpioDataRegs.GPACLEAR.bit.GPIO7=1;



}



