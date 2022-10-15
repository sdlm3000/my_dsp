/*
 * anglePred.h
 *
 *  Created on: 2022年10月8日
 *      Author: sdlm
 *    Describe: 角度预测相关函数
 */

#ifndef APP_INC_ANGLEPRED_H_
#define APP_INC_ANGLEPRED_H_


#define N           256     // 每个步态周期的最大点数

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
//#define L_1     0.41                    // 大腿长度
//#define L_2     0.43                    // 小腿长度
//#define L_3     0.12                    // 踝与足跟矢的状面的距离
//#define L_4     0.28                    // 足跟与足尖的矢状面的距离
//#define L_5     0.07                    // 足跟与踝的矢状面的水平距离
//#define L_6     0.22                    // 踝与足尖的矢状面的距离
//#define BETA_1  0.6228266               // 踝与足跟的垂直面夹角
//#define BETA_2  1.268131                // 踝与足尖的垂直面夹角

// sdlm
#define L_1     0.48                    // 大腿长度
#define L_2     0.43                    // 小腿长度
#define L_3     0.13                    // 踝与足跟矢的状面的距离
#define L_4     0.28                    // 足跟与足尖的矢状面的距离
#define L_5     0.09                    // 足跟与踝的矢状面的水平距离
#define L_6     0.208                   // 踝与足尖的矢状面的距离
#define BETA_1  0.764682                // 踝与足跟的垂直面夹角
#define BETA_2  1.151711                // 踝与足尖的垂直面夹角


float transCoods(float *acc, float *q);
void getCoeff(float *coeff, float *x, float *y, int size, int n);


#endif /* APP_INC_ANGLEPRED_H_ */
