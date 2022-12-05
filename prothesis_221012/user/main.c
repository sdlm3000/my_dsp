/*
 * main.c
 *
 *  Created on: 2022年10月12日
 *      Author: sdlm
 * 2022.10.12  用于假肢的步行实验，主要使用人体质心预测算法和自适应鲁棒控制
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

static float rsh_cosang[N], rth_cosang[N];          // 存放右侧大小腿角度相关变量
static float Coef[3];                       // 存放二项式拟合的系数

float dt = 0.02;                            // IMU的采样间隔，默认为0.02s，即50Hz
float velocity[N], distance[N];             // 表示重心角速度积分得到的速度、位移
float velo_pred[N], angle_pred[N];           // 表示预测得到的踝关节角度和角速度   
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

    //完成DSP的初始化
    // 步骤 1. 初始化系统控制:
    InitSysCtrl();

    // 步骤3. 清除所有中断,初始化中断向量表
    DINT;
    // 初始化PIE控制寄存器到他们的默认状态.
    InitPieCtrl();
    // 禁止CPU中断和清除所有CPU中断标志
    IER = 0x0000;
    IFR = 0x0000;
    //初始化PIE中断向量表，并使其指向中断服务子程序（ISR）
    InitPieVectTable();

    UARTb_Init(1152000);
    I2CA_Init();
    PWM_Init();
    ADC_Init();


    TIM0_Init(150, 10000);   // 10ms，用于其他传感器数据采集
    TIM1_Init(150, 20000);   // 20ms，用于采集IMU数据
    TIM2_Init(150, 1000);    // 1ms，用于假肢状态的判断

    // 起始卸荷
    UNLOAD();
    delay_ms(1000);
    MIDDLE();

    EINT;          // Enable Global interrupt INTM
    ERTM;          // Enable Global realtime interrupt DBGM

    // 初始化一些运算变量
    for(i = 0; i < N; i++) {
        if(i > 0) t[i] = t[i - 1] + dt;
        for(j = 2; j < 5; j++)
        {
            pow_x[i][j - 2] = pow(t[i], j);
        }
    }
    while(1)
    {
        // 用于接收蓝牙数据，并解析数据
        if (bt_uart_receive(&ch, 1))    /*!< 获取串口fifo一个字节 */
        {
            if (bt_unpack(ch))          /*!< 解析出有效数据包 */
            {
                btParsing(&rx_Packet);
            }
        }
        // 角度预测
        if(predict_flag == 1) {     // 对健全腿完整的一个步态周期进行解算
            sum = 0.0;
            for(i = 0; i < point_num; i++) {
                // 将小腿和大腿角度数据暂存，并预处理
                rth_cosang[i] = L_1 * cos(rthigh_euler_y[i]);
                rsh_cosang[i] = L_2 * cos(rshank_euler_y[i]);
                if(i > 0) {
                    // 加速度积分得到速度
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
                    // 速度积分得到位移
                    distance[i] = distance[i - 1] + (velocity[i - 1] + velocity[i]) * dt / 2;
                    sum += distance[i];
                }
            }
            for(i = 0; i < point_num; i++) {
                distance[i] -= mean;
            }
            // 二次项拟合
            getCoeff(Coef, t, distance, point_num, 2);
            buf_len = sprintf(buf, "%.6f %.6f %.6f\n", Coef[0], Coef[1], Coef[2]);
            usartb_sendData(buf, buf_len);
            // 踝关节角度预测
            for(i = 0; i < point_num; i++) {
                float a;
                distance[i] = distance[i] - Coef[0] - Coef[1] * t[i] - Coef[2] * t[i] * t[i];   // 质心轨迹
//                // 去除突变值
//                // a用于存放上次的质心位移，主要解决第一次为 0 的情况
//                if(i == 0) a = 0;
//                else a = last_dist;
//                if(abs(distance[i] - a) > 0.5) distance[i] = last_dist;
//                last_dist = distance[i];
                a = (distance[i] - rth_cosang[i] - rsh_cosang[i]) / L_3 + 2.0 * PI;
                a = a > 0 ? a : -a;
                angle_pred[i] = 180 - (BETA_1 + BETA_2 - a) / DEG2RAD - 92.67;
                // 预测的踝关节角速度，a用于存放上次的角度，主要解决第一次为 0 的情况
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
