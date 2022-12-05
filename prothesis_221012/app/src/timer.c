/*
 * timer.c
 *
 *  Created on: 2022年1月25日
 *      Author: sdlm
 */

#include "timer.h"
#include "uart.h"
#include "math.h"
#include "prothesis.h"
#include "jy61p.h"
#include "stdio.h"
#include "adc.h"
#include "arc.h"
#include "anglePred.h"

extern double Beta, Beta_last;
extern double W_BD;
extern double Alpha, Alpha_cos, Alpha_sin;
extern double F_A, T;
extern double delta_P, delta_P0, dd_P;

extern float pelvic_acc_z[N], t[N];
extern float rshank_euler_y[N], rthigh_euler_y[N];

extern float velo_pred[N], angle_pred[N];
extern int walk_cycle;
extern int point_num, predict_flag;
//extern float dt;


// ADC采样相关取平均值数组
static Uint32 sum1 = 0, sum2 = 0,sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
static Uint16 SampleTable1[BUF_SIZE];
static Uint16 SampleTable2[BUF_SIZE];
static Uint16 SampleTable3[BUF_SIZE];
static Uint16 SampleTable4[BUF_SIZE];
static Uint16 SampleTable5[BUF_SIZE];
static Uint16 SampleTable6[BUF_SIZE];
static Uint16 SampleTable7[BUF_SIZE];

// 压力过高的次数，连续超过一定次数才会出发过压保护，防止误触发
static int counter_P = 0;

// 踝关节角度平均值
static double angle_ave[5] = {0.0};

static float a_pel[3], ang_pel[3];
static float ang_rsh[3];
static float ang_rth[3];
static float a_rfo[3], ang_rfo[3];
static float ang_lsh[3];
static float ang_lth[3];
static float a_lfo[3], ang_lfo[3];
static float ang_rsh_off = 0.0, ang_rth_off = 0.0;
static float interval = 0.001;

// 被动背曲的起始索引
static int index_bei = 0, start_bei = 0;

static unsigned char chrTemp[30];

// 发送数据标志位
int DATA_SEND = 1;
//int predict_flag = 0;
//// 角度预测相关
//int point_num = 0;

// 残肢腿使用的预测数据
static int walk_state = 0, begin_flag = 0, index = 0;
int last_walk_cycle, walk_index;
float velo_pred_buf[N], angle_pred_buf[N];
int point_num_buf;

//定时器0初始化函数
//Freq：CPU时钟频率（150MHz）
//Period：定时周期值，单位us
void TIM0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    EDIS;

    //设置定时器0的中断入口地址为中断向量表的INT0
    EALLOW;
    PieVectTable.TINT0 = &TIM0_IRQn;
    EDIS;

    //指向定时器0的寄存器地址
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    //设置定时器0的周期寄存器值
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    //设置定时器预定标计数器值为0
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    //确保定时器0为停止状态
    CpuTimer0Regs.TCR.bit.TSS = 1;
    //重载使能
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    //开始定时器功能
    CpuTimer0Regs.TCR.bit.TSS=0;
    //开启CPU第一组中断并使能第一组中断的第7个小中断，即定时器0
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

}

void TIM1_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
    EDIS;

    EALLOW;
    PieVectTable.XINT13 = &TIM1_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    // Initialize timer period to maximum:
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer1Regs.TPR.all  = 0;
    CpuTimer1Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer1Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer1Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer1.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer1, Freq, Period);

    CpuTimer1Regs.TCR.bit.TSS=0;

    IER |= M_INT13;

}

static Uint16 buf[64] = {};
interrupt void TIM0_IRQn(void)
{
    int i;
    double temp;
    float T_pred;
    CpuTimer0.InterruptCount++;
    AdcRegs.ADCTRL2.all = 0x2000;   // 软件向SEQ1位写1开启转换触发
    // ADC电压转换，10次采样取平均值
    for (i = 0; i < AVG; i++)  {
        while (AdcRegs.ADCST.bit.INT_SEQ1 == 0) {}  // Wait for interrupt如何发起中断
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable1[i] = (AdcRegs.ADCRESULT0 >> 4);
        SampleTable2[i] = (AdcRegs.ADCRESULT1 >> 4);
        SampleTable3[i] = (AdcRegs.ADCRESULT2 >> 4);
        SampleTable4[i] = (AdcRegs.ADCRESULT3 >> 4);
        SampleTable5[i] = (AdcRegs.ADCRESULT4 >> 4);
        SampleTable6[i] = (AdcRegs.ADCRESULT5 >> 4);
        SampleTable7[i] = (AdcRegs.ADCRESULT6 >> 4);
        sum1 = sum1 + SampleTable1[i];
        sum2 = sum2 + SampleTable2[i];
        sum3 = sum3 + SampleTable3[i];
        sum4 = sum4 + SampleTable4[i];
        sum5 = sum5 + SampleTable5[i];
        sum6 = sum6 + SampleTable6[i];
        sum7 = sum7 + SampleTable7[i];
    }

    // 转换获得液压压力
    press1 = (((double)sum1) * 3 / 4096 / AVG) * K_GAIN1 + B_GAIN;
    press2 = (((double)sum2) * 3 / 4096 / AVG) * K_GAIN2 + B_GAIN;
    press3 = (((double)sum3) * 3 / 4096 / AVG) * K_GAIN3 + B_GAIN;

    // 内部液压压力限位
    if(press1 >= P_MAX || press1 <= 0) {
        counter_P++;
        if(counter_P >= 15) {
            motor_stop();
            counter_P = 0;
        }
    }
    // 转换获得踝关节角度
    angle = ((((double)sum4) * 3 / 4096 / AVG) * 100);//采样的实际电压 0-3v
    // 20220910  蹬腿极限 -> -12.70  收腿极限 -> 24.6
    angle = -165.4 + angle;
    temp = 0;
    for(i = 0; i < 4; i++) {
        angle_ave[i] = angle_ave[i + 1];
        temp = temp + angle_ave[i];
    }
    angle_ave[4] = angle;
    angle= (temp + angle_ave[4]) / 5; // 5点滑动平均

    // 转换获得足底压力
    P_foot1 = ((double)sum5) * 3 / 4096 / AVG; //采样的实际电压 0-3v
    P_foot2 = ((double)sum6) * 3 / 4096 / AVG; //采样的实际电压 0-3v
    P_foot3 = ((double)sum7) * 3 / 4096 / AVG; //采样的实际电压 0-3v

    Beta_last = Beta;
    Beta = angle;

//    W_BD = (Beta - Beta_last) * PI / 180.0 / 0.005;   // 定时周期为5ms
    W_BD = (Beta - Beta_last) / 0.01;   // 定时周期为5ms
    delta_P0 = press2 - press3;
    if(CpuTimer0.InterruptCount >= 10) {
        Alpha_cos = (L_BD * cos(Beta * PI / 180.0) - d_x) / L_AB;
        Alpha_sin = sqrt(1 - Alpha_cos * Alpha_cos);
        // 步骤4：求解T，F_A
        if(proState == MIDDLE_ZHI_STATE) {
            // 由于跖曲阶段比较短，目前这个部分默认使用原来的控制算法
            temp = Beta - xita_cp;
            T = (k1_cp * temp + k2_cp * temp * temp + b_cp * W_BD) * T_GAIN;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90.0 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // 求解delta_P
            delta_P = (F_A) / A_p / 1000000;
            dd_P = delta_P0 - delta_P;

            // 触地才开始进行PID控制
            if(P_foot3 > 0.5) middle_zhi_controlFlag = 1;
            if(middle_zhi_controlFlag == 1) {
                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
                if(valve_pwm_middle_zhi < MIN_SPEED) {
                    valve_pwm_middle_zhi = MIN_SPEED;
                }
                else if(valve_pwm_middle_zhi > MAX_VALVE_4) {
                    valve_pwm_middle_zhi = MAX_VALVE_4;
                }
            }
        }
        else if(proState == MIDDLE_BEI_STATE) {
            // 使用自适应鲁棒控制算法
            //
            if(walk_cycle < 2) {       // 第一个步态可能出现无预测角度的情况，控制律给默认值
                valve_pwm_middle_bei = MAX_VALVE_3 - 350;
            }
            else {
                float angle_in, velo_in;
                if(start_bei % 2 == 0) {
                    angle_in = angle_pred_buf[index_bei];
                    velo_in = velo_pred_buf[index_bei];
                }
                else {
                    if(index_bei + 1 < point_num_buf) index_bei++;
                    angle_in = (angle_pred_buf[index_bei - 1] + angle_pred_buf[index_bei]) / 2;
                    velo_in = (velo_pred_buf[index_bei - 1] + velo_pred_buf[index_bei]) / 2;        
                }
                T_pred = arc_controller(angle, angle_in, velo_in, interval);
                // 求解delta_P
                Alpha = acos(Alpha_cos);
                F_A = T_pred / L_BD * cos((90.0 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
                delta_P = (F_A) / A_p / 1000000;
                dd_P = delta_P0 - delta_P;
                valve_pwm_middle_bei = valve_pwm_middle_bei + (int)(Kp * dd_P);
                start_bei++;
            }
            if(valve_pwm_middle_bei < MIN_VALVE_3) {
                valve_pwm_middle_bei = MIN_VALVE_3;
            }
            else if(valve_pwm_middle_bei > MAX_VALVE_3) {
                valve_pwm_middle_bei = MAX_VALVE_3;
            }
        }
        else if(proState == PUSH_STATE)
        {
            valve_pwm_push = MAX_SPEED;

        }
        else if(proState == PULL_STATE) {
            if(walk_cycle != last_walk_cycle) {
                float angle_min = 10000.0;
                walk_index = 0;
                point_num_buf = point_num;
                for(i = 0; i < point_num_buf; i++) {
                    angle_pred_buf[i] = angle_pred[i];
                    velo_pred_buf[i] = velo_pred[i];
                    if(i < 60 && angle_pred_buf[i] < angle_min){
                        angle_min = angle_pred_buf[i];
                        index_bei = i;
                        start_bei = 0;
                    }
                }
            }
            last_walk_cycle = walk_cycle;
        }
    }

    // 发送数据至上位机
//    if(CpuTimer0.InterruptCount % 2 == 0 && DATA_SEND == 1)
    if(CpuTimer0.InterruptCount % 2 == 0)
    {
        int tmp_pwm = 0;
        if(proState == MIDDLE_ZHI_STATE) tmp_pwm = valve_pwm_middle_zhi;
        else if(proState == MIDDLE_BEI_STATE) tmp_pwm = valve_pwm_middle_bei;
        else if(proState == PUSH_STATE) tmp_pwm = valve_pwm_push;
        Uint16 dataLen = sprintf(buf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %d %d %d %d\r\n", angle, press1, press2, press3,
                                P_foot1, P_foot2, P_foot3, proState, motorState, tmp_pwm, CONTROL_FLAG);
        usartb_sendData(buf, dataLen);
    }

    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    sum5 = 0;
    sum6 = 0;
    sum7 = 0;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    EDIS;
}

interrupt void TIM1_IRQn(void)
{
    float angle_tmp;
//    Uint8 data_buf[10] = {};
//    Uint16 data_Len = 0;
    // 轮训接收各个IMU的数据
    // 7个IMU，但是为了整体周期性，按10个一次轮训
    scib_msg("1 ");
    if(I2C_ReadData(PELVIC, &chrTemp[0], AX, 24) == 0) {
        a_pel[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_pel[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_pel[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_pel[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_pel[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_pel[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scib_float(a_pel[0]);
        scib_xmit(' ');
        scib_float(a_pel[1]);
        scib_xmit(' ');
        scib_float(a_pel[2]);
        scib_xmit(' ');
        scib_float(ang_pel[0]);
        scib_xmit(' ');
        scib_float(ang_pel[1]);
        scib_xmit(' ');
        scib_float(ang_pel[2]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);

    scib_msg("2 ");
    if(I2C_ReadData(RTHIGH, &chrTemp[0], AX, 24) == 0) {
        ang_rth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scib_float(ang_rth[1]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);
    scib_msg("3 ");
    if(I2C_ReadData(RSHANK, &chrTemp[0], AX, 24) == 0) {
        ang_rsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scib_float(ang_rsh[1]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);
    scib_msg("4 ");
    if(I2C_ReadData(RFOOT, &chrTemp[0], AX, 24) == 0) {
        a_rfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_rfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_rfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_rfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_rfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_rfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scib_float(a_rfo[0]);
        scib_xmit(' ');
        scib_float(a_rfo[1]);
        scib_xmit(' ');
        scib_float(a_rfo[2]);
        scib_xmit(' ');
        scib_float(ang_rfo[0]);
        scib_xmit(' ');
        scib_float(ang_rfo[1]);
        scib_xmit(' ');
        scib_float(ang_rfo[2]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);
    scib_msg("5 ");
    if(I2C_ReadData(LTHIGH, &chrTemp[0], AX, 24) == 0) {
        ang_lth[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scib_float(ang_lth[1]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);
    scib_msg("6 ");
    if(I2C_ReadData(LSHANK, &chrTemp[0], AX, 24) == 0) {
        ang_lsh[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        scib_float(ang_lsh[1]);
        scib_msg("\r\n");
    }
    // DELAY_US(100);
    scib_msg("7 ");
    if(I2C_ReadData(LFOOT, &chrTemp[0], AX, 24) == 0) {
        a_lfo[0] = (float)CharToShort(&chrTemp[0]) / ACC_TRANS;
        a_lfo[1] = (float)CharToShort(&chrTemp[2]) / ACC_TRANS;
        a_lfo[2] = (float)CharToShort(&chrTemp[4]) / ACC_TRANS;
        ang_lfo[0] = (float)CharToShort(&chrTemp[18]) / ANGLE_TRANS;
        ang_lfo[1] = (float)CharToShort(&chrTemp[20]) / ANGLE_TRANS;
        ang_lfo[2] = (float)CharToShort(&chrTemp[22]) / ANGLE_TRANS;
        scib_float(a_lfo[0]);
        scib_xmit(' ');
        scib_float(a_lfo[1]);
        scib_xmit(' ');
        scib_float(a_lfo[2]);
        scib_xmit(' ');
        scib_float(ang_lfo[0]);
        scib_xmit(' ');
        scib_float(ang_lfo[1]);
        scib_xmit(' ');
        scib_float(ang_lfo[2]);
        scib_msg("\r\n");
    }


    // 寻找步态周期的起始和结束，同时存放预测算法需要的数据
    // 默认每次都能正常获取到IMU数据
    if(predict_flag == 0)
    {
        predict_flag = 0;
        if(walk_state == 0) {
            // 起始静置200个周期后，得到一些角度的偏移量
            begin_flag++;
            if(begin_flag > 50) {
                ang_rth_off = ang_rth[1];
                ang_rsh_off = ang_rsh[1];
                walk_state = 1;
            }
        }
        else if(walk_state == 1 || walk_state == 4) {  
            // 利用右小腿，来判断步态的起始和终止
            angle_tmp = ang_rsh[1] - ang_rsh_off;
            if((ang_rsh[1] > (ang_rsh_off + 1)) || walk_state == 4) {
                walk_state = 2;
                rshank_euler_y[index] = angle_tmp * DEG2RAD;
                rthigh_euler_y[index] = (ang_rth[1] - ang_rth_off) * DEG2RAD;
                pelvic_acc_z[index] = transCoods(a_pel, ang_pel);
                if(index < N - 1) index++;
            }
        }
        else if(walk_state == 2 || walk_state == 3) {
            rshank_euler_y[index] = (ang_rsh[1] - ang_rsh_off) * DEG2RAD;
            rthigh_euler_y[index] = (ang_rth[1] - ang_rth_off) * DEG2RAD;
            pelvic_acc_z[index] = transCoods(a_pel, ang_pel);
            if(walk_state == 2 && (ang_rsh[1] < (ang_rsh_off - 1))) walk_state = 3;
            else if((walk_state == 3 && ang_rsh[1] > (ang_rsh_off + 1))) {
                walk_state = 4;
                point_num = index + 1;
                index = 0;
                predict_flag = 1;
            }
            if(index != 0 && index < N - 1) index++;
        }
    }
    scib_msg("w ");
    scib_int(walk_state);
    scib_msg(" ");
    scib_int(index);
    scib_msg(" ");
    scib_int(point_num);
    scib_msg(" ");
    scib_int(predict_flag);
    scib_msg("\r\n");

}



void TIM2_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2
    EDIS;

    EALLOW;
    PieVectTable.TINT2 = &TIM2_IRQn;
    EDIS;

    // Initialize address pointers to respective timer registers:
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    // Initialize timer period to maximum:
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer2.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer2, Freq, Period);

    CpuTimer2Regs.TCR.bit.TSS=0;

    IER |= M_INT14;

}

// int tim2_cnt = 0;
interrupt void TIM2_IRQn(void)
{
    // 暂时用于测试保留下来
    // tim2_cnt++;
    // if(tim2_cnt >= 1000) {
    //     tim2_cnt = 0;
    //     scib_msg("m\r\n");
    // }
    // 假肢主要控制逻辑
    if(CONTROL_FLAG == NO_CONTROL_OUT)
    {
        motor_stop();
    }
    else if(CONTROL_FLAG == CONTROL_OUT)
    {
        // TODO: 控制率
        if(proState == MIDDLE_ZHI_STATE)
        {
            if(angle <= ANGLE_ZHI_BEI)
            {
                proState = MIDDLE_BEI_STATE;
            }
        }
        else if(proState == MIDDLE_BEI_STATE)
        {
            if(angle >= ANGLE_MIDDLE_PUSH)
            {
                proState = PUSH_STATE;
            }
        }
        else if(proState == PUSH_STATE)
        {
            if(angle <= ANGLE_PUAH_PULL)
            {
                proState = PULL_STATE;
            }
        }
        else if(proState == PULL_STATE)
        {
            if(angle >= ANGLE_PULL_MIDDLE || (angle >= -3.5 && P_foot3 >= 0.5))
            {
                proState = MIDDLE_ZHI_STATE;
            }
        }

        switch(proState)
        {
            case MIDDLE_ZHI_STATE:
                MIDDLE_zhi();
                break;
            case MIDDLE_BEI_STATE:
                MIDDLE_bei();
                break;
            case PUSH_STATE:
                PUSH_control();
                break;
            case PULL_STATE:
                PULL();
                valve_pwm_middle_zhi = MIN_SPEED;
                valve_pwm_middle_bei = MAX_VALVE_3 / 2;
                valve_pwm_push = MAX_SPEED;
                middle_zhi_controlFlag = 0;
                break;
            default:
                break;
        }

        if(press1 < (P_HIGH - 0.5) && press1 > 0)
        {
            motor_run();
        }
        else if(press1>(P_HIGH + 0.5))
        {
            motor_stop();
        }

    }
    else    // 默认都是 CONTROL_RESET
    {
        // 回到初始状态
        motor_stop();
        UNLOAD();
        MIDDLE();
        proState = MIDDLE_ZHI_STATE;
    }

}




