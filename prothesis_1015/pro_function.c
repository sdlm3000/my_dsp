/*
 * pro_function.c
 *
 *  Created on: 2020年9月4日
 *      Author: Administrator
 */

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "pro_header.h"
#include "math.h"

extern double press1,press2,press3;
extern float arg;
extern double arg_ave[5];
extern Uint16 press_plante1;
extern Uint16 press_plante2;
extern Uint16 press_plante3;

extern int flag_state1;

extern double volt1,volt2,volt3,volt4,volt5,volt6,volt7;
extern Uint32 sum1,sum2,sum3,sum4,sum5,sum6,sum7;
extern Uint16 SampleTable1[BUF_SIZE];
extern Uint16 SampleTable2[BUF_SIZE];
extern Uint16 SampleTable3[BUF_SIZE];
extern Uint16 SampleTable4[BUF_SIZE];
extern Uint16 SampleTable5[BUF_SIZE];
extern Uint16 SampleTable6[BUF_SIZE];
extern Uint16 SampleTable7[BUF_SIZE];


extern int valve_pwm_middle_zhi;
extern int valve_pwm_middle_bei;
extern int valve_pwm_push;
extern int switch_flag;
extern int state_motor;
extern int state;


extern int counter_time;
extern int counter;

extern int flag_run;

extern int counter_P;

extern double Beta, Beta_last;   // 角度制

extern double W_BD;

extern double Alpha, Alpha_cos, Alpha_sin;

extern double T, F_A;

extern double delta_P, delta_P0;

extern double dd_P;


extern double my_temp1,my_temp2;

//**************调试使用的临时函数***************//

void dsp_init(void)
{
    // 步骤 1. 初始化系统控制:
        InitSysCtrl();

    // 步骤2. 端口的初始化
        InitEPwm1Gpio();
        InitEPwm2Gpio();
        InitEPwm3Gpio();
        InitEPwm4Gpio();
        InitEPwm5Gpio();
        InitSciGpio();
        //ADC初始化
        EALLOW;
        SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
        EDIS;

    // 步骤3. 清除所有中断,初始化中断向量表
        DINT;
        // 初始化PIE控制寄存器到他们的默认状态.
        InitPieCtrl();
        // 禁止CPU中断和清除所有CPU中断标志
        IER = 0x0000;
        IFR = 0x0000;
        //初始化PIE中断向量表，并使其指向中断服务子程序（ISR）
        InitPieVectTable();
        EALLOW;
        PieVectTable.TINT0 = &cpu_timer0_isr;
        EDIS;

    // 步骤 4.初始化片内外设:
        InitCpuTimers();
    #if (CPU_FRQ_150MHZ)
        ConfigCpuTimer(&CpuTimer0, 150,5000);  //t=150*10000/150000000s,即10ms
    #endif

        EALLOW;
        SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;//Disable TBCLK within the ePWM
        EDIS;

        InitEPwm1Example();
        InitEPwm2Example();
        InitEPwm3Example();
        InitEPwm4Example();
        InitEPwm5Example();

        EALLOW;
        SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;//Enable TBCLK within the ePWM
        EDIS;


        IER |= M_INT1;
        PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
}

void initgpio(void)
{
    EALLOW;

//    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;      // Enable pullup on GPIO528
//    GpioDataRegs.GPASET.bit.GPIO0 = 1;      // Load output latch
//    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;     // GPIO528 = GPIO
//    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;      // GPIO528 = output

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;      // Enable pullup on GPIO528
    GpioDataRegs.GPASET.bit.GPIO2 = 1;      // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;     // GPIO528 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;      // GPIO528 = output

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;      // Enable pullup on GPIO528
    GpioDataRegs.GPASET.bit.GPIO4 = 1;      // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;     // GPIO528 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;      // GPIO528 = output

    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;      // Enable pullup on GPIO529
    GpioDataRegs.GPASET.bit.GPIO6 = 1;      // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;     // GPIO529 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;      // GPIO529 = output

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;      // Enable pullup on GPIO50
    GpioDataRegs.GPASET.bit.GPIO8 = 1;      // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;     // GPIO50 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;      // GPIO50 = output

    GpioDataRegs.GPADAT.all = 0x0;
    GpioCtrlRegs.GPAQSEL1.all = 0x0000;
    EDIS;
}


//延时函数
void delay(Uint32 t)
{
    Uint32 i = 0;
    for (i = 0; i < t; i++);
}



/********************PWM******************/
//Epwm1
void InitEPwm1Example()
{
   // Setup TBCLK
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;           // Set timer period 801 TBCLKs
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;     // Set compare A value
   EPwm1Regs.CMPB = EPWM1_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM1A on event A, up count
   EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM1A on event A, down count

   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM1B on event B, up count
   EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM1B on event B, down count
}

//EPWM2
void InitEPwm2Example()
{
   // Setup TBCLK
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;           // Set timer period 802 TBCLKs
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm2Regs.CMPA.half.CMPA = EPWM2_MIN_CMPA;     // Set compare A value
   EPwm2Regs.CMPB = EPWM2_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM2A on event A, up count
   EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM2A on event A, down count

   EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM2B on event B, up count
   EPwm2Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM2B on event B, down count
}

//Epwm3
void InitEPwm3Example()
{
   // Setup TBCLK
   EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;           // Set timer period 803 TBCLKs
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm3Regs.CMPA.half.CMPA = EPWM3_MIN_CMPA;     // Set compare A value
   EPwm3Regs.CMPB = EPWM3_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x3;       // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM3A on event A, up count
   EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM3A on event A, down count

   EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM3B on event B, up count
   EPwm3Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM3B on event B, down count
}

//EPwm4
void InitEPwm4Example()
{
   // Setup TBCLK
   EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD;           // Set timer period 804 TBCLKs
   EPwm4Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm4Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm4Regs.CMPA.half.CMPA = EPWM4_MIN_CMPA;     // Set compare A value
   EPwm4Regs.CMPB = EPWM4_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x4;       // Clock ratio to SYSCLKOUT
   EPwm4Regs.TBCTL.bit.CLKDIV = 0x7;//64分频

   // Setup shadowing
   EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM4A on event A, up count
   EPwm4Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM4A on event A, down count

   EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM4B on event B, up count
   EPwm4Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM4B on event B, down count
}

//EPwm5
void InitEPwm5Example()
{
   // Setup TBCLK
   EPwm5Regs.TBPRD = EPWM5_TIMER_TBPRD;           // Set timer period 805 TBCLKs
   EPwm5Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm5Regs.TBCTR = 0x0000;                      // Clear counter

   // Set Compare values
   EPwm5Regs.CMPA.half.CMPA = EPWM5_MIN_CMPA;     // Set compare A value
   EPwm5Regs.CMPB = EPWM5_MAX_CMPB;               // Set Compare B value

   // Setup counter mode
   EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up 对称输出模式
   EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载
   EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x5;       // Clock ratio to SYSCLKOUT
   EPwm5Regs.TBCTL.bit.CLKDIV = 0x7;//65分频

   // Setup shadowing
   EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
   EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set actions
   EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;             // Set PWM5A on event A, up count
   EPwm5Regs.AQCTLA.bit.CAD = AQ_SET;           // Clear PWM5A on event A, down count

   EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;             // Set PWM5B on event B, up count
   EPwm5Regs.AQCTLB.bit.CBD = AQ_SET;           // Clear PWM5B on event B, down count
}


/***********************SCI通信*********************/
//
void scia_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

    SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all =0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA =1;
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;
    #if (CPU_FRQ_150MHZ)
          SciaRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 37.5MHz.
          SciaRegs.SCILBAUD    =0x0027;
    #endif
    #if (CPU_FRQ_100MHZ)
      SciaRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 20MHz.
      SciaRegs.SCILBAUD    =0x0014;
    #endif
    SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

void scib_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

    ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    ScibRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.all =0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA =1;
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;
    #if (CPU_FRQ_150MHZ)
          ScibRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 37.5MHz.
          ScibRegs.SCILBAUD    =0x0027;
    #endif
    #if (CPU_FRQ_100MHZ)
      ScibRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 20MHz.
      ScibRegs.SCILBAUD    =0x0014;
    #endif
    ScibRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

//初始化FIFO
void scia_fifo_init()
{
    SciaRegs.SCIFFTX.all=0xE040;
    SciaRegs.SCIFFRX.all=0x204f;
    SciaRegs.SCIFFCT.all=0x0;
//    ScibRegs.SCIFFTX.all=0x8000;
//    ScibRegs.SCIFFTX.all=0xC028;
//    ScibRegs.SCIFFRX.all=0x0028;
//    SciaRegs.SCIFFCT.all=0x00;
}

void scib_fifo_init()
{
    ScibRegs.SCIFFTX.all=0xE040;
    ScibRegs.SCIFFRX.all=0x204f;
    ScibRegs.SCIFFCT.all=0x0;
//    ScibRegs.SCIFFTX.all=0x8000;
//    ScibRegs.SCIFFTX.all=0xC028;
//    ScibRegs.SCIFFRX.all=0x0028;
//    SciaRegs.SCIFFCT.all=0x00;
}

//发送
void scia_xmit(Uint32 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) { }
    SciaRegs.SCITXBUF=a;
}

//传输一个四位整数
void scia_int(int data_char)
{
    unsigned char sci_table[4];
    if(data_char<0)
    {
        data_char = -1 * data_char;
        scia_xmit('-');
    }
    sci_table[0]=data_char/1000;      //获得千位的数字
    sci_table[1]=data_char%1000/100;  //获得百位的数字
    sci_table[2]=data_char%100/10;    //获得十位的数字
    sci_table[3]=data_char%10;        //获得个位的数字
    if(sci_table[0]!= 0)
    {
        sci_table[0]=sci_table[0]+0x30;
        scia_xmit(sci_table[0]);
    }
    if(sci_table[1]!= 0 || sci_table[0]!= 0)
    {
        sci_table[1]=sci_table[1]+0x30;
        scia_xmit(sci_table[1]);
    }
    if(sci_table[1]!= 0 || sci_table[0]!= 0 || sci_table[2]!= 0)
    {
        sci_table[2]=sci_table[2]+0x30;
        scia_xmit(sci_table[2]);
    }
    sci_table[3]=sci_table[3]+0x30;
    scia_xmit(sci_table[3]);
}

//传输浮点数
void scia_float(double data)
{
    unsigned char sci_table[4];
    long a;
    a=data*100;                          //保留2位小数
    if(a<0)
    {
        a = -1 * a;
        scia_xmit('-');
    }
    sci_table[0]=a/1000;
    sci_table[1]=a%1000/100;
    sci_table[2]=a%100/10;
    sci_table[3]=a%10;

    if(sci_table[0]!= 0)
    {
        sci_table[0]=sci_table[0]+0x30;
        scia_xmit(sci_table[0]);
    }
    sci_table[1]=sci_table[1]+0x30;
    scia_xmit(sci_table[1]);
    scia_xmit('.');
    sci_table[2]=sci_table[2]+0x30;
    scia_xmit(sci_table[2]);
    sci_table[3]=sci_table[3]+0x30;
    scia_xmit(sci_table[3]);
}

//发送字符串
void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
}

//
Uint16 scia_rx()
{
    Uint16 Rchar;
    // Wait for inc character
    while(SciaRegs.SCIFFRX.bit.RXFFST !=1) { } // wait for XRDY =1 for empty state
    // Get character
    Rchar = SciaRegs.SCIRXBUF.all;
    return Rchar;
}


/***********************电磁阀相关函数********************/
//电磁阀部分的初始化
void valveinit(void)
{
    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioDataRegs.GPASET.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO2 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;
    GpioDataRegs.GPASET.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO4 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;
    GpioDataRegs.GPASET.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO6 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;
    GpioDataRegs.GPASET.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
//    GpioDataRegs.GPADAT.bit.GPIO8 = 0;

    GpioCtrlRegs.GPAQSEL1.all = 0x0000;
    EDIS;
}


//蹬腿
void PUSH(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = maxspeed;
    EPwm4Regs.CMPA.half.CMPA = maxspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//被动态
void MIDDLE(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

// 1和4互换，注意上面没有改变

//泄压
void UNLOAD(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = maxspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = maxspeed;
}

//被动跖屈
void MIDDLE_bei(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=minspeed;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = valve_pwm_middle_bei;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//被动背屈
void MIDDLE_zhi(void)
{
//    if(switch_flag == 1)
//    {
//        valve_pwm_middle=minspeed;
//        switch_flag=0;
//    }
    EPwm2Regs.CMPA.half.CMPA = valve_pwm_middle_zhi;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}

//蹬腿
void PUSH_control(void)
{
    EPwm2Regs.CMPA.half.CMPA = minspeed;
    EPwm3Regs.CMPA.half.CMPA = valve_pwm_push;
    EPwm4Regs.CMPA.half.CMPA = maxspeed;
    EPwm5Regs.CMPA.half.CMPA = minspeed;
}


//收腿
void PULL(void)
{
    EPwm2Regs.CMPA.half.CMPA = maxspeed;
    EPwm3Regs.CMPA.half.CMPA = minspeed;
    EPwm4Regs.CMPA.half.CMPA = minspeed;
    EPwm5Regs.CMPA.half.CMPA = maxspeed;
}




/********************电机相关函数****************/
void motor_run(void)
{
        EPwm1Regs.CMPA.half.CMPA = EPWM1_MAX_CMPA;
        state_motor = 1;
}

void motor_stop(void)
{
        EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;
        state_motor = 0;
}


/********************中断******************/
interrupt void cpu_timer0_isr(void)
{
    uchar i;
    double temp,temp2;
    CpuTimer0.InterruptCount++;
    EALLOW;
    AdcRegs.ADCTRL2.all = 0x2000;//软件向SEQ1位写1开启转换触发
    for (i=0;i<AVG;i++)//AVG=10
    {
        while (AdcRegs.ADCST.bit.INT_SEQ1== 0) {} // Wait for interrupt如何发起中断
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable1[i] =(AdcRegs.ADCRESULT0>>4);
        SampleTable2[i] =(AdcRegs.ADCRESULT1>>4);
        SampleTable3[i] =(AdcRegs.ADCRESULT2>>4);
        SampleTable4[i] =(AdcRegs.ADCRESULT3>>4);
        SampleTable5[i] =(AdcRegs.ADCRESULT4>>4);
        SampleTable6[i] =(AdcRegs.ADCRESULT5>>4);
        SampleTable7[i] =(AdcRegs.ADCRESULT6>>4);
        sum1 = sum1 + SampleTable1[i];
        sum2 = sum2 + SampleTable2[i];
        sum3 = sum3 + SampleTable3[i];
        sum4 = sum4 + SampleTable4[i];
        sum5 = sum5 + SampleTable5[i];
        sum6 = sum6 + SampleTable6[i];
        sum7 = sum7 + SampleTable7[i];
    }
    press1=((((double)sum1)*3/4096/AVG)*gain1-4)*1.25;
    scia_float(press1);
    press2=((((double)sum2)*3/4096/AVG)*gain2-4)*1.25;
    scia_xmit(' ');
    scia_float(press2);
    press3=((((double)sum3)*3/4096/AVG)*gain3-4)*1.25;
    scia_xmit(' ');
    scia_float(press3);

    if(press1>=P_max || press1<=0)
    {
        counter_P++;
        if(counter_P>=15)
        {
            motor_stop();
            flag_run = 0;
            UNLOAD();
            counter_P = 0;
            state = 0;
        }
    }
    arg = ((((double)sum4)*3/4096/AVG)*100);//采样的实际电压 0-3v
    arg = 66.5 - arg;     // 70为零位的角度传感器值
    temp2 = 0;
    for(i=0;i<4;i++)
    {
        arg_ave[i] = arg_ave[i+1];
        temp2 = temp2 + arg_ave[i];
    }
    arg_ave[4] = arg;
    arg = (temp2 + arg_ave[4]) / 5; // 10点滑动平均
    scia_xmit(' ');
    scia_float(arg);

    volt5=((double)sum5)*3/4096/AVG;//采样的实际电压 0-3v
    volt6=((double)sum6)*3/4096/AVG;//采样的实际电压 0-3v
    volt7=((double)sum7)*3/4096/AVG;//采样的实际电压 0-3v
    Beta_last = Beta;
    Beta = arg;

//    W_BD = (Beta - Beta_last) * PI / 180.0 / 0.005;   // 定时周期为5ms
    W_BD = (Beta - Beta_last) / 0.005;   // 定时周期为5ms
    delta_P0 = press2 - press3;
    if(CpuTimer0.InterruptCount>=10)
    {
        Alpha_cos = (L_BD * cos(Beta * PI / 180.0) - d_x) / L_AB;
        Alpha_sin = sqrt(1 - Alpha_cos * Alpha_cos);
        // 步骤4：求解T，F_A
        if(state==1)
        {
            temp = Beta - xita_cp;
            T = (k1_cp * temp + k2_cp * temp * temp + b_cp * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // 求解delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;


            if(volt7>0.5)
            {
                flag_state1 = 1;

            }
            if(flag_state1==1)
            {
                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
                if(valve_pwm_middle_zhi<minspeed)
                {
                    valve_pwm_middle_zhi = minspeed;
                }
                else if(valve_pwm_middle_zhi>max_valve4)
                {
                    valve_pwm_middle_zhi = max_valve4;
                }
            }
//            valve_pwm_middle_zhi = max_valve4;
        }
        else if(state==2)
        {
            temp = Beta - xita_cd;
            T = (k1_cd * temp + k2_cd * temp * temp + b_cd * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // 求解delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;
            valve_pwm_middle_zhi = 0;
            valve_pwm_middle_bei = (arg + 4) / 3 * max_valve3;
            if(valve_pwm_middle_bei<minspeed)
            {
                valve_pwm_middle_bei = minspeed;

            }
            else if(valve_pwm_middle_bei>max_valve3)
            {
                valve_pwm_middle_bei = max_valve3;
            }
//            valve_pwm_middle_zhi = 0;
//            valve_pwm_middle_bei = max_valve3;

//            if(delta_P>=0 && valve_pwm_middle_zhi>0)
//            {
//                valve_pwm_middle_zhi = valve_pwm_middle_zhi + (int)(Kp * dd_P);
//                if(valve_pwm_middle_zhi<minspeed)
//                {
//                    valve_pwm_middle_zhi = minspeed;
//                }
//                else if(valve_pwm_middle_zhi>=max_valve4)
//                {
//                    valve_pwm_middle_zhi = max_valve4;
//                }
//            }
//            else
//            {
//                valve_pwm_middle_bei = valve_pwm_middle_bei + (int)(Kp_2 * dd_P);
//                if(valve_pwm_middle_bei<minspeed)
//                {
//                    valve_pwm_middle_bei = minspeed;
//
//                }
//                else if(valve_pwm_middle_bei>max_valve3)
//                {
//                    valve_pwm_middle_bei = max_valve3;
//                }
//            }
//            valve_pwm_middle_zhi = 0;
//            valve_pwm_middle_bei = 1800;
        }
        else if(state==3)
        {
            temp = Beta - xita_pp;
            T = (k1_pp * temp + k2_pp * temp * temp + b_pp * W_BD) * xishu;
            Alpha = acos(Alpha_cos);
            F_A = T / L_BD * cos((90 - Beta) / 180.0 * PI - Alpha) * Alpha_sin;
            // 求解delta_P
            delta_P = (F_A) / A_p / 1000000;

            dd_P = delta_P0 - delta_P;

            valve_pwm_push = valve_pwm_push + (int)(Kp * dd_P);
            if(valve_pwm_push<min_valve2)
            {
                valve_pwm_push = min_valve2;
            }
            else if(valve_pwm_push>maxspeed)
            {
                valve_pwm_push = maxspeed;
            }
            valve_pwm_push = min_valve2;
        }
    }
    if(state==2 && arg>10.0)
    {
        counter++;
    }
    else if(state==4)
    {
        counter_time++;
    }
    scia_xmit(' ');
    scia_float(delta_P);
    scia_xmit(' ');
    if(state==1)
    {
        scia_int(valve_pwm_middle_zhi);
        scia_xmit(' ');
    }
    else if(state==2)
    {
        if(delta_P>=0)
        {
            scia_int(valve_pwm_middle_zhi);
            scia_xmit(' ');
        }
        else
        {
            scia_int(valve_pwm_middle_bei);
            scia_xmit(' ');
        }
    }
    else
    {
        scia_int(valve_pwm_push);
        scia_xmit(' ');
    }



    scia_xmit(state_motor+0x30);


    scia_xmit(' ');
    scia_float(volt5);
    scia_xmit(' ');
    scia_float(volt6);
    scia_xmit(' ');
    scia_float(volt7);

    scia_xmit(' ');
    scia_xmit(state+0x30);

    scia_xmit('\r');
    scia_xmit('\n');

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



