/*
 * main.c
 *
 *  Created on: 2022年1月29日
 *      Author: sdlm
 */

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "imu901.h"
#include "timer.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

Uint32 T = 0;

Uint8 res = 1;

void  main()
{
    Uint32 times = 0;
    Uint8 ch;
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
    UARTb_Init(115200);
    I2CA_Init();
    LED_Init();
    imu901_init();
    EINT;
    ERTM;

    while(res)
    {
        delay_ms(200);
        LED9_TOGGLE;
        res = mpu_dmp_init();

        uart_printf("mpu_dmp_init error: %d\r\n", res);
    }

    TIM0_Init(150,10000);//10ms
    TIM1_Init(150,5000);//200ms


    while(1)
    {

        if (imu901_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (imu901_unpack(ch))          /*!< 解析出有效数据包 */
            {
                if (rxPacket.startByte2 == UP_BYTE2)            /*!< 主动上传的数据包 */
                {
                    atkpParsing(&rxPacket);
                }
            }
        }

    }

}
