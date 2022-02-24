/*
 * pwm.c
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */


#include "pwm.h"
#include "common.h"


void PWM_Init()
{
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    InitEPwm3Gpio();
    InitEPwm4Gpio();
    InitEPwm5Gpio();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;//Disable TBCLK within the ePWM
    EDIS;

    InitEPwm1Example();
    InitEPwm2Example();
    InitEPwm3Example();
    InitEPwm4Example();
    InitEPwm5Example();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;//Enable TBCLK within the ePWM
    EDIS;
}

/********************PWM******************/
//Epwm1
void InitEPwm1Example()
{
   // Setup TBCLK
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;           // Set timer period 801 TBCLKs
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;     // Set compare A value
   EPwm1Regs.CMPB = EPWM1_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM1A on event A, up count
   EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM1A on event A, down count

   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM1B on event B, up count
   EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM1B on event B, down count
}

//EPWM2
void InitEPwm2Example()
{
   // Setup TBCLK
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;           // Set timer period 802 TBCLKs
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm2Regs.CMPA.half.CMPA = EPWM2_MIN_CMPA;     // Set compare A value
   EPwm2Regs.CMPB = EPWM2_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM2A on event A, up count
   EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM2A on event A, down count

   EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM2B on event B, up count
   EPwm2Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM2B on event B, down count
}

//Epwm3
void InitEPwm3Example()
{
   // Setup TBCLK
   EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;           // Set timer period 803 TBCLKs
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm3Regs.CMPA.half.CMPA = EPWM3_MIN_CMPA;     // Set compare A value
   EPwm3Regs.CMPB = EPWM3_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM3A on event A, up count
   EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM3A on event A, down count

   EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM3B on event B, up count
   EPwm3Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM3B on event B, down count
}

//EPwm4
void InitEPwm4Example()
{
   // Setup TBCLK
   EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD;           // Set timer period 804 TBCLKs
   EPwm4Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm4Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm4Regs.CMPA.half.CMPA = EPWM4_MIN_CMPA;     // Set compare A value
   EPwm4Regs.CMPB = EPWM4_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x4;       // Clock ratio to SYSCLKOUT
   EPwm4Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM4A on event A, up count
   EPwm4Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM4A on event A, down count

   EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM4B on event B, up count
   EPwm4Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM4B on event B, down count
}

//EPwm5
void InitEPwm5Example()
{
   // Setup TBCLK
   EPwm5Regs.TBPRD = EPWM5_TIMER_TBPRD;           // Set timer period 805 TBCLKs
   EPwm5Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm5Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm5Regs.CMPA.half.CMPA = EPWM5_MIN_CMPA;     // Set compare A value
   EPwm5Regs.CMPB = EPWM5_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x5;       // Clock ratio to SYSCLKOUT
   EPwm5Regs.TBCTL.bit.CLKDIV = 0x7;//65分频

   // Setup shadowing
   EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM5A on event A, up count
   EPwm5Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM5A on event A, down count

   EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM5B on event B, up count
   EPwm5Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM5B on event B, down count
}

