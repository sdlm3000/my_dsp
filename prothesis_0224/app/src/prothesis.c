/*
 * prothesis.c
 *
 *  Created on: Feb 19, 2022
 *      Author: as
 */


#include "prothesis.h"
#include "pwm.h"
#include "common.h"


int state_motor = 0;        //���ڱ�ʾ���������״̬��0 -> ����,1 -> ֹͣ

int valve_pwm_middle_zhi = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_middle_bei = 0;   //����״̬�£����ڿ��Ʒ���PWMֵ
int valve_pwm_push = 0;     //����״̬�£����ڿ��Ʒ���PWMֵ

/***********************��ŷ���غ���********************/
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
        state_motor = 1;
}

void motor_stop(void)
{
        EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;
        state_motor = 0;
}


