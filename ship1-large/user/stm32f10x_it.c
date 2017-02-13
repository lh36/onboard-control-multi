/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "systick.h"
#include "smg.h"
#include "public.h"
#include "gps.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void EXTI2_IRQHandler()	   //�ⲿ�ж�2�жϺ���
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
   		EXTI_ClearITPendingBit(EXTI_Line0);//���EXTI��·����λ
		delay_ms(10);//��������
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==Bit_RESET)	   //k_left��������
		{
			if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_0)==Bit_RESET)
			{
				//LED Ϩ��
			   GPIO_SetBits(GPIOC,GPIO_Pin_0);	
			}
			else
			{
			   //LED ����
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
			}
		} 
		while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0);
	}		
}

unsigned char RC_Flag;
unsigned char rx_buffer[50]; //�������ݻ�����
unsigned char rx_wr_index; //����дָ��
unsigned char Com_data_Recv[10];
unsigned char counter_command=0; //���������ֽڼ�����
unsigned char flag_full=0;
unsigned char flag_a1=0; 
unsigned char flag=0;//���յ�һ�鱾�������־
//�յ�һ���ֽڵ���һ�Ρ����յ����ֽ���Ϊ���롣

void Decode_frame(unsigned char data)
{
  if(data==0xa1) 
  {
  	if(flag_full==0)
	{
		flag_a1=1;
	}
	rx_buffer[rx_wr_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ��� 
  }
  else if(data==0x1a)
  {
	if(flag_a1)
	{
		flag_full=1;
		flag_a1=0;//���յ���һ��������ͷ������������ֵ�1a�����м�����ֱ�ӽ��ղ���������
		rx_buffer[rx_wr_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���
	}
	else 
	{
		if(flag_full)rx_buffer[rx_wr_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���	
	}
  }
  else
  {
  	if(flag_full)
	{
		rx_buffer[rx_wr_index++]=data; //��������ͷ����˵�����м����ݣ�ֱ�ӽ���
		if(rx_wr_index==rx_buffer[2]);//ֻҪ��ִ�е��˴���һ���Ѿ�����rx_buffer[2]�����յ�����ʾ�ֽڳ��ȵ�����
		{
			if(0xaa==rx_buffer[rx_wr_index-1]) //���յ�һ�����������ڱ��������a1 1a���ǰ�ͷ���������ڱ�������ı�־
			{
				char i;
				for(i=0;i<rx_buffer[2]+2;i++) Com_data_Recv[i]= rx_buffer[i]; //��ȡ������Ϣ
				flag=1;//���յ���һ������
				rx_wr_index=0;//���յ�һ������������������㡣
				flag_a1=0;//a1��ͷ����
				flag_full=0;//������ͷ����
			}

		}
	}
	else
	{
		rx_wr_index=0;//���û��������ͷ˵�����ջ�δ��ʼ�����������㣬�����¿�ʼ����
		flag_a1=0;//a1��ͷ��������	
	}
  }
}

unsigned char Get[26];//Э�����ݰ���
char receive[10]= {0,0,0,0,0,0,0,0,0,0};
char count=0;

char new_flag=0;
unsigned char recv;
void USART1_IRQHandler(void)	//����1�жϺ���
{
	USART_ClearFlag(USART1,USART_FLAG_TC);//��մ��ڷ�����ɱ�־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
		recv=USART_ReceiveData(USART1);
		Decode_frame(recv);	//�����֡����
	}
}

vu16 USART2_RX_STA=0;  
u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ��壬���USART2_MAX_RECV_LEN�ֽ�
u8  USART1_TX_BUF[USART2_MAX_RECV_LEN];	        //Ҫ���͸���λ��(����1)��gps�������
nmea_msg gpsx; 	//GPS��Ϣ
union gps_jw_inf	   //����γ��������
{
	u32 gps_f;//�����ͺ����
	unsigned char gps_h[4];
}f2h_jw;
/*����GPS�����жϴ�����*/
//ͨ���ж�2���ַ����ռ���Ƿ񳬹�10ms�������Ƿ�Ϊһ�����������ݣ�
//�����ַ����ռ������10ms����һ����������
//����10msû�н��յ��κ��������ʾ�˴����ݽ������
//[15]0��û�н��յ����ݣ�1�����յ�������
//[14]0�����յ������ݳ���
void USART2_IRQHandler(void)   //����2�жϺ���������gps���ݵĽ���
{
	u8 res;	//1byte ���� 

    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&0x8000)==0)//������һ�����ݣ���û�б��������ٽ�����������
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM7,0);//���������
				if(USART2_RX_STA==0)   //ʹ�ܶ�ʱ��7���ж� 
				{
					TIM_Cmd(TIM7,ENABLE);//ʹ�ܶ�ʱ��7
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ�������ֵ	 
			}
			else 
			{
				USART2_RX_STA|=0x8000;				//ǿ�Ʊ�ǽ������
			} 
		}
	}  				 											 
}
 
char GrevData[19];
unsigned char MTIflag=0;
unsigned char Gcounter=0;

float aaa,ad;
unsigned char out[8],ai,as;
void USART3_IRQHandler(void)	//����3�жϺ���
{
	USART_ClearFlag(USART3,USART_FLAG_TC);//��մ��ڷ�����ɱ�־λ
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
		GrevData[Gcounter]=USART_ReceiveData(USART3);
		if (GrevData[0]!=0XFA) return;
  		Gcounter++;
		if ((Gcounter>2 && GrevData[2]!=0x36) || (Gcounter>4 && GrevData[4]!=0x20) || (Gcounter>5 && GrevData[5]!=0x30))
		{
			Gcounter=0;
			return;
		} 
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
