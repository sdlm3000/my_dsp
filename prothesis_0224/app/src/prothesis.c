/*
 * prothesis.c
 *
 *  Created on: Feb 19, 2022
 *      Author: as
 */


#include "prothesis.h"
#include "pwm.h"
#include "common.h"

extern int valve_pwm_middle_zhi;
extern int valve_pwm_middle_bei;
extern int valve_pwm_push;
extern int state_motor;
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
void PUSH(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = maxspeed;
    EPwm4Regs.CMPA.half.CMPA = maxspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//����̬
void MIDDLE(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

// 1��4������ע������û�иı�

//йѹ
void UNLOAD(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = maxspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = maxspeed;
}

//��������
void MIDDLE_bei(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=minspeed;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = valve_pwm_middle_bei;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//��������
void MIDDLE_zhi(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=minspeed;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//����
void PUSH_control(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = valve_pwm_push;
    EPwm4Regs.CMPA.half.CMPA = maxspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}


//����
void PULL(void)
{
    EPwm2Regs.CMPA.half.CMPA = maxspeed;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = maxspeed;
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


