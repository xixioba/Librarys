#include "af_spi.h"

/**
  * º¯Êý¹¦ÄÜ: ²Á³ýÉÈÇø
  * ÊäÈë²ÎÊý: SectorAddr£º´ý²Á³ýÉÈÇøµØÖ·£¬ÒªÇóÎª4096±¶Êý
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£º´®ÐÐFlash×îÐ¡²Á³ý¿é´óÐ¡Îª4KB(4096×Ö½Ú)£¬¼´Ò»¸öÉÈÇø´óÐ¡£¬ÒªÇóÊäÈë²ÎÊý
  *           Îª4096±¶Êý¡£ÔÚÍù´®ÐÐFlashÐ¾Æ¬Ð´ÈëÊý¾ÝÖ®Ç°ÒªÇóÏÈ²Á³ý¿Õ¼ä¡£
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  /* ·¢ËÍFLASHÐ´Ê¹ÄÜÃüÁî */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* ²Á³ýÉÈÇø */
  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();
  /* ·¢ËÍÉÈÇø²Á³ýÖ¸Áî*/
  SPI_FLASH_SendByte(W25X_SectorErase);
  /*·¢ËÍ²Á³ýÉÈÇøµØÖ·µÄ¸ßÎ»*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* ·¢ËÍ²Á³ýÉÈÇøµØÖ·µÄÖÐÎ» */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* ·¢ËÍ²Á³ýÉÈÇøµØÖ·µÄµÍÎ» */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* ½ûÓÃ´®ÐÐFLASH: CS ¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();
  /* µÈ´ý²Á³ýÍê±Ï*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * º¯Êý¹¦ÄÜ: ²Á³ýÕûÆ¬
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£º²Á³ý´®ÐÐFlashÕûÆ¬¿Õ¼ä
  */
void SPI_FLASH_BulkErase(void)
{
 /* ·¢ËÍFLASHÐ´Ê¹ÄÜÃüÁî */
  SPI_FLASH_WriteEnable();

  /* ÕûÆ¬²Á³ý Erase */
  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();
  /* ·¢ËÍÕûÆ¬²Á³ýÖ¸Áî*/
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* ½ûÓÃ´®ÐÐFLASH: CS¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();

  /* µÈ´ý²Á³ýÍê±Ï*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * º¯Êý¹¦ÄÜ: Íù´®ÐÐFLASH°´Ò³Ð´ÈëÊý¾Ý£¬µ÷ÓÃ±¾º¯ÊýÐ´ÈëÊý¾ÝÇ°ÐèÒªÏÈ²Á³ýÉÈÇø
  * ÊäÈë²ÎÊý: pBuffer£º´ýÐ´ÈëÊý¾ÝµÄÖ¸Õë
  *           WriteAddr£ºÐ´ÈëµØÖ·
  *           NumByteToWrite£ºÐ´ÈëÊý¾Ý³¤¶È£¬±ØÐëÐ¡ÓÚµÈÓÚSPI_FLASH_PerWritePageSize
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£º´®ÐÐFlashÃ¿Ò³´óÐ¡Îª256¸ö×Ö½Ú
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* ·¢ËÍFLASHÐ´Ê¹ÄÜÃüÁî */
  SPI_FLASH_WriteEnable();

   /* Ñ°ÕÒ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();
  /* Ð´ËÍÐ´Ö¸Áî*/
  SPI_FLASH_SendByte(W25X_PageProgram);
  /*·¢ËÍÐ´µØÖ·µÄ¸ßÎ»*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*·¢ËÍÐ´µØÖ·µÄÖÐÎ»*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*·¢ËÍÐ´µØÖ·µÄµÍÎ»*/
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: SPI_FLASH_PageWrite too large!\n");
  }

  /* Ð´ÈëÊý¾Ý*/
  while (NumByteToWrite--)
  {
     /* ·¢ËÍµ±Ç°ÒªÐ´ÈëµÄ×Ö½ÚÊý¾Ý */
    SPI_FLASH_SendByte(*pBuffer);
     /* Ö¸ÏòÏÂÒ»×Ö½ÚÊý¾Ý */
    pBuffer++;
  }

  /* ½ûÓÃ´®ÐÐFLASH: CS ¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();

  /* µÈ´ýÐ´ÈëÍê±Ï*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * º¯Êý¹¦ÄÜ: Íù´®ÐÐFLASHÐ´ÈëÊý¾Ý£¬µ÷ÓÃ±¾º¯ÊýÐ´ÈëÊý¾ÝÇ°ÐèÒªÏÈ²Á³ýÉÈÇø
  * ÊäÈë²ÎÊý: pBuffer£º´ýÐ´ÈëÊý¾ÝµÄÖ¸Õë
  *           WriteAddr£ºÐ´ÈëµØÖ·
  *           NumByteToWrite£ºÐ´ÈëÊý¾Ý³¤¶È
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£º¸Ãº¯Êý¿ÉÒÔÉèÖÃÈÎÒâÐ´ÈëÊý¾Ý³¤¶È
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* ÈôµØÖ·Óë SPI_FLASH_PageSize ¶ÔÆë  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* ÈôµØÖ·Óë SPI_FLASH_PageSize ²»¶ÔÆë */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * º¯Êý¹¦ÄÜ: ´Ó´®ÐÐFlash¶ÁÈ¡Êý¾Ý
  * ÊäÈë²ÎÊý: pBuffer£º´æ·Å¶ÁÈ¡µ½Êý¾ÝµÄÖ¸Õë
  *           ReadAddr£º¶ÁÈ¡Êý¾ÝÄ¿±êµØÖ·
  *           NumByteToRead£º¶ÁÈ¡Êý¾Ý³¤¶È
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£º¸Ãº¯Êý¿ÉÒÔÉèÖÃÈÎÒâ¶ÁÈ¡Êý¾Ý³¤¶È
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();

  /* ·¢ËÍ ¶Á Ö¸Áî */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* ·¢ËÍ ¶Á µØÖ·¸ßÎ» */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* ·¢ËÍ ¶Á µØÖ·ÖÐÎ» */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* ·¢ËÍ ¶Á µØÖ·µÍÎ» */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* ¶ÁÈ¡Êý¾Ý */
  {
     /* ¶ÁÈ¡Ò»¸ö×Ö½Ú*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Ö¸ÏòÏÂÒ»¸ö×Ö½Ú»º³åÇø */
    pBuffer++;
  }

  /* ½ûÓÃ´®ÐÐFLASH: CS ¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();
}

/**
  * º¯Êý¹¦ÄÜ: ¶ÁÈ¡´®ÐÐFlashÐÍºÅµÄID
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: uint32_t£º´®ÐÐFlashµÄÐÍºÅID
  * Ëµ    Ã÷£º  FLASH_ID      ICÐÍºÅ      ´æ´¢¿Õ¼ä´óÐ¡         
                0xEF3015      W25X16        2M byte
                0xEF4015	    W25Q16        4M byte
                0XEF4017      W25Q64        8M byte
                0XEF4018      W25Q128       16M byte  (YS-F1Pro¿ª·¢°åÄ¬ÈÏÅäÖÃ)
  */
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();

  /* ·¢ËÍÃüÁî£º¶ÁÈ¡Ð¾Æ¬ÐÍºÅID */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* ´Ó´®ÐÐFlash¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* ´Ó´®ÐÐFlash¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* ´Ó´®ÐÐFlash¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  /* ½ûÓÃ´®ÐÐFlash£ºCS¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();
  
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}

/**
  * º¯Êý¹¦ÄÜ: ¶ÁÈ¡´®ÐÐFlashÉè±¸ID
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: uint32_t£º´®ÐÐFlashµÄÉè±¸ID
  * Ëµ    Ã÷£º
  */
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t Temp = 0;

  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();

  /* ·¢ËÍÃüÁî£º¶ÁÈ¡Ð¾Æ¬Éè±¸ID * */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  /* ´Ó´®ÐÐFlash¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* ½ûÓÃ´®ÐÐFlash£ºCS¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();

  return Temp;
}

/**
  * º¯Êý¹¦ÄÜ: Æô¶¯Á¬Ðø¶ÁÈ¡Êý¾Ý´®
  * ÊäÈë²ÎÊý: ReadAddr£º¶ÁÈ¡µØÖ·
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºInitiates a read data byte (READ) sequence from the Flash.
  *           This is done by driving the /CS line low to select the device,
  *           then the READ instruction is transmitted followed by 3 bytes
  *           address. This function exit and keep the /CS line low, so the
  *           Flash still being selected. With this technique the whole
  *           content of the Flash is read with a single READ instruction.
  */
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * º¯Êý¹¦ÄÜ: ´Ó´®ÐÐFlash¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: uint8_t£º¶ÁÈ¡µ½µÄÊý¾Ý
  * Ëµ    Ã÷£ºThis function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
uint8_t SPI_FLASH_ReadByte(void)
{
  uint8_t d_read,d_send=Dummy_Byte;
  if(HAL_SPI_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read;    
}

/**
  * º¯Êý¹¦ÄÜ: Íù´®ÐÐFlash¶ÁÈ¡Ð´ÈëÒ»¸ö×Ö½ÚÊý¾Ý²¢½ÓÊÕÒ»¸ö×Ö½ÚÊý¾Ý
  * ÊäÈë²ÎÊý: byte£º´ý·¢ËÍÊý¾Ý
  * ·µ »Ø Öµ: uint8_t£º½ÓÊÕµ½µÄÊý¾Ý
  * Ëµ    Ã÷£ºÎÞ
  */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read; 
}

/**
  * º¯Êý¹¦ÄÜ: Ê¹ÄÜ´®ÐÐFlashÐ´²Ù×÷
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void SPI_FLASH_WriteEnable(void)
{
  /* Ñ¡Ôñ´®ÐÐFLASH: CSµÍµçÆ½ */
  FLASH_SPI_CS_ENABLE();

  /* ·¢ËÍÃüÁî£ºÐ´Ê¹ÄÜ */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* ½ûÓÃ´®ÐÐFlash£ºCS¸ßµçÆ½ */
  FLASH_SPI_CS_DISABLE();
}

/**
  * º¯Êý¹¦ÄÜ: µÈ´ýÊý¾ÝÐ´ÈëÍê³É
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºPolls the status of the Write In Progress (WIP) flag in the
  *           FLASH's status  register  and  loop  until write  opertaion
  *           has completed.
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}


/**
  * º¯Êý¹¦ÄÜ: ½øÈëµôµçÄ£Ê½
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}   

/**
  * º¯Êý¹¦ÄÜ: »½ÐÑ´®ÐÐFlash
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE(); 
}   




void Copy_font_from_sd(void)
{
FATFS fs;													/* FatFsÎÄ¼þÏµÍ³¶ÔÏó */
FIL file;													/* ÎÄ¼þ¶ÔÏó */
FRESULT f_res;                    /* ÎÄ¼þ²Ù×÷½á¹û */
UINT fnum;            					  /* ÎÄ¼þ³É¹¦¶ÁÐ´ÊýÁ¿ */
	uint32_t write_addr=0,j=0,i=0;
	uint8_t tempbuf[256]={0};
    printf("\r\n0x%X\r\n",SPI_FLASH_ReadID());
	if(FATFS_GetAttachedDriversNbr()!=0)
	{
			if(f_mount (&fs, "0:", 1)!=FR_OK)
			{
					LCD_Show(160,20,"FAILED MOUNT");
					DEBUG("FATFS MOUNT WRONG");
					while(1);
			}
			
	}
	else
	{
			LCD_Show(160,20,"NO ATTACHERDRIVERS\n");
			DEBUG("NO ATTACHERDRIVERS");
			while(1);
	}
    f_res = f_open(&file,"0:GBK_1616.FON",FA_OPEN_EXISTING | FA_READ);  
    if(f_res==FR_OK)
    {
      // ²Á³ý¿Õ¼ä
      write_addr=GBK_1616_ADDR;
      for(j=0;j<GBK_1616_SIZE;j++)//²Á³ýÉÈÇø
      {
        SPI_FLASH_SectorErase(write_addr+j*4096);
          for(i=0;i<16;i++)
          {
            f_res = f_read( &file, tempbuf, 256, &fnum);//¶ÁÈ¡Êý¾Ý	 
            if(f_res!=FR_OK||fnum!=256)
            {            //Ö´ÐÐ´íÎó  
                DEBUG();
                break;
            }
            SPI_FLASH_PageWrite(tempbuf,write_addr+j*4096+i*256,256);  //¿½±´Êý¾Ýµ½´®ÐÐflashÉÏ
          }
            
      }
    f_close(&file);
    
    SPI_FLASH_BufferRead(tempbuf,GBK_1616_ADDR,256);			//¶ÁÈ¡Êý¾Ý£¬´òÓ¡ÑéÖ¤
    printf("readbuf GBK_1616.FON:\n");
    for(j=0;j<256;j++)
      printf("%02X ",tempbuf[j]);

}
    }


