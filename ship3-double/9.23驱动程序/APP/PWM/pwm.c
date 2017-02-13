#include "pwm.h"

/*******************************************************************************
* �� �� ��         : pwm_init
* ��������		   : IO�˿ڼ�TIM3��ʼ������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
extern u16 speed1;
extern u16 speed2;
extern int e2;
extern int angle;

void moter_pwm_init(u16 prd,u16 prc)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //����һ���ṹ�������������ʼ��GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//����һ���ṹ�������������ʼ����ʱ��

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	/* ����ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //���pwm�ź�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//���pwm�ź�
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6; //TIM_CH1|TIM_CH2,PA0,PA1  /���PWM�ź�PA6
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period =prd; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler =prc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = speed1*10; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx��ͨ��1
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ�//	TIM_OCInitStructure.TIM_Pulse = speed; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_Pulse = speed2*10; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx��ͨ��2

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = angle+75; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMxͨ��1�����

  //TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��,�߼���ʱ��	

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH2Ԥװ��ʹ��
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	
	TIM_ARRPreloadConfig(TIM2,ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3,ENABLE ); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}
 int setAngle(int a)
 {
   
   if(a>16){a=16;}
   else if(a<-16){a=-16;}
//   else if(a<3&&a>-3){a=0;}
   return a;
 }
