/*
 * pwm.h
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */

#ifndef _PWM_H_
#define _PWM_H_

//PWM宏定义
#define EPWM1_TIMER_TBPRD  2000  // Period register 这个需要改！！7500对应13Hz？？？？   2000对应50hz???  1000对应100hz
#define EPWM1_MAX_CMPA     1995
#define EPWM1_MIN_CMPA        5
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB       50
#define EPWM2_TIMER_TBPRD  2000  // Period register 这个需要改！！
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50
#define EPWM3_TIMER_TBPRD  2000  // Period register 这个需要改！！
#define EPWM3_MAX_CMPA     1950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB       50
#define EPWM4_TIMER_TBPRD  2000  // Period register 这个需要改！！
#define EPWM4_MAX_CMPA     1950
#define EPWM4_MIN_CMPA       50
#define EPWM4_MAX_CMPB     1950
#define EPWM4_MIN_CMPB       50
#define EPWM5_TIMER_TBPRD  2000  // Period register 这个需要改！！
#define EPWM5_MAX_CMPA     1950
#define EPWM5_MIN_CMPA       50
#define EPWM5_MAX_CMPB     1950
#define EPWM5_MIN_CMPB       50

void PWM_Init();
void InitEPwm1Example();
void InitEPwm2Example();
void InitEPwm3Example();
void InitEPwm4Example();
void InitEPwm5Example();



#endif /* _PWM_H_ */
