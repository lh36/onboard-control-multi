#ifndef __GPS_H
#define __GPS_H	 
#include "sys.h"   

//GPS NMEA-0183协议重要参数结构体定义 
  
//UTC时间信息
__packed typedef struct  
{										    
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
}nmea_utc_time;   	   
//NMEA 0183 协议解析后数据存放结构体
__packed typedef struct  
{										    
	nmea_utc_time utc;			//UTC时间
	u32 latitude;				//纬度 分扩大100000倍,实际要除以100000				  
	u32 longitude;			    //经度 分扩大100000倍,实际要除以100000	 
	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
				 
int NMEA_Str2num(u8 *buf,u8*dx);
void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
#endif  

 



