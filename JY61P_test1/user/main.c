/*
 * main.c
 *
 *  Created on: 2022年5月10日
 *      Author: sdlm
 *
 *  测试JY61P通过IIC读取数据的功能
 */

#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "jy61p.h"
#include "anglePred.h"
#include "math.h"
#include "stdio.h"

extern int point_num, predict_flag, flag;
extern float pelvic_acc_z[N], t[N];
extern float rshank_euler_y[N], rthigh_euler_y[N];
extern float matFunX[3][3], matFunY[3][1], pow_x[N][5];


float rsh_cosang[N], rth_cosang[N];
float velo[N] = {0.0};
float dist_ang[N] = {0.0};


float dt = 0.02;
float sum = 0.0, mean = 0.0;
float Coef[3];

//int tim0_counter = 20000;


void  main()
{
    Uint8 buf[32] = {};
    Uint16 buf_len = 0;
    int i, j;
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    UARTa_Init(576000);
    I2CA_Init();

    TIM0_Init(150, 20000);   // 150000000Hz， 150分频， 5000点一个周期， 200Hz
//    TIM1_Init(150, 10000);
    TIM2_Init(150, 20000);
    //使能总中断
    EINT;
    ERTM;

    // 初始化一些运算变量
    for(i = 0; i < N; i++) {
        if(i > 0) t[i] = t[i - 1] + dt;
        for(j = 0; j < 5; j++)
        {
            pow_x[i][j] = pow(t[i], j);
        }
    }


    while(1)
    {
        if(predict_flag == 1) {
            scia_msg("a");
            sum = 0.0;
            for(i = 0; i < point_num; i++) {
                rth_cosang[i] = L_1 * cos(rthigh_euler_y[i]);
                rsh_cosang[i] = L_2 * cos(rshank_euler_y[i]);
                if(i > 0){
                    velo[i] = velo[i - 1] + (pelvic_acc_z[i - 1] + pelvic_acc_z[i]) * dt / 2;
                    sum += velo[i];
                }
            }
            scia_msg("b");
            predict_flag = 0;

            mean = sum / point_num;
            sum = 0.0;
            scia_msg("c");
            for(i = 0; i < point_num; i++) {
                velo[i] -= mean;
                if(i > 0) {
                    dist_ang[i] = dist_ang[i - 1] + (velo[i - 1] + velo[i]) * dt / 2;
                    sum += dist_ang[i];
                }
            }
            for(i = 0; i < point_num; i++) {
                dist_ang[i] -= mean;
            }
            // 二次项拟合
            scia_msg("d ");
            getCoeff(Coef, t, dist_ang, point_num, 2);
            buf_len = sprintf(buf, "%.6f %.6f %.6f ", Coef[0], Coef[1], Coef[2]);
            usarta_sendData(buf, buf_len);
            // 踝关节角度预测
            for(i = 0; i < point_num; i++) {
//                scia_int(i);
                float a;
                dist_ang[i] = dist_ang[i] - Coef[0] - Coef[1] * t[i] - Coef[2] * t[i] * t[i];   // 质心轨迹
                float tmp = dist_ang[i];
                a = (dist_ang[i] - rth_cosang[i] - rsh_cosang[i]) / L_3 + 2.0 * PI;
                a = a > 0 ? a : -a;
                dist_ang[i] = 180 - (BETA_1 + BETA_2 - a) / DEG2RAD - 92.67;
                buf_len = sprintf(buf, "P %.4f %.2f\n", tmp, dist_ang[i]);
                usarta_sendData(buf, buf_len);
	        }
            scia_msg("e");
            
        }
    }
}
