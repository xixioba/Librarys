#ifndef __12864TXA_H
#define __12864TXA_H
#include "main.h"
#include "stm32f1xx_hal.h"

#define BUSY_CHECK 1

#define REFRESH_MODE 1

#define RESET_1  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin,GPIO_PIN_SET)
#define RESET_0  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin,GPIO_PIN_RESET)

#define CS2_1  	HAL_GPIO_WritePin(LCD_CS2_GPIO_Port, LCD_CS2_Pin,GPIO_PIN_SET)//片选1
#define CS2_0  	HAL_GPIO_WritePin(LCD_CS2_GPIO_Port, LCD_CS2_Pin,GPIO_PIN_RESET)

#define CS1_1  	HAL_GPIO_WritePin(LCD_CS1_GPIO_Port, LCD_CS1_Pin,GPIO_PIN_SET)//片选2
#define CS1_0  	HAL_GPIO_WritePin(LCD_CS1_GPIO_Port, LCD_CS1_Pin,GPIO_PIN_RESET)

#define E_1  	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin,GPIO_PIN_SET)//使能信号
#define E_0  	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin,GPIO_PIN_RESET)

#define RW_1  	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin,GPIO_PIN_SET)//读写信号 
#define RW_0  	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin,GPIO_PIN_RESET)

#define RS_1  	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_SET)//寄存器选择
#define RS_0  	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_RESET)

#define LCD_Write(LCD_DATA)                 GPIOD->BSRR = (LCD_DATA<<8)&0Xff00;\
                                            GPIOD->BRR = (~(LCD_DATA<<8))&0Xff00;
#define LCD_Busy()  HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)
//#define LCD_NF()  HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)
//#define LCD_RESET()  HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11)

void LCD_SET_COM(uint8_t line);
void LCD_Switch(uint8_t state);
void LCD_SET_COLUMN(uint8_t column);
void LCD_SET_PAGE(uint8_t page);


//正常打点读点函数
uint8_t LCD_GET_Pixels(int x, int y);
uint8_t LCD_GET_Pixel(int x, int y);
void LCD_SET_Pixel(int x, int y, int PixelIndex);


//缓存模式
uint8_t LCD_GET_Pixels_M(int x, int y);
uint8_t LCD_GET_Pixel_M(int x, int y);
void LCD_SET_Pixel_M(int x, int y, int PixelIndex);
void LCD_Refresh_M(void);

void LCD_Init(void);

#endif
