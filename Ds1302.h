#ifndef _DS1302_H_
#define _DS1302_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "Date.h"

#define RST_1  HAL_GPIO_WritePin(DS_RSTx_GPIO_Port, DS_RSTx_Pin,GPIO_PIN_SET);DS1320_Delay(1)
#define RST_0  HAL_GPIO_WritePin(DS_RSTx_GPIO_Port, DS_RSTx_Pin,GPIO_PIN_RESET);DS1320_Delay(1)

#define CLK_1  HAL_GPIO_WritePin(DS_CLK_GPIO_Port, DS_CLK_Pin,GPIO_PIN_SET);DS1320_Delay(1)
#define CLK_0  HAL_GPIO_WritePin(DS_CLK_GPIO_Port, DS_CLK_Pin,GPIO_PIN_RESET);DS1320_Delay(1)

#define W_1    HAL_GPIO_WritePin(DS_IO_GPIO_Port, DS_IO_Pin,GPIO_PIN_SET)
#define W_0    HAL_GPIO_WritePin(DS_IO_GPIO_Port, DS_IO_Pin,GPIO_PIN_RESET)

void DS1302_INIT(void);
void DS1302_SET(void);
void DS1302_READ(void);
void DS1302_MSG(void);
void DS1320_Delay(uint32_t t);

void DS1302_TIME(uint16_t Y,uint16_t M,uint16_t D,uint16_t h,uint16_t m,uint16_t s);



#endif
