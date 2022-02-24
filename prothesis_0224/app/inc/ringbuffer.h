/**
  ****************************************************************************************************
  * @file    	ringbuffer.h
  * @author		����ԭ���Ŷ�(ALIENTEK)
  * @version    V1.0
  * @date		2020-04-17
  * @brief   	�����ͻ�������������
  * @license   	Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
  ****************************************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����ԭ�� STM32������
  * ������Ƶ:www.yuanzige.com
  * ������̳:www.openedv.com
  * ��˾��ַ:www.alientek.com
  * �����ַ:openedv.taobao.com
  *
  * �޸�˵��
  * V1.0 2020-04-17
  * ��һ�η���
  *
  ****************************************************************************************************
  */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include "DSP2833x_Device.h"    // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"  //  DSP2833x  Examples  Include  File
#include "common.h"


/*���λ��������ݽṹ*/
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



