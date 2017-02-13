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


unsigned int setMotorAll(unsigned int speedMMPS);//同时控制两轮
unsigned int setCarBase(unsigned int speedMMPSL,unsigned int speedMMPSR);//分别控制两轮

unsigned char setCarStat(unsigned char stat); //设置运动状态
unsigned char getCarStat(void);			  //获取运动状态

unsigned int setCarStop(void); //停船
unsigned int setCarAdvance(unsigned int speedMMPS);//以设置速度前进
unsigned int setCarBackoff(unsigned int speedMMPS);//以设置速度后退

unsigned int setCarArcBase(unsigned int speedMMPS,unsigned int radiusMM);	//以设置速度和转弯半径转弯
unsigned int setCarUpperLeft(unsigned int speedMMPS,unsigned int radiusMM);//以设置速度和半径左转
unsigned int setCarUpperRight(unsigned int speedMMPS,unsigned int radiusMM);//以设置速度和半径左转


#endif
