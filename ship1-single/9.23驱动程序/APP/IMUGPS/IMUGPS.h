#ifndef _IMUGPS_H
#define _IMUGPS_H
#include "stm32f10x.h" 								   
#include "stdio.h"
#include "stm32f10x_it.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"

 //UTCʱ����Ϣ
__packed typedef struct  
{										    
// 	u16 year;	//��
//	u8 month;	//��
//	u8 date;	//��
	u8 hour; 	//ʱ
	u8 min; 	//��
	u8 sec; 	//��
}nmea_utc_time;
 //GPS����������Ϣ
__packed typedef struct  
{										    
	nmea_utc_time utc;			//UTCʱ��
	u32 latitude;				//γ�ȣ�������10^5����λ����Ҫ����10^5.
//	u8 nshemi;					//�ϱ�γ				  
	u32 longitude;			    //���ȣ�������10^5����λ����Ҫ����10^5.
//	u8 ewhemi;					//������
//	u8 gpssta;					//GPS״̬				  
//	u8 fixmode;					//��λ����
//	int altitude;			 	//���θ߶ȣ��Ŵ���10������λ��ʵ�ʳ���10����λ����λm��	 
	u16 speed;					//�ٶȣ�������1000������λ������1000����λ����λ����/Сʱ�� 
}nmea_msg; 

#define b_uart_head  0x80  //�յ�A5 ͷ ��־λ
#define b_rx_over    0x40  //�յ�������֡��־
extern int16_t IMU_i;	  //����ǼĴ���
extern unsigned char rx_buffer[50]; //�������ݻ�����

//void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
void Decode_frame(unsigned char data);

#endif
