#include "IMUGPS.h"



//��buf�еõ���cx����������λ��
//����ֵ��0~0XFE,����������λ�õ�ƫ�ơ�0XFF�������ڵ�cx������							  
 u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
 {	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//��?��?'*'?��??��?������?��?,?��2?��??���̨�cx???oo?
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
 }
//m^n����������ֵm��n�η�
 u32 NMEA_Pow(u8 m,u8 n)
 {
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
 }
//strת��Ϊ���֣��ԡ��������ߡ�*������
//buf�����ִ洢��
//dx��С����λ�������ظ����ú���
//����ֵ��ת�������ֵ
 int NMEA_Str2num(u8 *buf,u8*dx)
 {
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*'))break;//��������
		if(*p=='.'){mask|=0X01;p++;}//����С����
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
		ires=ires%100;//����С����ǰ����λ�֣�ȥ����
	}
	if(flen>6)flen=6;	//��ౣ��5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
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

 //����GPRMC��Ϣ
 //gpsx��nmea��Ϣ�ṹ��
 //buf:���յ���GPS���ݻ������׵�ַ
 void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
 {
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;
//	u32 du;	   
//	float rs;  
	p1=(u8*)strstr((const char *)buf,"GNRMC");//ֻ�ж��ַ�����GNRMC����
	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ�䣬ȥ��ms��
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		du=temp/NMEA_Pow(10,dx+2);	//�õ���
//		rs=temp%NMEA_Pow(10,dx+2);				//�õ���		 
//		gpsx->latitude=du+rs/60;//ת��Ϊ�ȣ���λ����Ҫ����10^5��
		gpsx->latitude=temp*100/6; //����8λС������λ����Ҫ����10^8
	}
//	posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ
//	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		du=temp/NMEA_Pow(10,dx+2);	//�õ���
//		rs=temp%NMEA_Pow(10,dx+2);				//�õ���	 
//		gpsx->longitude=du+rs/60;//ת��Ϊ�ȣ���λ����Ҫ����10^5��
        gpsx->longitude=temp*100/6; //����8λС������λ����Ҫ����10^8 
	}
	posx=NMEA_Comma_Pos(p1,7);								//�õ���������
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//�ٶ�����1000������λ������1000
	}
//	posx=NMEA_Comma_Pos(p1,6);								//������������
//	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
//	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
//		gpsx->utc.date=temp/10000;
//		gpsx->utc.month=(temp/100)%100;
//		gpsx->utc.year=2000+temp%100;	 	 
//	} 
 }



unsigned char RC_Flag;
unsigned char rx_buffer[50]; //�������ݻ�����
unsigned char rx_wr_index; //����дָ��
int16_t IMU_i=0;	  //����ǼĴ���

//IMU��������
//�յ�һ���ֽڵ���һ�Ρ����յ����ֽ���Ϊ���롣
void Decode_frame(unsigned char data)
{
  if(data==0xa5) 
  { 
	RC_Flag|=b_uart_head; //������յ�A5 ��λ֡ͷ��רλ
    rx_buffer[rx_wr_index++]=data; //��������ֽ�.
  }
  else if(data==0x5a)
       { 
	   if(RC_Flag&b_uart_head) //�����һ���ֽ���A5 ��ô�϶� �����֡��ʼ�ֽ�
	     { rx_wr_index=0;  //���� ������ָ��
		   RC_Flag&=~b_rx_over; //���֡�Ÿոտ�ʼ��
         }
         else //��һ���ֽڲ���A5
		  rx_buffer[rx_wr_index++]=data;
         RC_Flag&=~b_uart_head; //��֡ͷ��־
       }
	   else
	   { rx_buffer[rx_wr_index++]=data;
		 RC_Flag&=~b_uart_head;
		 if(rx_wr_index==rx_buffer[0]) //�չ����ֽ���.
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
