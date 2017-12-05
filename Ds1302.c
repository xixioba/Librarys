#include "../Own/Ds1302.h"
#include "../Own/Rtos.h"

void DS1320_Delay(uint32_t t)
{
    t+=3;
    while(t--);
}

static void DS1302_W_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = DS_IO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS_IO_GPIO_Port, &GPIO_InitStruct);
}


static void DS1302_R_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = DS_IO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS_IO_GPIO_Port, &GPIO_InitStruct);
    W_1;
}

static void DS1302_BYTE_W(uint8_t data)
{
    uint8_t i;
    CLK_0;
    DS1302_W_INIT();
    for(i=0;i<8;i++)
    {
        if(data&0x01)
            W_1;
        else
            W_0;
        CLK_0;        
        CLK_1;        
        data>>=1;
    }   
}

static uint8_t DS1302_BYTE_R(void)
{
    uint8_t i,data=0;
    CLK_1;
    DS1302_R_INIT();
    for(i=0;i<8;i++)
    {
        data>>=1;
        CLK_1;       
        CLK_0; 
        if(HAL_GPIO_ReadPin(DS_IO_GPIO_Port, DS_IO_Pin))
            data|=0x80;              
    }   
    return data;
}

static void DS1302_W(uint8_t addr,uint8_t data)
{
    RST_0;    
    CLK_0;   
    RST_1;
    DS1302_BYTE_W(addr);
    DS1302_BYTE_W(data);
    RST_0;
    CLK_0;
}

uint8_t DS1302_R(uint8_t addr)
{
    uint8_t data;
    RST_0;
    CLK_0;
    RST_1;
    DS1302_BYTE_W(addr);
    data=DS1302_BYTE_R();
    data=(data>>4)*10+(data&0x0f);
    RST_0;
    CLK_0;
    
    return data;
}

void DS1302_INIT(void)//无需使用
{
    DS1302_W(0x8e,0x00); //控制写入WP=0
    DS1302_W(0x90,0xa5); //涓电流
    DS1302_W(0x80,0x00);//晶振启振
    DS1302_W(0x8e,0x80); //控制写入WP=1	
}

void DS1302_SET(void)
{
    DS1302_W(0x8e,0x00);
    DS1302_W(0x80,((time.s/10)<<4|(time.s%10))); 
    DS1302_W(0x82,((time.m/10)<<4|(time.m%10)));
    DS1302_W(0x84,((time.h/10)<<4|(time.h%10)));
    DS1302_W(0x86,((time.D/10)<<4|(time.D%10)));
    DS1302_W(0x88,((time.M/10)<<4|(time.M%10)));
    DS1302_W(0x8A,((time.W/10)<<4|(time.W%10)));
    DS1302_W(0x8C,((time.Y/10)<<4|(time.Y%10)));    
    //DS1302_W(0x80,0x00);//晶振启振
    DS1302_W(0x8e,0x80); //控制写入WP=1	
}

void DS1302_READ(void)
{

    time.s=DS1302_R(0x81);	//秒
    time.m=DS1302_R(0x83);	//分
    time.h=DS1302_R(0x85);	 	//时
    time.D=DS1302_R(0x87);	  	//日
    time.M=DS1302_R(0x89); 	//月 
    time.W=DS1302_R(0x8b);		//星期
    time.Y=DS1302_R(0x8d);      //年  
}

void DS1302_TIME(uint16_t Y,uint16_t M,uint16_t D,uint16_t h,uint16_t m,uint16_t s)
{
    time.s=s;	//秒
    time.m=m;	//分
    time.h=h;	 	//时
    time.D=D;	  	//日
    time.M=M; 	//月 
    //time.W=W;		//星期
    time.Y=Y;      //年 
    DS1302_SET();
}
void DS1302_MSG(void)
{ 
    if(time.flags==1)
    {
        DS1302_SET();        
    }
    else if(time.flags==0)
    {
        DS1302_READ();
    }
    //printf("%d %d %d %d %d %d %d \n",time.flags,time.Y,time.M,time.D,time.h,time.m,time.s);
}
