/*
 * mpu6050.c
 *
 *  Created on: 2022��1��19��
 *      Author: sdlm
 */



#include "mpu6050.h"
#include "uart.h"
#include "common.h"

Uint8 MPU_Init()
{
    Uint8 res;
    Uint16 k = 1;
    MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X80);  // ��λMPU6050
    delay_ms(100);
	MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X00);  //����MPU6050
	MPU_Set_Gyro_Fsr(3);                                //�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);                               //���ٶȴ�����,��2g
	MPU_Set_Rate(50);                                   //���ò�����50Hz
	MPU_Write_Byte(MPU_ADDR, MPU_INT_EN_REG, 0X00);     //�ر������ж�
	MPU_Write_Byte(MPU_ADDR, MPU_USER_CTRL_REG, 0X00);  //I2C��ģʽ�ر�
	MPU_Write_Byte(MPU_ADDR, MPU_FIFO_EN_REG, 0X00);
	MPU_Write_Byte(MPU_ADDR, MPU_INTBP_CFG_REG, 0X80);
    res = MPU_Read_Byte(MPU_ADDR, MPU_DEVICE_ID_REG);
    if(res == MPU_ADDR)
    {
        delay_ms(k);
		MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X01);
		MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT2_REG, 0X00);
		MPU_Set_Rate(50);						
    }
    else return 1;
    return 0;
}

Uint8 MPU_Set_Gyro_Fsr(Uint8 fsr)
{
	return MPU_Write_Byte(MPU_ADDR, MPU_GYRO_CFG_REG, (fsr << 3) | 3); 
}

Uint8 MPU_Set_Accel_Fsr(Uint8 fsr)
{
	return MPU_Write_Byte(MPU_ADDR, MPU_ACCEL_CFG_REG, fsr << 3);
}

Uint8 MPU_Set_LPF(Uint16 lpf)
{
	Uint8 data = 0;
	if(lpf >= 188) data = 1;
	else if(lpf >= 98) data = 2;
	else if(lpf >= 42) data = 3;
	else if(lpf >= 20) data = 4;
	else if(lpf >= 10) data = 5;
	else data = 6; 
	return MPU_Write_Byte(MPU_ADDR, MPU_CFG_REG, data); 
}

Uint8 MPU_Set_Rate(Uint16 rate)
{
	Uint8 data;
	if(rate > 1000) rate = 1000;
	if(rate < 4) rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_ADDR, MPU_SAMPLE_RATE_REG, data);
	if(data) return 1;
 	return MPU_Set_LPF(rate / 2);
}

Uint8 MPU_Write_Byte(Uint8 addr, Uint8 reg, Uint8 data)
{
    Uint8 status = 0;
    status = I2C_WriteData(addr, (Uint8 *)(&data), reg, 1);
    if(status != 0) return 1;
    return 0;
}

Uint8 MPU_Read_Byte(Uint8 addr, Uint8 reg)
{
    Uint8 data;
    Uint8 status = 0;
    status = I2C_ReadData(addr, (Uint8 *)(&data), reg, 1);
    if(status != 0) return 1;
    return data;
}

Uint8 MPU_Write_Len(Uint8 addr, Uint8 reg, Uint8 len, Uint8 *buf)
{
    Uint8 status = 0;
    status = I2C_WriteData(addr, (Uint8 *)(buf), reg, len);
    if(status != 0) return 1;
    return 0;
}

Uint8 MPU_Read_Len(Uint8 addr, Uint8 reg, Uint8 len, Uint8 *buf)
{
    Uint8 status = 0;
    status = I2C_ReadData(addr, (Uint8 *)(buf), reg, len);
    if(status != 0) return 1;
    return 0;
}

// �õ��¶�ֵ
// ����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    Uint8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf); 
    raw=((Uint16)buf[0] << 8) | buf[1];  
    temp = 36.53 + ((double)raw) / 340;  
    return temp * 100;;
}

//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
Uint8 MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
    Uint8 buf[6], res;  
	res = MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if(res == 0)
	{
		*gx = ((Uint16)buf[0] << 8) | buf[1];  
		*gy = ((Uint16)buf[2] << 8) | buf[3];  
		*gz = ((Uint16)buf[4] << 8) | buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//ax,ay,az:���ٶȼ�x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
Uint8 MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
    Uint8 buf[6], res;  
	res = MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	if(res == 0)
	{
		*ax = ((Uint16)buf[0] << 8) | buf[1];  
		*ay = ((Uint16)buf[2] << 8) | buf[3];  
		*az = ((Uint16)buf[4] << 8) | buf[5];
	} 	
    return res;;
}


//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
static void usart1_niming_report(Uint8 fun,Uint8*data,Uint8 len)
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
void mpu6050_send_kalman(short angle_m,short angle)
{
    Uint8 tbuf[4];
    tbuf[0] = (angle_m >> 8) & 0XFF;
    tbuf[1] = angle_m & 0XFF;
    tbuf[2] = (angle >> 8) & 0XFF;
    tbuf[3] = angle & 0XFF;
    usart1_niming_report(0XA1,tbuf,4);//�Զ���֡,0XA1
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
