/*
 * multi_mpu.h
 *
 *  Created on: 2022年3月5日
 *      Author: sdlm
 *  功能：用于片选多个mpu
 */

#ifndef _MULTI_MPU_H_
#define _MULTI_MPU_H_

#include "common.h"

#define  MPU1_OFF  (GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1)
#define  MPU1_ON   (GpioDataRegs.GPCSET.bit.GPIO68 = 1)

#define  MPU2_OFF  (GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1)
#define  MPU2_ON   (GpioDataRegs.GPCSET.bit.GPIO67 = 1)

#define  MPU3_OFF  (GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1)
#define  MPU3_ON   (GpioDataRegs.GPCSET.bit.GPIO66 = 1)

#define  MPU4_OFF  (GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1)
#define  MPU4_ON   (GpioDataRegs.GPCSET.bit.GPIO65 = 1)

#define  MPU5_OFF  (GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1)
#define  MPU5_ON   (GpioDataRegs.GPCSET.bit.GPIO64 = 1)

void AddrGpio_Init();

void mpu_select(int mpu_num);




#endif /* APP_INC_MULTI_MPU_H_ */
