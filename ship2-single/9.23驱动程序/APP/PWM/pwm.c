#include "pwm.h"

/*******************************************************************************
* 函 数 名         : pwm_init
* 函数功能		   : IO端口及TIM3初始化函数	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
extern u16 speed1;
extern u16 speed2;
extern int e2;
extern int angle;

void moter_pwm_init(u16 prd,u16 prc)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//声明一个结构体变量，用来初始化定时器

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	/* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //电机pwm信号
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//舵机pwm信号
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);  //使能GPIO外设时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6; //TIM_CH1|TIM_CH2,PA0,PA1  /舵角PWM信号PA6
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period =prd; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Prescaler =prc-1; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = speed1*10; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx的通道1
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲咳调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使�//	TIM_OCInitStructure.TIM_Pulse = speed; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_Pulse = speed2*10; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx的通道2

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = angle+75; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx通道1，舵机

  //TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能,高级定时器	

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH2预装载使能
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM2,ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3,ENABLE ); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}
 int setAngle(int a)
 {
   
   if(a>16){a=16;}
   else if(a<-16){a=-16;}
//   else if(a<3&&a>-3){a=0;}
   return a;
 }
