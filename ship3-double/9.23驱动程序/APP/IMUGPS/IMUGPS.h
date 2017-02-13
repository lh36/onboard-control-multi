#ifndef _IMUGPS_H
#define _IMUGPS_H
#include "stm32f10x.h" 								   
#include "stdio.h"
#include "stm32f10x_it.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"

 //UTC时间信息
__packed typedef struct  
{										    
// 	u16 year;	//年
//	u8 month;	//月
//	u8 date;	//日
	u8 hour; 	//时
	u8 min; 	//分
	u8 sec; 	//秒
}nmea_utc_time;
 //GPS解析出的信息
__packed typedef struct  
{										    
	nmea_utc_time utc;			//UTC时间
	u32 latitude;				//纬度，扩大了10^5，上位机需要除以10^5.
//	u8 nshemi;					//南北纬				  
	u32 longitude;			    //经度，扩大了10^5，上位机需要除以10^5.
//	u8 ewhemi;					//东西经
//	u8 gpssta;					//GPS状态				  
//	u8 fixmode;					//定位类型
//	int altitude;			 	//海拔高度，放大了10倍，上位机实际除以10，上位机单位m。	 
	u16 speed;					//速度，发达了1000倍，上位机除以1000，上位机单位公里/小时。 
}nmea_msg; 

#define b_uart_head  0x80  //收到A5 头 标志位
#define b_rx_over    0x40  //收到完整的帧标志
extern int16_t IMU_i;	  //航向角寄存器
extern unsigned char rx_buffer[50]; //接收数据缓冲区

//void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
void Decode_frame(unsigned char data);

#endif
