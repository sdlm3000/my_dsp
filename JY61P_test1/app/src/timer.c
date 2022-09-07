/*
 * timer.c
 *
 *  Created on: 2022��1��25��
 *      Author: sdlm
 */

#include "timer.h"
#include "leds.h"
#include "uart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#include "imu901.h"

#include "jy61p.h"
#include "i2c.h"

short gyroxa, gyroya, gyroza;    //������ԭʼ����
float pitcha, rolla, yawa;
int num1 = 0;
int num2 = 0;

//unsigned char chrTemp[30];
////    unsigned char str[100];
float a[3],w[3],h[3],Angle[3];


//��ʱ��0��ʼ������
//Freq��CPUʱ��Ƶ�ʣ�150MHz��
//Period����ʱ����ֵ����λus
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //���ö�ʱ��0���ж���ڵ�ַΪ�ж��������INT0
    EALLOW;
    PieVectTable.TINT0 = &TIM0_IRQn;
    EDIS;

    //ָ��ʱ��0�ļĴ�����ַ
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    //���ö�ʱ��0�����ڼĴ���ֵ
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    //���ö�ʱ��Ԥ���������ֵΪ0
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    //ȷ����ʱ��0Ϊֹͣ״̬
    CpuTimer0Regs.TCR.bit.TSS = 1;
    //����ʹ��
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    //��ʼ��ʱ������
    CpuTimer0Regs.TCR.bit.TSS=0;
    //����CPU��һ���жϲ�ʹ�ܵ�һ���жϵĵ�7��С�жϣ�����ʱ��0
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
//    //ʹ�����ж�
//    EINT;
//    ERTM;

}

void TIM1_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
    EDIS;

    EALLOW;
    PieVectTable.XINT13 = &TIM1_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    // Initialize timer period to maximum:
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer1Regs.TPR.all  = 0;
    CpuTimer1Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer1Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer1Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer1.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer1, Freq, Period);

    CpuTimer1Regs.TCR.bit.TSS=0;

    IER |= M_INT13;

}

//void ShortToChar(short sData,unsigned char cData[])
//{
//    cData[0]=sData&0xff;
//    cData[1]=sData>>8;
//}
//short CharToShort(unsigned char cData[])
//{
//    return ((short)cData[1]<<8)|cData[0];
//}
int counter = 0;
interrupt void TIM0_IRQn(void)
{
    counter++;
//    mpu6050_send_data((int)(rolla * 100), (int)(pitcha * 100), (int)(yawa * 100),
//                      (int)(attitude.roll * 100), (int)(attitude.pitch * 100), (int)(attitude.yaw * 100));//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
//    I2C_ReadData(0x50, &chrTemp[0], AX, 24);
//    a[0] = (float)CharToShort(&chrTemp[0])/32768*16;
//    a[1] = (float)CharToShort(&chrTemp[2])/32768*16;
//    a[2] = (float)CharToShort(&chrTemp[4])/32768*16;
//    w[0] = (float)CharToShort(&chrTemp[6])/32768*2000;
//    w[1] = (float)CharToShort(&chrTemp[8])/32768*2000;
//    w[2] = (float)CharToShort(&chrTemp[10])/32768*2000;
//
//    Angle[0] = (float)CharToShort(&chrTemp[18])/32768*180;
//    Angle[1] = (float)CharToShort(&chrTemp[20])/32768*180;
//    Angle[2] = (float)CharToShort(&chrTemp[22])/32768*180;
    Uint8 flag = 0x4 + 0x2 +0x1;
    if(counter == 1)
    {
        scia_msg("1 ");
        jy61p_GetData(flag, 0x50, a, w, Angle);
        jy61p_SendData(flag, a, w, Angle);
    }
    else if(counter == 2)
    {
        scia_msg("2 ");
        jy61p_GetData(flag, 0x51, a, w, Angle);
        jy61p_SendData(flag, a, w, Angle);
    }
    else if(counter == 3)
    {
        scia_msg("3 ");
        jy61p_GetData(flag, 0x52, a, w, Angle);
        jy61p_SendData(flag, a, w, Angle);
        counter = 0;
    }

//    num1++;
//    if(num1 % 100 == 0)
//    {
//        LED13_TOGGLE;
//        num1 = 0;
//    }
    EALLOW;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}



interrupt void TIM1_IRQn(void)
{
    EALLOW;
    num2++;
    if(num2 % 100 == 0)
    {
        LED12_TOGGLE;
        num2 = 0;
    }

    EDIS;
    if(mpu_dmp_get_data(&pitcha, &rolla, &yawa) == 0)
    {
        MPU_Get_Gyroscope(&gyroxa, &gyroya, &gyroza);    //�õ�����������
//        mpu6050_send_data((int)(rolla * 100), (int)(pitcha * 100), (int)(yawa * 10), gyroxa, gyroya , gyroza);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
    }

}



void TIM2_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2
    EDIS;

    EALLOW;
    PieVectTable.TINT2 = &TIM2_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    // Initialize timer period to maximum:
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer2.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer2, Freq, Period);

    CpuTimer2Regs.TCR.bit.TSS=0;

    IER |= M_INT14;

}

interrupt void TIM2_IRQn(void)
{
    EALLOW;
    LED11_TOGGLE;
    EDIS;

}




