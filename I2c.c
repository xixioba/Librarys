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
  * ��������: ͨ��I2Cд��һ��ֵ��ָ���Ĵ�����
  * �������: DevAddress��I2C�豸��ַ
  *           MemAddress��Ŀ��Ĵ���
  *           Value��ֵ
  * �� �� ֵ: ��
  * ˵    ��: ��
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
  * ��������: ͨ��I2Cд��һ�����ݵ�ָ���Ĵ�����
  * �������: DevAddress��I2C�豸��ַ
  *           MemAddress��Ŀ��Ĵ���
  *           MemAddSize���Ĵ����ߴ�(8λ����16λ)
  *           pBuffer��������ָ��
  *           Length������������
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��ѭ�����������һ����ʱʱ��
  */
HAL_StatusTypeDef  I2C_EEPROM_Write(uint8_t MemAddress,uint8_t *pBuffer,uint16_t DevAddress, uint16_t MemAddSize)
{
 if(Check_EEPROM_Status(DevAddress)!=HAL_OK)EEPROM_DEBUG();     
 return HAL_I2C_Mem_Write(&hi2c1, DevAddress, (uint16_t)MemAddress, MemAddSize, pBuffer, 8, 3000);;
}


/**
  * ��������: ͨ��I2C��ȡһ�μĴ������ݴ�ŵ�ָ���Ļ�������
  * �������: DevAddress��I2C�豸��ַ
  *           MemAddress��Ŀ��Ĵ���
  *           MemAddSize���Ĵ����ߴ�(8λ����16λ)
  *           pBuffer��������ָ��
  *           Length������������
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��
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
  * ��������: ���I2C�豸�Ƿ���׼���ÿ���ͨ��״̬
  * �������: DevAddress��I2C�豸��ַ
  *           Trials�����Բ��Դ���
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��
  */
__inline HAL_StatusTypeDef I2C_EEPROM_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, 3000));
}

void WriteEEPROMTest(uint16_t Number,uint8_t t )
{
    uint16_t i;
     printf("��д�������:\n");    
	for ( i=0; i<Number; i++ ) //��仺��
  {
    I2c_Buf_Write[i]=t;
    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n");
   }
  for(i=0;i<Number;i+=8)
  {
    I2C_EEPROM_Write(i,&I2c_Buf_Write[i]);
    HAL_Delay(5);// ����ʱ������
  }
    
}

void ReadEEPROMTest(uint16_t Number)
{
    uint16_t i;
      printf("����������:\n");
   I2C_EEPROM_Read(0,&I2c_Buf_Read[0],Number);
   for (i=0;i<Number;i++)
	 {    
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n");
	}
}



