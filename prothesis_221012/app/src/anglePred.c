/*
 * anglePred.c
 *
 *  Created on: 2022年10月8日
 *      Author: sdlm
 *    Describe: 角度预测相关函数
 */

#include "anglePred.h"
#include "math.h"

// 角度预测需要的变量
int predict_flag = 0;

float rshank_euler_y[N] = {0.0}, rthigh_euler_y[N] = {0.0};
float pelvic_acc_z[N] = {0.0};
float t[N] = {0.0};
// 角度预测相关
int point_num = 0;

// 二项式拟合的中间变量
float pow_x[N][3];

// 二项式拟合相关矩阵，默认二阶
static float matFunX[3][3] = {0.0};
static float matFunY[3][1] = {0.0};


// 3*3的矩阵乘法
static void mat_mult_33(float Dst[3][3], float SrcA[3][3], float SrcB[3][3])
{
    int i, j, k;

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			for(k = 0; k < 3; k++)
			{
				Dst[i][j] += SrcA[i][k] * SrcB[k][j];
			}
		}
	}
}

// 坐标转换
// 目前默认只返回转换后的z方向
float transCoods(float *acc, float *q)
{
    float sita, gamma, psi;
    float acc_z;
    int n;
    sita = q[0] * DEG2RAD;
    gamma = q[1] * DEG2RAD;
    psi = q[2] * DEG2RAD;
    
    float c1[3][3] = {{1, 0, 0},
                       {0, cos(sita), sin(sita)},
                       {0, -sin(sita), cos(sita)}};
    float c2[3][3] = {{cos(gamma), 0, -sin(gamma)},
                       {0, 1, 0},
                       {sin(gamma), 0, cos(gamma)}};
    float c3[3][3] = {{cos(psi), sin(psi), 0},
                       {-sin(psi), cos(psi), 0},
                       {0, 0, 1}};
    float ctmp[3][3] = {0.0};
    float c_n[3][3] = {0.0};
    mat_mult_33(ctmp, c1, c2);
    mat_mult_33(c_n, ctmp, c3);

    for(n = 0; n < 3; n++)
    {
        acc_z += c_n[n][2] * acc[n];
    }

    acc_z = acc_z * 9.8 - 9.8;

    return acc_z;

}


// 二阶多项式的拟合
void getCoeff(float *coeff, float *x, float *y, int size, int n)
{
    float sum;
    int i, j, k;
    //正规方程X
    for(i = 0; i <= n; i++)
    {
        for(j = 0; j <= n; j++)
        {
            int tmp = i + j;
            sum = 0;

//            for(k = 0; k < size; k++) sum += pow(x[k], j + i);
            for(k = 0; k < size; k++) {
                if(tmp == 0) sum += 1.0;
                else if(tmp == 1) sum += x[k];
                else sum += pow_x[k][tmp - 2];
            }
            matFunX[i][j] = sum;
        }
    }
    //正规方程Y
    for(i = 0; i <= n; i++)
    {
        sum = 0;
//        for(k = 0; k < size; k++) sum += y[k] * pow(x[k], i);
        for(k = 0; k < size; k++) {
            if(i == 0) sum += y[k];
            else if(i == 1) sum += y[k] * x[k];
            else sum += y[k] * pow_x[k][i - 2];
        }
        matFunY[i][0] = sum;
    }
    //矩阵行列式变换
    float num1, num2, ratio;

    for(i = 0; i < n; i++)
    {
        num1 = matFunX[i][i];
        for(j = i + 1; j <= n; j++)
        {
            num2 = matFunX[j][i];
            ratio = num2 / num1;
            for(k = 0; k <= n; k++) matFunX[j][k] = matFunX[j][k] - matFunX[i][k] * ratio;
            matFunY[j][0] = matFunY[j][0] - matFunY[i][0] * ratio;
        }

    }
    //计算拟合曲线的系数
    for(i = n; i >= 0; i--)
    {
        if(i == n) coeff[i] = matFunY[i][0] / matFunX[i][i];
        else
        {
            for(j = i + 1; j <= n; j++) matFunY[i][0] = matFunY[i][0] - coeff[j] * matFunX[i][j];
            coeff[i] = matFunY[i][0] / matFunX[i][i];
        }
    }
}




