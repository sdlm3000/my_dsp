/*
 * main.c
 *
 *  Created on: 2022��10��12��
 *      Author: sdlm
 * 2022.10.12  ���ڼ�֫�Ĳ���ʵ�飬��Ҫʹ����������Ԥ���㷨������Ӧ³������
 */
#include "common.h"
#include "uart.h"
// #include "prothesis.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "bluetooth.h"
#include "i2c.h"
#include "anglePred.h"
#include "math.h"
#include "stdio.h"


extern int point_num, predict_flag, flag;
extern float pelvic_acc_z[N], t[N];
extern float rshank_euler_y[N], rthigh_euler_y[N];
extern float pow_x[N][3];

static float rsh_cosang[N], rth_cosang[N];          // ����Ҳ��С�ȽǶ���ر���
static float Coef[3];                       // ��Ŷ���ʽ��ϵ�ϵ��

float dt = 0.02;                            // IMU�Ĳ��������Ĭ��Ϊ0.02s����50Hz
float velocity[N], distance[N];             // ��ʾ���Ľ��ٶȻ��ֵõ����ٶȡ�λ��
float velo_pred[N], angle_pred[N];           // ��ʾԤ��õ����׹ؽڽǶȺͽ��ٶ�   
int walk_cycle = 0;

static Uint8 buf[32] = {0};
static Uint16 buf_len = 0;

int main(void)
{
    Uint8 ch;
    int i, j;

    float sum = 0.0, mean = 0.0;
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

    UARTb_Init(1152000);
    I2CA_Init();
    PWM_Init();
    ADC_Init();


    TIM0_Init(150, 10000);   // 10ms�������������������ݲɼ�
    TIM1_Init(150, 20000);   // 20ms�����ڲɼ�IMU����
    TIM2_Init(150, 1000);    // 1ms�����ڼ�֫״̬���ж�

    // ��ʼж��
    UNLOAD();
    delay_ms(1000);
    MIDDLE();

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

    // ��ʼ��һЩ�������
    for(i = 0; i < N; i++) {
        if(i > 0) t[i] = t[i - 1] + dt;
        for(j = 2; j < 5; j++)
        {
            pow_x[i][j - 2] = pow(t[i], j);
        }
    }
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
        // �Ƕ�Ԥ��
        if(predict_flag == 1) {     // �Խ�ȫ��������һ����̬���ڽ��н���
            sum = 0.0;
            for(i = 0; i < point_num; i++) {
                // ��С�Ⱥʹ��ȽǶ������ݴ棬��Ԥ����
                rth_cosang[i] = L_1 * cos(rthigh_euler_y[i]);
                rsh_cosang[i] = L_2 * cos(rshank_euler_y[i]);
                if(i > 0) {
                    // ���ٶȻ��ֵõ��ٶ�
                    velocity[i] = velocity[i - 1] + (pelvic_acc_z[i - 1] + pelvic_acc_z[i]) * dt / 2;
                    sum += velocity[i];
                }
            }
            predict_flag = 0;
            mean = sum / point_num;
            sum = 0.0;
            for(i = 0; i < point_num; i++) {
                velocity[i] -= mean;
                if(i > 0) {
                    // �ٶȻ��ֵõ�λ��
                    distance[i] = distance[i - 1] + (velocity[i - 1] + velocity[i]) * dt / 2;
                    sum += distance[i];
                }
            }
            for(i = 0; i < point_num; i++) {
                distance[i] -= mean;
            }
            // ���������
            getCoeff(Coef, t, distance, point_num, 2);
            buf_len = sprintf(buf, "%.6f %.6f %.6f\n", Coef[0], Coef[1], Coef[2]);
            usartb_sendData(buf, buf_len);
            // �׹ؽڽǶ�Ԥ��
            for(i = 0; i < point_num; i++) {
                float a;
                distance[i] = distance[i] - Coef[0] - Coef[1] * t[i] - Coef[2] * t[i] * t[i];   // ���Ĺ켣
//                // ȥ��ͻ��ֵ
//                // a���ڴ���ϴε�����λ�ƣ���Ҫ�����һ��Ϊ 0 �����
//                if(i == 0) a = 0;
//                else a = last_dist;
//                if(abs(distance[i] - a) > 0.5) distance[i] = last_dist;
//                last_dist = distance[i];
                a = (distance[i] - rth_cosang[i] - rsh_cosang[i]) / L_3 + 2.0 * PI;
                a = a > 0 ? a : -a;
                angle_pred[i] = 180 - (BETA_1 + BETA_2 - a) / DEG2RAD - 92.67;
                // Ԥ����׹ؽڽ��ٶȣ�a���ڴ���ϴεĽǶȣ���Ҫ�����һ��Ϊ 0 �����
                if(i == 0) a = 0;
                else a = angle_pred[i - 1];
                velo_pred[i] = (angle_pred[i] - a) / dt;
                if(i % 2 == 0) {
                    scib_xmit('P');
                    scib_int(walk_cycle);
                    scib_xmit(' ');
                    scib_float(angle_pred[i]);
                    scib_msg("\r\n");
                }
            }
            walk_cycle++;
        }
    }
}
