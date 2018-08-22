#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim1;


#define P_V_I_RATIO 10

typedef struct 
{
    uint8_t flag;
    int8_t dir;
    int P;
    int I;
    int D;
    long int target;
    long int  actual;
    long int Err,Err_last,Err_pre,Err_integral;
    long int mission; 
}PID_Struct;
extern PID_Struct I;
extern PID_Struct V;
extern PID_Struct P;
extern uint16_t time_t;
extern int64_t M_CNT;
void V_PID(void);
void P_PID(void);
void INTERRUPT_PID(void);
void MOVE(uint64_t target);
void PID_INIT(void);
