/*
 * anglePred.h
 *
 *  Created on: 2022��10��8��
 *      Author: sdlm
 *    Describe: �Ƕ�Ԥ����غ���
 */

#ifndef APP_INC_ANGLEPRED_H_
#define APP_INC_ANGLEPRED_H_


#define N           256     // ÿ����̬���ڵ�������

#define PELVIC      0x55
#define RTHIGH      0x53
#define RSHANK      0x54
#define RFOOT       0x51
#define LTHIGH      0x50
#define LSHANK      0x56
#define LFOOT       0x52

#define DEG2RAD     0.0174539
#define PI          3.1415926

//// txy
//#define L_1     0.41                    // ���ȳ���
//#define L_2     0.43                    // С�ȳ���
//#define L_3     0.12                    // �������ʸ��״��ľ���
//#define L_4     0.28                    // ���������ʸ״��ľ���
//#define L_5     0.07                    // ������׵�ʸ״���ˮƽ����
//#define L_6     0.22                    // ��������ʸ״��ľ���
//#define BETA_1  0.6228266               // ��������Ĵ�ֱ��н�
//#define BETA_2  1.268131                // �������Ĵ�ֱ��н�

// sdlm
#define L_1     0.48                    // ���ȳ���
#define L_2     0.43                    // С�ȳ���
#define L_3     0.13                    // �������ʸ��״��ľ���
#define L_4     0.28                    // ���������ʸ״��ľ���
#define L_5     0.09                    // ������׵�ʸ״���ˮƽ����
#define L_6     0.208                   // ��������ʸ״��ľ���
#define BETA_1  0.764682                // ��������Ĵ�ֱ��н�
#define BETA_2  1.151711                // �������Ĵ�ֱ��н�


float transCoods(float *acc, float *q);
void getCoeff(float *coeff, float *x, float *y, int size, int n);


#endif /* APP_INC_ANGLEPRED_H_ */
