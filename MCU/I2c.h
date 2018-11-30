#ifndef __AF_I2C_H
#define __AF_I2C_H

#include "../Own/af_default.h"
/* 
 * EEPROM 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
#define EEPROM_I2C_ADDRESS                         0xA0
#define I2C_OWN_ADDRESS                            0x0A              // stm32本机I2C地址

__inline HAL_StatusTypeDef I2C_EEPROM_IsDeviceReady(uint16_t DevAddress=EEPROM_I2C_ADDRESS, uint32_t Trials=1);
uint8_t  I2C_EEPROM_Read( uint8_t MemAddress,uint8_t *pBuffer=NULL,uint16_t Length=1,uint16_t DevAddress=EEPROM_I2C_ADDRESS, uint16_t MemAddSize=I2C_MEMADD_SIZE_8BIT);
HAL_StatusTypeDef  I2C_EEPROM_Write( uint8_t MemAddress,uint8_t *pBuffer,uint16_t DevAddress=EEPROM_I2C_ADDRESS, uint16_t MemAddSize=I2C_MEMADD_SIZE_8BIT);
HAL_StatusTypeDef  I2C_EEPROM_WriteByte( uint8_t MemAddress, uint8_t Value,uint16_t DevAddress=EEPROM_I2C_ADDRESS,uint16_t MemAddSize=I2C_MEMADD_SIZE_8BIT);

void WriteEEPROMTest(uint16_t Number,uint8_t t=0xff);
void ReadEEPROMTest(uint16_t Number);


#define EEPROM_DEBUG_ON         1

#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                              printf("*DEBUG*[%s][%d]"fmt"\n",__FILE__,__LINE__,##arg);\
                                          }while(0)


#endif
