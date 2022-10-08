/*
 * main.c
 *
 *  Created on: 2022年5月10日
 *      Author: sdlm
 *
 *  测试JY61P通过IIC读取数据的功能
 */

#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "jy61p.h"
#include "anglePred.h"

extern int predict_flag;

void  main()
{

    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    UARTa_Init(230400);
//    UARTa_Init(115200);
    I2CA_Init();

    TIM0_Init(150, 5000);   // 150000000Hz， 150分频， 5000点一个周期， 200Hz
//    TIM1_Init(150,5000);
    //使能总中断
    EINT;
    ERTM;

    while(1)
    {
        if(predict_flag == 1)
        {
            
        }


    }

}
