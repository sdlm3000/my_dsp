/*
 * imu901.h
 *
 *  Created on: 2022��1��26��
 *      Author: sdlm
 */

#ifndef _IMU901_H_
#define _IMU901_H_

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "common.h"

#define imu901_uart_receive(data, len)  usartb_getRxData(data, len) /*!< ���ڻ�ȡ��������API */
#define imu901_uart_send(data, len)     usartb_sendData(data, len)  /*!< ����i��������API */

/*ģ������֡ͷ*/
#define UP_BYTE1        0x55
#define UP_BYTE2        0x55    /*!< ģ�������ϴ���ͷ2 */
#define UP_BYTE2_ACK    0xAF    /*!< ģ��ָ��Ӧ���ϴ���ͷ2 */


/* ģ�������ϴ�ָ��ID */
enum upmsgid
{
    UP_ATTITUDE     = 0x01,     /*!< ��̬�� */
    UP_QUAT         = 0x02,     /*!< ��Ԫ�� */
    UP_GYROACCDATA  = 0x03,     /*!< �����Ǽ��ٶ� */
    UP_MAGDATA      = 0x04,     /*!< �ų� */
    UP_BARODATA     = 0x05,     /*!< ��ѹ�� */
    UP_D03DATA      = 0x06,     /*!< �˿�״̬ */
};

/* �Ĵ����б� */
enum regTable
{
    REG_SAVE    = 0x00,         /*!< �����������         ��W��     */
    REG_SENCAL  = 0x01,         /*!< У׼����           ��W��     */
    REG_SENSTA  = 0x02,         /*!< У׼״̬           ��R��     */
    REG_GYROFSR = 0x03,         /*!< ����������          ��R/W�� */
    REG_ACCFSR  = 0x04,         /*!< ���ٶ�����          ��R/W�� */
    REG_GYROBW  = 0x05,         /*!< �����Ǵ���          ��R/W�� */
    REG_ACCBW   = 0x06,         /*!< ���ٶȴ���          ��R/W�� */
    REG_BAUD    = 0x07,         /*!< ������            ��R/W�� */
    REG_UPSET   = 0x08,         /*!< �ش���������         ��R/W�� */
    REG_UPSET2  = 0x09,         /*!< �ش���������2        ��R/W�� */
    REG_UPRATE  = 0x0A,         /*!< �ش�����           ��R/W�� */
    REG_ALG     = 0x0B,         /*!< �㷨ѡ��           ��R/W�� */
    REG_ASM     = 0x0C,         /*!< ��װ��ʽ           ��R/W�� */
    REG_GACOFF  = 0x0D,         /*!< ��������У׼         ��R/W�� */
    REG_BACOFF  = 0x0E,         /*!< ��ѹ����У׼         ��R/W�� */
    REG_LEDOFF  = 0x0F,         /*!< LED����          ��R/W�� */
    REG_D0MODE  = 0x10,         /*!< �˿�D0ģʽ         ��R/W�� */
    REG_D1MODE  = 0x11,         /*!< �˿�D1ģʽ         ��R/W�� */
    REG_D2MODE  = 0x12,         /*!< �˿�D2ģʽ         ��R/W�� */
    REG_D3MODE  = 0x13,         /*!< �˿�D3ģʽ         ��R/W�� */
    REG_D0PULSE = 0x14,         /*!< �˿�D0PWM����      ��R/W�� */
    REG_D1PULSE = 0x16,         /*!< �˿�D1PWM����      ��R/W�� */
    REG_D2PULSE = 0x18,         /*!< �˿�D2PWM����      ��R/W�� */
    REG_D3PULSE = 0x1A,         /*!< �˿�D3PWM����      ��R/W�� */
    REG_D0PERIOD    = 0x1C,     /*!< �˿�D0PWM����      ��R/W�� */
    REG_D1PERIOD    = 0x1F,     /*!< �˿�D1PWM����      ��R/W�� */
    REG_D2PERIOD    = 0x21,     /*!< �˿�D2PWM����      ��R/W�� */
    REG_D3PERIOD    = 0x23,     /*!< �˿�D2PWM����      ��R/W�� */

    REG_RESET   = 0x7F,         /*!< �ָ�Ĭ������         ��W��     */
};


#define ATKP_MAX_DATA_SIZE 28

/*ͨѶ���ݽṹ*/
typedef struct
{
    Uint8 startByte1;
    Uint8 startByte2;
    Uint8 msgID;
    Uint8 dataLen;
    Uint8 data[ATKP_MAX_DATA_SIZE];
    Uint8 checkSum;
} atkp_t;


/* ��̬�����ݽṹ�� */
typedef struct
{
    float roll;
    float pitch;
    float yaw;
} attitude_t;

/* ��Ԫ�����ݽṹ�� */
typedef struct
{
    float q0, q1, q2, q3;

} quaternion_t;

/* �����Ǽ��ٶ����ݽṹ�� */
typedef struct
{
    int16 gyro[3];    /*!< ������ԭʼ���� */
    int16 acc[3];     /*!< ���ٶ�ԭʼ���� */
    float   fgyroD[3];  /*!< ������ת�١�/S */
    float   faccG[3];   /*!< ���ٶ����� G */
} gyroAcc_t;


/* ���������ݽṹ */
typedef struct
{
    int16 mag[3];     /*!< �ų����� */
    float   temp;       /*!< �������¶� */
} mag_t;

/* ��ѹ�����ݽṹ */
typedef struct
{
    int32 pressure;   /*!< ��ѹֵPa */
    int32 altitude;   /*!< ���θ߶�cm */
    float   temp;       /*!< ��ѹ���¶� */
} baro_t;


/* �˿����ݽṹ */
typedef struct
{
    Uint16 d03data[4];
} ioStatus_t;



/* ���ּĴ������� */
typedef struct
{
    Uint8 gyroFsr;    /*!< REG_GYROFSR�Ĵ�����ֵ */
    Uint8 accFsr;
    Uint8 gyroBW;
    Uint8 accBW;
} regValue_t;


///* ģ�������ϴ������� */
extern attitude_t       attitude;       /*!< ��̬�� */
extern quaternion_t     quaternion;
extern gyroAcc_t        gyroAccData;
extern mag_t            magData;
extern baro_t           baroData;
extern ioStatus_t       iostatus;

/* ģ��Ĵ�������ֵ */
extern regValue_t   imu901Param;

/* ���ڽ��ս����ɹ������ݰ� */
extern atkp_t   rxPacket;

void imu901_init(void);

Uint8 imu901_unpack(Uint8 ch);
void atkpParsing(atkp_t *packet);

void atkpWriteReg(enum regTable reg, Uint16 data, Uint8 datalen);
Uint8 atkpReadReg(enum regTable reg, int16 *data);



#endif /* _IMU901_H_ */
