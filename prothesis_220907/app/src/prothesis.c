/*
 * prothesis.c
 *
 *  Created on: Feb 19, 2022
 *      Author: as
 */


#include "prothesis.h"
#include "pwm.h"
#include "common.h"

/********* 人体步态周期阶段变量 *********/
int proState = MIDDLE_ZHI_STATE;    // 状态：0-卸荷态，1-被动跖屈, 2-被动背屈，3-主动跖屈，4-主动背屈

/*********  假肢控制输出量 *********/
int motorState = 0;            // 用于表示电机的运行状态，0 -> 运行,1 -> 停止
int valve_pwm_middle_zhi = 1000;   // 被动状态下，用于控制阀的PWM值
int valve_pwm_middle_bei = 1000;   // 被动状态下，用于控制阀的PWM值
int valve_pwm_push = 1000;         // 蹬腿状态下，用于控制阀的PWM值
int middle_zhi_controlFlag = 0; // MIDDLE_ZHI_STATE开始调整PWM的标志位   

/*********  蓝牙可配置的变量 *********/
float ANGLE_PULL_MIDDLE = 2.0;
float ANGLE_ZHI_BEI = -3.0;
float ANGLE_MIDDLE_PUSH = 7.0;
float ANGLE_PUAH_PULL = -12.0;   

float P_MAX = 15.0;         // 高压区油液压力的最大值
float P_HIGH = 12.0;        // 假肢蓄能最大的标志位
int CONTROL_FLAG = 0;       // 假肢控制输出的标志位

int MIN_VALVE_2 = 1999;
int MAX_VALVE_3 = 1600;
int MAX_VALVE_4 = 300;

/********* 假肢的传感器参数 *********/
float a[3], w[3], Angle[3];                         // 陀螺仪数据获取
double P_foot1 = 0.0, P_foot2 = 0.0, P_foot3 = 0.0; // 足底压力
double press1 = 0, press2 = 0, press3 = 0;          // 液压压力传感器的压力值，单位 MPa
double angle = 0.0;                                  // 踝关节角度值

/********* 假肢的控制率相关参数 *********/
double Beta = 0.0, Beta_last = 0.0;     // 踝关节角度相关变量，角度制
double W_BD = 0.0;                      // 杆BD的转速，即踝关节角速度
double Alpha, Alpha_cos, Alpha_sin;     // 连杆与活塞杆的夹角相关变量
double F_A = 0.0;                       // 活塞杆输出力
double T = 0.0;                         // 踝关节输出力矩的期望值
double delta_P = 0.0;                   // 理想压差，P2-P3
double delta_P0 = 0.0;                  // 实际压差
double dd_P = 0.0;                      // 实际压差与理想压差的差值，delta_P0 - delta_P



/*********************** 电磁阀相关函数 ********************/
//电磁阀部分的初始化
void valveinit(void)
{
    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioDataRegs.GPASET.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO2 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;
    GpioDataRegs.GPASET.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO4 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;
    GpioDataRegs.GPASET.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO6 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;
    GpioDataRegs.GPASET.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO8 = 0;

    GpioCtrlRegs.GPAQSEL1.all = 0x0000;
    EDIS;
}

//收腿
void PULL(void)
{
    EPwm2Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MAX_SPEED;
}

//蹬腿
void PUSH(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}
// 1和4互换，注意上面没有改变

//泄压
void UNLOAD(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MAX_SPEED;
}

//被动态
void MIDDLE(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}

//被动跖屈
void MIDDLE_bei(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=MIN_SPEED;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = valve_pwm_middle_bei;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}

//被动背屈
void MIDDLE_zhi(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=MIN_SPEED;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}

//蹬腿
void PUSH_control(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = valve_pwm_push;
    EPwm4Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}


/********************电机相关函数****************/
void motor_run(void)
{
        EPwm1Regs.CMPA.half.CMPA = EPWM1_MAX_CMPA;
        motorState = 1;
}

void motor_stop(void)
{
        EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;
        motorState = 0;
}


