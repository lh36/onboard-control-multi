#include "usart.h"
extern vu16 USART2_RX_STA;
/*******************************************************************************
* 函 数 名         : usart_init
* 函数功能		   : 串口初始化函数（串口1上位机通信，串口2GPS通信，串口3惯导通信）
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void usart_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO
	NVIC_InitTypeDef NVIC_InitStructure;	//中断优先级结构体变量
	USART_InitTypeDef USART_InitStructure;	//串口结构体变量	
	
	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //打开PA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //打开PB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);		//打开管脚复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//打开串口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//打开串口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//打开串口时钟
	
	/*  配置GPIO的模式和IO口 */
	/*  对串口1、串口2、串口3的管脚分别进行初始化 */
	
	/*串口1管脚*/
	//先设置结构体参数
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9; //TX  (PA9)
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出

	//PA9管脚初始化
	GPIO_Init(GPIOA,&GPIO_InitStructure);	  //	GPIOCA(PA9)

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ; //TX	(PA10)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入  PA10管脚初始化 GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOA,&GPIO_InitStructure);	 //	GPIOCA(PA10)

	/*串口2管脚*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);	  //	GPIOCA(PA9)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入  PA10管脚初始化 GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOA,&GPIO_InitStructure);	 //	

	/*串口3管脚*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ; //TX  (PB10)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);	  //	GPIOCA(PB10)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11 ; //RX	(PB11)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入,PB10管脚初始化GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOB,&GPIO_InitStructure);	 //	GPIOCA(PB11)


	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;		 //硬件流失能
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = 115200; //修改串口2和3的波特率
	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);//使能串口1，仅使能了串口，中断还未打开
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //打开串口1接收中断 (选择接收中断)，即收到数据就进入中断
	USART_ClearFlag(USART1,USART_FLAG_TC);//清空串口发送完成标志位。情况串口待处理标志位，防止二异性

	USART_Cmd(USART2, ENABLE);//使能串口2，仅使能了串口，中断还未打开
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //打开串口2接收中断 (选择接收中断)，即收到数据就进入中断
	USART_ClearFlag(USART2,USART_FLAG_TC);//清空串口发送完成标志位。情况串口待处理标志位，防止二异性	

	USART_Cmd(USART3, ENABLE);//使能串口3，仅使能了串口，中断还未打开
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //打开串口3接收中断 (选择接收中断)，即收到数据就进入中断
	USART_ClearFlag(USART3,USART_FLAG_TC);

	
	/* 设置NVIC优先级参数 */	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 	//打开USART1_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为0,为最高级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //使能
	NVIC_Init(&NVIC_InitStructure); 
	 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 	//打开USART2_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级为2,为最高级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级为3 子优先级
	NVIC_Init(&NVIC_InitStructure); 
	 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 	//打开USART3_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级为1,为了避免上位机命令被角度接收中断打断，从而降低角度接收中断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级为0
	NVIC_Init(&NVIC_InitStructure);

	TIM7_Int_Init(99,7199);		//10ms中断
	USART2_RX_STA=0;		//清零
	TIM_Cmd(TIM7,DISABLE);			//关闭定时器7

}
