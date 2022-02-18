/*
 * timer.c
 *
 *  Created on: 2021年6月4日
 *      Author: 14472
 */

#include "timer.h"
#include "leds.h"


//定时器0初始化函数
//Freq：CPU时钟频率（150MHz）
//Period：定时周期值，单位us
void TIM0_Init(float Freq, float Period)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
	EDIS;

	//设置定时器0的中断入口地址为中断向量表的INT0
	EALLOW;
	PieVectTable.TINT0 = &TIM0_IRQn;
	EDIS;

	//指向定时器0的寄存器地址
	CpuTimer0.RegsAddr = &CpuTimer0Regs;
	//设置定时器0的周期寄存器值
	CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
	//设置定时器预定标计数器值为0
	CpuTimer0Regs.TPR.all  = 0;
	CpuTimer0Regs.TPRH.all = 0;
	//确保定时器0为停止状态
	CpuTimer0Regs.TCR.bit.TSS = 1;
	//重载使能
	CpuTimer0Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer0.InterruptCount = 0;

	ConfigCpuTimer(&CpuTimer0, Freq, Period);

	//开始定时器功能
	CpuTimer0Regs.TCR.bit.TSS=0;
	//开启CPU第一组中断并使能第一组中断的第7个小中断，即定时器0
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//使能总中断
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



