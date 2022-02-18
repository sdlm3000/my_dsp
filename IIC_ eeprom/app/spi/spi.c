/*
 * spi.c
 *
 *  Created on: 2021年6月25日
 *      Author: 14472
 */
#include "spi.h"


void SPIA_Init(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;   // SPI-A
	EDIS;

	InitSpiaGpio();

	// Initialize SPI FIFO registers
	SpiaRegs.SPIFFTX.all=0xE040;              //使能 FIFO，清除发送中断
	SpiaRegs.SPIFFRX.all=0x204f;              //使能 FIFO 接收 16 级深度
	SpiaRegs.SPIFFCT.all=0x0;                 //清除 FIFO 计数器

	SpiaRegs.SPICCR.all =0x000F;	             // 复位 SPI，上升沿发送，下降沿接收，16位数据
	SpiaRegs.SPICTL.all =0x0006;    		     // 无相位延时，主模式

	SpiaRegs.SPIBRR =0x007F;                      //确定 SPICLK
	SpiaRegs.SPICCR.all =0x009F;		         // 自测模式并从复位状态释放
	SpiaRegs.SPIPRI.bit.FREE = 1;                // 自由运行
}

Uint16 SPIA_SendReciveData(Uint16 dat)
{
	// Transmit data
	SpiaRegs.SPITXBUF=dat;

	// Wait until data is received
	while(SpiaRegs.SPIFFRX.bit.RXFFST !=1);

	return SpiaRegs.SPIRXBUF;
}




