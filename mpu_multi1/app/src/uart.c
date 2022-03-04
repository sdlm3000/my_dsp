/*
 * uart.c
 *
 *  Created on: 2021年6月24日
 *      Author: 14472
 */
#include "uart.h"
#include "stdio.h"
#define MAX_WRITE_LEN (528)


void UARTa_Init(Uint32 baud)
{
	unsigned char scihbaud=0;
	unsigned char scilbaud=0;
	Uint16 scibaud=0;

	scibaud=37500000/(8*baud)-1;
	scihbaud=scibaud>>8;
	scilbaud=scibaud&0xff;


	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
	EDIS;

	InitSciaGpio();

	//Initalize the SCI FIFO
	SciaRegs.SCIFFTX.all=0xE040;
	SciaRegs.SCIFFRX.all=0x204f;
	SciaRegs.SCIFFCT.all=0x0;

	// Note: Clocks were turned on to the SCIA peripheral
	// in the InitSysCtrl() function
	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
								   // No parity,8 char bits,
								   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
								   // Disable RX ERR, SLEEP, TXWAKE
	SciaRegs.SCICTL2.all =0x0003;
	SciaRegs.SCICTL2.bit.TXINTENA =1;
	SciaRegs.SCICTL2.bit.RXBKINTENA =1;
	SciaRegs.SCIHBAUD    =scihbaud;  // 9600 baud @LSPCLK = 37.5MHz.
	SciaRegs.SCILBAUD    =scilbaud;
//	SciaRegs.SCICCR.bit.LOOPBKENA =1; // Enable loop back
	SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

}

void uart_printf(char * fmt, ...) //自定义变参函数
{
    char *  arg_ptr;
    unsigned int  i ,  len;
    static char write_buf[MAX_WRITE_LEN];

    va_start(arg_ptr, fmt);
    len = vsprintf((char *)write_buf, fmt, arg_ptr);
    va_end(arg_ptr);

    for (i = 0; i < len; i++)
    {
        if ( '\0' != write_buf[i] )
        {
            while (SciaRegs.SCIFFTX.bit.TXFFST != 0); //使用SCIA
            SciaRegs.SCITXBUF=write_buf[i];
        }
    }
}

