/*
 * main.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


int T;

Uint8 res = 1;




void  main()
{
//    Uint8 t=0, report=1;            //默认开启上报
//    float pitch, roll, yaw;       //欧拉角
//    short aacx ,aacy, aacz;       //加速度传感器原始数据
//    short gyrox, gyroy, gyroz;    //陀螺仪原始数据
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    UARTa_Init(115200);
    I2CA_Init();
    LED_Init();

    uart_printf("start\r\n");
    while(res)
    {
        uart_printf("mpu6050 error\r\n");
        delay_ms(200);
        LED9_TOGGLE;
        res = mpu_dmp_init();
        uart_printf("mpu_dmp_init: %d\r\n", res);
    }
    TIM0_Init(150,100000);//10ms
    TIM1_Init(150,200000);//200ms
//    TIM2_Init(150,200000);//200ms
    while(1)
    {
        LED8_TOGGLE;
        DELAY_US(1000 * 100);
        uart_printf("hello\r\n");

    }

}
