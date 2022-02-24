/*
 * prothesis.h
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */

#ifndef _PROTHESIS_H_
#define _PROTHESIS_H_

#define AVG        8  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset

#define BUF_SIZE   5  // Sample buffer size

#define maxspeed  1999//MOSFET驱动的第二版电路时1999  光耦版本是0
#define minspeed 0
#define gain1 12.3
#define gain2 12.9
#define gain3 12.8

#define PI        3.1415926

//#define k1_cp     -1.9959
//#define k2_cp     -0.5562
//#define b_cp      -0.0886
//#define xita_cp   -3.9522
//
//#define k1_cd     -24.1096
//#define k2_cd     0.5893
//#define b_cd      -2.932
//#define xita_cd   4.9019
//
//#define k1_pp     -20.1343
//#define k2_pp     -0.6847
//#define b_pp      0.9591
//#define xita_pp   3.3376

//#define k1_cp   -3.8686
//#define k2_cp   1.4658
//#define b_cp    -0.0849
//#define xita_cp -0.3699
//
//#define k1_cd   -14.1197
//#define k2_cd   -0.8992
//#define b_cd    -0.8468
//#define xita_cd 7.323
//
//#define k1_pp   -14.4454
//#define k2_pp   -0.5698
//#define b_pp    0.4452
//#define xita_pp 6.1017

//#define k1_cp   -3.8686
//#define k2_cp   1.4658
//#define b_cp    -0.0422
//#define xita_cp -0.3699
//
//#define k1_cd   -14.1197
//#define k2_cd   -0.8992
//#define b_cd    -0.4210
//#define xita_cd 7.323
//
////#define k1_c   -9.6307
////#define k2_c   -1.082
////#define b_c    -0.0788
////#define xita_c 6.0822
//
//#define k1_pp   -14.4454
//#define k2_pp   -0.5698
//#define b_pp    0.2213
//#define xita_pp 6.1017


// ***************SUP
//#define k1_cp   -3.0
//#define k2_cp   0
//#define b_cp    -0.04
//#define xita_cp -1
//
//#define k1_cd   -5.0
//#define k2_cd   0
//#define b_cd    -0.04
//#define xita_cd 0
//
//#define k1_pp   -5        //  2km
//#define k2_pp   0
//#define b_pp    -0.01
//#define xita_pp -16

/****** 数据 TXY *******
#define k1_cp   -6.4823
#define k2_cp   1.8926
#define b_cp    -6.2498
#define xita_cp -1

#define k1_cd   -0.9373
#define k2_cd   -0.5822
#define b_cd    7.8374
#define xita_cd 0

#define k1_pp   5.0616        //  2km
#define k2_pp   -0.3843
#define b_pp    3.0588
#define xita_pp -13
****/
/****** 数据 TXY 1122*******/
#define k1_cp   -0.82
#define k2_cp   0.31
#define b_cp    -0.089
#define xita_cp -3

#define k1_cd   -8.1
#define k2_cd   1.51
#define b_cd    -1.19
#define xita_cd 0

#define k1_pp   -2.95        //  2km
#define k2_pp   -0.196
#define b_pp    0.05
#define xita_pp -13


#define m_A       0.11
#define A_p       1.7584e-4

#define L_BD      5.614e-2
#define L_AB      2e-2
#define d_x       4.6e-2
#define d_y       0.175

//有限状态机阈值的定义
#define v_leg_yz    4
#define zd_press1H  80
#define zd_press2H  80
#define zd_press1L  50
#define zd_press2L  50
#define angle_push  115

#define P_max       18             //高压区油液压力的最大值
#define P_min       0.1           //低压区油液压力的最小值

#define yuzhi_pwm        1300
#define P_high           14

#define Kp_2    100.0
#define Kp      50.0

#define xishu   1.2

#define max_valve4 300
#define max_valve3 1600
#define min_valve2 1999


void valveinit(void);
void PULL(void);                //收腿
void PUSH(void);                //蹬腿
void MIDDLE(void);
void MIDDLE_zhi(void);          //被动跖屈
void MIDDLE_bei(void);          //被动跖屈
void PUSH_control(void);
void UNLOAD(void);              //泄压

void motor_run(void);           //电机运行
void motor_stop(void);          //电机停止


#endif /* _PROTHESIS_H_ */
