/*
 * main.c
 *
 *  Created on: 2022年1月18日
 *      Author: sdlm
 */

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "key.h"
#include "smg.h"
#include "math.h"

Uint16 dat1[8]={0,0,0,0,0,0,0,0};
Uint16 dat[] = {0x30};
float t;
void  main()
{
//    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
//    // This function must reside in RAM
//    InitFlash();
    InitSysCtrl();

    UARTa_Init(115200);

    LED_Init();

    I2CA_Init();
//    printf("helloooo");
    SMG_Init();
    KEY_Init();

    uart_printf("helloooo");
    UARTa_SendString("hello\r\n");
//    k = 0;
//    while (k < 10){
//        k++;
//        if(AT24CXX_Check2())  //检测AT24C08是否正常
//        {
//            UARTa_SendString("AT24C08 NO!\r\n");
//            DELAY_US(100*1000);
//        }
//        else{
//            UARTa_SendString("AT24C08 OK!\r\n");
//        }
//    }

    while(1)
    {
//        int a = 100;
//        float b = 1.90;
//        char c[] = "world";
//
//        uart_printf("helloooo");
//        uart_printf("test: a - %d, b - %.2f, c - %s", a, b , c);
//        LED8_TOGGLE;
//
//        DELAY_US(1000 * 1000);
        t = atan2f(1.0, 2.0) * 57.3;


//        while(1)
//        {


//        key=KEY_Scan(0);
//        if(key==KEY1_PRESS)
//        {
//            k++;
//            if(k>255)
//            {
//                k=255;
//            }
//            dat[0] = k;
//            I2C_WriteData(0x50, dat, 0x00, 1);
//        }
//        else if(key==KEY2_PRESS)
//        {
//            I2C_ReadData(0x50, dat1, 0x00, 1);
//            k = dat1[0];
//        }
//        else if(key==KEY3_PRESS)
//        {
//            k=0;
//        }
//        SMG_DisplayInt(k);
    }

}
