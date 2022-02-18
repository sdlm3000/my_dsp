/*
 * mpu6050.c
 *
 *  Created on: 2022锟斤拷1锟斤拷19锟斤拷
 *      Author: sdlm
 */



#include "mpu6050.h"
#include "uart.h"
#include "common.h"

Uint8 MPU_Init()
{
    Uint8 res;
    Uint16 k = 1;
    MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X80);  // 复位MPU6050
    delay_ms(100);
	MPU_Write_Byte(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X00);  //唤醒MPU6050
	MPU_Set_Gyro_Fsr(3);                                //陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);                               //加速度传感器,±2g
	MPU_Set_Rate(50);                                   //设置采样率50Hz
	MPU_Write_Byte(MPU_ADDR, MPU_INT_EN_REG, 0X00);     //关闭所有中断
	MPU_Write_Byte(MPU_ADDR, MPU_USER_CTRL_REG, 0X00);  //I2C主模式关闭
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

// 得到温度值
// 返回值:温度值(扩大了100倍)
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

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//得到加速度值(原始值)
//ax,ay,az:加速度计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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


//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
static void usart1_niming_report(Uint8 fun,Uint8*data,Uint8 len)
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
void mpu6050_send_kalman(short angle_m,short angle)
{
    Uint8 tbuf[4];
    tbuf[0] = (angle_m >> 8) & 0XFF;
    tbuf[1] = angle_m & 0XFF;
    tbuf[2] = (angle >> 8) & 0XFF;
    tbuf[3] = angle & 0XFF;
    usart1_niming_report(0XA1,tbuf,4);//自定义帧,0XA1
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
