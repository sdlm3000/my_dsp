/*
 * main.c
 *
 *  Created on: 2022Äê1ÔÂ18ÈÕ
 *      Author: sdlm
 */

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


int T;
short ax, ay, az, gx, gy, gz;
short ggx,aax;
float pitch, roll, yaw;
Uint8 res = 1;


void  main()
{
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();

    UARTa_Init(115200);

    I2CA_Init();
    LED_Init();
//    MPU_Init();
//    delay_ms(1000);
    uart_printf("start\r\n");
    while(res)
    {
        uart_printf("mpu6050 error\r\n");
        delay_ms(200);
        res = mpu_dmp_init();
        uart_printf("mpu_dmp_init: %d\r\n", res);
    }
    while(1)
    {
        if(mpu_dmp_get_data(&pitch, &roll, &yaw, &ggx, &aax) == 0)
        {
            uart_printf("pitch:%f, roll:%f, yaw: %f, ggx: %d, aax: %d\r\n", pitch, roll, yaw, ggx, aax);
        }
        else uart_printf("error\r\n");
        if(MPU_Get_Accelerometer(&ax, &ay, &az) == 0){
            uart_printf("accx:%d, accy:%d, accz: %d\r\n", ax, ay, az);
        }
        if(MPU_Get_Gyroscope(&gx, &gy, &gz) == 0){
            uart_printf("gyrox:%d, gyroy:%d, gyroz: %d\r\n", gx, gy, gz);
        }
        LED8_TOGGLE;

        DELAY_US(1000 * 5);


    }

}
