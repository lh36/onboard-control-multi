#include "pwm.h"

/*******************************************************************************
* �� �� ��         : pwm_init
* ��������		   : IO�˿ڼ�TIM3��ʼ������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void pwm_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;   //����һ���ṹ�������������ʼ��GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//����һ���ṹ�������������ʼ����ʱ��

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_6; //ѡ��PC7��PC6���
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOC,&GPIO_InitStructure);


	//TIM3��ʱ����ʼ��
	//TIM_TimeBaseInitStructure.TIM_Prescaler = 3600-1;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��Ƶ��:72000000/3600=20000Hz,��1/20000=0.00005s
	//TIM_TimeBaseInitStructure.TIM_Period = 399;	   //400*0.00005=0.02s=20ms����ΪPWM����//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;//���÷ֱ��ʣ�HzתΪʱ�䣩������������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��Ƶ��:72000000/720=100000Hz,��1/100000=0.00001s=0.01ms
	TIM_TimeBaseInitStructure.TIM_Period = 2000;	   //1600*0.01ms=16ms����ΪPWM����//�����Զ���װ�ؼĴ������ڵ�ֵ
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//�ı�ָ���ܽŵ�ӳ��	//pC7

	//PWM��ʼ��	  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;	 //����Ϊ�ߵ�ƽ����ģʽ1��Ӧ

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);          //ʹ��ͨ��2
  	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	//ע��˴���ʼ��ʱTIM_OC2Init������TIM_OCInit������������Ϊ�̼���İ汾��һ����
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���

		
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܻ���ʧ��TIMx����
}
