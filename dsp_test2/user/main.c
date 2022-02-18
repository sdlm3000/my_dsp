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
float pitch, roll, yaw;
Uint8 res;


void  main()
{
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();

    UARTa_Init(115200);

    I2CA_Init();
//    res = MPU_Init();
//    uart_printf("MPU_init : %d\r\n", res);
    LED_Init();
    delay_ms(1000);

    res = mpu_dmp_init();
    uart_printf("res: %d\r\n", res);
    while(res)
    {
        uart_printf("mpu6050 error\r\n");
        delay_ms(200);
        res = mpu_dmp_init();
        uart_printf("mpu: %d\r\n", res);
    }
//    delay_ms(1000);
//    mpu_dmp_init();
//    delay_ms(1000);
//    mpu_dmp_init();
    while(1)
    {
        int tmp = -1;
        if(MPU_Get_Accelerometer(&ax, &ay, &az) == 0){
            uart_printf("accx:%d, accy:%d, accz: %d\r\n", ax, ay, az);
        }
        if(MPU_Get_Gyroscope(&gx, &gy, &gz) == 0){
            uart_printf("gyrox:%d, gyroy:%d, gyroz: %d\r\n", gx, gy, gz);
        }
        tmp = mpu_dmp_get_data(&pitch, &roll, &yaw);
        uart_printf("tmp : %d \r\n", tmp);
        if(!tmp)
        {
            uart_printf("pitch:%d, roll:%d, yaw: %d\r\n", pitch, roll, yaw);
        }
        else uart_printf("error\r\n");
        LED8_TOGGLE;
//        Uint8 res = MPU_Read_Byte(MPU_ADDR, MPU_DEVICE_ID_REG);
//        uart_printf("res: %d\r\n", res);

        DELAY_US(1000 * 500);


    }

}
