/*
 * uart.c
 *
 *  Created on: 2022��1��26��
 *      Author: sdlm
 */
#include "uart.h"
#include "leds.h"
#include "imu901.h"
#include "ringbuffer.h"
#include "stdio.h"

#define UARTB_RX_BUFFER_SIZE    256
Uint8 uartbRxBuffer[UARTB_RX_BUFFER_SIZE];

ringbuffer_t uartbRxFifo;

interrupt void UartB_RxInterIsr(void);

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
void uart_printf(char * fmt, ...) //�Զ����κ���
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
//            while (SciaRegs.SCIFFTX.bit.TXFFST != 0); //ʹ��SCIA
//            SciaRegs.SCITXBUF = write_buf[i];
            while (ScibRegs.SCIFFTX.bit.TXFFST != 0); //ʹ��SCIB
            ScibRegs.SCITXBUF = write_buf[i];
        }
    }
}

// Transmit a character from the SCI'
void uarta_send_char(Uint8 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0);
    SciaRegs.SCITXBUF=a;
}

void UARTb_Init(Uint32 baud)
{
    unsigned char scihbaud=0;
    unsigned char scilbaud=0;
    Uint16 scibaud=0;

    scibaud=37500000/(8*baud)-1;
    scihbaud=scibaud>>8;
    scilbaud=scibaud&0xff;


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-A
    EDIS;

    EALLOW;
    PieVectTable.SCIRXINTB = &UartB_RxInterIsr;  // SCIBX�����жϷ������
    EDIS;
    InitScibGpio();

    //Initalize the SCI FIFO
    ScibRegs.SCIFFTX.all=0xE040;
//    ScibRegs.SCIFFRX.all=0x204f;
    ScibRegs.SCIFFRX.all=0x0021;    // ����FIFO�����Ϊ��͵�5λ���˴�����Ϊ1��ÿ���ܵ�һ���ַ�������������ж�
    ScibRegs.SCIFFCT.all=0x0;
    ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    ScibRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    ScibRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.all = 0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA =1;
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;
    ScibRegs.SCIHBAUD    =scihbaud;  // 9600 baud @LSPCLK = 37.5MHz.
    ScibRegs.SCILBAUD    =scilbaud;
//  ScibRegs.SCICCR.bit.LOOPBKENA =1; // Enable loop back
    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

    IER |= M_INT9;    //����CPU�ж���9  SCI_RX  CANB�ж���1
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;     //����B�����ж�  PIE Group 9, INT3

    ringbuffer_init(&uartbRxFifo, uartbRxBuffer, UARTB_RX_BUFFER_SIZE);

}

Uint16 uartb_send_char(Uint8 a)
{
    Uint16 num = 50000;
    while (ScibRegs.SCIFFTX.bit.TXFFST != 0)
    {
        num--;
        if(num <= 0) return 1;
    }
    ScibRegs.SCITXBUF = a;
    return 0;
}

/**
  * @brief  ��ȡ����2����fifo������
  * @param  buf: ��ŵĻ�����
  * @param  len: ��Ҫ��ȡ�ĳ���
  * @retval uint16_t: ʵ�ʻ�ȡ���ĳ��� 0��ʾû�����ݿɻ�ȡ
  */
Uint16 usartb_getRxData(Uint8 *buf, Uint16 len)
{
    return ringbuffer_out(&uartbRxFifo, buf, len);
}

/**
  * @brief  ����b����
  * @param  data: ���͵�����
  * @param  len: ���ݳ���
  * @retval uint8_t: 0�ɹ� ������ʧ��
  */
Uint16 usartb_sendData(Uint8 *buf, Uint16 len)
{
    Uint16 i;
    for(i = 0; i < len; i++)
    {
        if(uartb_send_char(buf[i]) == 1) return 1;
    }
    return 0;
}

interrupt void UartB_RxInterIsr(void)
{
    Uint8 res = (ScibRegs.SCIRXBUF.all);
//    uart_printf("%c", res);
    ringbuffer_in_check(&uartbRxFifo, (Uint8 *)&res, 1); /*!< �����յ������ݷ���FIFO */
//    LED9_TOGGLE;
    EALLOW;
    ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;  // Clear Overflow flag
    ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;  // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
    EDIS;
}

// ����
//����
void scia_xmit(Uint32 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) { }
    SciaRegs.SCITXBUF=a;
}

//����һ����λ����
void scia_int(int data_char)
{
    unsigned char sci_table[4];
    if(data_char<0)
    {
        data_char = -1 * data_char;
        scia_xmit('-');
    }
    sci_table[0]=data_char/1000;      //���ǧλ������
    sci_table[1]=data_char%1000/100;  //��ð�λ������
    sci_table[2]=data_char%100/10;    //���ʮλ������
    sci_table[3]=data_char%10;        //��ø�λ������
    if(sci_table[0]!= 0)
    {
        sci_table[0]=sci_table[0]+0x30;
        scia_xmit(sci_table[0]);
    }
    if(sci_table[1]!= 0 || sci_table[0]!= 0)
    {
        sci_table[1]=sci_table[1]+0x30;
        scia_xmit(sci_table[1]);
    }
    if(sci_table[1]!= 0 || sci_table[0]!= 0 || sci_table[2]!= 0)
    {
        sci_table[2]=sci_table[2]+0x30;
        scia_xmit(sci_table[2]);
    }
    sci_table[3]=sci_table[3]+0x30;
    scia_xmit(sci_table[3]);
}

//���両����, xxx.xx
void scia_float(float data)
{
    unsigned char sci_table[5];
    long a;
    a=data*100;                          //����2λС��
    if(a<0)
    {
        a = -1 * a;
        scia_xmit('-');
    }
    sci_table[0]=a / 10000;
    sci_table[1]=a % 10000 / 1000;
    sci_table[2]=a % 1000 / 100;
    sci_table[3]=a % 100 / 10;
    sci_table[4]=a % 10;

    if(sci_table[0] != 0)
    {
        sci_table[0]=sci_table[0]+0x30;
        scia_xmit(sci_table[0]);
    }
    if(sci_table[1] != 0)
    {
        sci_table[1] = sci_table[1] + 0x30;
        scia_xmit(sci_table[1]);
    }
    sci_table[2] = sci_table[2]+0x30;
    scia_xmit(sci_table[2]);
    scia_xmit('.');
    sci_table[3] = sci_table[3] + 0x30;
    scia_xmit(sci_table[3]);
    sci_table[4] = sci_table[4] + 0x30;
    scia_xmit(sci_table[4]);
}

//�����ַ���
void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
}


