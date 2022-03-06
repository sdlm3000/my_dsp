/*
 * mpu6050.c
 *
 *  Created on: 2022锟斤拷1锟斤拷19锟斤拷
 *      Author: sdlm
 */



#include "mpu6050.h"
#include "uart.h"

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
