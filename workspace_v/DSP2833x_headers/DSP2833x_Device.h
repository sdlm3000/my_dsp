// TI File $Revision: /main/3 $
// Checkin $Date: March 28, 2007   16:18:05 $
//###########################################################################
//
// FILE:   DSP2833x_Device.h
//
// TITLE:  DSP2833x Device Definitions.
//功能： DSP2833x包含所有的外设头文件及通用寄存器的宏定义
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#ifndef DSP2833x_DEVICE_H
#define DSP2833x_DEVICE_H


#ifdef __cplusplus
extern "C" {
#endif


#define   TARGET   1
//---------------------------------------------------------------------------
// User To Select Target Device:
//用户选择目标板类型，该优点是可以根据实际的需要仅要修改宏定义则可实现不同目标板的转换；
//宏定义的对象一般是一些可以变换，但又有具体的含义的名称；
#define   DSP28_28335   TARGET
#define   DSP28_28334   0
#define   DSP28_28332   0


//---------------------------------------------------------------------------
// Common CPU Definitions:
//CPU的通用宏定义

extern cregister volatile unsigned int IFR;//定义一个控制寄存器类型的全局变量
extern cregister volatile unsigned int IER;//定义一个控制寄存器类型的全局变量

#define  EINT   asm(" clrc INTM")  //用宏定义的方式实现对汇编语言的替代；
#define  DINT   asm(" setc INTM")
#define  ERTM   asm(" clrc DBGM")
#define  DRTM   asm(" setc DBGM")
#define  EALLOW asm(" EALLOW")
#define  EDIS   asm(" EDIS")
#define  ESTOP0 asm(" ESTOP0")

#define M_INT1  0x0001     //使用宏定义的方式实现16位寄存器中的位的定义
#define M_INT2  0x0002
#define M_INT3  0x0004
#define M_INT4  0x0008
#define M_INT5  0x0010
#define M_INT6  0x0020
#define M_INT7  0x0040
#define M_INT8  0x0080
#define M_INT9  0x0100
#define M_INT10 0x0200
#define M_INT11 0x0400
#define M_INT12 0x0800
#define M_INT13 0x1000
#define M_INT14 0x2000
#define M_DLOG  0x4000
#define M_RTOS  0x8000

#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080
#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000



//---------------------------------------------------------------------------
// For Portability, User Is Recommended To Use Following Data Type Size
// Definitions For 16-bit and 32-Bit Signed/Unsigned Integers:
//变量类型重新定义；

#ifndef DSP28_DATA_TYPES
#define DSP28_DATA_TYPES
typedef int             int16;
typedef long            int32;
typedef unsigned int    Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;
#endif


//---------------------------------------------------------------------------
// Include All Peripheral Header Files:
//所包含的外设头文件

#include "DSP2833x_Adc.h"                // ADC Registers
#include "DSP2833x_DevEmu.h"             // Device Emulation Registers
#include "DSP2833x_CpuTimers.h"          // 32-bit CPU Timers
#include "DSP2833x_ECan.h"               // Enhanced eCAN Registers
#include "DSP2833x_ECap.h"               // Enhanced Capture
#include "DSP2833x_DMA.h"                // DMA Registers
#include "DSP2833x_EPwm.h"               // Enhanced PWM
#include "DSP2833x_EQep.h"               // Enhanced QEP
#include "DSP2833x_Gpio.h"               // General Purpose I/O Registers
#include "DSP2833x_I2c.h"                // I2C Registers
#include "DSP2833x_McBSP.h"              // McBSP
#include "DSP2833x_PieCtrl.h"            // PIE Control Registers
#include "DSP2833x_PieVect.h"            // PIE Vector Table
#include "DSP2833x_Spi.h"                // SPI Registers
#include "DSP2833x_Sci.h"                // SCI Registers
#include "DSP2833x_SysCtrl.h"            // System Control/Power Modes
#include "DSP2833x_XIntrupt.h"           // External Interrupts
#include "DSP2833x_Xintf.h"              // XINTF External Interface
//#include "DSP2833x_DefaultIsr.h"		 // Default Interrupt Service Routines Definitions

// Include files not used with DSP/BIOS
#ifndef DSP28_BIOS
#include "DSP2833x_DefaultISR.h"		// Default Interrupt Service Routines Definitions
#endif

#if DSP28_28335				//DSP28335外设数宏定义；
#define DSP28_EPWM1  1
#define DSP28_EPWM2  1
#define DSP28_EPWM3  1
#define DSP28_EPWM4  1
#define DSP28_EPWM5  1
#define DSP28_EPWM6  1
#define DSP28_ECAP1  1
#define DSP28_ECAP2  1
#define DSP28_ECAP3  1
#define DSP28_ECAP4  1
#define DSP28_ECAP5  1
#define DSP28_ECAP6  1
#define DSP28_EQEP1  1
#define DSP28_EQEP2  1
#define DSP28_ECANA  1
#define DSP28_ECANB  1
#define DSP28_MCBSPA 1
#define DSP28_MCBSPB 1
#define DSP28_SPIA   1
#define DSP28_SCIA   1
#define DSP28_SCIB   1
#define DSP28_SCIC   1
#define DSP28_I2CA   1
// ECCTL1 ( ECAP Control Reg 1)
//==========================
// CAPxPOL bits
#define EC_RISING 0x0
#define EC_FALLING 0x1
// CTRRSTx bits
#define EC_ABS_MODE 0x0
#define EC_DELTA_MODE 0x1
// PRESCALE bits
#define EC_BYPASS 0x0
#define EC_DIV1 0x0
#define EC_DIV2 0x1
#define EC_DIV4 0x2
#define EC_DIV6 0x3
#define EC_DIV8 0x4
#define EC_DIV10 0x5
// ECCTL2 ( ECAP Control Reg 2)
//==========================
// CONT/ONESHOT bit
#define EC_CONTINUOUS 0x0
#define EC_ONESHOT 0x1
// STOPVALUE bit
#define EC_EVENT1 0x0
#define EC_EVENT2 0x1
#define EC_EVENT3 0x2
#define EC_EVENT4 0x3
// RE-ARM bit
#define EC_ARM 0x1
// TSCTRSTOP bit
#define EC_FREEZE 0x0
#define EC_RUN 0x1
// SYNCO_SEL bit
#define EC_SYNCIN 0x0
#define EC_CTR_PRD 0x1
#define EC_SYNCO_DIS 0x2
// CAP/APWM mode bit
#define EC_CAP_MODE 0x0
#define EC_APWM_MODE 0x1
// APWMPOL bit
#define EC_ACTV_HI 0x0
#define EC_ACTV_LO 0x1
// Generic
#define EC_DISABLE 0x0
#define EC_ENABLE 0x1
#define EC_FORCE 0x1
#endif  // end DSP28_28335

#if DSP28_28334
#define DSP28_EPWM1  1
#define DSP28_EPWM2  1
#define DSP28_EPWM3  1
#define DSP28_EPWM4  1
#define DSP28_EPWM5  1
#define DSP28_EPWM6  1
#define DSP28_ECAP1  1
#define DSP28_ECAP2  1
#define DSP28_ECAP3  1
#define DSP28_ECAP4  1
#define DSP28_ECAP5  0
#define DSP28_ECAP6  0
#define DSP28_EQEP1  1
#define DSP28_EQEP2  1
#define DSP28_ECANA  1
#define DSP28_ECANB  1
#define DSP28_MCBSPA 1
#define DSP28_MCBSPB 1
#define DSP28_SPIA   1
#define DSP28_SCIA   1
#define DSP28_SCIB   1
#define DSP28_SCIC   1
#define DSP28_I2CA   1
#endif  // end DSP28_28334

#if DSP28_28332
#define DSP28_EPWM1  1
#define DSP28_EPWM2  1
#define DSP28_EPWM3  1
#define DSP28_EPWM4  1
#define DSP28_EPWM5  1
#define DSP28_EPWM6  1
#define DSP28_ECAP1  1
#define DSP28_ECAP2  1
#define DSP28_ECAP3  1
#define DSP28_ECAP4  1
#define DSP28_ECAP5  0
#define DSP28_ECAP6  0
#define DSP28_EQEP1  1
#define DSP28_EQEP2  1
#define DSP28_ECANA  1
#define DSP28_ECANB  1
#define DSP28_MCBSPA 1
#define DSP28_MCBSPB 0
#define DSP28_SPIA   1
#define DSP28_SCIA   1
#define DSP28_SCIB   1
#define DSP28_SCIC   0
#define DSP28_I2CA   1
#endif  // end DSP28_28332

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_DEVICE_H definition


//===========================================================================
// End of file.
//===========================================================================
