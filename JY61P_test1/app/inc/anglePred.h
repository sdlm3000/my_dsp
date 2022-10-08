/*
 * anglePred.h
 *
 *  Created on: 2022��10��8��
 *      Author: sdlm
 *    Describe: �Ƕ�Ԥ����غ���
 */

#ifndef APP_INC_ANGLEPRED_H_
#define APP_INC_ANGLEPRED_H_

#define DEG2RAD     0.0174539

#define L_1     0.41                    // ���ȳ���
#define L_2     0.43                    // С�ȳ���
#define L_3     0.12                    // �������ʸ��״��ľ���
#define L_4     0.28                    // ���������ʸ״��ľ���
#define L_5     0.07                    // ������׵�ʸ״���ˮƽ����
#define L_6     0.22                    // ��������ʸ״��ľ���
#define BETA_1  asin(L_5 / L_3)         // ��������Ĵ�ֱ��н�
#define BETA_2  asin((L_4 - L_5) / L_6) // �������Ĵ�ֱ��н�

void getCoeff(float *coeff, float *x, float *y, int size, int n);


#endif /* APP_INC_ANGLEPRED_H_ */
