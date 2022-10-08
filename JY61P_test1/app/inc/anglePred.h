/*
 * anglePred.h
 *
 *  Created on: 2022年10月8日
 *      Author: sdlm
 *    Describe: 角度预测相关函数
 */

#ifndef APP_INC_ANGLEPRED_H_
#define APP_INC_ANGLEPRED_H_

#define DEG2RAD     0.0174539

#define L_1     0.41                    // 大腿长度
#define L_2     0.43                    // 小腿长度
#define L_3     0.12                    // 踝与足跟矢的状面的距离
#define L_4     0.28                    // 足跟与足尖的矢状面的距离
#define L_5     0.07                    // 足跟与踝的矢状面的水平距离
#define L_6     0.22                    // 踝与足尖的矢状面的距离
#define BETA_1  asin(L_5 / L_3)         // 踝与足跟的垂直面夹角
#define BETA_2  asin((L_4 - L_5) / L_6) // 踝与足尖的垂直面夹角

void getCoeff(float *coeff, float *x, float *y, int size, int n);


#endif /* APP_INC_ANGLEPRED_H_ */
