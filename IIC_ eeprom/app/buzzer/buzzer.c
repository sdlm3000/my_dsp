/*
 * buzzer.c
 *
 *  Created on: 2021年5月21日
 *      Author: 14472
 */

#include  "buzzer.h"


void  BUZZER_Init(void)
{

  EALLOW;



  //BUZZER 端口配置
  GpioCtrlRegs.GPAMUX1.bit.GPIO7=0; GpioCtrlRegs.GPADIR.bit.GPIO7=1; GpioCtrlRegs.GPAPUD.bit.GPIO7=0;



  EDIS;



  GpioDataRegs.GPACLEAR.bit.GPIO7=1;



}



