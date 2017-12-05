#include "12864TXA.h"
GPIO_InitTypeDef GPIO_InitStruct;
uint8_t LCD_MEM[8][128]={0xff};
uint16_t LCD_MEM_State=0x0000;
void LcdDelay(void)	
{
    uint8_t i=5;
    while(i--);
}

void LCD_W_INIT()
{
  GPIO_InitStruct.Pin = LCD_7_Pin|LCD_6_Pin|LCD_5_Pin|LCD_4_Pin 
                          |LCD_3_Pin|LCD_2_Pin|LCD_1_Pin|LCD_0_Pin;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_INPUT
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void LCD_R_INIT()
{
  GPIO_InitStruct.Pin = LCD_7_Pin|LCD_6_Pin|LCD_5_Pin|LCD_4_Pin 
                          |LCD_3_Pin|LCD_2_Pin|LCD_1_Pin|LCD_0_Pin;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


static uint8_t DATA_REVERSE(uint8_t buff)
{
//	uint8_t i;
//	uint8_t buff2=0;
//	for(i=0;i<8;i++){
//		buff2=buff2<<1;
//		if((buff&0x01)==0x01)
//		{
//			buff2++;	
//		}
//		buff=buff>>1;
//	}
//	return buff2;
    return buff;
}

static uint8_t  LCD_Read(void)
{
    uint16_t pins;
    uint8_t pinh;
    pins=GPIOD->IDR;
    pins=pins>>8;
    pinh=pins&0xff;
    pinh=DATA_REVERSE(pinh);
    return pinh;
}


static void LCD_CS(uint8_t screE)		 //选择屏幕
{
   switch(screE)				//转换
   {
      case 0:CS1_1;CS2_1;break;//全屏
      case 1:CS1_1;CS2_0;break;//左屏
      case 2:CS1_0;CS2_1;break;//右屏
   }
}

static void Busy(void)
{
    LCD_R_INIT();
    E_0;
    RS_0;
    RW_1;
    E_1;  
	while(LCD_Busy());
    E_0;
    LCD_W_INIT();
}

static void LCD_CMD(uint8_t command) 
{
#if BUSY_CHECK==1 
    Busy();
#endif
    E_0;
	RS_0;
    RW_0;
    LCD_Write(DATA_REVERSE(command));
    E_1;   
	E_0;
}

static void LCD_W_DATA(uint8_t DATA) 
{
#if BUSY_CHECK==1 
    Busy();
#endif
    E_0;
	RS_1;
    RW_0;         
    LCD_Write(DATA_REVERSE(DATA));
    E_1;    
	E_0;
}

static uint8_t LCD_R_DATA(void) 
{
    uint8_t temp;
#if BUSY_CHECK==1 
    Busy();
#endif
    E_0;   
    RS_1;
	RW_1;
    LCD_R_INIT();
    E_1;
	temp=LCD_Read();
	E_0;
    LCD_W_INIT();
    return temp;
}

void LCD_SET_PAGE(uint8_t page)
{
    page=page|0xb8;
    LCD_CMD(page);
}

void LCD_SET_COLUMN(uint8_t column)
{
    column=column|0x40;
    LCD_CMD(column);
}

void LCD_Switch(uint8_t state)
{
    state=state|0x3e;
    LCD_CMD(state);
}

void LCD_SET_COM(uint8_t line)
{
    line=line|0xc0;
    LCD_CMD(line);
}

void LCD_CLEAR()
{
    uint8_t i,j;
    LCD_CS(0);
    for(i=0;i<8;i++)
    {
        LCD_SET_PAGE(i);
        LCD_SET_COLUMN(0);
        for(j=0;j<64;j++)
        {
            LCD_W_DATA(0x00);
        }
    }
}

void LCD_Init(void)
{
    RESET_1;
    LCD_Switch(0);
//    LCD_SET_COM(0);
    LCD_CLEAR();
//    LCD_SET_COM(0);    
    LCD_Switch(1);
}


static uint8_t LCD_SET_XY(int x, int y)
{
    uint8_t offset,column,page;
    if(x>63)
    {
        LCD_CS(2);
        column=x-64;
    }
    else
    {
        LCD_CS(1);
        column=x;
    }
    page=y/8;
    offset=y%8;
    LCD_SET_PAGE(page);
    LCD_SET_COLUMN(column);    
    return offset;
}
    
uint8_t LCD_GET_Pixels(int x, int y)
{
    LCD_SET_XY(x,y);    
    LCD_R_DATA();
    return LCD_R_DATA();
}
uint8_t LCD_GET_Pixel(int x, int y)
{
    uint8_t bits;
    bits=LCD_GET_Pixels(x,y);
    return (bits>>(y%8))&0x01;
}

void LCD_SET_Pixel(int x, int y, int PixelIndex)
{
    uint8_t offset,bits;  
    bits=LCD_GET_Pixels(x,y);
    offset=LCD_SET_XY(x,y);
    if(PixelIndex==((bits>>offset)&0x01))
       ;
    else
    {
        if(PixelIndex)
            bits|=(0x01<<offset);
        else
            bits&=~(1<<offset);  
        LCD_W_DATA(bits);				
    }
}

//下面是缓存方案


uint8_t LCD_GET_Pixels_M(int x, int y)
{
	return LCD_MEM[y/8][x];
}

uint8_t LCD_GET_Pixel_M(int x, int y)
{
	return (LCD_GET_Pixels_M(x,y)>>(y%8))&0x01;
}

void LCD_SET_Pixel_M(int x, int y, int PixelIndex)
{
	uint8_t bits,offset;
	bits=LCD_GET_Pixels_M(x,y);
	offset=y%8;
	if(PixelIndex!=((bits>>offset)&0x01))
	{
		if(PixelIndex)
            bits|=(0x01<<offset);
        else
            bits&=~(1<<offset);
        LCD_MEM[y/8][x]=bits; 
		if (x<64)
			LCD_MEM_State|=(0x01<<(y/8));
		else
			LCD_MEM_State|=(0x01<<(y/8+8));
	}   
}

void LCD_Refresh_M(void)
{
	uint8_t i,j;
    if(REFRESH_MODE)
        LCD_MEM_State=0xffff;//ffff全部读写
	if (LCD_MEM_State)
	{
		for ( i = 0; i < 16; ++i)
		{
			if((LCD_MEM_State>>i)&0x01)
			{
				LCD_SET_COLUMN(0);				
				if (i>7)
				{
					LCD_CS(2);
                    LCD_SET_PAGE(i-8);
					for (j = 64; j < 128; ++j)
					{
						LCD_W_DATA(LCD_MEM[i-8][j]);
					}					
				}
				else
				{
					LCD_CS(1);
                    LCD_SET_PAGE(i);
					for (j = 0; j < 64; ++j)
					{
						LCD_W_DATA(LCD_MEM[i][j]);
					}
				}
				LCD_MEM_State&=~(0x01<<i);
			}
		}		
	}
}

void LCD_SET_Pixel_D(int x, int y, int PixelIndex)//直接读取内存，写入LCD
{
    uint8_t offset,bits;   
	bits=LCD_GET_Pixels_M(x,y);
	offset=LCD_SET_XY(x,y);
    if(PixelIndex==((bits>>offset)&0x01))
       ;
    else
    {
        if(PixelIndex)
            bits|=(0x01<<offset);
        else
            bits&=~(1<<offset);  
        LCD_W_DATA(bits);				
    }
    LCD_SET_Pixel_M(x,y,PixelIndex);
}
