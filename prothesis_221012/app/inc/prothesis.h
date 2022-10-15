/*
 * prothesis.h
 *
 *  Created on: Feb 19, 2022
 *      Author: sdlm
 */

#ifndef _PROTHESIS_H_
#define _PROTHESIS_H_

#define BUF_SIZE    10  // Sample buffer size

#define MAX_SPEED   1999//MOSFET驱动的第二版电路时1999  光耦版本是0
#define MIN_SPEED   0

// 以第一个传感器的K值为基准，在卸载态，标定后两个传感器的K值
#define K_GAIN1     16.50
#define K_GAIN2     16.99
#define K_GAIN3     16.75
#define B_GAIN      -4.98
#define PI          3.1415926

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

// PID控制器的参数
#define Kp      50.0

// 力矩计算的调整增益
#define T_GAIN  1.2


/* 假肢的控制输出状态 */
enum pro_control
{
    NO_CONTROL_OUT          = 0x00,     /*!< 不输出 */
    CONTROL_OUT             = 0x01,     /*!< 输出 */
    CONTROL_RESET           = 0x02,     /*!< 控制初始化 */
};

/* 假肢的步态 */
enum prothesis_state
{
    UNLOAD_STATE            = 0x00,     /*!< 卸荷态 */
    MIDDLE_ZHI_STATE        = 0x01,     /*!< 被动跖曲 */
    MIDDLE_BEI_STATE        = 0x02,     /*!< 被动背曲 */
    PUSH_STATE              = 0x03,     /*!< 主动跖曲 */
    PULL_STATE              = 0x04,     /*!< 主动背曲 */
};

/*********  步态状态变量 *********/
extern int proState; 

extern int motorState;

/********* 假肢的控制量 *********/
extern int valve_pwm_middle_zhi;
extern int valve_pwm_middle_bei;
extern int valve_pwm_push;
extern int middle_zhi_controlFlag;

/********* 用于蓝牙修改的变量 *********/
extern float ANGLE_MIDDLE_PUSH;
extern float ANGLE_PUAH_PULL;   
extern float ANGLE_PULL_MIDDLE;
extern float ANGLE_ZHI_BEI;
extern float P_MAX;
extern float P_HIGH;
extern int CONTROL_FLAG;
extern int MIN_VALVE_2;
extern int MAX_VALVE_3;
extern int MAX_VALVE_4;


/********* 假肢的传感器参数 *********/
extern float a[3], w[3], Angle[3];
extern double P_foot1, P_foot2, P_foot3;
extern double press1, press2, press3;
extern double angle;



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
