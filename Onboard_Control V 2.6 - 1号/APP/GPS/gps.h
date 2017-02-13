#ifndef __GPS_H
#define __GPS_H	 
#include "sys.h"   

//GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� 
  
//UTCʱ����Ϣ
__packed typedef struct  
{										    
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
}nmea_utc_time;   	   
//NMEA 0183 Э����������ݴ�Žṹ��
__packed typedef struct  
{										    
	nmea_utc_time utc;			//UTCʱ��
	u32 latitude;				//γ�� ������100000��,ʵ��Ҫ����100000				  
	u32 longitude;			    //���� ������100000��,ʵ��Ҫ����100000	 
	u16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
				 
int NMEA_Str2num(u8 *buf,u8*dx);
void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
#endif  

 



