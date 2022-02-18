/*
 * exint.c
 *
 *  Created on: 2021��6��3��
 *      Author: 14472
 */

#include "exint.h"
#include "leds.h"
#include "key.h"


void EXINT1_Init(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    // GPIO input clock
	EDIS;

	EALLOW;
	//KEY�˿�����
	GpioCtrlRegs.GPAMUX1.bit.GPIO12=0;
	GpioCtrlRegs.GPADIR.bit.GPIO12=0;
	GpioCtrlRegs.GPAPUD.bit.GPIO12=0;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;        // �ⲿ�ж�1��XINT1����ϵͳʱ��SYSCLKOUTͬ��

	GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;
	GpioCtrlRegs.GPBDIR.bit.GPIO48=1;
	GpioCtrlRegs.GPBPUD.bit.GPIO48=0;
	GpioDataRegs.GPBCLEAR.bit.GPIO48=1;
	EDIS;

	EALLOW;
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;   // XINT1��GPIO12
	EDIS;

	EALLOW;	// �޸ı������ļĴ������޸�ǰӦ���EALLOW���
	PieVectTable.XINT1 = &EXINT1_IRQn;
	EDIS;   // EDIS����˼�ǲ������޸ı������ļĴ���

	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // ʹ��PIE��1��INT4

	XIntruptRegs.XINT1CR.bit.POLARITY = 0;      // �½��ش����ж�
	XIntruptRegs.XINT1CR.bit.ENABLE= 1;        // ʹ��XINT1

	IER |= M_INT1;                              // ʹ��CPU�ж�1��INT1��
	EINT;                                       // ��ȫ���ж�
	ERTM;
}

interrupt void EXINT1_IRQn(void)
{
	Uint32 i;
	for(i=0;i<10000;i++);    //����������
	while(!KEY_H1);
	LED13_TOGGLE;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}


void EXINT2_Init(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    // GPIO input clock
	EDIS;

	EALLOW;
	//KEY�˿�����
	GpioCtrlRegs.GPAMUX1.bit.GPIO13=0;
	GpioCtrlRegs.GPADIR.bit.GPIO13=0;
	GpioCtrlRegs.GPAPUD.bit.GPIO13=0;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 2;        // �ⲿ�ж�2��XINT2�������޶�6����������
	GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   // ÿ���������ڵ�����Ϊ510*SYSCLKOUT

	GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;
	GpioCtrlRegs.GPBDIR.bit.GPIO48=1;
	GpioCtrlRegs.GPBPUD.bit.GPIO48=0;
	GpioDataRegs.GPBCLEAR.bit.GPIO48=1;
	EDIS;

	EALLOW;
	GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 13;   // XINT2��GPIO13
	EDIS;

	EALLOW;	// �޸ı������ļĴ������޸�ǰӦ���EALLOW���
	PieVectTable.XINT2 = &EXINT2_IRQn;
	EDIS;   // EDIS����˼�ǲ������޸ı������ļĴ���

	PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // ʹ��PIE��1��INT5

	XIntruptRegs.XINT2CR.bit.POLARITY = 0;      // �½��ش����ж�
	XIntruptRegs.XINT2CR.bit.ENABLE = 1;        // ʹ��XINT2

	IER |= M_INT1;                              // ʹ��CPU�ж�1��INT1��
	EINT;                                       // ��ȫ���ж�
	ERTM;
}

interrupt void EXINT2_IRQn(void)
{
	Uint32 i;
	for(i=0;i<10000;i++);    //����������
	while(!KEY_H2);
	LED12_TOGGLE;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}



