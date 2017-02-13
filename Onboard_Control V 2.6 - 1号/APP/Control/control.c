#include "control.h"
/*******************************************************************************
* �� �� ��         : boat_advance
* ��������		   : ǰ������
* ��    ��         : ��
* ��    ��         : ��
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
	unsigned int delta=(int)(0.5/radiusMM*16+0.5);//���¸�����ΧΪ8�����巶Χ16�������0.5��Ϊ����������
	if(delta>=10) delta=10;
	V1=150-(speedMMPS-delta);//ת��뾶������1-16�ף�����С�ֱ��ʾ���
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
