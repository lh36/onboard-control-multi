/*******************************************************************************
*                 
*                 		       普中科技
--------------------------------------------------------------------------------
* 实 验 名		 : PWM实验
* 实验说明       : 通过定时器3输出一个PWM控制LED小灯，LED小灯呈现呼吸效果
* 连接方式       : 
* 注    意		 : 	所用函数在头文件.c文件内
*******************************************************************************/

#include "stm32f10x_it.h"
#include "public.h"
#include "usart.h"
#include "systick.h"
#include "pwm.h"
#include "switch.h"
#include "control.h"
#include "gps.h"
#include "CRC16.h"
#include <stdio.h>
//#include "DHT11.h"
#include <math.h>
#include <stdlib.h>
/****************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
extern char receive[10];  //外部定义的变量在此处引用，因而要加extern
extern char count;
extern char flag;
extern unsigned char GrevData[60];
extern unsigned char Gcounter;
extern unsigned char MTIflag;
extern unsigned char Get[26];
extern char recv;
extern vu16 USART2_RX_STA;//串口2接收gps数据的计数和标志
extern u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];
extern u8 USART1_TX_BUF[USART2_MAX_RECV_LEN];	//要发送给上位机(串口1)的gps数据组包
extern nmea_msg gpsx; 	//GPS信息
extern unsigned char Com_data_Recv[10];	 //接收到的上位机的属于本机的命令
u32 ti_PC6=150;	 
u32 ti_PC7=150; 	 //=30,1.5ms;=22,1.1ms,=38,1.9ms  周期：16.04ms
int speed=0;
extern union gps_jw_inf	   //经度纬度联合体
{
	float gps_f;//浮点型航向角
	unsigned char gps_h[4];
}f2h_jw;

char val;

int fputc(int ch, FILE *f)
{
   USART_SendData(USART1, (unsigned char) ch);// USART1 可以换成 USART2 等
   while (!(USART1->SR & USART_FLAG_TXE));
   return (ch);
} 	
// 接收数据
int GetKey (void) 
{
   while (!(USART1->SR & USART_FLAG_RXNE));
   return ((int)(USART1->DR & 0x1FF));
} 
//显示GPS信息 
void Gps_Msg_Show(void)
{
 	float tp;		   	 
	tp=gpsx.longitude;	   
	printf("Longitude:%.5f %1c   ",tp/=100000,'E');	// 经度字符串
 	   
	tp=gpsx.latitude;	   
	printf("Latitude:%.5f %1c   ",tp/=100000,'N');	//纬度字符串
 	 			   
	tp=gpsx.speed;	   
 	printf("Speed:%.3fkm/h     ",tp/=1000);		    		//速度字符串	 
	    
	printf("UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//时间字符串	  
}



int main()
{	
	u16 i,rxlen; //gps数据长度
	u8 temp_left,temp_right;//左脉宽//右脉宽

	switch_Init();
	pwm_init();	 //PWM初始化
	usart_init();
	TIM_SetCompare1(TIM3, ti_PC6);//设置TIMx捕获比较2寄存器值，通道1PC6脉宽
	TIM_SetCompare2(TIM3, ti_PC7);//设置TIMx捕获比较2寄存器值，通道2PC7脉宽
	Get[0] = 0xa5;
	Get[1] = 0x5a;
	Get[2] = 26;
	Get[3] = 0x01;
	GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1);
	while(1)
	{
		if(USART2_RX_STA&0X8000)
		{

			rxlen=USART2_RX_STA&0X7FFF;	//得到数据长度
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART2_RX_BUF[i];	   
 			USART2_RX_STA=0;		   	//启动下一次接收
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串，将其存入GPS信息结构体
			//提取纬度信息
		//	f2h_jw.gps_f = (float)gpsx.latitude/100000.0f;   
			
			Get[4] = gpsx.latitude>>24;						   //协议组装
			Get[5] = (gpsx.latitude>>16)&0xff;				   //移位优先级要低于求与运算，故一定要加运算符
			Get[6] = (gpsx.latitude>>8)&0xff;
			Get[7] = gpsx.latitude&0xff;
			//提取经度信息
		//	f2h_jw.gps_f = (float)gpsx.longitude/100000.0f;   
			Get[8] = gpsx.longitude>>24;						   //协议组装
			Get[9] = (gpsx.longitude>>16)&0xff;
			Get[10] = (gpsx.longitude>>8)&0xff;
			Get[11] = gpsx.longitude&0xff;
			//提取时间信息（格林尼治时间）
			Get[12] = gpsx.utc.hour;					   //提取时间信息（格林尼治时间）
			Get[13] = gpsx.utc.min;
			Get[14] = gpsx.utc.sec;
			//提取速度信息;
			Get[15] = gpsx.speed>>8; 
			Get[16] = gpsx.speed&0xff;
		}
		if (Gcounter>19)
		{
			Get[17]=GrevData[18];
			Get[18]=GrevData[17];
			Get[19]=GrevData[16];
			Get[20]=GrevData[15];
			MTIflag=1;
			Gcounter=0;
		}
		if(flag==1)	//flag 为接收到上位机命令的标志
		{
			flag=0;
			val=Com_data_Recv[3];
			if(val=='N')	 //低速前进
			{
				speed=10;
				setCarAdvance(speed);
			}
			else if(val=='M')	//高速前进
			{
				speed=30;
				setCarAdvance(speed);
			}
			else if(val=='A')  //以固定速度前进
			{
				setCarAdvance(speed);
			}
			else if(val=='B')  //倒车
			{
				setCarBackoff(speed);
			}
			else if(val=='S') //停车
			{
				setCarStop();
				speed=0;
			}
			else if(val=='G') //获取姿态信息
			{
				int grade;
				Get[21] = 0;	//舵角信息
				//速度等级信息提取
				temp_left = abs(150 - TIM_GetCapture1(TIM3));//左推进等级
				temp_right= abs(150 - TIM_GetCapture2(TIM3));//右推进等级
				grade =	(temp_left+temp_right)/2;
				Get[22] =  grade&0xff;
			
				//CRC校验
				Get[23] = CRC16(Get, 23)/256;
				Get[24] = CRC16(Get, 23)%256;
				Get[25]	= 0xaa;
				 for (i=0;i<sizeof(Get)/sizeof(char);i++)
			 	{							   
			 		USART_SendData(USART1,Get[i]);//通过外设USARTx发送单个数据
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);
			 	}
				
			}
			else if(val=='Y')//切换成高电平，遥控
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1);	 //IO口输出高电平
			}
			else if(val=='Z')//切换成低电平，自主控制
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1); //IO口输出低电平
			}
			else if(val=='Q')//左加速
			{
				u16 temp=TIM_GetCapture1(TIM3);
				temp--;
				TIM_SetCompare1(TIM3, temp);	
			}
			else if(val=='L')//左减速
			{
				u16 temp=TIM_GetCapture1(TIM3);
				temp++;
				TIM_SetCompare1(TIM3, temp);	
			}
			else if(val=='R') //右加速
			{
				u16 temp=TIM_GetCapture2(TIM3);
				temp--;
				TIM_SetCompare2(TIM3, temp);
			}
			else if(val=='V') //右减速
			{
				u16 temp=TIM_GetCapture2(TIM3);
				temp++;
				TIM_SetCompare2(TIM3, temp);
			}
			else if(val=='I')
			{
				speed++;
				if(speed<=13)
				{
					speed=13;
				}
				if(speed>40)
				{
					speed=40;
				}
				setCarAdvance(speed);
			}
			else if(val=='D')
			{
				speed--;
				if((0<speed)&&(speed<13))
				{
					speed=0;
				}
				setCarAdvance(speed);
			}
			else
			{
				int error;
				if (val <= 64)
				error = val - 32;
				if(error!=0)	//正左负右
				{
					int r=(int)(180/(abs(error)*1.5*3.1415926)); //设置每秒转x°，则由误差得出
					if (error>0)
					{
						setCarUpperLeft(134,r); //制导角-实际角，error为正，需要左转。
						
					}
					else 
					{
						setCarUpperRight(134,r);	//error为负，需要右转。
					}
				}
				else
				{
					setCarAdvance(18);
				}
			}
		}//if(flag==1) 接到上位机命令
	}//while(1)
}



