/*
 * main.c
 *
 *  Created on: 2021年5月14日
 *      Author: 14472
 */


#include "DSP2833x_Device.h"	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"	//  DSP2833x  Examples  Include  File



#include "leds.h"
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include "24cxx.h"
#include "key.h"



/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void main()
{
//    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
//    // This function must reside in RAM
//    InitFlash();
	int i=0;
	unsigned char key=0,k=0;


	InitSysCtrl();
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	//复制对时间敏感代码和FLASH配置代码到RAM中
	// 包括FLASH初始化函数 InitFlash();
	// 链接后将产生 RamfuncsLoadStart, RamfuncsLoadEnd, 和RamfuncsRunStart
	// 参数. 请参考 F28335.cmd 文件
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	// 调用FLASH初始化函数来设置flash等待状态
	// 这个函数必须在RAM中运行
	InitFlash();

	LED_Init();
	TIM0_Init(150,200000);//200ms
	UARTa_Init(4800);
	SMG_Init();
	KEY_Init();
	AT24CXX_Init();

	while(AT24CXX_Check())  //检测AT24C08是否正常
	{
		UARTa_SendString("AT24C08 NO!\r\n");
		DELAY_US(500*1000);
	}
	UARTa_SendString("AT24C08 OK!\r\n");


	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRESS)
		{
			k++;
			if(k>255)
			{
				k=255;
			}
			AT24CXX_WriteOneByte(0,k);

		}
		else if(key==KEY2_PRESS)
		{
			k=AT24CXX_ReadOneByte(0);
		}
		else if(key==KEY3_PRESS)
		{
			k=0;
		}

		SMG_DisplayInt(k);
	}
}

