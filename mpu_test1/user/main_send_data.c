/*
 * main_send_data.c
 *
 *  Created on: 2022��1��24��
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

void  main()
{
    Uint8 t=0, report=1;            //Ĭ�Ͽ����ϱ�
    float pitch, roll, yaw;       //ŷ����
    short aacx ,aacy, aacz;       //���ٶȴ�����ԭʼ����
    short gyrox, gyroy, gyroz;    //������ԭʼ����
    short temp;                 //�¶�
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
            temp=MPU_Get_Temperature(); //�õ��¶�ֵ
//            MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //�õ����ٶȴ���������
            MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //�õ�����������
            if(report)mpu6050_send_data((int)(roll*100),(int)(pitch*100),(int)(yaw*10), gyrox,gyroy,gyroz);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
//            if(report)usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
        }
        t++;
    }

}


