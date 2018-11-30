#include "../Own/RS485.h"
#include "Date.h"
uint16_t   usRegInputStart = REG_INPUT_START;
uint16_t   usRegInputBuf[REG_INPUT_NREGS];
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];
IEE754_LH IEE754;
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  printf("eMBRegInputCB\n");
  if((usAddress>=REG_INPUT_START)&&(usAddress+usNRegs<=REG_INPUT_START+REG_INPUT_NREGS))
  {
    iRegIndex=(int)(usAddress-usRegInputStart);
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex]>>8);
      *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex]&0xFF);
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,eMBRegisterMode eMode )
{
    //错误状态
    eMBErrorCode eStatus = MB_ENOERR;
    //偏移量
    int16_t iRegIndex;
    printf("eMBRegHoldingCB\n");
    //判断寄存器是不是在范围内
    if( ( (int16_t)usAddress >= REG_HOLDING_START )&&( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
    //计算偏移量
        iRegIndex = ( int16_t )( usAddress - REG_HOLDING_START);
        switch ( eMode )
        {
        //读处理函数 
        case MB_REG_READ:
            while( usNRegs > 0 )
                {
                    *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
                    *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
                    iRegIndex++;
                    usNRegs--;
                }
            break;
        //写处理函数 
        case MB_REG_WRITE:
            while( usNRegs > 0 )
                {
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
            break;
        }
    }
    else
        eStatus = MB_ENOREG;
    return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,eMBRegisterMode eMode )
{
  printf("eMBRegCoilsCB\n");
  return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  printf("eMBRegDiscreteCB\n");
  return MB_ENOREG;
}

void RS485_MSG(void)
{
    static uint8_t state_485=0;
    uint16_t i=0,temp[2];
    eMBErrorCode    eStatus;
    if(system.OnOff_485==1&&state_485==0)//打开
    {
        state_485=1;
        eStatus = eMBInit( MB_RTU, system.Addr_485, 1, system.Bps_485, MB_PAR_NONE);
        /* Enable the Modbus Protocol Stack. */
        eStatus = eMBEnable(  );
        for(i=0;i<200;i++)
            usRegInputBuf[i]=0;
    }
    else if(system.OnOff_485==0&&state_485==1)//关闭
    {
        state_485=0;
        eStatus=eMBDisable();
    }
    if(system.SChange_485==1)
    {
        eStatus=eMBDisable();
        eStatus = eMBInit( MB_RTU, system.Addr_485, 1, system.Bps_485, MB_PAR_NONE);
        eStatus = eMBEnable(  );
    }
    if(system.OnOff_485==1&&state_485==1)//处理数据
    {
        Decimals_IEE754(measure.CY,&usRegHoldingBuf[1],4);
        Decimals_IEE754(measure.YL,&usRegHoldingBuf[3],4);
        Decimals_IEE754(measure.T,&usRegHoldingBuf[5],4);
        Decimals_IEE754(measure.PT,&usRegHoldingBuf[7],4);
        Decimals_IEE754(measure.F,&usRegHoldingBuf[9],8);
        Decimals_IEE754(measure.EF,&usRegHoldingBuf[14],8);
//        Decimals_IEE754(1000,&usRegInputBuf[1],4);
//        Decimals_IEE754(1000,&usRegInputBuf[3],4);
//        Decimals_IEE754(1000,&usRegInputBuf[5],4);
//        Decimals_IEE754(1000,&usRegInputBuf[7],4);
//        Decimals_IEE754(1000000,&usRegInputBuf[9],8);
//        Decimals_IEE754(1000000,&usRegInputBuf[13],8);
    }
       
}

uint8_t* Decimals_IEE754(double t_Source,uint16_t *Target,uint8_t Mode)//MODE=4 float =8 double
{
    uint8_t i;uint16_t p;
    double H_Temp,L_Temp;
    if(Mode==8)
    {
          IEE754.Temp.Source=(int)(t_Source/100);
          for(i=0;i<4;i++)
             IEE754.Array[i]=IEE754.Temp.Target[3-i];
          IEE754.Temp.Source=(int)t_Source%100;
          for(i=0;i<4;i++)
            IEE754.Array[i+4]=IEE754.Temp.Target[3-i];
          *Target=IEE754.Array[0]<<8|IEE754.Array[1];
          *(Target+1)=IEE754.Array[2]<<8|IEE754.Array[3];
          return &IEE754.Array[0];
    }
    else if(Mode==4)
    {
        IEE754.Temp.Source=t_Source;
        for(i=0;i<4;i++)
             IEE754.Array[i]=IEE754.Temp.Target[3-i];
        *Target=IEE754.Array[0]<<8|IEE754.Array[1];
        return &IEE754.Array[0];
    }
    else
        return 0;
}
double IEE754_Decimals(uint32_t *t_Source,uint8_t Mode)
{
    double temp_H,temp_L;
    if(Mode==8)
    {
        memcpy(IEE754.Temp.Target,t_Source,4);
        temp_H=IEE754.Temp.Source;
        memcpy(IEE754.Temp.Target,t_Source+1,4);
        temp_L=IEE754.Temp.Source;
        IEE754.Decimals=temp_H*100+temp_L;
        return  IEE754.Decimals;
    }
    else if(Mode==4)
    {
        memcpy(IEE754.Temp.Target,t_Source,4);
        IEE754.Decimals=IEE754.Temp.Source;
        return  IEE754.Decimals;
    }
    else
        return 0;
}