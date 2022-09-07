/*
 * main.c
 *
 *  Created on: 2022��5��10��
 *      Author: sdlm
 *
 *  ����JY61Pͨ��IIC��ȡ���ݵĹ���
 */

#include "common.h"
#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "jy61p.h"

//void ShortToChar(short sData,unsigned char cData[])
//{
//    cData[0]=sData&0xff;
//    cData[1]=sData>>8;
//}
//short CharToShort(unsigned char cData[])
//{
//    return ((short)cData[1]<<8)|cData[0];
//}

void  main()
{

//    unsigned char chrTemp[30];
////    unsigned char str[100];
//    float a[3],w[3],h[3],Angle[3];
//    Uint8 t=0, report=1;            //Ĭ�Ͽ����ϱ�
//    float pitch, roll, yaw;       //ŷ����
//    short aacx ,aacy, aacz;       //���ٶȴ�����ԭʼ����
//    short gyrox, gyroy, gyroz;    //������ԭʼ����
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
//    LED_Init();

    TIM0_Init(150,50000);    // 10ms
//    TIM1_Init(150,5000);  // 10ms
    //ʹ�����ж�
    EINT;
    ERTM;

    while(1)
    {
//        scia_msg("3 ");
//        delay_ms(100);

    }

}
