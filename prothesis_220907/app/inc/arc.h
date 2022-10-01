/*
 * arc.h
 *
 *  Created on: 2022年10月1日
 *      Author: sdlm
 *    Describe: 自适应鲁棒控制器
 */

#ifndef APP_INC_ARC_H_
#define APP_INC_ARC_H_


float arc_controller(float q, float q_h, float dq_h, float interval);


#endif /* APP_INC_ARC_H_ */
