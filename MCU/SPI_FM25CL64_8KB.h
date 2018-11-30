#ifndef __SPI_FM25CL64_8KB_H
#define __SPI_FM25CL64_8KB_H

#include "stm32f1xx_hal.h"
#include "Date.h"
#define FM25CL64_SPI_CS_ENABLE()          HAL_GPIO_WritePin(FM25CL64_NSS_GPIO_Port, FM25CL64_NSS_Pin, GPIO_PIN_RESET);
#define FM25CL64_SPI_CS_DISABLE()         HAL_GPIO_WritePin(FM25CL64_NSS_GPIO_Port, FM25CL64_NSS_Pin, GPIO_PIN_SET);
#define FM25CL64_SPI_WP_ENABLE()          HAL_GPIO_WritePin(FM25CL64_WPx_GPIO_Port, FM25CL64_WPx_Pin, GPIO_PIN_RESET);
#define FM25CL64_SPI_WP_DISABLE()         HAL_GPIO_WritePin(FM25CL64_WPx_GPIO_Port, FM25CL64_WPx_Pin, GPIO_PIN_SET);


#define FM25CL64_WREN 0x06//设置写使能锁存器0000 0110b
#define FM25CL64_WRDI 0x04//写禁止0000 0100b
#define FM25CL64_RDSR 0x05//读状态寄存器0000 0101b
#define FM25CL64_WRSR 0x01//写状态寄存器0000 0001b
#define FM25CL64_READ 0x03//读存储器数据0000 0011b
#define FM25CL64_WRITE 0x02//写存储器数据0000 0010b

extern SPI_HandleTypeDef hspi3;

uint8_t FM25CL64_SR(uint8_t byte);
void FM25CL64_WriteByte(uint16_t Addr,uint8_t Data);
uint8_t FM25CL64_ReadByte(uint16_t Addr);
uint8_t FM25CL64_Write(uint16_t Addr,uint8_t* pBuffer, uint16_t NumByte);
uint8_t FM25CL64_Read(uint16_t Addr,uint8_t* pBuffer, uint16_t NumByte);
uint8_t FM25CL64_RW_Byte(uint8_t byte);
void FLASH_READ(void);
void FLASH_MSG(void);
void FLASH_Init(uint8_t byte);
#endif
