/*
 * arc.h
 *
 *  Created on: 2022��10��1��
 *      Author: sdlm
 *    Describe: ����Ӧ³��������
 */

#include "common.h"
#include "arc.h"

const static float lim_max[3] = {400, 100, 300};    // ����Ӧ�������ֵ
const static float lim_min[3] = {-150, -100, -3000};// ����Ӧ������Сֵ
const static float w[3] = {20, 0.001, 200};         // ��Ȩ����
const static float k = 1;                           // ³�����Ʊ���ϵ��
static float thita[3] = {-80.8, 20, 20};            // ����Ӧ����



float arc_controller(float q, float q_h, float dq_h, float interval)
{
    float e, u_a, u_s, u_out;
    Uint16 i;

    // ��������ʼ���
    e = q - q_h;        // �Ƕ����
    u_a = 1 / thita[1] * (thita[0] * q + thita[2] - dq_h);  // Ua����
    u_s = k * e;        // Us����
    u_out = u_a + u_s;

    // ��������Ӧ��
    float dthita[3];
    dthita[0] = w[0] * q * e;
    dthita[1] = w[1] * u_a * e;
    dthita[2] = w[2] * e;

    // ������ӳ��
    for(i = 0; i < 3; i++)
    {
        if(thita[i] >= lim_max[i] && dthita[i] > 0){
            dthita[i] = 0;
        }
        else if(thita[i] <= lim_min[i] && dthita[i] < 0){
            dthita[i] = 0;
        }
        // ����
        thita[i] += dthita[i] * interval;
    }
    return u_out;
}

