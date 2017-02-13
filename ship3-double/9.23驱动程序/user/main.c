/*******************************************************************************
*                 
*                 		       ���пƼ�
--------------------------------------------------------------------------------
* ʵ �� ��		 : PWMʵ��
* ʵ��˵��       : ͨ����ʱ��3���һ��PWM����LEDС�ƣ�LEDС�Ƴ��ֺ���Ч��
* ���ӷ�ʽ       : 
* ע    ��		 : 	���ú�����ͷ�ļ�.c�ļ���
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
u8 GPS_temp[200];//GPS�����ݴ�
unsigned char IMU_temp[50];//GPS�����ݴ�
nmea_msg gpsx;
u16 speed1=0;  //pwm��������λms
u16 speed2=0;
u16 control_speed=10; //�����ٶ�
unsigned char Get[35];//״̬����֡
int angle=0;	 //���
char k=0; //����Ĵ���
char kv=0;
char val=0;
char val_v=0;
u8 K_flag=0;//�����־λ
u16 temp=0;//�ٶȻ���
extern unsigned char Com_data_Recv[10];
extern unsigned char flag;//���յ�һ�鱾�������־

 int fputc(int ch, FILE *f)
 {
   USART_SendData(USART1, (unsigned char) ch);// USART1 ���Ի��� USART2 ��
   while (!(USART1->SR & USART_FLAG_TXE));
   return (ch);
 } 	
// ��������
 int GetKey (void) 
 {
   while (!(USART1->SR & USART_FLAG_RXNE));
   return ((int)(USART1->DR & 0x1FF));
 }	

//������
 int main()
 {
	u16 i,rxlen;
	u8 led=0; //LEDָʾ��
	u8 ctrl_flag=1;	//���Ʊ�־λ

	union conv	   //  �����������
	{
	  float IMU_f;//�����ͺ����
	  unsigned char IMU_b[4];
	}c;

	
	LED_Init();	
	moter_pwm_init(1000,1440);	//����pwm������Ϊ72000000/1440/1000=50Hz
	usart_init(38400,38400);	//���ô���1������2������3������
	GPIO_ResetBits(GPIOE,LIN2|RIN2);
    GPIO_SetBits(GPIOE,LIN1|RIN1);

	Get[0]=0xA5;
    Get[1]=0x5A;
    Get[2]=26;//���ݳ���λ
    Get[3]=ID_ship;//��ַλ

    while(1)
	{

	 if(USART2_RX_STA&0X8000)		//�յ�GPS���ݽ��д���
	 {
		rxlen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		for(i=0;i<rxlen;i++)GPS_temp[i]=USART2_RX_BUF[i];	   
		USART2_RX_STA=0;		   	//������һ�ν���
		GPS_temp[i]=0;			//�Զ���ӽ�����

//	    GPS_Analysis(&gpsx,(u8*)GPS_temp);
		NMEA_GPRMC_Analysis(&gpsx,(u8*)GPS_temp);//GPS���ݷ���

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
 
	  if(USART3_RX_STA&0X8000)		//�յ�IMU���ݽ��д���
	 {
		rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		for(i=0;i<rxlen;i++)IMU_temp[i]=USART3_RX_BUF[i];	   
		USART3_RX_STA=0;		   	//������һ�ν���
		IMU_temp[i]=0;			//�Զ���ӽ�����
		for(i=0;i<rxlen;i++) Decode_frame(IMU_temp[i]);	 //IMU���ݷ���

	    c.IMU_f=-(float)IMU_i/10.0f;
	    
		Get[17]=c.IMU_b[0];   //������ת��Ϊ���ֽ�
		Get[18]=c.IMU_b[1];
		Get[19]=c.IMU_b[2];
		Get[20]=c.IMU_b[3];
  	 }

	 while(flag==1)//���յ���λ��ָ�� 	
	  {
	   flag=0;
	   val=Com_data_Recv[3];
	   val_v=Com_data_Recv[4];
	   if(val=='G')
		{  
		  Get[21]=angle+30;//���
     	  Get[22]=speed1;//��λ
		  Get[23]='0';//��żУ��
     	  Get[24]='0';
		  Get[25]=0xAA;	//��β
		  for (i=0;i<26;i++)
		 	{
		 		USART_SendData(USART1,Get[i]);//ͨ������USARTx���͵�������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);
				if(Get[i]==0xAA) break;
		 	}
		  led=!led;	 	
		  if(led==0)GPIO_ResetBits(GPIOC,LED0);
		  else GPIO_SetBits(GPIOC,LED0);
		}

		else if(val=='S')	   //ͣ��
		{
		 speed1=0;
		 speed2=0;
		 TIM_SetCompare1(TIM2, speed1*10);
		 TIM_SetCompare2(TIM2, speed2*10);
		 angle=0;
		 TIM_SetCompare1(TIM3, (angle+75));
		 ctrl_flag=1; 
		}

		else if(val=='F')	 //����
		{
		  GPIO_ResetBits(GPIOE,LIN2|RIN2);
          GPIO_SetBits(GPIOE,LIN1|RIN1);
		}
		 //����
		else if(val=='B')
		{
		  GPIO_SetBits(GPIOE,LIN2|RIN2);
          GPIO_ResetBits(GPIOE,LIN1|RIN1);
		}
		//��ת
		else if(val=='R')
		{
		angle--;
		if(angle<-16){angle=-16;}
		TIM_SetCompare1(TIM3, (angle+75)); 
		}
		//��ת
		else if(val=='Q')
		{
		angle++;
		if(angle>16){angle=16;}
		TIM_SetCompare1(TIM3, (angle+75)); 
		}
		//����
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
		//����
		else if(val=='D')
		{
		speed1--;
		if(speed1>20||speed1>20){speed1=0;}
		temp=speed1*10;
		TIM_SetCompare1(TIM2,temp);
		temp=speed2*10;
		TIM_SetCompare2(TIM2,temp); 
		}

		else if (val <= 64)  //�ջ�����
		{
			int err; //����һ�����ƫ��
			int Ctr_angle; //�ջ�����ʱ�Ķ���ź�
			if((val_v>=100)&&(val_v<=200)) // ��ֵһ�ο����ٶ�
			{	control_speed=val_v-50;//ȡ�������ٶ��ٶȷ�Χ��5��15����50-150
				speed1=(int)(control_speed/10+0.5);
//				speed2=control_speed;
//		        temp=speed1*10;
		        TIM_SetCompare1(TIM2,control_speed);
//				temp=speed2*10;
		        TIM_SetCompare2(TIM2,control_speed);
			}						
			  err = val - 32;	 //��淽ʽ�����ң�������ƫ���С�ķ���ת��
			  Ctr_angle=setAngle(err);
			  angle=Ctr_angle;
			  TIM_SetCompare1(TIM3, (75+Ctr_angle));
		    			
	    } 			
	 }
 }

}
