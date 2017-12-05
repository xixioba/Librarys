#include "InfraRed.h"

    
void m_delay1us(const uint32_t count)
{
    if(count==1)
        __NOP();
    __HAL_TIM_SET_COUNTER(&htim6,0);      
    while(__HAL_TIM_GET_COUNTER(&htim6)<count);
}


static uint8_t ReadInfraRed(uint8_t *Arr)
{
    uint16_t time=0;
    uint8_t i=0,j=0;

    if(HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_RESET)
    { 
        HAL_Delay(2);
        if(HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_RESET)
        {       
            while((HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_RESET)&&(time++<9000))m_delay1us(1);
            if(time<3000||time>=9000)return InfradRed_ERROR;time=0;            
            while((HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_SET)&&(time++<5000))m_delay1us(1);
            if(time<1000||time>=5000)return InfradRed_ERROR;time=0;
            for(i=0;i<4;i++)
            {
                for(j=0;j<8;j++)
                {
                    time=0;
                    while((HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_RESET)&&(time++<800))m_delay1us(1);
                    if(time>=800)return InfradRed_ERROR;
                    time=0;               
                    while((HAL_GPIO_ReadPin(InfradRed_GPIO_Port, InfradRed_Pin)==GPIO_PIN_SET)&&(time++)<2000)m_delay1us(1);
                    *Arr>>=1;
                    if((time>=800)&(time<2000))
                        *Arr|=0x80;
                    else if(time>=2000)
                        return InfradRed_REPEAT;
//                    printf("%5d",time);
                }
                Arr++;
//                printf("\n");
            }
            
                    return InfradRed_OK;
        }
        else return InfradRed_ERROR;
    }
    else return InfradRed_ERROR;
}


uint16_t ReadIFR(void)
{
    static uint8_t status2;//≈–∂œ «∑Ò”–ƒ⁄»›
    static uint8_t count;//≈–∂œ «∑Ò≥¨ ±£¨≥¨ ±«Âø’
    uint8_t status;
    uint8_t InfraRed_value[4];
    static uint16_t code;
    status=ReadInfraRed(InfraRed_value); 
    if(status==InfradRed_OK)
        {
            status2=1;
            count=0;
            if(((InfraRed_value[0]&0xff)==(~InfraRed_value[1]&0xff))&((InfraRed_value[2]&0xff)==(~InfraRed_value[3]&0xff)))
            code=InfraRed_value[0]<<8|InfraRed_value[2];
//            printf("code=%d",code);
            return code;   
        }   
    else if(status==InfradRed_REPEAT)
        {
//            printf("count=%d\n",count); ºÏ≤‚÷ÿ∏¥√¸¡Óº‰∏Ù
            if(status2==1)
            {
                count=0;                            
                return code;
                
            }
                
        }
    else if(status==InfradRed_ERROR)
        {
            if(++count>100)
            {
                status2=0;
                count=0;
                code=0;   
            }            
            HAL_Delay(1);            
            return 0;
        }
    return 0;
}

