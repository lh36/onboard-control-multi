#ifndef _control_H
#define _control_H
#include "stm32f10x.h"


#define WHEELSPAN 500

enum 
{
	STAT_UNKNOWN,
	STAT_STOP,
	STAT_ADVANCE,
	STAT_BACKOFF,
	STAT_ROTATELEFT,
	STAT_ROTATERIGHT,
	STAT_UPPERLEFT,
	STAT_LOWERLEFT,
	STAT_LOWERRIGHT,
	STAT_UPPERRIGHT,
};


unsigned int setMotorAll(unsigned int speedMMPS);//ͬʱ��������
unsigned int setCarBase(unsigned int speedMMPSL,unsigned int speedMMPSR);//�ֱ��������

unsigned char setCarStat(unsigned char stat); //�����˶�״̬
unsigned char getCarStat(void);			  //��ȡ�˶�״̬

unsigned int setCarStop(void); //ͣ��
unsigned int setCarAdvance(unsigned int speedMMPS);//�������ٶ�ǰ��
unsigned int setCarBackoff(unsigned int speedMMPS);//�������ٶȺ���

unsigned int setCarArcBase(unsigned int speedMMPS,unsigned int radiusMM);	//�������ٶȺ�ת��뾶ת��
unsigned int setCarUpperLeft(unsigned int speedMMPS,unsigned int radiusMM);//�������ٶȺͰ뾶��ת
unsigned int setCarUpperRight(unsigned int speedMMPS,unsigned int radiusMM);//�������ٶȺͰ뾶��ת


#endif
