/*******************************************************************************
*                 
*                 		       普中科技
--------------------------------------------------------------------------------
* 实 验 名		 : PWM实验
* 实验说明       : 通过定时器3输出一个PWM控制LED小灯，LED小灯呈现呼吸效果
* 连接方式       : 
* 注    意		 : 	所用函数在头文件.c文件内
*******************************************************************************/


#include "public.h"
#include "pwm.h"
#include "systick.h"
#include "usart.h"
#include "stm32f10x_it.h"
#include "stdio.h"
#include "led.h"
#include "IMUGPS.h"
#include <math.h>
#include <stdlib.h>

/****************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
u8 GPS_temp[200];//GPS数据暂存
unsigned char IMU_temp[50];//GPS数据暂存
nmea_msg gpsx;
u16 speed1=0;  //pwm波脉宽，单位ms
u16 speed2=0;
u16 control_speed=10; //跟随速度
unsigned char Get[35];//状态数据帧
int angle=0;	 //舵角
char k=0; //命令寄存器
char kv=0;
char val=0;
char val_v=0;
u8 K_flag=0;//命令标志位
u16 temp=0;//速度缓存
extern unsigned char Com_data_Recv[10];
extern unsigned char flag;//接收到一组本机命令标志

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

//主函数
 int main()
 {
	u16 i,rxlen;
	u8 led=0; //LED指示灯
	u8 ctrl_flag=1;	//控制标志位

	union conv	   //  航向角联合体
	{
	  float IMU_f;//浮点型航向角
	  unsigned char IMU_b[4];
	}c;

	
	LED_Init();	
	moter_pwm_init(1000,1440);	//设置pwm波周期为72000000/1440/1000=50Hz
	usart_init(38400,38400);	//设置串口1、串口2、串口3波特率
	GPIO_ResetBits(GPIOE,LIN2|RIN2);
    GPIO_SetBits(GPIOE,LIN1|RIN1);

	Get[0]=0xA5;
    Get[1]=0x5A;
    Get[2]=26;//数据长度位
    Get[3]=ID_ship;//地址位

    while(1)
	{

	 if(USART2_RX_STA&0X8000)		//收到GPS数据进行处理
	 {
		rxlen=USART2_RX_STA&0X7FFF;	//得到数据长度
		for(i=0;i<rxlen;i++)GPS_temp[i]=USART2_RX_BUF[i];	   
		USART2_RX_STA=0;		   	//启动下一次接收
		GPS_temp[i]=0;			//自动添加结束符

//	    GPS_Analysis(&gpsx,(u8*)GPS_temp);
		NMEA_GPRMC_Analysis(&gpsx,(u8*)GPS_temp);//GPS数据分析

		Get[4]=gpsx.latitude>>24;
		Get[5]=(gpsx.latitude>>16)&0xFF;
		Get[6]=(gpsx.latitude>>8)&0xFF;
		Get[7]=gpsx.latitude&0xFF;

		Get[8]=gpsx.longitude>>24;
		Get[9]=(gpsx.longitude>>16)&0xFF;
		Get[10]=(gpsx.longitude>>8)&0xFF;
		Get[11]=gpsx.longitude&0xFF;

		Get[12]=gpsx.utc.hour;
		Get[13]=gpsx.utc.min;
		Get[14]=gpsx.utc.sec;

		Get[15]=(gpsx.speed>>8)&0xFF;
		Get[16]=gpsx.speed&0xFF;
  	 }
 
	  if(USART3_RX_STA&0X8000)		//收到IMU数据进行处理
	 {
		rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度
		for(i=0;i<rxlen;i++)IMU_temp[i]=USART3_RX_BUF[i];	   
		USART3_RX_STA=0;		   	//启动下一次接收
		IMU_temp[i]=0;			//自动添加结束符
		for(i=0;i<rxlen;i++) Decode_frame(IMU_temp[i]);	 //IMU数据分析

	    c.IMU_f=-(float)IMU_i/10.0f;
	    
		Get[17]=c.IMU_b[0];   //浮点数转换为四字节
		Get[18]=c.IMU_b[1];
		Get[19]=c.IMU_b[2];
		Get[20]=c.IMU_b[3];
  	 }

	 while(flag==1)//接收到上位机指令 	
	  {
	   flag=0;
	   val=Com_data_Recv[3];
	   val_v=Com_data_Recv[4];
	   if(val=='G')
		{  
		  Get[21]=angle+30;//舵角
     	  Get[22]=speed1;//档位
		  Get[23]='0';//奇偶校验
     	  Get[24]='0';
		  Get[25]=0xAA;	//包尾
		  for (i=0;i<26;i++)
		 	{
		 		USART_SendData(USART1,Get[i]);//通过外设USARTx发送单个数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);
				if(Get[i]==0xAA) break;
		 	}
		  led=!led;	 	
		  if(led==0)GPIO_ResetBits(GPIOC,LED0);
		  else GPIO_SetBits(GPIOC,LED0);
		}

		else if(val=='S')	   //停车
		{
		 speed1=0;
		 speed2=0;
		 TIM_SetCompare1(TIM2, speed1*10);
		 TIM_SetCompare2(TIM2, speed2*10);
		 angle=0;
		 TIM_SetCompare1(TIM3, (angle+75));
		 ctrl_flag=1; 
		}

		else if(val=='F')	 //正车
		{
		  GPIO_ResetBits(GPIOE,LIN2|RIN2);
          GPIO_SetBits(GPIOE,LIN1|RIN1);
		}
		 //倒车
		else if(val=='B')
		{
		  GPIO_SetBits(GPIOE,LIN2|RIN2);
          GPIO_ResetBits(GPIOE,LIN1|RIN1);
		}
		//右转
		else if(val=='R')
		{
		angle--;
		if(angle<-16){angle=-16;}
		TIM_SetCompare1(TIM3, (angle+75)); 
		}
		//左转
		else if(val=='Q')
		{
		angle++;
		if(angle>16){angle=16;}
		TIM_SetCompare1(TIM3, (angle+75)); 
		}
		//加速
		else if(val=='I')
		{
		 speed1++;
		 speed2++;
		 if(speed1>20||speed2>20){speed1=0;speed2=0;}
		 temp=speed1*10;
		 TIM_SetCompare1(TIM2,temp);
		 temp=speed2*10;
		 TIM_SetCompare2(TIM2,temp); 
		}
		//减速
		else if(val=='D')
		{
		speed1--;
		if(speed1>20||speed1>20){speed1=0;}
		temp=speed1*10;
		TIM_SetCompare1(TIM2,temp);
		temp=speed2*10;
		TIM_SetCompare2(TIM2,temp); 
		}

		else if (val <= 64)  //闭环控制
		{
			int err; //定义一个舵角偏差
			int Ctr_angle; //闭环控制时的舵角信号
			if((val_v>=100)&&(val_v<=200)) // 赋值一次控制速度
			{	control_speed=val_v-50;//取出控制速度速度范围在5—15，即50-150
				speed1=(int)(control_speed/10+0.5);
//				speed2=control_speed;
//		        temp=speed1*10;
		        TIM_SetCompare1(TIM2,control_speed);
//				temp=speed2*10;
		        TIM_SetCompare2(TIM2,control_speed);
			}						
			  err = val - 32;	 //打舵方式正左负右，即船向偏差减小的方向转动
			  Ctr_angle=setAngle(err);
			  angle=Ctr_angle;
			  TIM_SetCompare1(TIM3, (75+Ctr_angle));
		    			
	    } 			
	 }
 }

}
