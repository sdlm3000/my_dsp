/*
 * timer.c
 *
 *  Created on: 2021��6��4��
 *      Author: 14472
 */

#include "timer.h"
#include "leds.h"


//��ʱ��0��ʼ������
//Freq��CPUʱ��Ƶ�ʣ�150MHz��
//Period����ʱ����ֵ����λus
void TIM0_Init(float Freq, float Period)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
	EDIS;

	//���ö�ʱ��0���ж���ڵ�ַΪ�ж��������INT0
	EALLOW;
	PieVectTable.TINT0 = &TIM0_IRQn;
	EDIS;

	//ָ��ʱ��0�ļĴ�����ַ
	CpuTimer0.RegsAddr = &CpuTimer0Regs;
	//���ö�ʱ��0�����ڼĴ���ֵ
	CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
	//���ö�ʱ��Ԥ���������ֵΪ0
	CpuTimer0Regs.TPR.all  = 0;
	CpuTimer0Regs.TPRH.all = 0;
	//ȷ����ʱ��0Ϊֹͣ״̬
	CpuTimer0Regs.TCR.bit.TSS = 1;
	//����ʹ��
	CpuTimer0Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer0.InterruptCount = 0;

	ConfigCpuTimer(&CpuTimer0, Freq, Period);

	//��ʼ��ʱ������
	CpuTimer0Regs.TCR.bit.TSS=0;
	//����CPU��һ���жϲ�ʹ�ܵ�һ���жϵĵ�7��С�жϣ�����ʱ��0
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//ʹ�����ж�
	EINT;
	ERTM;

}

interrupt void TIM0_IRQn(void)
{
	EALLOW;
	LED13_TOGGLE;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	EDIS;
}

void TIM1_Init(float Freq, float Period)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
	EDIS;

	EALLOW;
	PieVectTable.XINT13 = &TIM1_IRQn;
	EDIS;

	// Initialize address pointers to respective timer registers:
	CpuTimer1.RegsAddr = &CpuTimer1Regs;
	// Initialize timer period to maximum:
	CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer1Regs.TPR.all  = 0;
	CpuTimer1Regs.TPRH.all = 0;
	// Make sure timers are stopped:
	CpuTimer1Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer1Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer1.InterruptCount = 0;

	ConfigCpuTimer(&CpuTimer1, Freq, Period);

	CpuTimer1Regs.TCR.bit.TSS=0;

	IER |= M_INT13;

	EINT;
	ERTM;

}

interrupt void TIM1_IRQn(void)
{
	EALLOW;
	LED12_TOGGLE;
	EDIS;
}



void TIM2_Init(float Freq, float Period)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2
	EDIS;

	EALLOW;
	PieVectTable.TINT2 = &TIM2_IRQn;
	EDIS;

	// Initialize address pointers to respective timer registers:
	CpuTimer2.RegsAddr = &CpuTimer2Regs;
	// Initialize timer period to maximum:
	CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer2Regs.TPR.all  = 0;
	CpuTimer2Regs.TPRH.all = 0;
	// Make sure timers are stopped:
	CpuTimer2Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer2Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer2.InterruptCount = 0;

	ConfigCpuTimer(&CpuTimer2, Freq, Period);

	CpuTimer2Regs.TCR.bit.TSS=0;

	IER |= M_INT14;

	EINT;
	ERTM;

}

interrupt void TIM2_IRQn(void)
{
	EALLOW;
	LED11_TOGGLE;
	EDIS;

}



