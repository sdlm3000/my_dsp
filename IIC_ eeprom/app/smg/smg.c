/*
 * smg.c
 *
 *  Created on: 2021年6月8日
 *      Author: 14472
 */

#include "smg.h"


unsigned char smgduan[16]={0xC0, 0xf9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90,
             0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};//0~F 数码管段选数据

void SMG_Init(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;// 开启GPIO时钟
	//SMG数据控制端口配置
	GpioCtrlRegs.GPBMUX2.bit.GPIO56=0;
	GpioCtrlRegs.GPBDIR.bit.GPIO56=1;
	GpioCtrlRegs.GPBPUD.bit.GPIO56=0;

	GpioCtrlRegs.GPBMUX2.bit.GPIO54=0;
	GpioCtrlRegs.GPBDIR.bit.GPIO54=1;
	GpioCtrlRegs.GPBPUD.bit.GPIO54=0;

	GpioCtrlRegs.GPCMUX1.bit.GPIO70=0;
	GpioCtrlRegs.GPCDIR.bit.GPIO70=1;
	GpioCtrlRegs.GPCPUD.bit.GPIO70=0;

	GpioCtrlRegs.GPCMUX1.bit.GPIO71=0;
	GpioCtrlRegs.GPCDIR.bit.GPIO71=1;
	GpioCtrlRegs.GPCPUD.bit.GPIO71=0;

	GpioCtrlRegs.GPCMUX1.bit.GPIO72=0;
	GpioCtrlRegs.GPCDIR.bit.GPIO72=1;
	GpioCtrlRegs.GPCPUD.bit.GPIO72=0;

	GpioCtrlRegs.GPCMUX1.bit.GPIO73=0;
	GpioCtrlRegs.GPCDIR.bit.GPIO73=1;
	GpioCtrlRegs.GPCPUD.bit.GPIO73=0;

	EDIS;

	GpioDataRegs.GPCSET.bit.GPIO70=1;
	GpioDataRegs.GPCSET.bit.GPIO71=1;
	GpioDataRegs.GPCSET.bit.GPIO72=1;
	GpioDataRegs.GPCSET.bit.GPIO73=1;

}

//74HC164数据发送函数
//dat：所要发送的数据
void HC164SendData(unsigned char dat)
{
	char i=1;

	for(i=0;i<8;i++)
	{
		SPICLKA_SETL;
		if(dat&0x80)
			SPISIMOA_SETH;//高电平灭数码管LED段
		else
			SPISIMOA_SETL;//低电平点亮数码管LED段
		SPICLKA_SETH;
		dat<<=1;
	}
}

//数码管显示整数函数
//num：整数
void SMG_DisplayInt(Uint16 num)
{
	unsigned char buf[4];
	unsigned char i=0;

	buf[0]=smgduan[num/1000];     //千位
	buf[1]=smgduan[num%1000/100];    //百位
	buf[2]=smgduan[num%1000%100/10];   //十位
	buf[3]=smgduan[num%1000%100%10];   //个位

	for(i=0;i<4;i++)
	{
		HC164SendData(buf[i]);
		switch(i)
		{
			case 0: SEG1_SETH;SEG2_SETL;SEG3_SETL;SEG4_SETL;break;
			case 1: SEG1_SETL;SEG2_SETH;SEG3_SETL;SEG4_SETL;break;
			case 2: SEG1_SETL;SEG2_SETL;SEG3_SETH;SEG4_SETL;break;
			case 3: SEG1_SETL;SEG2_SETL;SEG3_SETL;SEG4_SETH;break;
		}
		DELAY_US(5000);
	}
}

//数码管显示小数函数
//num：小数
//dotnum：小数点后面有效位数（最大3）
void SMG_DisplayFloat(float num,unsigned char dotnum)
{
	unsigned char buf[4];
	unsigned char i;
	Uint16 value=0;

	if(dotnum==1)
	{
		value=num*10;
		buf[0]=smgduan[value/1000];
		buf[1]=smgduan[value%1000/100];
		buf[2]=smgduan[value%1000%100/10]&0x7f;
		buf[3]=smgduan[value%1000%100%10];
	}
	else if(dotnum==2)
	{
		value=num*100;
		buf[0]=smgduan[value/1000];
		buf[1]=smgduan[value%1000/100]&0x7f;
		buf[2]=smgduan[value%1000%100/10];
		buf[3]=smgduan[value%1000%100%10];
	}
	else if(dotnum==3)
	{
		value=num*1000;
		buf[0]=smgduan[value/1000]&0x7f;
		buf[1]=smgduan[value%1000/100];
		buf[2]=smgduan[value%1000%100/10];
		buf[3]=smgduan[value%1000%100%10];
	}

	for(i=0;i<4;i++)
	{
		HC164SendData(buf[i]);
		switch(i)
		{
			case 0: SEG1_SETH;SEG2_SETL;SEG3_SETL;SEG4_SETL;break;
			case 1: SEG1_SETL;SEG2_SETH;SEG3_SETL;SEG4_SETL;break;
			case 2: SEG1_SETL;SEG2_SETL;SEG3_SETH;SEG4_SETL;break;
			case 3: SEG1_SETL;SEG2_SETL;SEG3_SETL;SEG4_SETH;break;
		}
		DELAY_US(5000);
	}
}



