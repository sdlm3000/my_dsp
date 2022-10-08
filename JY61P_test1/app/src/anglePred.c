/*
 * anglePred.c
 *
 *  Created on: 2022��10��8��
 *      Author: sdlm
 *    Describe: �Ƕ�Ԥ����غ���
 */

#include "anglePred.h"
#include "math.h"




// 3*3�ľ���˷�
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

// ����ת��
// ĿǰĬ��ֻ����ת�����z����
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

// Ĭ�϶���
float matFunX[3][256] = {0.0};
float matFunY[3][1] = {0.0};
// ���׶���ʽ�����
void getCoeff(float *coeff, float *x, float *y, int size, int n)
{
    float sum;
    int i, j, k;
    //���淽��X
    for(i = 0; i <= n; i++)
    {
        for(j = 0; j <= n; j++)
        {
            sum = 0;
            for(k = 0; k < size; k++) sum += pow(x[k], j + i);
            matFunX[i][j] = sum;
        }
    }
    //���淽��Y
    for(i = 0; i <= n; i++)
    {
        sum = 0;
        for(k = 0; k < size; k++) sum += y[k] * pow(x[k], i);
        matFunY[i][0] = sum;
    }
    //��������ʽ�任
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
    //����������ߵ�ϵ��
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




