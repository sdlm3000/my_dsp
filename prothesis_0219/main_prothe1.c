/*
 * main.c
 *
 *  Created on: 2022��2��19��
 *      Author: sdlm
 * ���ڼ�֫��ʵ��̨������ʵ��
 * ��ȡ���߹����е��ڲ�Һѹѹ��ֵ���׹ؽڽǶ�ֵ
 * ͬʱ��Ҫ���ݽ��Ƕȴ�������ʵ�ʽǶȽ���ӳ��
 * ��ͨ��ģ�ͼ����ÿ���Ƕ�ֵ���ڵĸ��е�ѹ����
 * ����PID����
 */

#include "common.h"
#include "prothesis.h"
#include "pwm.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"




double volt1=0, volt2=0, volt3=0, volt4=0, volt5=0, volt6=0, volt7=0;
Uint32 sum1=0, sum2=0,sum3=0, sum4=0, sum5=0, sum6=0, sum7=0;
Uint16 SampleTable1[BUF_SIZE];
Uint16 SampleTable2[BUF_SIZE];
Uint16 SampleTable3[BUF_SIZE];
Uint16 SampleTable4[BUF_SIZE];
Uint16 SampleTable5[BUF_SIZE];
Uint16 SampleTable6[BUF_SIZE];
Uint16 SampleTable7[BUF_SIZE];

int i=0;
int flag_chong = 0;

double arg_ave[5] = {0.0};

double press1 = 0, press2 = 0, press3 = 0;
double arg = 0.0;

double Beta = 0.0, Beta_last = 0.0;   // �Ƕ���

double W_BD = 0.0;

double Alpha, Alpha_cos, Alpha_sin;

double T = 0.0, F_A = 0.0;



// ��Ҫ���ڱ���̬���Է���PWM����PID���ƣ�ʵ�ּ�֫���迹�������׹ؽڵ��迹ģ��
double delta_P = 0.0;       // ����ѹ�P2-P3
double delta_P0 = 0.0;      // ʵ��ѹ��
double dd_P = 0.0;          // ʵ��ѹ��������ѹ��Ĳ�ֵ��delta_P0 - delta_P



Uint16 press_plante1 = 0;
Uint16 press_plante2 = 0;
Uint16 press_plante3 = 0;

// ״̬��0-ж��̬��1-����������2-����������3-����������4-��������
// ����1��4����Ӧֵ����̬������Ϊһ��
int state = 0;               //���ڱ�ʾ��֫��״̬
int state0 = 0;               //���ڱ�ʾ��֫����һ��״̬

int state_motor = 0;        //���ڱ�ʾ���������״̬��0 -> ����,1 -> ֹͣ

int switch_flag = 0;

int valve_pwm_middle_zhi = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_middle_bei = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_push = 0;     //����״̬�£����ڿ��Ʒ���PWMֵ

int flag_run = 0;
int flag_circu = 0;         // �������˶��ı�־λ
int flag_state1 = 0;        // state1��ʼ����PWM�ı�־λ

int counter_time = 0;       // ������������������֮�����ʱ������
int counter = 0;            // ���ڼ�¼�����¼��Ĵ�������ֹ�󴥷�
int counter_P = 0;          // ���ڼ�¼ѹ�������¼��Ĵ�������ֹ�󴥷�

long my_debug = 0;

int counter_circu = 0;

double my_temp1,my_temp2;




int main(void)
{
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

    UARTa_Init(115200);
    PWM_Init();
    TIM0_Init(150, 5000);

    EPwm1Regs.CMPA.half.CMPA = minspeed;

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

//    CpuTimer0Regs.TCR.all = 0x4001;     // ����TIE = 1��������ʱ��0�ж�

    UNLOAD();
    DELAY_US(50000);
    MIDDLE();
    DELAY_US(1500000);          //�ȴ�ʱ�䣬Ϊ��������Ԥ�������ʱ��
    state = 1;
    flag_run = 1;
    flag_circu = 0;
    valve_pwm_middle_zhi = minspeed;
    valve_pwm_middle_bei = minspeed;
    valve_pwm_push = maxspeed;

    while(1)
    {
        if(flag_run==1)
        {
            my_debug++;
            if(state==1)
            {
                if(arg<=-3)
                {
                    state = 2;
                }
            }
            if(state==2)
            {
                counter++;
                if(arg>=5)//if(volt7 - volt6 <= 0.07)// if(arg>=4.5)
                {
//                    if(counter>=10)
//                    {
                    state = 3;
                    counter = 0;
//                    }
                }
            }
            if(state==3)
            {
                if(arg<=-11.5)  // && press_plante2<=0.1)
                {
                    state = 4;
                }
            }
//            if(state==4)
//            {
//                if(counter_time>=30)
//                {
//                   state = 5;
//                   counter_time = 0;
//                }
//            }
            if(state==4)
            {
                if(arg>=0 || volt7>=0.8)
                {
                    state = 1;
                }
            }
            switch(state)
            {
                case 1:
                    MIDDLE_zhi();
                    motor_stop();
                    flag_chong = 0;
                    break;
                case 2:
                    MIDDLE_bei();
                    motor_stop();
                    flag_chong = 0;
                    break;
                case 3:
                    PUSH_control();
                    flag_chong = 1;
                    break;
//                case 4:
//                    motor_stop();
//                    break;
                case 4:
                    PULL();
                    motor_run();
                    flag_chong = 1;
                    valve_pwm_middle_zhi = minspeed;
                    valve_pwm_middle_bei = minspeed;
                    valve_pwm_push = maxspeed;
                    flag_state1 = 0;
                    break;
                default:
                    break;
            }
//            if(flag_chong==1)
//            {
            if(press1<(P_high-0.2))
            {
                motor_run();
            }
            else if(press1>(P_high+0.2))
            {
                motor_stop();
            }
//            }

        }

//        // ��֫��ѭ������
//        if(flag_circu == 1)
//        {
//            if(state == 1)
//            {
//                if(arg>=16)
//                {
//                    state = 2;
//                    counter_circu++;
//                }
//            }
//            if(state == 2)
//            {
//                if(arg<=-5)
//                {
//                    state = 1;
//                }
//            }
//            switch(state)
//            {
//                case 1:
//                    PULL();
//                    break;
//                case 2:
//                    PUSH();
//                    break;
//
//                default:
//                    break;
//            }
//            if(press1>12 || press2>12 || press3>12 || counter_circu>6)
//            {
//
//                motor_stop();
//                UNLOAD();
//                DELAY_US(1000);
//                MIDDLE();
//                flag_circu = 0;
//            }
//            else
//            {
//                motor_run();
//
//            }
//        }

    }
}
