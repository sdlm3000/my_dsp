/*
 * imu901.h
 *
 *  Created on: 2022年1月26日
 *      Author: sdlm
 */

#ifndef _IMU901_H_
#define _IMU901_H_

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "common.h"

#define imu901_uart_receive(data, len)  usartb_getRxData(data, len) /*!< 串口获取接收数据API */
#define imu901_uart_send(data, len)     usartb_sendData(data, len)  /*!< 串口i发送数据API */

/*模块上行帧头*/
#define UP_BYTE1        0x55
#define UP_BYTE2        0x55    /*!< 模块主动上传包头2 */
#define UP_BYTE2_ACK    0xAF    /*!< 模块指令应答上传包头2 */


/* 模块主动上传指令ID */
enum upmsgid
{
    UP_ATTITUDE     = 0x01,     /*!< 姿态角 */
    UP_QUAT         = 0x02,     /*!< 四元数 */
    UP_GYROACCDATA  = 0x03,     /*!< 陀螺仪加速度 */
    UP_MAGDATA      = 0x04,     /*!< 磁场 */
    UP_BARODATA     = 0x05,     /*!< 气压计 */
    UP_D03DATA      = 0x06,     /*!< 端口状态 */
};

/* 寄存器列表 */
enum regTable
{
    REG_SAVE    = 0x00,         /*!< 保存参数命令         （W）     */
    REG_SENCAL  = 0x01,         /*!< 校准命令           （W）     */
    REG_SENSTA  = 0x02,         /*!< 校准状态           （R）     */
    REG_GYROFSR = 0x03,         /*!< 陀螺仪量程          （R/W） */
    REG_ACCFSR  = 0x04,         /*!< 加速度量程          （R/W） */
    REG_GYROBW  = 0x05,         /*!< 陀螺仪带宽          （R/W） */
    REG_ACCBW   = 0x06,         /*!< 加速度带宽          （R/W） */
    REG_BAUD    = 0x07,         /*!< 波特率            （R/W） */
    REG_UPSET   = 0x08,         /*!< 回传内容设置         （R/W） */
    REG_UPSET2  = 0x09,         /*!< 回传内容设置2        （R/W） */
    REG_UPRATE  = 0x0A,         /*!< 回传速率           （R/W） */
    REG_ALG     = 0x0B,         /*!< 算法选择           （R/W） */
    REG_ASM     = 0x0C,         /*!< 安装方式           （R/W） */
    REG_GACOFF  = 0x0D,         /*!< 陀螺仪自校准         （R/W） */
    REG_BACOFF  = 0x0E,         /*!< 气压计自校准         （R/W） */
    REG_LEDOFF  = 0x0F,         /*!< LED开关          （R/W） */
    REG_D0MODE  = 0x10,         /*!< 端口D0模式         （R/W） */
    REG_D1MODE  = 0x11,         /*!< 端口D1模式         （R/W） */
    REG_D2MODE  = 0x12,         /*!< 端口D2模式         （R/W） */
    REG_D3MODE  = 0x13,         /*!< 端口D3模式         （R/W） */
    REG_D0PULSE = 0x14,         /*!< 端口D0PWM脉宽      （R/W） */
    REG_D1PULSE = 0x16,         /*!< 端口D1PWM脉宽      （R/W） */
    REG_D2PULSE = 0x18,         /*!< 端口D2PWM脉宽      （R/W） */
    REG_D3PULSE = 0x1A,         /*!< 端口D3PWM脉宽      （R/W） */
    REG_D0PERIOD    = 0x1C,     /*!< 端口D0PWM周期      （R/W） */
    REG_D1PERIOD    = 0x1F,     /*!< 端口D1PWM周期      （R/W） */
    REG_D2PERIOD    = 0x21,     /*!< 端口D2PWM周期      （R/W） */
    REG_D3PERIOD    = 0x23,     /*!< 端口D2PWM周期      （R/W） */

    REG_RESET   = 0x7F,         /*!< 恢复默认命令         （W）     */
};


#define ATKP_MAX_DATA_SIZE 28

/*通讯数据结构*/
typedef struct
{
    Uint8 startByte1;
    Uint8 startByte2;
    Uint8 msgID;
    Uint8 dataLen;
    Uint8 data[ATKP_MAX_DATA_SIZE];
    Uint8 checkSum;
} atkp_t;


/* 姿态角数据结构体 */
typedef struct
{
    float roll;
    float pitch;
    float yaw;
} attitude_t;

/* 四元数数据结构体 */
typedef struct
{
    float q0, q1, q2, q3;

} quaternion_t;

/* 陀螺仪加速度数据结构体 */
typedef struct
{
    int16 gyro[3];    /*!< 陀螺仪原始数据 */
    int16 acc[3];     /*!< 加速度原始数据 */
    float   fgyroD[3];  /*!< 陀螺仪转速°/S */
    float   faccG[3];   /*!< 加速度重力 G */
} gyroAcc_t;


/* 磁力计数据结构 */
typedef struct
{
    int16 mag[3];     /*!< 磁场数据 */
    float   temp;       /*!< 磁力计温度 */
} mag_t;

/* 气压计数据结构 */
typedef struct
{
    int32 pressure;   /*!< 气压值Pa */
    int32 altitude;   /*!< 海拔高度cm */
    float   temp;       /*!< 气压计温度 */
} baro_t;


/* 端口数据结构 */
typedef struct
{
    Uint16 d03data[4];
} ioStatus_t;



/* 部分寄存器参数 */
typedef struct
{
    Uint8 gyroFsr;    /*!< REG_GYROFSR寄存器的值 */
    Uint8 accFsr;
    Uint8 gyroBW;
    Uint8 accBW;
} regValue_t;


///* 模块主动上传的数据 */
extern attitude_t       attitude;       /*!< 姿态角 */
extern quaternion_t     quaternion;
extern gyroAcc_t        gyroAccData;
extern mag_t            magData;
extern baro_t           baroData;
extern ioStatus_t       iostatus;

/* 模块寄存器参数值 */
extern regValue_t   imu901Param;

/* 串口接收解析成功的数据包 */
extern atkp_t   rxPacket;

void imu901_init(void);

Uint8 imu901_unpack(Uint8 ch);
void atkpParsing(atkp_t *packet);

void atkpWriteReg(enum regTable reg, Uint16 data, Uint8 datalen);
Uint8 atkpReadReg(enum regTable reg, int16 *data);



#endif /* _IMU901_H_ */
