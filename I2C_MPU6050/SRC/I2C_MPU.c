// Modified From TI File $Revision: /main/9 $
// Checkin $Date: December 11, 2014   11:15:30 $
//###########################################################################
//
// FILE:    Example_28335I2C_MPU6050.c
//
// TITLE:   DSP28335 I2C Read MPU6050 Example
//
// ASSUMPTIONS:
//
//    This program requires the DSP2833x header files.
//
//    This program requires an external I2C MPU6050 connected to
//    the I2C bus at address {0x68}.
//
//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 28335 Boot Mode table is shown below.
//    For information on configuring the boot mode of an eZdsp,
//    please refer to the documentation included with the eZdsp,
//    Now testing on NanJing Yan Xu  YXDSP-F28335 evaluate board.
//
//       $Boot_Table:
//
//         GPIO87   GPIO86     GPIO85   GPIO84
//          XA15     XA14       XA13     XA12
//           PU       PU         PU       PU
//        ==========================================
//            1        1          1        1    Jump to Flash
//            1        1          1        0    SCI-A boot
//            1        1          0        1    SPI-A boot
//            1        1          0        0    I2C-A boot
//            1        0          1        1    eCAN-A boot
//            1        0          1        0    McBSP-A boot
//            1        0          0        1    Jump to XINTF x16
//            1        0          0        0    Jump to XINTF x32
//            0        1          1        1    Jump to OTP
//            0        1          1        0    Parallel GPIO I/O boot
//            0        1          0        1    Parallel XINTF boot
//            0        1          0        0    Jump to SARAM	    <- "boot to SARAM"
//            0        0          1        1    Branch to check boot mode
//            0        0          1        0    Boot to flash, bypass ADC cal
//            0        0          0        1    Boot to SARAM, bypass ADC cal
//            0        0          0        0    Boot to SCI-A, bypass ADC cal
//                                              Boot_Table_End$
//
// DESCRIPTION:
//
//    This program will write 1-14 words to RTC and read them back.
//    The data written and the RTC address written to are contained
//    in the message structure, I2cMsgOut1. The data read back will be
//    contained in the message structure I2cMsgIn1.
//
//    This program will work with the on-board I2C MPU supplied on
//    the F2833x eZdsp.
//
//
//###########################################################################
// Original Author: D.F.
// Modify Author: Eason Jhon (Yicheng Zhang)
// @ ROOM214.#3 Building.SCUT 
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: Jan 09, 2015 $
//###########################################################################


#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "MPU6050_Memmap.h"
#define I2C_SLAVE_ADDR       0x68  //MPU Address
#define I2C_NUMBYTES          1
#define I2C_RNUMBYTES         14  //14 Bytes  Basic params according to 7 varities.
#define Data_NUM            7
#define SelftestAddr    0x0d   //读自检参数初地址
#define Recieve_Data    0
#define Send_Data     1
//#define
//#define I2C_RTC_LOW_ADDR   0x30

// Note: I2C Macros used in this example can be found in the
// DSP2833x_I2C_defines.h file

// Prototype statements for functions found within this file.
//extern void  InitI2C(void);   //初始化I2CA的中断资源函数 in file ‘DSP2833x_I2C’
//extern void SCIB_Initial(void);//初始化SCIB
void MPU_Initial(void); //初始化MPU6050工作模式函数
Uint16 I2CA_WriteData(struct I2CMSG *msg);  //向总线写一帧数据
Uint16 I2CA_ReadData(struct I2CMSG *msg);  //从总线读一帧数据
void WriteData(struct I2CMSG *msg,Uint16 *MsgBuffer,Uint16 MemoryAdd,Uint16 NumOfBytes);  //连续写数据
interrupt void i2c_int1a_isr(void);  //I2C 中断服务子程序
interrupt void scib_int_isr(void); //SCI 中断服务子程序
void pass(void);  //通过？
void fail(void); //失败？

// Global variables
// One byte will be used for the outgoing address,
struct I2CMSG I2cMsgOut1={I2C_MSGSTAT_SEND_WITHSTOP,  //初始化一个输出数据帧 {状态、MPU地址、字节数，内部地址}
                          I2C_SLAVE_ADDR,
                          I2C_NUMBYTES,
                          SelftestAddr};


struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_SEND_NOSTOP, //初始化一个读入数据帧 {状态、MPU地址、字节数，内部地址}
                          I2C_SLAVE_ADDR,
                          I2C_RNUMBYTES,
                          ACCEL_XOUT_H};

struct I2CMSG *CurrentMsgPtr;		// Used in interrupts 当前消息指针
//参数表，通过查表给MPU写入参数值 ：{内部地址，值}
const Uint16 Wparam[][2] =
   {
	   // {寄存器地址,寄存器值},
	   //------(暂时没用)-----------------
       //{SELF_TEST_X,},
       //{SELF_TEST_Y,},
       //{SELF_TEST_Z,},
       //{SELF_TEST_A,},
	   {PWR_MGMT_1,1}, // 退出睡眠模式，设取样时钟为陀螺X轴。
	   //{PWR_MGMT_2,0},
	   {SMPLRT_DIV,4}, // 取样时钟4分频，1k/4，取样率为25Hz。
	   {CONFIG,2	}, // 低通滤波，截止频率100Hz左右。
	   {GYRO_CONFIG,0<<3  }, // 陀螺量程 0:250dps;1:500dps;2:1000dps,3:2000dps。
	   {ACCEL_CONFIG,0<<3	}, // 加速度计量程0:2g; 1: 4g;	2:8g; 3:16g。
//	   {I2C_MST_CTRL,0},    //MPU不作为主设备使用
	   {INT_PIN_CFG,0x32  }, // 中断信号为高电平，推挽输出，直到有读取操作才消失，直通辅助I2C。
	   {INT_ENALE,1 	  }, // 使用“数据准备好”中断。
	   {USER_CTRL,0x00	},  // 不使用辅助I2C。
	   //{FIFO_R_W,0},            //
   };
Uint16 PassCount; //跳过的次数统计
Uint16 FailCount; //失败统计
Uint16 Sys_stat;
//Uint16 Rparams[Data_NUM];//data need to be read
//Uint16 ACCEL_X;   //give their names.
//Uint16 ACCEL_Y;
//Uint16 ACCEL_Z;
//Uint16 TEMP;
//Uint16 GYRO_X;
//Uint16 GYRO_Y;
//Uint16 GYRO_Z;
Uint8 databuffer[I2C_RNUMBYTES];  //暂存传感器8位数据；


//*====================================$0 主程序=================================================*/
void main(void)
{
   Uint16 i;
   CurrentMsgPtr = &I2cMsgOut1;//当前需发送的数据;
//-----------------$1 系统初始化----------------------------------------------------------
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();    //初始化系统控制块
//  InitPeripheralClocks(); //初始化外设时钟   included in the InitSysCtrl()
//-----------------$2 GPIO----------------------------------------------------------
// Step 2. Initalize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for I2C functionality
// 相关GPIO的设置在I2C 、SCI初始化中完成，如果需要对其他GPIO进行设置，可以在这里加入代码
//-----------------$3 配置中断-----------------------------------------------------------
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
  DINT;   //屏蔽CPU中断
// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.


// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;  //屏蔽所有中断
   IFR = 0x0000;  //清空中断标志位

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.

   InitPieCtrl();  //初始化外设向量控制寄存器
   InitPieVectTable();  //初始化外设中断向量表
   //清零数据缓冲区
      for(i=0;i<I2C_RNUMBYTES;i+=1)
     	{
        databuffer[i]=0;  //clear databuffer
      	//ScibRegs.SCITXBUF==databuffer[i];  //putdata into txbuffer
         }
        i=0;
        PassCount=0;
        FailCount=0;
 // Enable interrupts required for this example
// Enable I2C interrupt 1 in the PIE: Group 8 interrupt 1
//I2C中断为第8组里的中断序号1
   PieCtrlRegs.PIEIER8.bit.INTx1 = 1;//允许I2C中断
//第9组的第4位为 SCIBTXINT  3位 SCIBRXINT
   PieCtrlRegs.PIEIER9.bit.INTx4 = 1;//允许SCIB TX中断
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;	// This is needed to write to EALLOW protected registers
   PieVectTable.I2CINT1A = &i2c_int1a_isr;   //指定I2C中断服务子程序
   PieVectTable.SCITXINTB = &scib_int_isr;  //指定SCI的中断服务子程序
   EDIS;   // This is needed to disable write to EALLOW protected registers
   //-----------------$4 外设初始化-----------------------------------------------------------
// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   InitI2C();  //初始化I2C相关资源
//-----------------$5 中断初始化-----------------------------------------------------------
// Step 5. Initial the SCI
// MPU_Initial();
   InitSci();  //初始化SCI

// Enable CPU INT8 which is connected to PIE group 8
   IER |= M_INT8; //允许向CPU发送中断
   IER |= M_INT9; //是否放到I2C结束后再允许？
   EINT;  //开放CPU中断
   ERTM;  //开全局中断
   Sys_stat=Recieve_Data;
   I2cMsgOut1.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
//   param_msg.SlaveAddress=I2C_SLAVE_ADDR;
  // param_msg.NumOfBytes=Wnum;
//*--------------------$6 主循环体--------------------------------------------------------------*/
   for(;;)
   {
      //////////////////////////////////
      // Write data to MPU CTRL section //
      //////////////////////////////////
      
      // Check the outgoing message to see if it should be sent.
      // In this example it is initialized to send with a stop bit.

   if(Sys_stat==Recieve_Data)
    {
	   I2caRegs.I2CFFRX.bit.RXFFIENA=1; //允许I2C中断
	  // I2caRegs.I2CFFRX.bit.RXFFINTCLR=1;
	   ScibRegs.SCIFFTX.bit.TXFFINTCLR=0; //屏蔽TX FF中断
	   ScibRegs.SCIFFTX.bit.TXFFIENA=0;

	   if(I2cMsgOut1.MsgStatus == I2C_MSGSTAT_SEND_WITHSTOP)
        {
    	  DELAY_US(100000); //延时约700us,等待总线稳定
    	  MPU_Initial(); //初始化MPU6050

          }  // end of write section

      ///////////////////////////////////
      // Read data from RTC section //
      ///////////////////////////////////

      // Check outgoing message status. Bypass read section if status is
      // not inactive.
         if (I2cMsgOut1.MsgStatus == I2C_MSGSTAT_INACTIVE)
        {
         // Check incoming message status.
            if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP)//无停止位发送
              {
            // MPU address setup portion
              while(I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS) //读数据失败
              {
               // Maybe setup an attempt counter to break an infinite while
               // loop. The RTC will send back a NACK while it is performing
               // a write operation. Even though the write communique is
               // complete at this point, the RTC could still be busy
               // programming the data. Therefore, multiple attempts are
               // necessary.
            	  if(FailCount>20|PassCount>20){break;}
            }
            // Update current message pointer and message status
            CurrentMsgPtr = &I2cMsgIn1;  //消息指针指向输入数据
            I2cMsgIn1.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
         }

         // Once message has progressed past setting up the internal address
         // of the RTC, send a restart to read the data bytes from the
         // RTC. Complete the communique with a stop bit. MsgStatus is
         // updated in the interrupt service routine.
         else if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_RESTART)
         {
            DELAY_US(100000);
//   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
           //读取MPU6050的14个连续地址数据
            for(i=0;i<I2C_RNUMBYTES;i+=1)
			  { 
                databuffer[i]= I2cMsgIn1.MsgBuffer[i];  //get the Byte DATA from MPU
      		   }
            i=0;
//	           ACCEL_X=Rparams[0];   //give their names.
//			   ACCEL_Y=Rparams[1];
//			   ACCEL_Z=Rparams[2];
//			   TEMP=Rparams[3];
//			   GYRO_X=Rparams[4];
//			   GYRO_Y=Rparams[5];
//			   GYRO_Z=Rparams[6];
            // Read data portion


            while(I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS)
            {
               // Maybe setup an attempt counter to break an infinite while
               // loop.
            	if(FailCount>20|PassCount>20){break;}
            }//end of while;
            // Update current message pointer and message status
            CurrentMsgPtr = &I2cMsgIn1;                //指定要读取的数据
            I2cMsgIn1.MsgStatus = I2C_MSGSTAT_READ_BUSY;  //总线修改为读繁忙状态
            Sys_stat=Send_Data;
            ScibRegs.SCIFFTX.bit.TXFFIENA=1; //开串口TX FIFO中断；
            ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;//清除FIFO中断标志

         }

      } // end of read section



   }//end if(Sys_stat)
   else if(Sys_stat==Send_Data)
   {
	   while(!ScibRegs.SCICTL2.bit.TXRDY){};//等待发送总线空闲
	    ScibRegs.SCIFFTX.bit.TXFFIENA=1;
	    I2caRegs.I2CFFRX.bit.RXFFIENA=0;
	    I2caRegs.I2CFFRX.bit.RXFFINTCLR=0;//

   }
   else
   {
	 Sys_stat=Recieve_Data;
   }




   }   // end of for(;;)
}   // end of main


/*-----------------------------------------------初始化函数-------------------------------------------------------*/
void MPU_Initial()
{
//Uint16 tested;
Uint16 Wnum,i,PTR,CONTENT;
//struct I2CMSG param_msg;  //定义一个数据帧结构体变量
//functions need to be realized:
//1. registers configuration
Wnum=sizeof(Wparam)/2;   //get the size of param_pairs.
//param_msg.MsgStatus=I2C_MSGSTAT_SEND_WITHSTOP;  //选定发送方式，发送一个数据给一个停止位
//param_msg.SlaveAddress=I2C_SLAVE_ADDR;    //I2C从地址
//param_msg.NumOfBytes=Wnum;   //写的字节数
   for(i=0;i<Wnum;i++)     //查参数表，给MPU寄存器赋值
      {
         PTR=Wparam[i][1];    //取出需传送的数据值
         CONTENT=Wparam[i][0];   //MPU内部地址
         WriteData(&I2cMsgOut1,&PTR,CONTENT,1);              //write the param to MPU6050 register
 //取一个数，传送一个能否修改为一次传送14个数?
       }
}


/*-----------------------------------------------$7 I2C读写函数-------------------------------------------------------*/

Uint16 I2CA_WriteData(struct I2CMSG *msg)
{
   Uint16 i;

   // Wait until the STP bit is cleared from any previous master communication.
   // Clearing of this bit by the module is delayed until after the SCD bit is
   // set. If this bit is not checked prior to initiating a new message, the
   // I2C could get confused.
   if (I2caRegs.I2CMDR.bit.STP == 1)  //如果停止位为1
   {
      return I2C_STP_NOT_READY_ERROR;  //返回错误信息
   }

   // Setup slave address
   I2caRegs.I2CSAR = msg->SlaveAddress;  //设置从设备地址

   // Check if bus busy
   if (I2caRegs.I2CSTR.bit.BB == 1)  //总线繁忙
   {
      return I2C_BUS_BUSY_ERROR;  //返回错误信息
   }

   // Setup number of bytes to send
   // MsgBuffer + Address
   I2caRegs.I2CCNT = msg->NumOfBytes+1; //还需加上从设备地址 1个字节

   // Setup data to send
//   I2caRegs.I2CDXR = msg->MemoryHighAddr;
   I2caRegs.I2CDXR = msg->MPUAddr; //地址数据读入寄存器
// for (i=0; i<msg->NumOfBytes-2; i++)
   for (i=0; i<msg->NumOfBytes; i++) //通过I2C的FIFO连续读取14个参数

   {
      I2caRegs.I2CDXR = *(msg->MsgBuffer+i); //写数据存到发送寄存器
   }

   // Send start as master transmitter
   I2caRegs.I2CMDR.all = 0x6E20;  //设定I2C工作模式

   return I2C_SUCCESS;  //返回成功信号
}


Uint16 I2CA_ReadData(struct I2CMSG *msg)
{
   // Wait until the STP bit is cleared from any previous master communication.
   // Clearing of this bit by the module is delayed until after the SCD bit is
   // set. If this bit is not checked prior to initiating a new message, the
   // I2C could get confused.
   if (I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;   //如果STP位为1，则报错
   }

   I2caRegs.I2CSAR = msg->SlaveAddress; //设定从设备地址

   if(msg->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP)
   {
      // Check if bus busy
      if (I2caRegs.I2CSTR.bit.BB == 1)//现在繁忙
      {
         return I2C_BUS_BUSY_ERROR;  //返回错误信息
      }
      I2caRegs.I2CCNT = 1;//传几字节的地址
//      I2caRegs.I2CDXR = msg->MemoryHighAddr;
      I2caRegs.I2CDXR = msg->MPUAddr; //地址发往寄存器
      I2caRegs.I2CMDR.all = 0x2620;			// Send data to setup MPU address 发送地址
   }  //发送从设备地址
   else if(msg->MsgStatus == I2C_MSGSTAT_RESTART)
   {
      I2caRegs.I2CCNT = msg->NumOfBytes;	// Setup how many bytes to expect //数据长度
      I2caRegs.I2CMDR.all = 0x2C20;			// Send restart as master receiver //发送数据
   }

   return I2C_SUCCESS;  //返回传输成功状态信号
}

//连续向I2C总线写数据
void WriteData(struct I2CMSG *msg,Uint16 *MsgBuffer,Uint16 MemoryAdd,Uint16 NumOfBytes)
{
	Uint16 i,Error;
	for(i = 0; i < I2C_RNUMBYTES; i++)
	{
		msg->MsgBuffer[i] = MsgBuffer[i];		//将数据传送至结构体的附加数组
	}
	//msg->MemoryHighAddr = MemoryAdd >> 8;
	msg->MPUAddr = MemoryAdd & 0xff;  //取低8位
	msg->NumOfBytes = NumOfBytes;  //写几个字节
	Error = I2CA_WriteData(&I2cMsgOut1); //调用I2CA_WriteData

	if (Error == I2C_SUCCESS)
	{
		CurrentMsgPtr = &I2cMsgOut1;    //需传送的数据指针指向I2cMsgOut1
		I2cMsgOut1.MsgStatus = I2C_MSGSTAT_WRITE_BUSY;
	}
	while(I2cMsgOut1.MsgStatus != I2C_MSGSTAT_INACTIVE);
	DELAY_US(1000);
}


void pass()
{
    asm("  ESTOP0");  //跳过，停止仿真
    PassCount++;
   // for(;;);
}

void fail()
{
    asm("  ESTOP0");  //出错，跟pass同样的作用
    FailCount++;
   // for(;;);
}


//*--------------------------------中断服务子程序--------------------------------*/
interrupt void i2c_int1a_isr(void)     // I2CA 中断服务子程序
{
   Uint16 IntSource, i;
  // ScibRegs.SCIFFTX.bit.TXFFIENA=0;
   // Read interrupt source
   IntSource = I2caRegs.I2CISRC.all;
  DINT;
   // Interrupt source = stop condition detected
   if(IntSource == I2C_SCD_ISRC)
   {
      // If completed message was writing data, reset msg to inactive state
      if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_WRITE_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;  //将信息状态修改为繁忙
      }
      else
      {
         // If a message receives a NACK during the address setup portion of the
         // RTC read, the code further below included in the register access ready
         // interrupt source code will generate a stop condition. After the stop
         // condition is received (here), set the message status to try again.
         // User may want to limit the number of retries before generating an error.
         if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
         }
         // If completed message was reading RTC data, reset msg to inactive state
         // and read data from FIFO.
         else if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_READ_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;//I2C_MSGSTAT_INACTIVE;
            for(i=0; i < CurrentMsgPtr->NumOfBytes; i++)  //read 14data from MPU and save them in MsgBuffer
            {
              CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR;  //连续读取FIFO寄存器的数据
            }
		 }
        // ScibRegs.SCIFFTX.bit.TXFFIENA=1;
      }
   }  // end of stop condition detected
   // Interrupt source = Register Access Ready
   // This interrupt is used to determine when the RTC address setup portion of the
   // read data communication is complete. Since no stop bit is commanded, this flag
   // tells us when the message has been sent instead of the SCD flag. If a NACK is
   // received, clear the NACK bit and command a stop. Otherwise, move on to the read
   // data portion of the communication.
   else if(IntSource == I2C_ARDY_ISRC)
   {
      if(I2caRegs.I2CSTR.bit.NACK == 1)
      {
         I2caRegs.I2CMDR.bit.STP = 1;
         I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
      }
      else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;  //复位消息状态
      }
   }  // end of register access ready

   else
   {
      // Generate some error due to invalid interrupt source
      asm("   ESTOP0");  //有错误的话，停止仿真，
   }

   // Enable future I2C (PIE Group 8) interrupts

   PieCtrlRegs.PIEACK.all = PIEACK_GROUP8 | PIEACK_GROUP9; //使能外设中断 第8组
  //同时使能第9组中断
   EINT;
}

interrupt void scib_int_isr(void)
{
	Uint16 i;


    for(i=0;i<I2C_RNUMBYTES;i++)  //连续发送14个字节到 SCIFIFO
    {
    	ScibRegs.SCITXBUF=databuffer[i];
    }

    //
    ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;//清除FIFO中断标志
    Sys_stat=Recieve_Data;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP8 | PIEACK_GROUP9;  //使能外设中断 第8组同时使能第9组中断

}

//
//*===========================================================================
//---------------------EOF---------------------------------------------------
//===========================================================================*/
