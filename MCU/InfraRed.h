#ifndef __INFRARED_H
#define __INFRARED_H

#include "stm32f1xx_hal.h"
#include "main.h"



typedef enum
{
    InfradRed_ERROR=0x00,
    InfradRed_OK=0x01,
    InfradRed_REPEAT=0x02
}InfradRed_State;

extern TIM_HandleTypeDef htim6;
void m_delay1us(const uint32_t count);
uint16_t ReadIFR(void);
#endif
