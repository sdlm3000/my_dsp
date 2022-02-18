// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:27 $
//###########################################################################
//
// FILE:	DSP2833x_I2C.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitI2C: 
//---------------------------------------------------------------------------
// This function initializes the I2C to a known state.
//
void InitI2C(void)
{
	// Initialize I2C-A:
	   I2caRegs.I2CMDR.all = 0x0000;	// Take I2C reset 复位I2C
	   									// Stop I2C when suspended
	   InitI2CGpio();  //初始化GPIO
	//复位FIFO寄存器
	   I2caRegs.I2CFFTX.all = 0x0000;	// Disable FIFO mode and TXFIFO
	   I2caRegs.I2CFFRX.all = 0x0040;	// Disable RXFIFO, clear RXFFINT,
	//根据晶振不同设置预定标寄存器的值
	   #if (CPU_FRQ_150MHZ)             // Default - For 150MHz SYSCLKOUT
	        I2caRegs.I2CPSC.all = 14;   // Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
	   #endif
	   #if (CPU_FRQ_100MHZ)             // For 100 MHz SYSCLKOUT
	     I2caRegs.I2CPSC.all = 9;	    // Prescaler - need 7-12 Mhz on module clk (100/10 = 10MHz)
	   #endif
	//设定时钟值
//	   I2caRegs.I2CCLKL = 20;			// NOTE: must be non zero
//	   I2caRegs.I2CCLKH =15;			// NOTE: must be non zero
//	   I2caRegs.I2CIER.all = 0x24;		// Enable SCD & ARDY interrupts
//	   I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset 从复位状态恢复
//							        	// Stop I2C when suspended
//	   I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO  允许写数据FIFO
//	   I2caRegs.I2CFFRX.all = 0x206E;	// Enable RXFIFO, clear RXFFINT,  允许读取FIFO
	     I2caRegs.I2CCLKL = 10;			// NOTE: must be non zero
	      I2caRegs.I2CCLKH = 5;			// NOTE: must be non zero
	      I2caRegs.I2CIER.all = 0x24;		// Enable SCD & ARDY interrupts

	      I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset
	      									// Stop I2C when suspended

	      I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
	      I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT


//	   I2caRegs.I2CFFRX.bit.RXFFINTCLR =1;  //清除I2C FIFO中断标志位
//     I2caRegs.I2CFFRX.bit.RXFFRST=0;   //复位I2C FIFO

	   //EDIS;
	   //return;
	//tbd...
}	

//---------------------------------------------------------------------------
// Example: InitI2CGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as I2C pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// Only one GPIO pin should be enabled for SDAA operation.
// Only one GPIO pin shoudl be enabled for SCLA operation. 
// Comment out other unwanted lines.

void InitI2CGpio(void)
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

/* Configure SCI pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be I2C functional pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation
	
    EDIS;
}

	
//===========================================================================
// End of file.
//===========================================================================
