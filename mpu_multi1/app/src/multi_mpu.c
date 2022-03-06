/*
 * multi_mpu.c
 *
 *  Created on: 2022年3月5日
 *      Author: as
 */


#include "multi_mpu.h"


void AddrGpio_Init()
{
    EALLOW;
    // MPU1 端口配置
    GpioCtrlRegs.GPCMUX1.bit.GPIO68=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO68=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO68=0;

    // MPU2 端口配置
    GpioCtrlRegs.GPCMUX1.bit.GPIO67=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO67=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO67=0;

    // MPU3 端口配置
    GpioCtrlRegs.GPCMUX1.bit.GPIO66=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO66=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO66=0;

    // MPU4 端口配置
    GpioCtrlRegs.GPCMUX1.bit.GPIO65=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO65=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO65=0;

    //MPU5 端口配置
    GpioCtrlRegs.GPCMUX1.bit.GPIO64=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO64=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO64=0;

    EDIS;
}

void mpu_select(int mpu_num)
{
    MPU1_OFF;
    MPU2_OFF;
    MPU3_OFF;
    MPU4_OFF;
    MPU5_OFF;
    switch(mpu_num)
    {
        case 1:
            MPU1_ON;
            break;
        case 2:
            MPU2_ON;
            break;
        case 3:
            MPU3_ON;
            break;
        case 4:
            MPU4_ON;
            break;
        case 5:
            MPU5_ON;
            break;
    }
}


