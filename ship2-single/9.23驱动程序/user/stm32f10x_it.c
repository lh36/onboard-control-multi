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

void EXTI2_IRQHandler()	   //外部中断2中断函数
{
	
}

unsigned char RE_Flag;
unsigned char head_flag=0; 
//char flag=0;//接收到一组本机命令标志
//收到一个字节调用一次。把收到的字节做为输入。

unsigned char rx_command_buffer[50]; //接收数据缓冲区
unsigned char rx_wr_command_index; //缓冲写指针
unsigned char Com_data_Recv[10];
unsigned char flag_full=0;
unsigned char flag_a1=0; 
unsigned char flag=0;//接收到一组本机命令标志
//收到一个字节调用一次。把收到的字节做为输入。

void Decode_Command_frame(unsigned char data)
{
  if(data==0xa2) 
  {
  	if(flag_full==0)
	{
		flag_a1=1;
	}
	rx_command_buffer[rx_wr_command_index++]=data; //有完整包头，则说明是中间数据，直接接收 
  }
  else if(data==0x2a)
  {
	if(flag_a1)
	{
		flag_full=1;
		flag_a1=0;//接收到了一组完整包头，则接下来出现的1a都是中间数据直接接收不进入这里
		rx_command_buffer[rx_wr_command_index++]=data; //有完整包头，则说明是中间数据，直接接收
	}
	else 
	{
		if(flag_full)rx_command_buffer[rx_wr_command_index++]=data; //有完整包头，则说明是中间数据，直接接收	
	}
  }
  else
  {
  	if(flag_full)
	{
		rx_command_buffer[rx_wr_command_index++]=data; //有完整包头，则说明是中间数据，直接接收
		if(rx_wr_command_index==rx_command_buffer[2]);//只要能执行到此处则一定已经有了rx_buffer[2]，接收到了所示字节长度的数据
		{
			if(0xaa==rx_command_buffer[rx_wr_command_index-1]) //接收到一组完整的属于本机的命令。a1 1a既是包头，又是属于本机命令的标志
			{
				char i;
				for(i=0;i<rx_command_buffer[2];i++) Com_data_Recv[i]= rx_command_buffer[i]; //拿取命令信息
				flag=1;//接收到了一组命令
				rx_wr_command_index=0;//接收到一组完整的命令，索引清零。
				flag_a1=0;//a1包头清零
				flag_full=0;//完整包头清零
			}

		}
	}
	else
	{
		rx_wr_command_index=0;//如果没有完整包头说明接收还未开始，把索引清零，以重新开始接收
		flag_a1=0;//a1包头重新置零	
	}
  }
}
 
unsigned char recv;
static unsigned char headf=0;//报头标志位
u8 k_cont=0;
u8 kv_cont=0;
void USART1_IRQHandler(void)	//串口1中断函数
{	 		
	USART_ClearFlag(USART1,USART_FLAG_TC);
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
	{
		recv=USART_ReceiveData(USART1);
		Decode_Command_frame(recv);
	}
}

 
u8 USART2_RX_BUF[200];//接收缓存
vu16 USART2_RX_STA=0; //GPS接收数据状态
u8 j=0;

void USART2_IRQHandler(void)	//串口2中断函数
{   
    u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据，还没有处理，则不再接收其他数据
		{ 
			if(USART2_RX_STA<600)	//还可以接收数据
			{
			  
//			   {j=3;}//剩余最后三位数据
//		      j--;
			  USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值
			   if(res=='*')
			  {
			    USART2_RX_STA|=1<<15;
			    return;
			  }	 
			  
			}
			else 
			{
				USART2_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  				 	
}

u8 USART3_RX_BUF[600];//接收缓存
vu16 USART3_RX_STA=0; //IMU接收数据状态

void USART3_IRQHandler(void)		//串口3，与232同路，惯导
{	
    u8 res;//接收数据暂存
    USART_ClearFlag(USART3,USART_FLAG_TC);
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
	{
	   res =USART_ReceiveData(USART3);
	   if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据，还没有处理，则不再接收其他数据
	   { 
			if(USART3_RX_STA<200)	//还可以接收数据
			{

			  USART3_RX_BUF[USART3_RX_STA++]=res;	//记录接收到的值
			  if(res==0XAA)		      
			  {	 			    
			     USART3_RX_STA|=1<<15;
			  	 USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清中断标志.
			     return;
			  }		  	 
			}
			else 
			{
				USART3_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清中断标志.  
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
