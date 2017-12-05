#include "../Own/SPI_FM25CL64_8KB.h"
#include <string.h> 


/*
  * ��������: ��spi��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: Byte������������ 0 Read only,else,Write and Read
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
*/
uint8_t FM25CL64_RW_Byte(uint8_t byte)
{
  uint8_t d_read=0;
  if(HAL_SPI_TransmitReceive(&hspi3,&byte,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=0; 
  return d_read; 
}


static void FM25CL64_WriteEnable()
{
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_WREN);
    FM25CL64_SPI_CS_DISABLE();
}

static void FM25CL64_WriteDisable()
{
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_WRDI);
    FM25CL64_SPI_CS_DISABLE();
}

/*
WPEN 0 0 0 BP1 BP0 WEL 0 

BP1 BP0 �ܱ����ĵ�ַ��Χ
0 0 ��
0 1 1800h��1FFFh����1/4 ��ַ������
1 0 1000h��1FFFh����1/2 ��ַ������
1 1 0000h��1FFFh�������洢���У�

WEL WPEN /WP �ܱ����Ĵ洢��    δ�ܵ������Ĵ洢��   ״̬�Ĵ���
0   X     X     �ܱ�����            �ܱ�����        �ܱ�����
1   0     X     �ܱ�����            δ�ܱ�����      δ�ܱ�����
1   1     0     �ܱ�����            δ�ܱ�����      �ܱ�����
1   1     1     �ܱ�����            δ�ܱ�����      δ�ܱ�����

0x00 ȡ������ 0x84 ������1/4
*/
uint8_t FM25CL64_SR(uint8_t byte)
{
    uint8_t read=0;
    FM25CL64_SPI_WP_DISABLE();
    FM25CL64_WriteEnable();
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_WRSR);
    read=FM25CL64_RW_Byte(byte);
    FM25CL64_SPI_CS_DISABLE(); 
    FM25CL64_WriteDisable();
    FM25CL64_SPI_WP_ENABLE();
    return read;
}

void FM25CL64_WriteByte(uint16_t Addr,uint8_t Data)
{
    FM25CL64_WriteEnable();
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_WRITE);
    FM25CL64_RW_Byte(Addr>>8);
    FM25CL64_RW_Byte(Addr&0xFF);
    FM25CL64_RW_Byte(Data);
    FM25CL64_SPI_CS_DISABLE();
}

uint8_t FM25CL64_ReadByte(uint16_t Addr)
{
    uint8_t Data=0;
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_READ);
    FM25CL64_RW_Byte(Addr>>8);
    FM25CL64_RW_Byte(Addr&0xff);
    Data=FM25CL64_RW_Byte(0);
    FM25CL64_SPI_CS_DISABLE();
    return Data;
}
    
uint8_t FM25CL64_Write(uint16_t Addr,uint8_t* pBuffer, uint16_t NumByte)
{
    FM25CL64_WriteEnable();
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_WRITE);
    FM25CL64_RW_Byte(Addr>>8);
    FM25CL64_RW_Byte(Addr&0xff);
    if(pBuffer==NULL)
    {
        FM25CL64_RW_Byte(NumByte);
    }
    else 
    {
        if((Addr+NumByte)<=0x1fff)
        {
            while(NumByte--)
            {
               FM25CL64_RW_Byte(*pBuffer);
               pBuffer++;
            }
        }
        else
            return 1;
    }
    FM25CL64_SPI_CS_DISABLE();
        return 0;
}

uint8_t FM25CL64_Read(uint16_t Addr,uint8_t* pBuffer, uint16_t NumByte)
{
    FM25CL64_WriteEnable();
    FM25CL64_SPI_CS_ENABLE();
    FM25CL64_RW_Byte(FM25CL64_READ);
    FM25CL64_RW_Byte(Addr>>8);
    FM25CL64_RW_Byte(Addr&0xff);
    if(pBuffer==NULL)
    {
       return FM25CL64_RW_Byte(0);
    }
    else 
    {
        if((Addr+NumByte)<=0x1fff)
        {
            while(NumByte--)
            {
               * pBuffer=FM25CL64_RW_Byte(*pBuffer);
               pBuffer++;
            }
        }
        else
            return 1;
    }
    FM25CL64_SPI_CS_DISABLE();
        return 0;
}



void FLASH_MSG(void)
{ 
    uint8_t str[200]={0};
    if(measure.Setflag)
    {
    memcpy(str,&tongdao,sizeof(tongdao));
    FM25CL64_Write(0x1600,str, sizeof(tongdao));
    memcpy(str,&buchang,sizeof(buchang));
    FM25CL64_Write(0x1500,str, sizeof(buchang));
    memcpy(str,&shuchu,sizeof(shuchu));
    FM25CL64_Write(0x1400,str, sizeof(shuchu));
    memcpy(str,&baojing,sizeof(baojing));
    FM25CL64_Write(0x1300,str, sizeof(baojing));
    memcpy(str,&jiaodui,sizeof(jiaodui));
    FM25CL64_Write(0x1200,str, sizeof(jiaodui));
    memcpy(str,&system,sizeof(system));
    FM25CL64_Write(0x1100,str, sizeof(system));
        measure.Setflag=0;
    }
    memcpy(str,&measure.EF,sizeof(measure.EF));
    FM25CL64_Write(0x1000,str, sizeof(measure.EF));
}
    

void FLASH_READ(void)
{
    uint8_t str[200]={0};
    FM25CL64_Read(0x1600,str,sizeof(tongdao));
    memcpy(&tongdao,str,sizeof(tongdao));
    FM25CL64_Read(0x1500,str,sizeof(buchang));
    memcpy(&buchang,str,sizeof(buchang));
    FM25CL64_Read(0x1400,str,sizeof(shuchu));
    memcpy(&shuchu,str,sizeof(shuchu));
    FM25CL64_Read(0x1300,str,sizeof(baojing));
    memcpy(&baojing,str,sizeof(baojing));
    FM25CL64_Read(0x1200,str,sizeof(jiaodui));
    memcpy(&jiaodui,str,sizeof(jiaodui));
    FM25CL64_Read(0x1100,str,sizeof(system));
    memcpy(&system,str,sizeof(system));
    
    FM25CL64_Read(0x1000,str,sizeof(measure.EF));
    memcpy(&measure.EF,str,sizeof(measure.EF));
}

void FLASH_Init(uint8_t byte)
{
    uint8_t str[200]={0};
    
    if(FM25CL64_ReadByte(0x1700)!=byte||FM25CL64_ReadByte(0x1700)==0xff||FM25CL64_ReadByte(0x1701)==1)
    {
        FM25CL64_Write(0x1600,str, sizeof(tongdao));
        FM25CL64_Write(0x1500,str, sizeof(buchang));
        FM25CL64_Write(0x1400,str, sizeof(shuchu));
        FM25CL64_Write(0x1300,str, sizeof(baojing));
        FM25CL64_Write(0x1200,str, sizeof(jiaodui));
        FM25CL64_Write(0x1100,str, sizeof(system));
        FM25CL64_Write(0x1000,str, 0x100);
        FM25CL64_Write(0x500,str, sizeof(PVD_stack));//PVD
        Date_Init();
        memcpy(str,&tongdao,sizeof(tongdao));
        FM25CL64_Write(0x1600,str, sizeof(tongdao));
        memcpy(str,&buchang,sizeof(buchang));
        FM25CL64_Write(0x1500,str, sizeof(buchang));
        memcpy(str,&shuchu,sizeof(shuchu));
        FM25CL64_Write(0x1400,str, sizeof(shuchu));
        memcpy(str,&baojing,sizeof(baojing));
        FM25CL64_Write(0x1300,str, sizeof(baojing));
        memcpy(str,&jiaodui,sizeof(jiaodui));
        FM25CL64_Write(0x1200,str, sizeof(jiaodui));
        memcpy(str,&system,sizeof(system));
        FM25CL64_Write(0x1100,str, sizeof(system));
        FM25CL64_WriteByte(0x1700,byte);
        FM25CL64_WriteByte(0x1701,0);   
    } 
    FLASH_READ();    
}

