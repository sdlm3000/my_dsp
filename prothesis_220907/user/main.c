/*
 * main.c
 *
 *  Created on: 2022��09��07��
 *      Author: sdlm
 * 2022.09.10  ���ڼ�֫�Ĳ���ʵ�飬��Ҫʹ����������Ԥ���㷨������Ӧ³������
 * 2022.10.12  ����汾��Ҫ���ڼ�֫����ʵ��Ĳ��ԣ��������㷨��
 */
#include "common.h"
#include "uart.h"
#include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "bluetooth.h"
#include "i2c.h"

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
        // ���ڽ����������ݣ�����������
        if (bt_uart_receive(&ch, 1))    /*!< ��ȡ����fifoһ���ֽ� */
        {
            if (bt_unpack(ch))          /*!< ��������Ч���ݰ� */
            {
                btParsing(&rx_Packet);
            }
        }
        // ��֫��Ҫ�����߼�
        if(CONTROL_FLAG == NO_CONTROL_OUT)
        {
            motor_stop();
        }
        else if(CONTROL_FLAG == CONTROL_OUT)
        {
            // TODO: ������
            if(proState == MIDDLE_ZHI_STATE)
            {
                if(angle <= ANGLE_ZHI_BEI)
                {
                    proState = MIDDLE_BEI_STATE;
                }
            }
            else if(proState == MIDDLE_BEI_STATE)
            {
                if(angle >= ANGLE_MIDDLE_PUSH)
                {
                    proState = PUSH_STATE;
                }
            }
            else if(proState == PUSH_STATE)
            {
                if(angle <= ANGLE_PUAH_PULL)
                {
                    proState = PULL_STATE;
                }
            }
            else if(proState == PULL_STATE)
            {
                if(angle >= ANGLE_PULL_MIDDLE)
                {
                    proState = MIDDLE_ZHI_STATE;
                }
            }

            switch(proState)
            {
                case MIDDLE_ZHI_STATE:
                    MIDDLE_zhi();
                    break;
                case MIDDLE_BEI_STATE:
                    MIDDLE_bei();
                    break;
                case PUSH_STATE:
                    PUSH_control();
                    break;
                case PULL_STATE:
                    PULL();
                    valve_pwm_middle_zhi = MIN_SPEED;
                    valve_pwm_middle_bei = MIN_SPEED;
                    valve_pwm_push = MAX_SPEED;
                    middle_zhi_controlFlag = 0;
                    break;
                default:
                    break;
            }

            if(press1 < (P_HIGH - 0.5) && press1 > 0)
            {
                motor_run();
            }
            else if(press1>(P_HIGH + 0.5))
            {
                motor_stop();
            }

        }
        else    // Ĭ�϶��� CONTROL_RESET
        {
            // �ص���ʼ״̬
            motor_stop();
            UNLOAD();
            delay_ms(1000);
            MIDDLE();
            proState = MIDDLE_ZHI_STATE;
        }


    }
}
