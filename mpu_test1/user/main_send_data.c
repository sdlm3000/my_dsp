/*
 * main_send_data.c
 *
 *  Created on: 2022年1月24日
 *      Author: Administrator
 */

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "leds.h"
#include "uart.h"
#include "i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


int T;
short ax, ay, az, gx, gy, gz;
float pitch, roll, yaw;
Uint8 res = 1;


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

void  main()
{
    Uint8 t=0, report=1;            //默认开启上报
    float pitch, roll, yaw;       //欧拉角
    short aacx ,aacy, aacz;       //加速度传感器原始数据
    short gyrox, gyroy, gyroz;    //陀螺仪原始数据
    short temp;                 //温度
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // This function must reside in RAM
    InitFlash();
    InitSysCtrl();

    UARTa_Init(115200);

    I2CA_Init();
    LED_Init();
    delay_ms(1000);
    uart_printf("start\r\n");
    while(res)
    {
        uart_printf("mpu6050 error\r\n");
        delay_ms(200);
        res = mpu_dmp_init();
        uart_printf("mpu_dmp_init: %d\r\n", res);
    }
    while(1)
    {
        LED8_TOGGLE;
//        DELAY_US(1000 * 5);
        if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
        {
            temp=MPU_Get_Temperature(); //得到温度值
//            MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据
            MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据
            if(report)mpu6050_send_data((int)(roll*100),(int)(pitch*100),(int)(yaw*10), gyrox,gyroy,gyroz);//用自定义帧发送加速度和陀螺仪原始数据
//            if(report)usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
        }
        t++;
    }

}


