/**
  ****************************************************************************************************
  * @file    	ringbuffer.h
  * @author		正点原子团队(ALIENTEK)
  * @version    V1.0
  * @date		2020-04-17
  * @brief   	环形型缓冲区操作驱动
  * @license   	Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
  ****************************************************************************************************
  * @attention
  *
  * 实验平台:正点原子 STM32开发板
  * 在线视频:www.yuanzige.com
  * 技术论坛:www.openedv.com
  * 公司网址:www.alientek.com
  * 购买地址:openedv.taobao.com
  *
  * 修改说明
  * V1.0 2020-04-17
  * 第一次发布
  *
  ****************************************************************************************************
  */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File
#include "common.h"


/*环形缓冲区数据结构*/
typedef struct
{
    Uint8  *buffer;
    Uint16 size;
    Uint16 in;
    Uint16 out;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t *fifo, Uint8 *buffer, Uint16 size);

Uint16 ringbuffer_getUsedSize(ringbuffer_t *fifo);
Uint16 ringbuffer_getRemainSize(ringbuffer_t *fifo);
Uint8 ringbuffer_isEmpty(ringbuffer_t *fifo);

void ringbuffer_in(ringbuffer_t *fifo, Uint8 *data, Uint16 len);
Uint8 ringbuffer_in_check(ringbuffer_t *fifo, Uint8 *data, Uint16 len);
Uint16 ringbuffer_out(ringbuffer_t *fifo, Uint8 *buf, Uint16 len);



#endif /* _RINGBUFFER_H_ */



