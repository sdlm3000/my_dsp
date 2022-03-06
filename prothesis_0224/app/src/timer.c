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
#include "prothesis.h"

short gyroxa, gyroya, gyroza;    //������ԭʼ����
float pitcha, rolla, yawa;
int num1 = 0;
int num2 = 0;

static double volt1 = 0, volt2 = 0, volt3 = 0, volt4 = 0, volt5 = 0, volt6 = 0, volt7 = 0;
static Uint32 sum1 = 0, sum2 = 0,sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
static Uint16 SampleTable1[BUF_SIZE];
static Uint16 SampleTable2[BUF_SIZE];
static Uint16 SampleTable3[BUF_SIZE];
static Uint16 SampleTable4[BUF_SIZE];
static Uint16 SampleTable5[BUF_SIZE];
static Uint16 SampleTable6[BUF_SIZE];
static Uint16 SampleTable7[BUF_SIZE];

static double arg_ave[5] = {0.0};

double press1 = 0, press2 = 0, press3 = 0;
double arg = 0.0;

int counter_P = 0;

extern int pwm1, pwm2;


//��ʱ��0��ʼ������
//Freq��CPUʱ��Ƶ�ʣ�150MHz��
//Period����ʱ����ֵ����λus
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //���ö�ʱ��0���ж���ڵ�ַΪ�ж���������INT0
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

interrupt void TIM0_IRQn(void)
{
    int i;
    double temp,temp2;
    CpuTimer0.InterruptCount++;
    AdcRegs.ADCTRL2.all = 0x2000;   // ������SEQ1λд1����ת������
    for (i = 0;i < AVG;i++) // AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {}  // Wait for interrupt��η����ж�
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable1[i] = (AdcRegs.ADCRESULT0 >> 4);
        SampleTable2[i] = (AdcRegs.ADCRESULT1 >> 4);
        SampleTable3[i] = (AdcRegs.ADCRESULT2 >> 4);
        SampleTable4[i] = (AdcRegs.ADCRESULT3 >> 4);
        SampleTable5[i] = (AdcRegs.ADCRESULT4 >> 4);
        SampleTable6[i] = (AdcRegs.ADCRESULT5 >> 4);
        SampleTable7[i] = (AdcRegs.ADCRESULT6 >> 4);
        sum1 = sum1 + SampleTable1[i];
        sum2 = sum2 + SampleTable2[i];
        sum3 = sum3 + SampleTable3[i];
        sum4 = sum4 + SampleTable4[i];
        sum5 = sum5 + SampleTable5[i];
        sum6 = sum6 + SampleTable6[i];
        sum7 = sum7 + SampleTable7[i];
    }
    press1 = ((((double)sum1) * 3 / 4096 / AVG) * GAIN1 - 4) * 1.25;
    press2 = ((((double)sum2) * 3 / 4096 / AVG) * GAIN2 - 4) * 1.25;
    press3 = ((((double)sum3) * 3 / 4096 / AVG) * GAIN3 - 4) * 1.25;

    if(press1 >= P_max || press1 <= 0)
    {
        counter_P++;
        if(counter_P >= 15)
        {
            motor_stop();
            UNLOAD();
            counter_P = 0;
        }
    }
    arg = ((((double)sum4) * 3 / 4096 / AVG) * 100);//������ʵ�ʵ�ѹ 0-3v
    arg = 66.5 - arg;     // 70Ϊ��λ�ĽǶȴ�����ֵ
    temp2 = 0;
    for(i = 0; i < 4; i++)
    {
        arg_ave[i] = arg_ave[i+1];
        temp2 = temp2 + arg_ave[i];
    }
    arg_ave[4] = arg;
    arg = (temp2 + arg_ave[4]) / 5; // 5�㻬��ƽ��
    if(CpuTimer0.InterruptCount % 2 == 0)
    {
        uart_printf("%d %d %.2f %.2f %.2f %.2f\r\n", pwm1, pwm2, arg, press1, press2, press3);
    }
    volt5 = ((double)sum5) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v
    volt6 = ((double)sum6) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v
    volt7 = ((double)sum7) * 3 / 4096 / AVG; //������ʵ�ʵ�ѹ 0-3v

    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    sum5 = 0;
    sum6 = 0;
    sum7 = 0;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
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



