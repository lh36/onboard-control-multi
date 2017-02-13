#include "usart.h"
extern vu16 USART2_RX_STA;
/*******************************************************************************
* �� �� ��         : usart_init
* ��������		   : ���ڳ�ʼ������������1��λ��ͨ�ţ�����2GPSͨ�ţ�����3�ߵ�ͨ�ţ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void usart_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO
	NVIC_InitTypeDef NVIC_InitStructure;	//�ж����ȼ��ṹ�����
	USART_InitTypeDef USART_InitStructure;	//���ڽṹ�����	
	
	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //��PAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //��PBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);		//�򿪹ܽŸ��ù���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//�򿪴���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//�򿪴���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//�򿪴���ʱ��
	
	/*  ����GPIO��ģʽ��IO�� */
	/*  �Դ���1������2������3�Ĺܽŷֱ���г�ʼ�� */
	
	/*����1�ܽ�*/
	//�����ýṹ�����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9; //TX  (PA9)
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������

	//PA9�ܽų�ʼ��
	GPIO_Init(GPIOA,&GPIO_InitStructure);	  //	GPIOCA(PA9)

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ; //TX	(PA10)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������  PA10�ܽų�ʼ�� GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOA,&GPIO_InitStructure);	 //	GPIOCA(PA10)

	/*����2�ܽ�*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	  //	GPIOCA(PA9)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������  PA10�ܽų�ʼ�� GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOA,&GPIO_InitStructure);	 //	

	/*����3�ܽ�*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ; //TX  (PB10)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);	  //	GPIOCA(PB10)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11 ; //RX	(PB11)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������,PB10�ܽų�ʼ��GPIO_Mode_IN_FLOATING	
	GPIO_Init(GPIOB,&GPIO_InitStructure);	 //	GPIOCA(PB11)


	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;		 //Ӳ����ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = 115200; //�޸Ĵ���2��3�Ĳ�����
	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1����ʹ���˴��ڣ��жϻ�δ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //�򿪴���1�����ж� (ѡ������ж�)�����յ����ݾͽ����ж�
	USART_ClearFlag(USART1,USART_FLAG_TC);//��մ��ڷ�����ɱ�־λ��������ڴ������־λ����ֹ������

	USART_Cmd(USART2, ENABLE);//ʹ�ܴ���2����ʹ���˴��ڣ��жϻ�δ��
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //�򿪴���2�����ж� (ѡ������ж�)�����յ����ݾͽ����ж�
	USART_ClearFlag(USART2,USART_FLAG_TC);//��մ��ڷ�����ɱ�־λ��������ڴ������־λ����ֹ������	

	USART_Cmd(USART3, ENABLE);//ʹ�ܴ���3����ʹ���˴��ڣ��жϻ�δ��
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�򿪴���3�����ж� (ѡ������ж�)�����յ����ݾͽ����ж�
	USART_ClearFlag(USART3,USART_FLAG_TC);

	
	/* ����NVIC���ȼ����� */	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 	//��USART1_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�Ϊ0,Ϊ��߼�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //��Ӧ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //ʹ��
	NVIC_Init(&NVIC_InitStructure); 
	 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 	//��USART2_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�Ϊ2,Ϊ��߼�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //��Ӧ���ȼ�Ϊ3 �����ȼ�
	NVIC_Init(&NVIC_InitStructure); 
	 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 	//��USART3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�Ϊ1,Ϊ�˱�����λ������ǶȽ����жϴ�ϣ��Ӷ����ͽǶȽ����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //��Ӧ���ȼ�Ϊ0
	NVIC_Init(&NVIC_InitStructure);

	TIM7_Int_Init(99,7199);		//10ms�ж�
	USART2_RX_STA=0;		//����
	TIM_Cmd(TIM7,DISABLE);			//�رն�ʱ��7

}
