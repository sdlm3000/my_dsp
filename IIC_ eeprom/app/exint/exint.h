/*
 * exint.h
 *
 *  Created on: 2021��6��3��
 *      Author: 14472
 */

#ifndef EXINT_H_
#define EXINT_H_


#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�


void EXINT1_Init(void);
interrupt void EXINT1_IRQn(void);

void EXINT2_Init(void);
interrupt void EXINT2_IRQn(void);



#endif /* EXINT_H_ */




