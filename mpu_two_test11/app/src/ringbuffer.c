/**
  ****************************************************************************************************
  * @file    	ringbuffer.c
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


#include "ringbuffer.h"




/**
  * @brief  fifo��ʼ��
  * @param  fifo: ʵ��
  * @param  buffer: fifo�Ļ�����
  * @param  size: ��������С
  * @retval None
  */
void ringbuffer_init(ringbuffer_t *fifo, Uint8 *buffer, Uint16 size)
{
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
}


/**
  * @brief  ��ȡ�Ѿ�ʹ�õĿռ�
  * @param  fifo: ʵ��
  * @retval Uint16: ��ʹ�ø���
  */
Uint16 ringbuffer_getUsedSize(ringbuffer_t *fifo)
{
    if (fifo->in >= fifo->out)
        return (fifo->in - fifo->out);
    else
        return (fifo->size - fifo->out + fifo->in);
}


/**
  * @brief  ��ȡδʹ�ÿռ�
  * @param  fifo: ʵ��
  * @retval Uint16: ʣ�����
  */
Uint16 ringbuffer_getRemainSize(ringbuffer_t *fifo)
{
    return (fifo->size - ringbuffer_getUsedSize(fifo) - 1);
}


/**
  * @brief  FIFO�Ƿ�Ϊ��
  * @param  fifo: ʵ��
  * @retval Uint8: 1 Ϊ�� 0 ��Ϊ�գ������ݣ�
  */
Uint8 ringbuffer_isEmpty(ringbuffer_t *fifo)
{
    return (fifo->in == fifo->out);
}


/**
  * @brief  �������ݵ����λ������������ʣ��ռ䣩
  * @param  fifo: ʵ��
  * @param  data: &#&
  * @param  len: &#&
  * @retval none
  */
void ringbuffer_in(ringbuffer_t *fifo, Uint8 *data, Uint16 len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        fifo->buffer[fifo->in] = data[i];
        fifo->in = (fifo->in + 1) % fifo->size;
    }
}


/**
  * @brief  �������ݵ����λ�����(��ʣ��ռ��⣬�ռ䲻�㷢��ʧ��)
  * @param  fifo: ʵ��
  * @param  data: &#&
  * @param  len: &#&
  * @retval Uint8: 0 �ɹ� 1ʧ�ܣ��ռ䲻�㣩
  */
Uint8 ringbuffer_in_check(ringbuffer_t *fifo, Uint8 *data, Uint16 len)
{
    Uint16 remainsize = ringbuffer_getRemainSize(fifo);

    if (remainsize < len) //�ռ䲻��
        return 1;

    ringbuffer_in(fifo, data, len);

    return 0;
}


/**
  * @brief  �ӻ��λ�������ȡ����
  * @param  fifo: ʵ��
  * @param  buf: �������
  * @param  len: ������鳤��
  * @retval Uint16: ʵ�ʶ�ȡ����
  */
Uint16 ringbuffer_out(ringbuffer_t *fifo, Uint8 *buf, Uint16 len)
{
    int i;
    Uint16 remainToread = ringbuffer_getUsedSize(fifo);

    if (remainToread > len) 
    {
        remainToread = len;
    }

    for (i = 0; i < remainToread; i++)
    {
        buf[i] = fifo->buffer[fifo->out];
        fifo->out = (fifo->out + 1) % fifo->size;
    }

    return remainToread;
}







/*******************************END OF FILE************************************/


