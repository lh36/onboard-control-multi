/*******************************************************************************
*                 
*                 		       ���пƼ�
--------------------------------------------------------------------------------
* ʵ �� ��		 : PWMʵ��
* ʵ��˵��       : ͨ����ʱ��3���һ��PWM����LEDС�ƣ�LEDС�Ƴ��ֺ���Ч��
* ���ӷ�ʽ       : 
* ע    ��		 : 	���ú�����ͷ�ļ�.c�ļ���
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
extern char receive[10];  //�ⲿ����ı����ڴ˴����ã����Ҫ��extern
extern char count;
extern char flag;
extern unsigned char GrevData[60];
extern unsigned char Gcounter;
extern unsigned char MTIflag;
extern unsigned char Get[26];
extern char recv;
extern vu16 USART2_RX_STA;//����2����gps���ݵļ����ͱ�־
extern u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];
extern u8 USART1_TX_BUF[USART2_MAX_RECV_LEN];	//Ҫ���͸���λ��(����1)��gps�������
extern nmea_msg gpsx; 	//GPS��Ϣ
extern unsigned char Com_data_Recv[10];	 //���յ�����λ�������ڱ���������
u32 ti_PC6=150;	 
u32 ti_PC7=150; 	 //=30,1.5ms;=22,1.1ms,=38,1.9ms  ���ڣ�16.04ms
int speed=0;
extern union gps_jw_inf	   //����γ��������
{
	float gps_f;//�����ͺ����
	unsigned char gps_h[4];
}f2h_jw;

char val;

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
//��ʾGPS��Ϣ 
void Gps_Msg_Show(void)
{
 	float tp;		   	 
	tp=gpsx.longitude;	   
	printf("Longitude:%.5f %1c   ",tp/=100000,'E');	// �����ַ���
 	   
	tp=gpsx.latitude;	   
	printf("Latitude:%.5f %1c   ",tp/=100000,'N');	//γ���ַ���
 	 			   
	tp=gpsx.speed;	   
 	printf("Speed:%.3fkm/h     ",tp/=1000);		    		//�ٶ��ַ���	 
	    
	printf("UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//ʱ���ַ���	  
}



int main()
{	
	u16 i,rxlen; //gps���ݳ���
	u8 temp_left,temp_right;//������//������

	switch_Init();
	pwm_init();	 //PWM��ʼ��
	usart_init();
	TIM_SetCompare1(TIM3, ti_PC6);//����TIMx����Ƚ�2�Ĵ���ֵ��ͨ��1PC6����
	TIM_SetCompare2(TIM3, ti_PC7);//����TIMx����Ƚ�2�Ĵ���ֵ��ͨ��2PC7����
	Get[0] = 0xa5;
	Get[1] = 0x5a;
	Get[2] = 26;
	Get[3] = 0x01;
	GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1);
	while(1)
	{
		if(USART2_RX_STA&0X8000)
		{

			rxlen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART2_RX_BUF[i];	   
 			USART2_RX_STA=0;		   	//������һ�ν���
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ������������GPS��Ϣ�ṹ��
			//��ȡγ����Ϣ
		//	f2h_jw.gps_f = (float)gpsx.latitude/100000.0f;   
			
			Get[4] = gpsx.latitude>>24;						   //Э����װ
			Get[5] = (gpsx.latitude>>16)&0xff;				   //��λ���ȼ�Ҫ�����������㣬��һ��Ҫ�������
			Get[6] = (gpsx.latitude>>8)&0xff;
			Get[7] = gpsx.latitude&0xff;
			//��ȡ������Ϣ
		//	f2h_jw.gps_f = (float)gpsx.longitude/100000.0f;   
			Get[8] = gpsx.longitude>>24;						   //Э����װ
			Get[9] = (gpsx.longitude>>16)&0xff;
			Get[10] = (gpsx.longitude>>8)&0xff;
			Get[11] = gpsx.longitude&0xff;
			//��ȡʱ����Ϣ����������ʱ�䣩
			Get[12] = gpsx.utc.hour;					   //��ȡʱ����Ϣ����������ʱ�䣩
			Get[13] = gpsx.utc.min;
			Get[14] = gpsx.utc.sec;
			//��ȡ�ٶ���Ϣ;
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
		if(flag==1)	//flag Ϊ���յ���λ������ı�־
		{
			flag=0;
			val=Com_data_Recv[3];
			if(val=='N')	 //����ǰ��
			{
				speed=10;
				setCarAdvance(speed);
			}
			else if(val=='M')	//����ǰ��
			{
				speed=30;
				setCarAdvance(speed);
			}
			else if(val=='A')  //�Թ̶��ٶ�ǰ��
			{
				setCarAdvance(speed);
			}
			else if(val=='B')  //����
			{
				setCarBackoff(speed);
			}
			else if(val=='S') //ͣ��
			{
				setCarStop();
				speed=0;
			}
			else if(val=='G') //��ȡ��̬��Ϣ
			{
				int grade;
				Get[21] = 0;	//�����Ϣ
				//�ٶȵȼ���Ϣ��ȡ
				temp_left = abs(150 - TIM_GetCapture1(TIM3));//���ƽ��ȼ�
				temp_right= abs(150 - TIM_GetCapture2(TIM3));//���ƽ��ȼ�
				grade =	(temp_left+temp_right)/2;
				Get[22] =  grade&0xff;
			
				//CRCУ��
				Get[23] = CRC16(Get, 23)/256;
				Get[24] = CRC16(Get, 23)%256;
				Get[25]	= 0xaa;
				 for (i=0;i<sizeof(Get)/sizeof(char);i++)
			 	{							   
			 		USART_SendData(USART1,Get[i]);//ͨ������USARTx���͵�������
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);
			 	}
				
			}
			else if(val=='Y')//�л��ɸߵ�ƽ��ң��
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1);	 //IO������ߵ�ƽ
			}
			else if(val=='Z')//�л��ɵ͵�ƽ����������
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1); //IO������͵�ƽ
			}
			else if(val=='Q')//�����
			{
				u16 temp=TIM_GetCapture1(TIM3);
				temp--;
				TIM_SetCompare1(TIM3, temp);	
			}
			else if(val=='L')//�����
			{
				u16 temp=TIM_GetCapture1(TIM3);
				temp++;
				TIM_SetCompare1(TIM3, temp);	
			}
			else if(val=='R') //�Ҽ���
			{
				u16 temp=TIM_GetCapture2(TIM3);
				temp--;
				TIM_SetCompare2(TIM3, temp);
			}
			else if(val=='V') //�Ҽ���
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
				if(error!=0)	//������
				{
					int r=(int)(180/(abs(error)*1.5*3.1415926)); //����ÿ��תx�㣬�������ó�
					if (error>0)
					{
						setCarUpperLeft(134,r); //�Ƶ���-ʵ�ʽǣ�errorΪ������Ҫ��ת��
						
					}
					else 
					{
						setCarUpperRight(134,r);	//errorΪ������Ҫ��ת��
					}
				}
				else
				{
					setCarAdvance(18);
				}
			}
		}//if(flag==1) �ӵ���λ������
	}//while(1)
}



