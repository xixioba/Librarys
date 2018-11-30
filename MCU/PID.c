#include "../Own/PID.h"
#include <math.h>

PID_Struct V;
PID_Struct P;
PID_Struct I;
int64_t M_CNT=0;
uint16_t time_t=0;

void I_PID()
{

    I.actual=__HAL_TIM_GET_COMPARE(&htim3,TIM_CHANNEL_1);
    I.Err=I.target-I.actual;
    I.mission=I.P*(I.Err-I.Err_last)+I.I*I.Err+I.D*(I.Err-2*I.Err_last+I.Err_pre)+I.actual;
    I.Err_pre=I.Err_last;
    I.Err_last=I.Err;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (int)I.mission);
}
void V_PID()
{
    V.Err=V.target-V.actual;
    V.mission=V.P*(V.Err-V.Err_last)+V.I*V.Err+V.D*(V.Err-2*V.Err_last+V.Err_pre);
    V.Err_pre=V.Err_last;
    V.Err_last=V.Err;
    if( V.mission>0)
        {
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, V.mission>1000?1000:abs((int)V.mission)+abs((int)V.actual));
        }
    else if( V.mission<0)
    {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, V.mission<-1000?1000:abs((int)V.mission)+abs((int)V.actual));
    }
    else 
    {
        if(!(P.Err-P.mission))
        {
        P.flag=0;
        }
    }
}

void P_PID()
{
    P.Err=P.target-P.actual;
    P.mission=P.P*(P.Err-P.Err_last)+P.I*P.Err+P.D*(P.Err-2*P.Err_last+P.Err_pre);
    P.Err_pre=P.Err_last;
    P.Err_last=P.Err;
    V.target=P.mission;
}

void MOVE(uint64_t target)
{
    P.target=target+P.Err;
    P.actual=0;
    P.flag=1;
    P_PID();
//    V_PID();
//    I_PID();    
}


void PID_INIT()
{
    memset(I,0,sizeof(I));
    memset(V,0,sizeof(V));
    memset(P,0,sizeof(P));
//    I.P=1;   
//    I.I=1;
//    I.D=1; 
    V.P=5;
    V.I=1;
    V.D=1;
    P.P=1;   
    P.I=1;
    P.D=0;
    

}

void INTERRUPT_PID()
{
    static uint16_t i;
    V.actual=M_CNT;
    M_CNT=0;    
    if(!__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1))
    {
        P.actual+=V.actual;
        P.dir=1;
    }            
    else
    {
        P.actual-=V.actual;
        P.dir=-1;
    }     
    if(P.flag==1)
        time_t++;        
    if(i++>=P_V_I_RATIO)
    {
        P_PID();
        i=0;
    }
    V_PID(); 
}

