/*
 * adc.c
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */
#include "adc.h"
#include "common.h"

void ADC_Init()
{
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
    EDIS;
    InitAdc();  // 初始化ADC
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;     //采样开关的时间
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;    //时钟分频
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;           // 设置成连续运行，
    AdcRegs.ADCTRL1.bit.CPS = 0;                //预定标系数=1；
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;           //级联序列器方式
  //  AdcRegs.ADCTRL1.bit.SEQ_OVRD =1;
    AdcRegs.ADCMAXCONV.all=0x0006;              //设置5个转换//改了这里
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;      //设置ADCINA0作为第一个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;      //设置ADCINA1作为第二个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;      //设置ADCINA2作为第三个变换
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;      //设置ADCINA3作为第四个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;      //设置ADCINA4作为第五个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;      //设置ADCINA5作为第六个变换
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;      //设置ADCINA5作为第七个变换
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;       // Enable SEQ1 interrupt (every EOS)

    AdcRegs.ADCTRL2.all = 0x2000;       //软件向SEQ1位写1开启转换触发
}




