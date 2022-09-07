/*
 * main.c
 *
 *  Created on: 2022��09��06��
 *      Author: sdlm
 * ���ڼ�֫��Ӳ����������ԣ����һ���Ϊ֮�󴩴�ʵ��Ļ����汾
 */
#include "common.h"
#include "uart.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "bluetooth.h"
#include "i2c.h"

int rt_flag1 = 0;    // 1�������ȣ�2�������

int main(void)
{
    Uint8 ch;

    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();

    //���DSP�ĳ�ʼ��
    // ���� 1. ��ʼ��ϵͳ����:
    InitSysCtrl();

    // ����3. ��������ж�,��ʼ���ж�������
    DINT;
    // ��ʼ��PIE���ƼĴ��������ǵ�Ĭ��״̬.
    InitPieCtrl();
    // ��ֹCPU�жϺ��������CPU�жϱ�־
    IER = 0x0000;
    IFR = 0x0000;
    //��ʼ��PIE�ж���������ʹ��ָ���жϷ����ӳ���ISR��
    InitPieVectTable();

    UARTb_Init(115200);
    I2CA_Init();
    PWM_Init();
    ADC_Init();


    TIM0_Init(150, 5000);   // 5ms

//    // ���ڲɼ�IMU����
//    TIM1_Init(150, 5000);   // 5ms

    // ��ʼж��
    UNLOAD();
    delay_ms(1000);
    MIDDLE();

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

    while(1)
    {
        if (bt_uart_receive(&ch, 1))    /*!< ��ȡ����fifoһ���ֽ� */
        {
            if (bt_unpack(ch))          /*!< ��������Ч���ݰ� */
            {
                btParsing(&rx_Packet);
            }
        }

    }
}
