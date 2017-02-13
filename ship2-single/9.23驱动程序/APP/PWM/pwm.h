#ifndef _pwm_H
#define _pwm_H
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#define LIN1 GPIO_Pin_0
#define LIN2 GPIO_Pin_1
#define RIN1 GPIO_Pin_2
#define RIN2 GPIO_Pin_3

void moter_pwm_init(u16 prd,u16 prc);
int setAngle(int a);


#endif
