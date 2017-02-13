#include "stm32f10x_it.h"
#include "systick.h"
#include "pwm.h"
#include "stm32f10x_tim.h"

extern u16 speed1;
extern int angle;
extern u8  K_flag;
extern char  k;
extern char  kv;


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void EXTI2_IRQHandler()	   //�ⲿ�ж�2�жϺ���
{
	
}

unsigned char RE_Flag;
unsigned char head_flag=0; 
//char flag=0;//���յ�һ�鱾�������־
//�յ�һ���ֽڵ���һ�Ρ����յ����ֽ���Ϊ���롣

unsigned char rx_command_buffer[50]; //�������ݻ�����
unsigned char rx_wr_command_index; //����дָ��
unsigned char Com_data_Recv[10];
unsigned char flag_full=0;
unsigned char flag_a1=0; 
unsigned char flag=0;//���յ�һ�鱾�������־
//�յ�һ���ֽڵ���һ�Ρ����յ����ֽ���Ϊ���롣

void Decode_Command_frame(unsigned char data)
{
  if(data==0xa2) 
  {
  	if(flag_full==0)
	{
		flag_a1=1;
	}
	rx_command_buffer[rx_wr_command_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ��� 
  }
  else if(data==0x2a)
  {
	if(flag_a1)
	{
		flag_full=1;
		flag_a1=0;//���յ���һ��������ͷ������������ֵ�1a�����м�����ֱ�ӽ��ղ���������
		rx_command_buffer[rx_wr_command_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���
	}
	else 
	{
		if(flag_full)rx_command_buffer[rx_wr_command_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���	
	}
  }
  else
  {
  	if(flag_full)
	{
		rx_command_buffer[rx_wr_command_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���
		if(rx_wr_command_index==rx_command_buffer[2]);//ֻҪ��ִ�е��˴���һ���Ѿ�����rx_buffer[2]�����յ�����ʾ�ֽڳ��ȵ�����
		{
			if(0xaa==rx_command_buffer[rx_wr_command_index-1]) //���յ�һ�����������ڱ��������a1 1a���ǰ�ͷ���������ڱ�������ı�־
			{
				char i;
				for(i=0;i<rx_command_buffer[2];i++) Com_data_Recv[i]= rx_command_buffer[i]; //��ȡ������Ϣ
				flag=1;//���յ���һ������
				rx_wr_command_index=0;//���յ�һ������������������㡣
				flag_a1=0;//a1��ͷ����
				flag_full=0;//������ͷ����
			}

		}
	}
	else
	{
		rx_wr_command_index=0;//���û��������ͷ˵�����ջ�δ��ʼ�����������㣬�����¿�ʼ����
		flag_a1=0;//a1��ͷ��������	
	}
  }
}
 
unsigned char recv;
static unsigned char headf=0;//��ͷ��־λ
u8 k_cont=0;
u8 kv_cont=0;
void USART1_IRQHandler(void)	//����1�жϺ���
{	 		
	USART_ClearFlag(USART1,USART_FLAG_TC);
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
		recv=USART_ReceiveData(USART1);
		Decode_Command_frame(recv);
	}
}

 
u8 USART2_RX_BUF[200];//���ջ���
vu16 USART2_RX_STA=0; //GPS��������״̬
u8 j=0;

void USART2_IRQHandler(void)	//����2�жϺ���
{   
    u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&(1<<15))==0)//�������һ�����ݣ���û�д������ٽ�����������
		{ 
			if(USART2_RX_STA<600)	//�����Խ�������
			{
			  
//			   {j=3;}//ʣ�������λ����
//		      j--;
			  USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ
			   if(res=='*')
			  {
			    USART2_RX_STA|=1<<15;
			    return;
			  }	 
			  
			}
			else 
			{
				USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
	}  				 	
}

u8 USART3_RX_BUF[600];//���ջ���
vu16 USART3_RX_STA=0; //IMU��������״̬

void USART3_IRQHandler(void)		//����3����232ͬ·���ߵ�
{	
    u8 res;//���������ݴ�
    USART_ClearFlag(USART3,USART_FLAG_TC);
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
	   res =USART_ReceiveData(USART3);
	   if((USART3_RX_STA&(1<<15))==0)//�������һ�����ݣ���û�д������ٽ�����������
	   { 
			if(USART3_RX_STA<200)	//�����Խ�������
			{

			  USART3_RX_BUF[USART3_RX_STA++]=res;	//��¼���յ���ֵ
			  if(res==0XAA)		      
			  {	 			    
			     USART3_RX_STA|=1<<15;
			  	 USART_ClearITPendingBit(USART3, USART_IT_RXNE); //���жϱ�־.
			     return;
			  }		  	 
			}
			else 
			{
				USART3_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); //���жϱ�־.  
	}  
}



void TIM3_IRQHandler()	  //��ʱ��3�жϺ���
{
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
