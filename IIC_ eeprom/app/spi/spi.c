/*
 * spi.c
 *
 *  Created on: 2021��6��25��
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
	SpiaRegs.SPIFFTX.all=0xE040;              //ʹ�� FIFO����������ж�
	SpiaRegs.SPIFFRX.all=0x204f;              //ʹ�� FIFO ���� 16 �����
	SpiaRegs.SPIFFCT.all=0x0;                 //��� FIFO ������

	SpiaRegs.SPICCR.all =0x000F;	             // ��λ SPI�������ط��ͣ��½��ؽ��գ�16λ����
	SpiaRegs.SPICTL.all =0x0006;    		     // ����λ��ʱ����ģʽ

	SpiaRegs.SPIBRR =0x007F;                      //ȷ�� SPICLK
	SpiaRegs.SPICCR.all =0x009F;		         // �Բ�ģʽ���Ӹ�λ״̬�ͷ�
	SpiaRegs.SPIPRI.bit.FREE = 1;                // ��������
}

Uint16 SPIA_SendReciveData(Uint16 dat)
{
	// Transmit data
	SpiaRegs.SPITXBUF=dat;

	// Wait until data is received
	while(SpiaRegs.SPIFFRX.bit.RXFFST !=1);

	return SpiaRegs.SPIRXBUF;
}




