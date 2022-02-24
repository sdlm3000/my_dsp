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

double volt1=0, volt2=0, volt3=0, volt4=0, volt5=0, volt6=0, volt7=0;
Uint32 sum1=0, sum2=0,sum3=0, sum4=0, sum5=0, sum6=0, sum7=0;
Uint16 SampleTable1[BUF_SIZE];
Uint16 SampleTable2[BUF_SIZE];
Uint16 SampleTable3[BUF_SIZE];
Uint16 SampleTable4[BUF_SIZE];
Uint16 SampleTable5[BUF_SIZE];
Uint16 SampleTable6[BUF_SIZE];
Uint16 SampleTable7[BUF_SIZE];

int i = 0;

double arg_ave[5] = {0.0};

double press1 = 0, press2 = 0, press3 = 0;
double arg = 0.0;

Uint16 press_plante1 = 0;
Uint16 press_plante2 = 0;
Uint16 press_plante3 = 0;

int state_motor = 0;        //���ڱ�ʾ���������״̬��0 -> ����,1 -> ֹͣ

int valve_pwm_middle_zhi = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_middle_bei = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_push = 0;     //����״̬�£����ڿ��Ʒ���PWMֵ

int counter_P = 0;          // ���ڼ�¼ѹ�������¼��Ĵ�������ֹ�󴥷�

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

    EPwm1Regs.CMPA.half.CMPA = minspeed;

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
//        delay_ms(1000);
//        uart_printf("hello\r\n");
//        LED8_TOGGLE;
    }
}
