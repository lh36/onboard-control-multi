#include "pwm.h"

/*******************************************************************************
* º¯ Êı Ãû         : pwm_init
* º¯Êı¹¦ÄÜ		   : IO¶Ë¿Ú¼°TIM3³õÊ¼»¯º¯Êı	   
* Êä    Èë         : ÎŞ
* Êä    ³ö         : ÎŞ
*******************************************************************************/
extern u16 speed1;
extern u16 speed2;
extern int e2;
extern int angle;

void moter_pwm_init(u16 prd,u16 prc)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //ÉùÃ÷Ò»¸ö½á¹¹Ìå±äÁ¿£¬ÓÃÀ´³õÊ¼»¯GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//ÉùÃ÷Ò»¸ö½á¹¹Ìå±äÁ¿£¬ÓÃÀ´³õÊ¼»¯¶¨Ê±Æ÷

	TIM_OCInitTypeDef TIM_OCInitStructure;//¸ù¾İTIM_OCInitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèTIMx

	/* ¿ªÆôÊ±ÖÓ */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //µç»úpwmĞÅºÅ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//¶æ»úpwmĞÅºÅ
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6; //TIM_CH1|TIM_CH2,PA0,PA1  /¶æ½ÇPWMĞÅºÅPA6
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;;  //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period =prd; //ÉèÖÃÔÚÏÂÒ»¸ö¸üĞÂÊÂ¼ş×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ	 
	TIM_TimeBaseStructure.TIM_Prescaler =prc-1; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ıÊıµÄÔ¤·ÖÆµÖµ  ²»·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊıÄ£Ê½
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //¸ù¾İTIM_TimeBaseInitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊıµ¥Î»
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = speed1*10; //ÉèÖÃ´ı×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ĞÔ:TIMÊä³ö±È½Ï¼«ĞÔ¸ß
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //¸ù¾İTIM_OCInitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèTIMxµÄÍ¨µÀ1
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿Èµ÷ÖÆÄ£Ê½1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹Ä//	TIM_OCInitStructure.TIM_Pulse = speed; //ÉèÖÃ´ı×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_Pulse = speed2*10; //ÉèÖÃ´ı×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ĞÔ:TIMÊä³ö±È½Ï¼«ĞÔ¸ß
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //¸ù¾İTIM_OCInitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèTIMxµÄÍ¨µÀ2

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = angle+75; //ÉèÖÃ´ı×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ĞÔ:TIMÊä³ö±È½Ï¼«ĞÔ¸ß
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //¸ù¾İTIM_OCInitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèTIMxÍ¨µÀ1£¬¶æ»ú

  //TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE Ö÷Êä³öÊ¹ÄÜ,¸ß¼¶¶¨Ê±Æ÷	

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1Ô¤×°ÔØÊ¹ÄÜ	 
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH2Ô¤×°ÔØÊ¹ÄÜ
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1Ô¤×°ÔØÊ¹ÄÜ	 
	
	TIM_ARRPreloadConfig(TIM2,ENABLE); //Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ÷
	TIM_ARRPreloadConfig(TIM3,ENABLE ); //Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ÷
	
	TIM_Cmd(TIM2, ENABLE);  //Ê¹ÄÜTIM2
	TIM_Cmd(TIM3, ENABLE);  //Ê¹ÄÜTIM3
}
 int setAngle(int a)
 {
   
   if(a>16){a=16;}
   else if(a<-16){a=-16;}
//   else if(a<3&&a>-3){a=0;}
   return a;
 }
