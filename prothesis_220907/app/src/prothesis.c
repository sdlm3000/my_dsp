/*
 * prothesis.c
 *
 *  Created on: Feb 19, 2022
 *      Author: as
 */


#include "prothesis.h"
#include "pwm.h"
#include "common.h"

/********* ���岽̬���ڽ׶α��� *********/
int proState = MIDDLE_ZHI_STATE;    // ״̬��0-ж��̬��1-��������, 2-����������3-����������4-��������

/*********  ��֫��������� *********/
int motorState = 0;            // ���ڱ�ʾ���������״̬��0 -> ����,1 -> ֹͣ
int valve_pwm_middle_zhi = 1000;   // ����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_middle_bei = 1000;   // ����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_push = 1000;         // ����״̬�£����ڿ��Ʒ���PWMֵ
int middle_zhi_controlFlag = 0; // MIDDLE_ZHI_STATE��ʼ����PWM�ı�־λ   

/*********  ���������õı��� *********/
float ANGLE_PULL_MIDDLE = 2.0;
float ANGLE_ZHI_BEI = -3.0;
float ANGLE_MIDDLE_PUSH = 7.0;
float ANGLE_PUAH_PULL = -12.0;   

float P_MAX = 15.0;         // ��ѹ����Һѹ�������ֵ
float P_HIGH = 12.0;        // ��֫�������ı�־λ
int CONTROL_FLAG = 0;       // ��֫��������ı�־λ

int MIN_VALVE_2 = 1999;
int MAX_VALVE_3 = 1600;
int MAX_VALVE_4 = 300;

/********* ��֫�Ĵ��������� *********/
float a[3], w[3], Angle[3];                         // ���������ݻ�ȡ
double P_foot1 = 0.0, P_foot2 = 0.0, P_foot3 = 0.0; // ���ѹ��
double press1 = 0, press2 = 0, press3 = 0;          // Һѹѹ����������ѹ��ֵ����λ MPa
double angle = 0.0;                                  // �׹ؽڽǶ�ֵ

/********* ��֫�Ŀ�������ز��� *********/
double Beta = 0.0, Beta_last = 0.0;     // �׹ؽڽǶ���ر������Ƕ���
double W_BD = 0.0;                      // ��BD��ת�٣����׹ؽڽ��ٶ�
double Alpha, Alpha_cos, Alpha_sin;     // ����������˵ļн���ر���
double F_A = 0.0;                       // �����������
double T = 0.0;                         // �׹ؽ�������ص�����ֵ
double delta_P = 0.0;                   // ����ѹ�P2-P3
double delta_P0 = 0.0;                  // ʵ��ѹ��
double dd_P = 0.0;                      // ʵ��ѹ��������ѹ��Ĳ�ֵ��delta_P0 - delta_P



/*********************** ��ŷ���غ��� ********************/
//��ŷ����ֵĳ�ʼ��
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

//����
void PULL(void)
{
    EPwm2Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MAX_SPEED;
}

//����
void PUSH(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}
// 1��4������ע������û�иı�

//йѹ
void UNLOAD(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MAX_SPEED;
}

//����̬
void MIDDLE(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm4Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}

//��������
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

//��������
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

//����
void PUSH_control(void)
{
    EPwm2Regs.CMPA.half.CMPA = MIN_SPEED;
    EPwm3Regs.CMPA.half.CMPA = valve_pwm_push;
    EPwm4Regs.CMPA.half.CMPA = MAX_SPEED;
    EPwm5Regs.CMPA.half.CMPA = MIN_SPEED;
}


/********************�����غ���****************/
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


