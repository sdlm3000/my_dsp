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
    InitAdc();  // ��ʼ��ADC
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;     //�������ص�ʱ��
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;    //ʱ�ӷ�Ƶ
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;           // ���ó��������У�
    AdcRegs.ADCTRL1.bit.CPS = 0;                //Ԥ����ϵ��=1��
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;           //������������ʽ
  //  AdcRegs.ADCTRL1.bit.SEQ_OVRD =1;
    AdcRegs.ADCMAXCONV.all=0x0006;              //����5��ת��//��������
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;      //����ADCINA0��Ϊ��һ���任
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;      //����ADCINA1��Ϊ�ڶ����任
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;      //����ADCINA2��Ϊ�������任
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;      //����ADCINA3��Ϊ���ĸ��任
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;      //����ADCINA4��Ϊ������任
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;      //����ADCINA5��Ϊ�������任
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;      //����ADCINA5��Ϊ���߸��任
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;       // Enable SEQ1 interrupt (every EOS)

    AdcRegs.ADCTRL2.all = 0x2000;       //�����SEQ1λд1����ת������
}




