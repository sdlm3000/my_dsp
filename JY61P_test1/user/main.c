/*
 * main.c
 *
 *  Created on: 2022��5��10��
 *      Author: sdlm
 *
 *  ����JY61Pͨ��IIC��ȡ���ݵĹ���
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

    TIM0_Init(150, 5000);   // 150000000Hz�� 150��Ƶ�� 5000��һ�����ڣ� 200Hz
//    TIM1_Init(150,5000);
    //ʹ�����ж�
    EINT;
    ERTM;

    while(1)
    {
        if(predict_flag == 1)
        {
            
        }


    }

}
