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
extern double press1, press2, press3;
extern int pwm1, pwm2;
int rt_flag = 0;    // 1�������ȣ�2�������
float pwm_counter = 1.0;

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
    delay_ms(1000);
    MIDDLE();
//    delay_ms(4000);         //�ȴ�ʱ�䣬Ϊ��������Ԥ�������ʱ��
    for(;;)
    {
        if (bt_uart_receive(&ch, 1))    /*!< ��ȡ����fifoһ���ֽ� */
        {
            if (bt_unpack(ch))          /*!< ��������Ч���ݰ� */
            {
                btParsing(&rx_Packet);
            }
        }
        if(state == 3)
        {
            if(press1 > P_max || press2 > P_max || press3 > P_max)
            {
                motor_stop();
                UNLOAD();
                delay_ms(200);
                pwm1 = 0;
                pwm2 = 0;
                MIDDLE();
                state = 2;
            }
        }
        else if(state == 4)
        {
            if(rt_flag == 1)
            {
                if(arg > ANGLE_PULL)
                {
                    PULL();
                }
                else
                {
                    rt_flag = 2;
                }
            }
            else if(rt_flag == 2)
            {
                if(arg < ANGLE_PUSH)
                {
                    PUSH();
                }
                else
                {
                    rt_flag = 1;
                }
            }
        }
//        else if(state == 0) // ��Ƶ����
//        {
//
//            EPwm2Regs.CMPA.half.CMPA = 0.2 * MAX_SPEED;
//            EPwm3Regs.CMPA.half.CMPA = 0.3 * MAX_SPEED;
//            EPwm5Regs.CMPA.half.CMPA = 0.8 * MAX_SPEED;
//            EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD * pwm_counter;           // Set timer period 804 TBCLKs
//            EPwm4Regs.CMPA.half.CMPA = 0.6 * EPWM4_TIMER_TBPRD * pwm_counter;
//            pwm_counter += 0.05;
//        }

    }
}
