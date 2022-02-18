/*
 * i2c.c
 *
 *  Created on: 2022年1月18日
 *      Author: sdlm
 */


#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File

#include "i2c.h"

void I2CA_Init()
{
    InitI2CGpio();

    I2caRegs.I2CMDR.all = 0x0000;   // 复位IIC
    // Initialize I2C
    I2caRegs.I2CSAR = 0x0050;        // Slave address - EEPROM control code

    #if (CPU_FRQ_150MHZ)             // Default - For 150MHz SYSCLKOUT
         I2caRegs.I2CPSC.all = 14;   // Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
    #endif
    #if (CPU_FRQ_100MHZ)             // For 100 MHz SYSCLKOUT
      I2caRegs.I2CPSC.all = 9;       // Prescaler - need 7-12 Mhz on module clk (100/10 = 10MHz)
    #endif

    I2caRegs.I2CCLKL = 5;           // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;            // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x00;      // Enable SCD & ARDY interrupts
    I2caRegs.I2CFFTX.all = 0x0000;   // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x0000;   // Enable RXFIFO, clear RXFFINT,
    I2caRegs.I2CMDR.all = 0x0020;    // Take I2C out of reset
                                     // Stop I2C when suspended

//    I2caRegs.I2CFFTX.all = 0x6000;   // Enable FIFO mode and TXFIFO
//    I2caRegs.I2CFFRX.all = 0x2040;   // Enable RXFIFO, clear RXFFINT,

    return;
}

static Uint16 I2C_wait_xrdy()
{
    Uint16 i, num = 5000;
    for(i = 0; i < num; i++)
    {
        if(I2caRegs.I2CSTR.bit.XRDY != 0) return 0;
    }
    return 1;
}

static Uint16 I2C_wait_rrdy()
{
    Uint16 i, num = 5000;
    for(i = 0; i < num; i++)
    {
        if(I2caRegs.I2CSTR.bit.RRDY != 0) return 0;
    }
    return 1;
}

Uint8 I2C_WriteData(Uint8 SlaveAddr, Uint8 *Wdata, Uint8 RomAddress, Uint8 number)
{
    Uint16 i;
    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
      return I2C_BUS_BUSY_ERROR;
    }
    if(I2C_wait_xrdy() == 1)
       return I2C_BUS_BUSY_ERROR;
    I2caRegs.I2CSAR = SlaveAddr;
    I2caRegs.I2CCNT = number + 1;
    I2caRegs.I2CDXR = RomAddress;
    DELAY_US(1);
    I2caRegs.I2CMDR.all = 0x6E20;
    for (i = 0; i < number; i++)
    {
        if(I2C_wait_xrdy() == 1)
           return I2C_BUS_BUSY_ERROR;
        I2caRegs.I2CDXR = *Wdata;
        Wdata++;
        if (I2caRegs.I2CSTR.bit.NACK == 1)
          return I2C_BUS_BUSY_ERROR;
    }
    DELAY_US(2000);
    return I2C_SUCCESS;
}

Uint8 I2C_ReadData(Uint8 SlaveAddr, Uint8 *Rdata, Uint8 RomAddress, Uint8 number)
{
    Uint16  i,Temp;

    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
       return I2C_BUS_BUSY_ERROR;
    }
    if(I2C_wait_xrdy() == 1)
        return I2C_BUS_BUSY_ERROR;
    I2caRegs.I2CSAR = SlaveAddr;
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CDXR = RomAddress;
    DELAY_US(1);
    I2caRegs.I2CMDR.all = 0x6620;
    if (I2caRegs.I2CSTR.bit.NACK == 1)
        return I2C_BUS_BUSY_ERROR;
    DELAY_US(50);
    if(I2C_wait_xrdy() == 1)
        return I2C_BUS_BUSY_ERROR;
    I2caRegs.I2CSAR = SlaveAddr;
    I2caRegs.I2CCNT = number;
    DELAY_US(1);
    I2caRegs.I2CMDR.all = 0x6C20;
    if (I2caRegs.I2CSTR.bit.NACK == 1)
        return I2C_BUS_BUSY_ERROR;
    for(i = 0; i < number; i++)
    {
        if(I2C_wait_rrdy() == 1)
            return I2C_BUS_BUSY_ERROR;
        Temp = I2caRegs.I2CDRR;
        if (I2caRegs.I2CSTR.bit.NACK == 1)
          return I2C_BUS_BUSY_ERROR;
        *Rdata = Temp;
        Rdata++;
    }
    return I2C_SUCCESS;
}

