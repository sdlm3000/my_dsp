/*
 * common.c
 *
 *  Created on: 2022Äê1ÔÂ25ÈÕ
 *      Author: sdlm
 */

#include "common.h"

void delay_ms(Uint16 nms)
{
    DELAY_US(nms * 1000);
}
