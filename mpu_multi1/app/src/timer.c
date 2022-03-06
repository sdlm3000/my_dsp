/*
 * timer.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "timer.h"
#include "leds.h"
#include "uart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "multi_mpu.h"

//short gyrox, gyroy, gyroz;    //陀螺仪原始数据
//float pitch, roll, yaw;

float pitch[5], roll[5], yaw[5];
short gyrox[5], gyroy[5], gyroz[5];    //陀螺仪原始数据
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


//定时器0初始化函数
//Freq：CPU时钟频率（150MHz）
//Period：定时周期值，单位us
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //设置定时器0的中断入口地址为中断向量表的INT0
    EALLOW;
    PieVectTable.TINT0 = &TIM0_IRQn;
    EDIS;

    //指向定时器0的寄存器地址
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    //设置定时器0的周期寄存器值
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    //设置定时器预定标计数器值为0
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    //确保定时器0为停止状态
    CpuTimer0Regs.TCR.bit.TSS = 1;
    //重载使能
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    //开始定时器功能
    CpuTimer0Regs.TCR.bit.TSS=0;
    //开启CPU第一组中断并使能第一组中断的第7个小中断，即定时器0
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    //使能总中断
    EINT;
    ERTM;

}

// Transmit a character from the SCI'
void usart1_send_char(Uint8 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0);
    SciaRegs.SCITXBUF=a;
}

//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(Uint8 fun,Uint8*data,Uint8 len)
{
    Uint8 send_buf[32];
    Uint8 i;
    if(len>28)return;   //最多28字节数据
    send_buf[len+3]=0;  //校验数置零
    send_buf[0]=0X88;   //帧头
    send_buf[1]=fun;    //功能字
    send_buf[2]=len;    //数据长度
    for(i=0;i<len;i++)send_buf[3+i]=data[i];            //复制数据
    for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];   //计算校验和
    for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);  //发送数据到串口1
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
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
    usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}
//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
    Uint8 tbuf[28];
    Uint8 i;
    for(i=0;i<28;i++)tbuf[i]=0;//清0
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
    usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
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
////    //            MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据
////    //        MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据
////    //        uart_printf("data:fdfsafds\r\n");
////    //        uart_printf("data: %.2f, %.2f, %.2f, %d, %d, %d\r\n", roll, pitch, yaw, gyrox, gyroy, gyroz);
////            uart_printf("%.2f, %.2f, %.2f", roll, pitch, yaw);
////    //        mpu6050_send_data((int)(roll*100),(int)(pitch*100),(int)(yaw*10), gyrox,gyroy,gyroz);//用自定义帧发送加速度和陀螺仪原始数据
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

    AdcRegs.ADCTRL2.all = 0x2000;   // 软件向SEQ1位写1开启转换触发
    for (i = 0;i < 10;i++) // AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {}  // Wait for interrupt如何发起中断
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




