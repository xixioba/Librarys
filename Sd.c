#include "../Own/af_sd.h"

FRESULT SD_Test()
{
    if(FATFS_GetAttachedDriversNbr()!=0)
    {
        f_mount (&fs, NULL , 1);
        if(f_mount (&fs, "0:", 1)!=FR_OK)
        {
            DEBUG("FATFS MOUNT WRONG\n");
            return FR_DISK_ERR;
        }
            return FR_OK;
    }
    else
        DEBUG("NO ATTACHERDRIVERS\n");            
    return FR_DISK_ERR;
}





/**
  * ��������: ��SD����ȡGBK��
  * �������: pBuffer�����ݱ����ַ
  *           gbk �������ַ����ֽ���
  *           font������ѡ��
  *           ��ѡֵ��USE_FONT_16 ��16������
  *                   USE_FONT_24 ��24������ 
  * �� �� ֵ: uint8_t: 0:��ȡʧ�ܣ�1����ȡ�ɹ�
  * ˵    ����ʹ�øú���֮ǰȷ����������f_mount(&fs,"0:",1);����
  */
uint8_t GetGBKCode_SD(uint8_t * pBuffer,const uint16_t Char)
{
  uint32_t pos;
  uint8_t high8bit,low8bit;
  static uint8_t everRead = 0;
  high8bit=Char>> 8;
  low8bit=Char& 0x00FF;
    if(everRead==0)
    {
        if(FATFS_GetAttachedDriversNbr()!=0)
        {
            if(f_mount (&fs, "0:", 1)!=FR_OK)
            {
                LCD_Show(160,20,"FAILED MOUNT\n");
                DEBUG("FATFS MOUNT WRONG");
                while(1);
            }
            else
            {
                everRead=1;
            }
            
        }
        else
            LCD_Show(160,20,"NO ATTACHERDRIVERS\n");
    }

    // 16*16��С�ĺ��� ����ģ ռ��16*16/8���ֽ�
    pos=((high8bit-0xa1)*94+low8bit-0xa1)*16*16/8;
    fres=f_open(&file,"0:/GBK_1616.FON",FA_OPEN_EXISTING|FA_READ);
  if(fres == FR_OK ) 
  {
    f_lseek (&file, pos);		//ָ��ƫ��
    fres=f_read(&file,pBuffer,32,&fnum); 
    f_close(&file);      
    return 1;  
  }    
  else
    return 0;    
}
