/*
 * main.c
 *
 *  Created on: 2022��2��24��
 *      Author: sdlm
 * ���ڼ�֫��ʵ��̨���������Ի�ʵ��
 */

#include "common.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "leds.h"
#include "bluetooth.h"


//int counter_P = 0;          // ���ڼ�¼ѹ�������¼��Ĵ�������ֹ�󴥷�
extern int state;

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
    PWM_Init();
    LED_Init();
    ADC_Init();
    TIM0_Init(150, 5000);

    EPwm1Regs.CMPA.half.CMPA = MIN_SPEED;

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

//    CpuTimer0Regs.TCR.all = 0x4001;     // ����TIE = 1��������ʱ��0�ж�
    UNLOAD();
    DELAY_US(50000);
    MIDDLE();
    DELAY_US(1500000);          //�ȴ�ʱ�䣬Ϊ��������Ԥ�������ʱ��
    for(;;)
    {
        if (bt_uart_receive(&ch, 1))    /*!< ��ȡ����fifoһ���ֽ� */
        {
            if (bt_unpack(ch))          /*!< ��������Ч���ݰ� */
            {
                btParsing(&rx_Packet);
            }
        }
        if(state == 3 && arg < 0)
        {
            motor_stop();

            MIDDLE();
        }
    }
}
