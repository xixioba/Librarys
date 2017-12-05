#ifndef _RS485_H
#define _RS485_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "mb.h"
#include "mbport.h"
#include "tim.h"
#define RS485_Rx   HAL_GPIO_WritePin(RS485_RT_GPIO_Port, RS485_RT_Pin, GPIO_PIN_RESET);
#define RS485_Tx    HAL_GPIO_WritePin(RS485_RT_GPIO_Port, RS485_RT_Pin, GPIO_PIN_SET);
/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 0
#define REG_INPUT_NREGS 255

#define REG_HOLDING_START 0
#define REG_HOLDING_NREGS 255

#define USARTx    USART3
#define USART485 huart3

#define TIM485 htim1

extern uint16_t   usRegInputStart;
extern uint16_t   usRegInputBuf[REG_INPUT_NREGS];
extern uint16_t   usRegHoldingBuf[REG_HOLDING_NREGS];
void RS485_MSG(void);
/*
    RS485移植freemodbus 
    1.移植所有文件，修REG_INPUT_NREGS寄存器数量
    2.修改所用USART（中断模式）,TIM
    3.IT中断函数添加USART函数*2（RXNE,TXE)，TIM函数*1
    4.TIM.PSC=3599;中断模式 reset
    03,06->holding reg
    04->input reg
*/

typedef union
{
   float Source;
   uint8_t  Target[4];
}IEE754_HL;
typedef struct 
{
    IEE754_HL Temp;
    uint8_t Array[8];
    double Decimals;
}IEE754_LH;
uint8_t* Decimals_IEE754(double t_Source,uint16_t *Target,uint8_t Mode);//MODE=4 float =8 double
double IEE754_Decimals(uint32_t *t_Source,uint8_t Mode);
#endif
