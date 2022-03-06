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
#include "multi_mpu.h"

//short gyrox, gyroy, gyroz;    //������ԭʼ����
//float pitch, roll, yaw;

float pitch[5], roll[5], yaw[5];
short gyrox[5], gyroy[5], gyroz[5];    //������ԭʼ����
short mpu_flag = 0;
int counter = 0;

static Uint16 sum1 = 0, sum2 = 0,sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
static Uint16 SampleTable1[10];
static Uint16 SampleTable2[BUF_SIZE];
static Uint16 SampleTable3[BUF_SIZE];
static Uint16 SampleTable4[BUF_SIZE];
static Uint16 SampleTable5[BUF_SIZE];
static Uint16 SampleTable6[BUF_SIZE];
static Uint16 SampleTable7[BUF_SIZE];


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
    //ʹ�����ж�
    EINT;
    ERTM;

}

// Transmit a character from the SCI'
void usart1_send_char(Uint8 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0);
    SciaRegs.SCITXBUF=a;
}

//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(Uint8 fun,Uint8*data,Uint8 len)
{
    Uint8 send_buf[32];
    Uint8 i;
    if(len>28)return;   //���28�ֽ�����
    send_buf[len+3]=0;  //У��������
    send_buf[0]=0X88;   //֡ͷ
    send_buf[1]=fun;    //������
    send_buf[2]=len;    //���ݳ���
    for(i=0;i<len;i++)send_buf[3+i]=data[i];            //��������
    for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];   //����У���
    for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);  //�������ݵ�����1
}
//���ͼ��ٶȴ��������ݺ�����������
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
    Uint8 tbuf[12];
    tbuf[0]=(aacx>>8)&0XFF;
    tbuf[1]=aacx&0XFF;
    tbuf[2]=(aacy>>8)&0XFF;
    tbuf[3]=aacy&0XFF;
    tbuf[4]=(aacz>>8)&0XFF;
    tbuf[5]=aacz&0XFF;
    tbuf[6]=(gyrox>>8)&0XFF;
    tbuf[7]=gyrox&0XFF;
    tbuf[8]=(gyroy>>8)&0XFF;
    tbuf[9]=gyroy&0XFF;
    tbuf[10]=(gyroz>>8)&0XFF;
    tbuf[11]=gyroz&0XFF;
    usart1_niming_report(0XA1,tbuf,12);//�Զ���֡,0XA1
}
//ͨ������1�ϱ���������̬���ݸ�����
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
    Uint8 tbuf[28];
    Uint8 i;
    for(i=0;i<28;i++)tbuf[i]=0;//��0
    tbuf[0]=(aacx>>8)&0XFF;
    tbuf[1]=aacx&0XFF;
    tbuf[2]=(aacy>>8)&0XFF;
    tbuf[3]=aacy&0XFF;
    tbuf[4]=(aacz>>8)&0XFF;
    tbuf[5]=aacz&0XFF;
    tbuf[6]=(gyrox>>8)&0XFF;
    tbuf[7]=gyrox&0XFF;
    tbuf[8]=(gyroy>>8)&0XFF;
    tbuf[9]=gyroy&0XFF;
    tbuf[10]=(gyroz>>8)&0XFF;
    tbuf[11]=gyroz&0XFF;
    tbuf[18]=(roll>>8)&0XFF;
    tbuf[19]=roll&0XFF;
    tbuf[20]=(pitch>>8)&0XFF;
    tbuf[21]=pitch&0XFF;
    tbuf[22]=(yaw>>8)&0XFF;
    tbuf[23]=yaw&0XFF;
    usart1_niming_report(0XAF,tbuf,28);//�ɿ���ʾ֡,0XAF
}

interrupt void TIM0_IRQn(void)
{
    int index;

    EALLOW;
    PieCtrlRegs.PIEACK.bit.ACK1=1;
    counter++;
    if(counter == 1){
//        mpu_select(1);
        MPU5_OFF;
        MPU1_ON;
//        uart_printf("-data5\r\n");
//        uart_printf("-\r\n");
    }
    else if(counter == 2){
//        mpu_select(2);
        MPU2_ON;
        MPU1_OFF;
//        uart_printf("-\r\n");
    }
    else if(counter == 3){
//        mpu_select(3);
        MPU2_OFF;
        MPU3_ON;
//        uart_printf("-\r\n");
    }
    else if(counter == 4){
//        mpu_select(4);
        MPU3_OFF;
        MPU4_ON;
//        uart_printf("-\r\n");
    }
    else if(counter == 5){
//        mpu_select(5);
        MPU4_OFF;
        MPU5_ON;
//        uart_printf("-\r\n");
        counter = 0;
        mpu_flag = 0;
    }
    index = counter - 1;
    if(counter <= 3 && counter > 0)
    {
        if(mpu_dmp_get_data(&pitch[index], &roll[index], &yaw[index], &gyrox[index], &gyroy[index], &gyroz[index])==0)
        {
//            uart_printf("%.2f", roll);
//            uart_printf("0");
            mpu_flag = mpu_flag & (0x1 << counter);
        }
    }



    EDIS;
}

//interrupt void TIM0_IRQn(void)
//{
//    EALLOW;
//    PieCtrlRegs.PIEACK.bit.ACK1=1;
//    counter++;
//    LED9_TOGGLE;
//    LED8_TOGGLE;
//    LED9_TOGGLE;
//    LED8_TOGGLE;
//    LED9_TOGGLE;
//    uart_printf("data: ");
////    if(counter == 5)
////    {
////        if(mpu_dmp_get_data(&pitch, &roll, &yaw)==0)
////        {
////    //            MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //�õ����ٶȴ���������
////    //        MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //�õ�����������
////    //        uart_printf("data:fdfsafds\r\n");
////    //        uart_printf("data: %.2f, %.2f, %.2f, %d, %d, %d\r\n", roll, pitch, yaw, gyrox, gyroy, gyroz);
////            uart_printf("%.2f, %.2f, %.2f", roll, pitch, yaw);
////    //        mpu6050_send_data((int)(roll*100),(int)(pitch*100),(int)(yaw*10), gyrox,gyroy,gyroz);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
////    //            if(report)usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
////        }
////        counter = 0;
////    }
//
//    uart_printf("\r\n");
//    EDIS;
//}

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
    EINT;
    ERTM;

}

interrupt void TIM1_IRQn(void)
{
    int i;
    float test1;
    EALLOW;
    LED9_TOGGLE;
//    uart_printf("%0.2f %0.2f %0.2f %d %d %d %0.2f %0.2f %0.2f %d %d %d %0.2f %0.2f %0.2f %d %d %d\r\n",
//                roll[0], pitch[0],yaw[0], gyrox[0], gyroy[0], gyroz[0],
//                roll[1], pitch[1],yaw[1], gyrox[1], gyroy[1], gyroz[1],
//                roll[2], pitch[2],yaw[2], gyrox[2], gyroy[2], gyroz[2]);
//    uart_printf("%0.2f %0.2f %0.2f %d %d %d\r\n", roll[0], pitch[0],yaw[0], gyrox[0], gyroy[0], gyroz[0]);

    AdcRegs.ADCTRL2.all = 0x2000;   // �����SEQ1λд1����ת������
    for (i = 0;i < 10;i++) // AVG=10
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
    sum1 *= 0.1;
    sum2 *= 0.1;
    sum3 *= 0.1;
    sum4 *= 0.1;
    sum5 *= 0.1;
    sum6 *= 0.1;
    sum7 *= 0.1;
    test1 = (counter + 1) * 0.1 * 0.5454 * 21.24 * 5.023 * 564.22 * 546.2 / 654.1 * 153.3 * 56.3 * 5.31 * 65.1 / 65.31 * 5.126 / 56.6 * 56.3 /2.1;
    test1 = test1 * 65.3 / 3.21 / 5 * (counter + 1);
    test1 = 30.32;
//    uart_printf("%d %.2f\r\n", sum1, test1);
    scia_float(roll[0]);
    scia_msg(" ");
    scia_float(roll[1]);
    scia_msg(" ");
    scia_float(roll[2]);
    scia_msg(" ");
    scia_float(test1);
    scia_msg(" ");
    scia_int(sum1);
    scia_msg(" ");
    scia_int(sum2);
    scia_msg(" ");
    scia_int(sum3);
    scia_msg(" ");
    scia_int(sum4);
    scia_msg(" ");
    scia_int(sum5);
    scia_msg(" ");
    scia_int(sum6);
    scia_msg(" ");
    scia_int(sum7);
    scia_msg("\r\n");
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    sum5 = 0;
    sum6 = 0;
    sum7 = 0;
    EDIS;
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

    EINT;
    ERTM;

}

interrupt void TIM2_IRQn(void)
{
    EALLOW;
//    LED11_TOGGLE;

    EDIS;
}




