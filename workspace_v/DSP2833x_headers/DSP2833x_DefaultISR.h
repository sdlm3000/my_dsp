// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:45:37 $
//###########################################################################
//
// FILE:    DSP2833x_DefaultIsr.h
//文件名：
// TITLE:   DSP2833x Devices Default Interrupt Service Routines Definitions.
//功能：实现中断地址函数的初始化声明；
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#ifndef DSP2833x_DEFAULT_ISR_H
#define DSP2833x_DEFAULT_ISR_H

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------
// Default Interrupt Service Routine Declarations:
// 外部默认中断函数是声明，该函数位于DSP2833x_DefaultIsr.c文件中；
// The following function prototypes are for the 
// default ISR routines used with the default PIE vector table.
// This default vector table is found in the DSP2833x_PieVect.h 
// file.  
//

// Non-Peripheral Interrupts:
extern interrupt void INT13_ISR(void);     // XINT13 or CPU-Timer 1
extern interrupt void INT14_ISR(void);     // CPU-Timer2
extern interrupt void DATALOG_ISR(void);   // Datalogging interrupt
extern interrupt void RTOSINT_ISR(void);   // RTOS interrupt
extern interrupt void EMUINT_ISR(void);    // Emulation interrupt
extern interrupt void NMI_ISR(void);       // Non-maskable interrupt
extern interrupt void ILLEGAL_ISR(void);   // Illegal operation TRAP
extern interrupt void USER1_ISR(void);     // User Defined trap 1
extern interrupt void USER2_ISR(void);     // User Defined trap 2
extern interrupt void USER3_ISR(void);     // User Defined trap 3
extern interrupt void USER4_ISR(void);     // User Defined trap 4
extern interrupt void USER5_ISR(void);     // User Defined trap 5
extern interrupt void USER6_ISR(void);     // User Defined trap 6
extern interrupt void USER7_ISR(void);     // User Defined trap 7
extern interrupt void USER8_ISR(void);     // User Defined trap 8
extern interrupt void USER9_ISR(void);     // User Defined trap 9
extern interrupt void USER10_ISR(void);    // User Defined trap 10
extern interrupt void USER11_ISR(void);    // User Defined trap 11
extern interrupt void USER12_ISR(void);    // User Defined trap 12

// Group 1 PIE Interrupt Service Routines:
extern interrupt void  SEQ1INT_ISR(void);   // ADC Sequencer 1 ISR
extern interrupt void  SEQ2INT_ISR(void);   // ADC Sequencer 2 ISR
extern interrupt void  XINT1_ISR(void);     // External interrupt 1
extern interrupt void  XINT2_ISR(void);     // External interrupt 2
extern interrupt void  ADCINT_ISR(void);    // ADC
extern interrupt void  TINT0_ISR(void);     // Timer 0
extern interrupt void  WAKEINT_ISR(void);   // WD

// Group 2 PIE Interrupt Service Routines:
extern interrupt void EPWM1_TZINT_ISR(void);    // EPWM-1
extern interrupt void EPWM2_TZINT_ISR(void);    // EPWM-2
extern interrupt void EPWM3_TZINT_ISR(void);    // EPWM-3
extern interrupt void EPWM4_TZINT_ISR(void);    // EPWM-4
extern interrupt void EPWM5_TZINT_ISR(void);    // EPWM-5
extern interrupt void EPWM6_TZINT_ISR(void);    // EPWM-6
      
// Group 3 PIE Interrupt Service Routines:
extern interrupt void EPWM1_INT_ISR(void);    // EPWM-1
extern interrupt void EPWM2_INT_ISR(void);    // EPWM-2
extern interrupt void EPWM3_INT_ISR(void);    // EPWM-3
extern interrupt void EPWM4_INT_ISR(void);    // EPWM-4
extern interrupt void EPWM5_INT_ISR(void);    // EPWM-5
extern interrupt void EPWM6_INT_ISR(void);    // EPWM-6
      
// Group 4 PIE Interrupt Service Routines:
extern interrupt void ECAP1_INT_ISR(void);    // ECAP-1
extern interrupt void ECAP2_INT_ISR(void);    // ECAP-2
extern interrupt void ECAP3_INT_ISR(void);    // ECAP-3
extern interrupt void ECAP4_INT_ISR(void);    // ECAP-4
extern interrupt void ECAP5_INT_ISR(void);    // ECAP-5
extern interrupt void ECAP6_INT_ISR(void);    // ECAP-6
     
// Group 5 PIE Interrupt Service Routines:
extern interrupt void EQEP1_INT_ISR(void);    // EQEP-1
extern interrupt void EQEP2_INT_ISR(void);    // EQEP-2

// Group 6 PIE Interrupt Service Routines:
extern interrupt void SPIRXINTA_ISR(void);   // SPI-A
extern interrupt void SPITXINTA_ISR(void);   // SPI-A
extern interrupt void MRINTA_ISR(void);      // McBSP-A
extern interrupt void MXINTA_ISR(void);      // McBSP-A
extern interrupt void MRINTB_ISR(void);      // McBSP-B
extern interrupt void MXINTB_ISR(void);      // McBSP-B

// Group 7 PIE Interrupt Service Routines:
extern interrupt void DINTCH1_ISR(void);     // DMA-Channel 1
extern interrupt void DINTCH2_ISR(void);     // DMA-Channel 2
extern interrupt void DINTCH3_ISR(void);     // DMA-Channel 3
extern interrupt void DINTCH4_ISR(void);     // DMA-Channel 4
extern interrupt void DINTCH5_ISR(void);     // DMA-Channel 5
extern interrupt void DINTCH6_ISR(void);     // DMA-Channel 6

// Group 8 PIE Interrupt Service Routines: 
extern interrupt void I2CINT1A_ISR(void);     // I2C-A
extern interrupt void I2CINT2A_ISR(void);     // I2C-A
extern interrupt void SCIRXINTC_ISR(void);    // SCI-C
extern interrupt void SCITXINTC_ISR(void);    // SCI-C
   
// Group 9 PIE Interrupt Service Routines:
extern interrupt void SCIRXINTA_ISR(void);    // SCI-A
extern interrupt void SCITXINTA_ISR(void);    // SCI-A
extern interrupt void SCIRXINTB_ISR(void);    // SCI-B
extern interrupt void SCITXINTB_ISR(void);    // SCI-B
extern interrupt void ECAN0INTA_ISR(void);    // eCAN-A
extern interrupt void ECAN1INTA_ISR(void);    // eCAN-A
extern interrupt void ECAN0INTB_ISR(void);    // eCAN-B
extern interrupt void ECAN1INTB_ISR(void);    // eCAN-B

// Group 10 PIE Interrupt Service Routines:
 
// Group 11 PIE Interrupt Service Routines: 

// Group 12 PIE Interrupt Service Routines: 
extern interrupt void  XINT3_ISR(void);     // External interrupt 3
extern interrupt void  XINT4_ISR(void);     // External interrupt 4
extern interrupt void  XINT5_ISR(void);     // External interrupt 5
extern interrupt void  XINT6_ISR(void);     // External interrupt 6
extern interrupt void  XINT7_ISR(void);     // External interrupt 7
extern interrupt void  LVF_ISR(void);       // Latched overflow flag
extern interrupt void  LUF_ISR(void);       // Latched underflow flag

// Catch-all for Reserved Locations For testing purposes:
extern interrupt void PIE_RESERVED(void);       // Reserved for test
extern interrupt void rsvd_ISR(void);           // for test
extern interrupt void INT_NOTUSED_ISR(void);    // for unused interrupts

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif    // end of DSP2833x_DEFAULT_ISR_H definition

//===========================================================================
// End of file.
//===========================================================================
