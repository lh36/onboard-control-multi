#include "IMUGPS.h"



//从buf中得到第cx个逗号所在位置
//返回值：0~0XFE,代表逗号所在位置的偏移。0XFF代表不存在第cx个逗号							  
 u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
 {	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//ó?μ?'*'?ò??·?·¨×?·?,?ò2?′??úμúcx???oo?
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
 }
//m^n函数，返回值m的n次方
 u32 NMEA_Pow(u8 m,u8 n)
 {
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
 }
//str转换为数字，以‘，’或者‘*’结束
//buf：数字存储区
//dx：小数点位数，返回给调用函数
//返回值：转换后的数值
 int NMEA_Str2num(u8 *buf,u8*dx)
 {
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*'))break;//遇到结束
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
		ires=ires%100;//保留小数点前的两位分，去掉度
	}
	if(flen>6)flen=6;	//最多保留5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
 }



//
//void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
//{
//
//	NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMC?a??
//}

 //分析GPRMC信息
 //gpsx：nmea信息结构体
 //buf:接收到的GPS数据缓存区首地址
 void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
 {
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;
//	u32 du;	   
//	float rs;  
	p1=(u8*)strstr((const char *)buf,"GNRMC");//只判断字符串“GNRMC”。
	posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间，去掉ms。
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		du=temp/NMEA_Pow(10,dx+2);	//得到度
//		rs=temp%NMEA_Pow(10,dx+2);				//得到分		 
//		gpsx->latitude=du+rs/60;//转化为度，上位机需要除以10^5。
		gpsx->latitude=temp*100/6; //保留8位小数，上位机需要除以10^8
	}
//	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬
//	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		du=temp/NMEA_Pow(10,dx+2);	//得到度
//		rs=temp%NMEA_Pow(10,dx+2);				//得到分	 
//		gpsx->longitude=du+rs/60;//转换为度，上位机需要除以10^5。
        gpsx->longitude=temp*100/6; //保留8位小数，上位机需要除以10^8 
	}
	posx=NMEA_Comma_Pos(p1,7);								//得到地面速率
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//速度扩大1000倍，上位机除以1000
	}
//	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
//	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
//	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
//		gpsx->utc.date=temp/10000;
//		gpsx->utc.month=(temp/100)%100;
//		gpsx->utc.year=2000+temp%100;	 	 
//	} 
 }



unsigned char RC_Flag;
unsigned char rx_buffer[50]; //接收数据缓冲区
unsigned char rx_wr_index; //缓冲写指针
int16_t IMU_i=0;	  //航向角寄存器

//IMU解析函数
//收到一个字节调用一次。把收到的字节做为输入。
void Decode_frame(unsigned char data)
{
  if(data==0xa5) 
  { 
	RC_Flag|=b_uart_head; //如果接收到A5 置位帧头标专位
    rx_buffer[rx_wr_index++]=data; //保存这个字节.
  }
  else if(data==0x5a)
       { 
	   if(RC_Flag&b_uart_head) //如果上一个字节是A5 那么认定 这个是帧起始字节
	     { rx_wr_index=0;  //重置 缓冲区指针
		   RC_Flag&=~b_rx_over; //这个帧才刚刚开始收
         }
         else //上一个字节不是A5
		  rx_buffer[rx_wr_index++]=data;
         RC_Flag&=~b_uart_head; //清帧头标志
       }
	   else
	   { rx_buffer[rx_wr_index++]=data;
		 RC_Flag&=~b_uart_head;
		 if(rx_wr_index==rx_buffer[0]) //收够了字节数.
	     {  
			IMU_i=rx_buffer[2];
			IMU_i<<=8;
			IMU_i |= rx_buffer[3];
			if(IMU_i&0x8000)
			{
			   IMU_i=0-(IMU_i&0x7fff);
			} 
			else IMU_i=(IMU_i&0x7fff);
			rx_wr_index=0;

          }
	   }
}
