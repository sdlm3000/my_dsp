/*
 * i2c.h
 *
 *  Created on: 2022Äê1ÔÂ18ÈÕ
 *      Author: sdlm
 */

#ifndef I2C_H_
#define I2C_H_

typedef unsigned char Uint8;

void I2CA_Init();

//Uint8 I2CA_WriteData(struct I2CMSG *msg);
//Uint8 I2CA_ReadData(struct I2CMSG *msg);
//unsigned char AT24CXX_Check(void);

Uint8 I2C_ReadData(Uint8 SlaveAddr, Uint8 *Rdata, Uint8 RomAddress, Uint8 number);
Uint8 I2C_WriteData(Uint8 SlaveAddr, Uint8 *Wdata, Uint8 RomAddress, Uint8 number);

unsigned char AT24CXX_Check2(void);

#endif /* I2C_H_ */
