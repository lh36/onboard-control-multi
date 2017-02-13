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

void EXTI2_IRQHandler()	   //外部中断2中断函数
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
   		EXTI_ClearITPendingBit(EXTI_Line0);//清除EXTI线路挂起位
		delay_ms(10);//消抖处理
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==Bit_RESET)	   //k_left按键按下
		{
			if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_0)==Bit_RESET)
			{
				//LED 熄灭
			   GPIO_SetBits(GPIOC,GPIO_Pin_0);	
			}
			else
			{
			   //LED 发光
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
			}
		} 
		while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0);
	}		
}

unsigned char RC_Flag;
unsigned char rx_buffer[50]; //接收数据缓冲区
unsigned char rx_wr_index; //缓冲写指针
unsigned char Com_data_Recv[10];
unsigned char counter_command=0; //接收命令字节计数器
unsigned char flag_full=0;
unsigned char flag_a1=0; 
unsigned char flag=0;//接收到一组本机命令标志
//收到一个字节调用一次。把收到的字节做为输入。

void Decode_frame(unsigned char data)
{
  if(data==0xa1) 
  {
  	if(flag_full==0)
	{
		flag_a1=1;
	}
	rx_buffer[rx_wr_index++]=data; //有完整包头，则说明是中间数据，直接接收 
  }
  else if(data==0x1a)
  {
	if(flag_a1)
	{
		flag_full=1;
		flag_a1=0;//接收到了一组完整包头，则接下来出现的1a都是中间数据直接接收不进入这里
		rx_buffer[rx_wr_index++]=data; //有完整包头，则说明是中间数据，直接接收
	}
	else 
	{
		if(flag_full)rx_buffer[rx_wr_index++]=data; //有完整包头，则说明是中间数据，直接接收	
	}
  }
  else
  {
  	if(flag_full)
	{
		rx_buffer[rx_wr_index++]=data; //有完整包头，则说明是中间数据，直接接收
		if(rx_wr_index==rx_buffer[2]);//只要能执行到此处则一定已经有了rx_buffer[2]，接收到了所示字节长度的数据
		{
			if(0xaa==rx_buffer[rx_wr_index-1]) //接收到一组完整的属于本机的命令。a1 1a既是包头，又是属于本机命令的标志
			{
				char i;
				for(i=0;i<rx_buffer[2]+2;i++) Com_data_Recv[i]= rx_buffer[i]; //拿取命令信息
				flag=1;//接收到了一组命令
				rx_wr_index=0;//接收到一组完整的命令，索引清零。
				flag_a1=0;//a1包头清零
				flag_full=0;//完整包头清零
			}

		}
	}
	else
	{
		rx_wr_index=0;//如果没有完整包头说明接收还未开始，把索引清零，以重新开始接收
		flag_a1=0;//a1包头重新置零	
	}
  }
}

unsigned char Get[26];//协议数据包组
char receive[10]= {0,0,0,0,0,0,0,0,0,0};
char count=0;

char new_flag=0;
unsigned char recv;
void USART1_IRQHandler(void)	//串口1中断函数
{
	USART_ClearFlag(USART1,USART_FLAG_TC);//清空串口发送完成标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
	{
		recv=USART_ReceiveData(USART1);
		Decode_frame(recv);	//进入解帧程序
	}
}

vu16 USART2_RX_STA=0;  
u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲，最大USART2_MAX_RECV_LEN字节
u8  USART1_TX_BUF[USART2_MAX_RECV_LEN];	        //要发送给上位机(串口1)的gps数据组包
nmea_msg gpsx; 	//GPS信息
union gps_jw_inf	   //经度纬度联合体
{
	u32 gps_f;//浮点型航向角
	unsigned char gps_h[4];
}f2h_jw;
/*接收GPS数据中断处理函数*/
//通过判断2个字符接收间隔是否超过10ms来决定是否为一次连续的数据，
//两个字符接收间隔超过10ms则不是一次连续数据
//超过10ms没有接收到任何数据则表示此次数据接收完毕
//[15]0：没有接收到数据；1：接收到了数据
//[14]0：接收到的数据长度
void USART2_IRQHandler(void)   //串口2中断函数，处理gps数据的接收
{
	u8 res;	//1byte 数据 

    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&0x8000)==0)//接收完一批数据，还没有被处理，则不再接收其他数据
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM7,0);//计数器清空
				if(USART2_RX_STA==0)   //使能定时器7的中断 
				{
					TIM_Cmd(TIM7,ENABLE);//使能定时器7
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的数据值	 
			}
			else 
			{
				USART2_RX_STA|=0x8000;				//强制标记接收完成
			} 
		}
	}  				 											 
}
 
char GrevData[19];
unsigned char MTIflag=0;
unsigned char Gcounter=0;

float aaa,ad;
unsigned char out[8],ai,as;
void USART3_IRQHandler(void)	//串口3中断函数
{
	USART_ClearFlag(USART3,USART_FLAG_TC);//清空串口发送完成标志位
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
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
void TIM3_IRQHandler()	  //定时器3中断函数
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
