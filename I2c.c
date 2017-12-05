#include "../Own/af_i2c.h"

 uint8_t I2c_Buf_Write[256]={0};
 uint8_t I2c_Buf_Read[256]={0};


static HAL_StatusTypeDef Check_EEPROM_Status(uint16_t DevAddress)
{
  uint8_t i=0;
  while(I2C_EEPROM_IsDeviceReady(DevAddress)!=HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        MX_I2C1_Init();
        i++;
        if(i>10)
        {
            return HAL_ERROR;
        }
    }
    return HAL_OK;
}    

/**
  * 函数功能: 通过I2C写入一个值到指定寄存器内
  * 输入参数: DevAddress：I2C设备地址
  *           MemAddress：目标寄存器
  *           Value：值
  * 返 回 值: 无
  * 说    明: 无
  */
HAL_StatusTypeDef I2C_EEPROM_WriteByte( uint8_t MemAddress, uint8_t Value,uint16_t DevAddress,uint16_t MemAddSize)
{
    HAL_StatusTypeDef status;
    if(Check_EEPROM_Status(DevAddress)!=HAL_OK)EEPROM_DEBUG(); 
    status=HAL_I2C_Mem_Write(&hi2c1, DevAddress, (uint16_t)MemAddress, MemAddSize, &Value, 1, 3000);
    HAL_Delay(5);
    return status;
}

/**
  * 函数功能: 通过I2C写入一段数据到指定寄存器内
  * 输入参数: DevAddress：I2C设备地址
  *           MemAddress：目标寄存器
  *           MemAddSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 在循环调用是需加一定延时时间
  */
HAL_StatusTypeDef  I2C_EEPROM_Write(uint8_t MemAddress,uint8_t *pBuffer,uint16_t DevAddress, uint16_t MemAddSize)
{
 if(Check_EEPROM_Status(DevAddress)!=HAL_OK)EEPROM_DEBUG();     
 return HAL_I2C_Mem_Write(&hi2c1, DevAddress, (uint16_t)MemAddress, MemAddSize, pBuffer, 8, 3000);;
}


/**
  * 函数功能: 通过I2C读取一段寄存器内容存放到指定的缓冲区内
  * 输入参数: DevAddress：I2C设备地址
  *           MemAddress：目标寄存器
  *           MemAddSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
uint8_t I2C_EEPROM_Read(uint8_t MemAddress,uint8_t *pBuffer,uint16_t Length,uint16_t DevAddress, uint16_t MemAddSize)
{
    uint8_t value = 0;
     if(Check_EEPROM_Status(DevAddress)!=HAL_OK)EEPROM_DEBUG(); 
    if (Length==1)
        return HAL_I2C_Mem_Read(&hi2c1, DevAddress, (uint16_t)MemAddress, MemAddSize, &value, 1, 3000);
    else
        return HAL_I2C_Mem_Read(&hi2c1, DevAddress, (uint16_t)MemAddress, MemAddSize, pBuffer, Length, 3000);
}

/**
  * 函数功能: 检测I2C设备是否处于准备好可以通信状态
  * 输入参数: DevAddress：I2C设备地址
  *           Trials：尝试测试次数
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
__inline HAL_StatusTypeDef I2C_EEPROM_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, 3000));
}

void WriteEEPROMTest(uint16_t Number,uint8_t t )
{
    uint16_t i;
     printf("待写入的数据:\n");    
	for ( i=0; i<Number; i++ ) //填充缓冲
  {
    I2c_Buf_Write[i]=t;
    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n");
   }
  for(i=0;i<Number;i+=8)
  {
    I2C_EEPROM_Write(i,&I2c_Buf_Write[i]);
    HAL_Delay(5);// 短延时不能少
  }
    
}

void ReadEEPROMTest(uint16_t Number)
{
    uint16_t i;
      printf("读出的数据:\n");
   I2C_EEPROM_Read(0,&I2c_Buf_Read[0],Number);
   for (i=0;i<Number;i++)
	 {    
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n");
	}
}



