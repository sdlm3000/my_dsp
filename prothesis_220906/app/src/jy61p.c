/*
 *  jy61p.c
 *
 *  Created on: 2022Äê05ÔÂ10ÈÕ
 *      Author: sdlm
 */

#include "jy61p.h"
#include "i2c.h"
#include "uart.h"

static void ShortToChar(short sData,unsigned char cData[])
{
    cData[0]=sData & 0xff;
    cData[1]=sData >> 8;
}
static short CharToShort(unsigned char cData[])
{
    return ((short)cData[1] << 8) | cData[0];
}


Uint8 jp61p_Init(void)
{
    return 0;
}


Uint8 jy61p_GetData(Uint8 flag, Uint8 mpu_addr, float  * a, float  * w, float  * Angle)
{
    unsigned char chrTemp[30];
    if(I2C_ReadData(mpu_addr, &chrTemp[0], AX, 24) != 0)
    {
        return 1;
    }
    a[0] = (float)CharToShort(&chrTemp[0]) / 32768 * 16;
    a[1] = (float)CharToShort(&chrTemp[2]) / 32768 * 16;
    a[2] = (float)CharToShort(&chrTemp[4]) / 32768 * 16;
    w[0] = (float)CharToShort(&chrTemp[6]) / 32768 * 2000;
    w[1] = (float)CharToShort(&chrTemp[8]) / 32768 * 2000;
    w[2] = (float)CharToShort(&chrTemp[10]) / 32768 * 2000;
    Angle[0] = (float)CharToShort(&chrTemp[18]) / 32768 * 180;
    Angle[1] = (float)CharToShort(&chrTemp[20]) / 32768 * 180;
    Angle[2] = (float)CharToShort(&chrTemp[22]) / 32768 * 180;
    return 0;
}

void jy61p_SendData(Uint8 flag, float *a, float *w, float *Angle)
{
    if(flag & 0x0001)
    {
        scib_float(a[0]);
        scib_xmit(' ');
        scib_float(a[1]);
        scib_xmit(' ');
        scib_float(a[2]);
        scib_xmit(' ');
    }
    if(flag & 0x0002)
    {
        scib_float(w[0]);
        scib_xmit(' ');
        scib_float(w[1]);
        scib_xmit(' ');
        scib_float(w[2]);
        scib_xmit(' ');

    }
    if(flag & 0x0004)
    {
        scib_float(Angle[0]);
        scib_xmit(' ');
        scib_float(Angle[1]);
        scib_xmit(' ');
        scib_float(Angle[2]);
        scib_xmit(' ');
    }

    scib_xmit('\n');
}
