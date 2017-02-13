#include "control.h"
/*******************************************************************************
* 函 数 名         : boat_advance
* 函数功能		   : 前进函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
unsigned char _carStat;
unsigned int setMotorAll(unsigned int speedMMPS)
{
	TIM_SetCompare1(TIM3,speedMMPS);
	TIM_SetCompare2(TIM3,speedMMPS);
	return speedMMPS;
}
unsigned int setCarBase(unsigned int speedMMPSL,unsigned int speedMMPSR) 
{
	TIM_SetCompare1(TIM3,speedMMPSL);
	TIM_SetCompare2(TIM3,speedMMPSR);
	return speedMMPSL+speedMMPSR;
}

unsigned char getCarStat()
{
	return _carStat;
}
unsigned char setCarStat(unsigned char carStat) 
{
    return _carStat=carStat;
}

unsigned int setCarStop()
{
	setCarStat(STAT_STOP);
	return setMotorAll(150);	
}
unsigned int setCarAdvance(unsigned int speedMMPS) 
{
	unsigned int temp=150-speedMMPS;
	setCarStat(STAT_ADVANCE);
	return setMotorAll(temp);
}
unsigned int setCarBackoff(unsigned int speedMMPS) 
{
	unsigned int temp=150+speedMMPS;
	setCarStat(STAT_BACKOFF);
	return setMotorAll(temp);
}

unsigned int setCarArcBase(unsigned int speedMMPS,unsigned int radiusMM) 
{
	unsigned int V1,V2;
	unsigned int delta=(int)(0.5/radiusMM*16+0.5);//上下浮动范围为8，整体范围16，后面加0.5是为了四舍五入
	if(delta>=10) delta=10;
	V1=150-(speedMMPS-delta);//转弯半径限制在1-16米，由最小分辨率决定
	V2=150-(speedMMPS+delta);

	switch(getCarStat()) 
	{
		case STAT_UPPERLEFT:
			setCarBase(V1,V2); break;
		case STAT_UPPERRIGHT:
			setCarBase(V2,V1); break;
	}

	return speedMMPS;
}

unsigned int setCarUpperLeft(unsigned int speedMMPS,unsigned int radiusMM) 
{
	setCarStat(STAT_UPPERLEFT);
	return setCarArcBase(speedMMPS,radiusMM);
}
unsigned int setCarUpperRight(unsigned int speedMMPS,unsigned int radiusMM) 
{
	setCarStat(STAT_UPPERRIGHT);
	return setCarArcBase(speedMMPS,radiusMM);
}
