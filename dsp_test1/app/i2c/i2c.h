/*
 * i2c.h
 *
 *  Created on: 2022Äê1ÔÂ18ÈÕ
 *      Author: sdlm
 */

#ifndef I2C_H_
#define I2C_H_



void I2CA_Init();

//Uint16 I2CA_WriteData(struct I2CMSG *msg);
//Uint16 I2CA_ReadData(struct I2CMSG *msg);
//unsigned char AT24CXX_Check(void);

Uint16 I2C_ReadData(Uint16 SlaveAddr, Uint16 *Rdata, Uint16 RomAddress, Uint16 number);
Uint16 I2C_WriteData(Uint16 SlaveAddr, Uint16 *Wdata, Uint16 RomAddress, Uint16 number);

unsigned char AT24CXX_Check2(void);

#endif /* I2C_H_ */
