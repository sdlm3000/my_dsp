/*
 * relay.c
 *
 *  Created on: 2021年5月21日
 *      Author: 14472
 */

#include  "relay.h"






void  Relay_Init(void)
{

    EALLOW;



    //继电器端口配置
    GpioCtrlRegs.GPAMUX1.bit.GPIO6=0;
    GpioCtrlRegs.GPADIR.bit.GPIO6=1;
    GpioCtrlRegs.GPAPUD.bit.GPIO6=0;



    EDIS;



    GpioDataRegs.GPACLEAR.bit.GPIO6=1;
}



