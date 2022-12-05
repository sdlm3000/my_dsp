/*
 * arc.h
 *
 *  Created on: 2022年10月1日
 *      Author: sdlm
 *    Describe: 自适应鲁棒控制器
 */

#include "common.h"
#include "arc.h"

const static float lim_max[3] = {400, 100, 300};    // 自适应参数最大值
const static float lim_min[3] = {-150, -100, -3000};// 自适应参数最小值
const static float w[3] = {20, 0.001, 200};         // 加权因子
const static float k = 1;                           // 鲁棒控制比例系数
static float thita[3] = {-80.8, 20, 20};            // 自适应参数
static float dthita_last[3];
static float dthita[3];



float arc_controller(float q, float q_h, float dq_h, float interval)
{
    float e, u_a, u_s, u_out;
    Uint16 i;

    // 输出控制率计算
    e = q - q_h;        // 角度误差
    if(thita[1] == 0) thita[1] = 0.1;
    u_a = -1 / thita[1] * (thita[0] * q + thita[2] - dq_h);  // Ua计算
    u_s = k * e;        // Us计算
    u_out = u_a + u_s;

    // 更新自适应参数的变化率
    dthita_last[0] = dthita[0];
    dthita_last[1] = dthita[1];
    dthita_last[2] = dthita[2];
    dthita[0] = w[0] * q * e;
    dthita[1] = w[1] * u_a * e;
    dthita[2] = w[2] * e;

    // 非连续映射
    for(i = 0; i < 3; i++)
    {
        if(thita[i] >= lim_max[i] && dthita[i] > 0){
            dthita[i] = 0;
            dthita_last[i] = 0;
        }
        else if(thita[i] <= lim_min[i] && dthita[i] < 0){
            dthita[i] = 0;
            dthita_last[i] = 0;
        }
        // 积分，更新自适应参数
        thita[i] = thita[i] + (dthita[i] + dthita_last[i]) * interval / 2;
    }
    return u_out;
}

