#include <stdint.h>
//#include "main.h"
//#include "ac78xx_irq_cb.h"
#include "adc.h"
//#include "can.h"
//#include "ac78xx_adc.h"
#include "alarm.h"

ALM_TYPE_FLAG AlmFlag;

ALM_PARA_STR AcOverVolt;
ALM_PARA_STR AcUnderVolt;
ALM_PARA_STR BatOverVolt;
ALM_PARA_STR BatUnderVolt;
ALM_PARA_STR InnerOverTemp;
ALM_PARA_STR BatOverTemp;

void ALM_Init(void)
{
	AlmFlag.all = 0;

	AcOverVolt.GeneCnt = 10;
	AcOverVolt.GeneVal = (uint32_t)270*1000;
	AcOverVolt.RecvCnt = 200;
	AcOverVolt.RecvVal = (uint32_t)250*1000;
	
    AcUnderVolt.GeneCnt = 10;
	AcUnderVolt.GeneVal = (uint32_t)80*1000;
	AcUnderVolt.RecvCnt = 200;
	AcUnderVolt.RecvVal = (uint32_t)90*1000;
	
	BatOverVolt.GeneCnt = 10;
	BatOverVolt.GeneVal = ((uint32_t)35<<10);
	BatOverVolt.RecvCnt = 200;
	BatOverVolt.RecvVal = ((uint32_t)30<<10);
	
	BatUnderVolt.GeneCnt = 30;
	BatUnderVolt.GeneVal = ((uint32_t)16<<10);
	BatUnderVolt.RecvCnt = 1000;
	BatUnderVolt.RecvVal = ((uint32_t)18<<10);
	
	InnerOverTemp.GeneCnt = 10;//10S
	InnerOverTemp.GeneVal = 90;
	InnerOverTemp.RecvCnt = 60; //1min
	InnerOverTemp.RecvVal = 80;
	
	BatOverTemp.GeneCnt = 5;//5S
	BatOverTemp.GeneVal = 60;
	BatOverTemp.RecvCnt = 60; //1min
	BatOverTemp.RecvVal = 50;	
}

//AC_OVP
void ALM_AcOverVoltDetect(void)
{
	static uint16_t sAcVoltOverCnt[2] = {0};
	
	if( AcVoltAvg >= AcOverVolt.GeneVal)
	{
		sAcVoltOverCnt[0]++;
		sAcVoltOverCnt[1] = 0;
		if( sAcVoltOverCnt[0] >= AcOverVolt.GeneCnt)
		{
			sAcVoltOverCnt[0] = AcOverVolt.GeneCnt;
			AlmFlag.bit.AcOverVolt = ALM_FAULT;				
		}
	}
	
	if( AcVoltAvg <= AcOverVolt.RecvVal )
	{
		sAcVoltOverCnt[1]++;
		sAcVoltOverCnt[0] = 0;
		if( sAcVoltOverCnt[1] >= AcOverVolt.RecvCnt )
		{
			sAcVoltOverCnt[1] = AcOverVolt.RecvCnt;
			AlmFlag.bit.AcOverVolt = ALM_NORMAL;
		}
	}
}

//AC_UVP
void ALM_AcUnderVoltDetect(void)
{
	static uint16_t sAcVoltUnderCnt[2] = {0};
	
	if( AcVoltAvg <= AcUnderVolt.GeneVal )
	{
		sAcVoltUnderCnt[0]++;
		sAcVoltUnderCnt[1] = 0;
		if( sAcVoltUnderCnt[0] >= AcUnderVolt.GeneCnt )
		{
			sAcVoltUnderCnt[0] = AcUnderVolt.GeneCnt;
			AlmFlag.bit.AcUnderVolt = ALM_FAULT;
		}
	}
	
	if( AcVoltAvg >= AcUnderVolt.RecvVal )
	{
		sAcVoltUnderCnt[1]++;
		sAcVoltUnderCnt[0] = 0;
		if( sAcVoltUnderCnt[1] >= AcUnderVolt.RecvCnt)
		{
			sAcVoltUnderCnt[1] = AcUnderVolt.RecvVal;
			AlmFlag.bit.AcUnderVolt = ALM_NORMAL;
		}
	}
}

void ALM_BatOverVoltDetect(void)
{
	static uint16_t sBatOvpCnt[2] = {0};
	/*BAT_OVP*/
	if( OutVoltAvg >= BatOverVolt.GeneVal)
	{
		sBatOvpCnt[1] = 0;
		if(++sBatOvpCnt[0] >= BatOverVolt.GeneCnt)
		{
			sBatOvpCnt[0] = BatOverVolt.GeneCnt;
			AlmFlag.bit.BatOverVolt = ALM_FAULT;
		}
	}

	if( OutVoltAvg < BatOverVolt.RecvVal )
	{
		sBatOvpCnt[0] = 0;
		if( ++sBatOvpCnt[1] >= BatOverVolt.RecvCnt )
		{
			sBatOvpCnt[1] = BatOverVolt.RecvCnt;
			AlmFlag.bit.BatOverVolt = ALM_NORMAL;
		}
	}
}

//BAT_UVP
void ALM_BatUnderVoltDetect(void)
{
	static uint16_t sBatUvpCnt[2] = {0};
	
	if( OutVoltAvg <= BatUnderVolt.GeneVal)
	{
		sBatUvpCnt[1] = 0;
		if(++sBatUvpCnt[0] >= BatUnderVolt.GeneCnt)
		{
			sBatUvpCnt[0] = BatUnderVolt.GeneCnt;
			AlmFlag.bit.BatUnderVolt = ALM_FAULT;
		}
	}
	
	if( OutVoltAvg >= BatUnderVolt.RecvVal )
	{
		sBatUvpCnt[0] = 0;
		if(++sBatUvpCnt[1] >= BatUnderVolt.RecvCnt)
		{
			sBatUvpCnt[1] = BatUnderVolt.RecvCnt;
			AlmFlag.bit.BatUnderVolt = ALM_NORMAL;
		}
	}
}

//OVT
void ALM_OverTempDetect(void)
{
	static uint16_t sOverTempCnt[2] = {0};
	
	if( IcTempAvg >= InnerOverTemp.GeneVal )
	{
		sOverTempCnt[1] = 0;
		if(++sOverTempCnt[0] >= InnerOverTemp.GeneCnt)
		{
			sOverTempCnt[0] = InnerOverTemp.GeneCnt;
			AlmFlag.bit.TemperOver = ALM_FAULT;
		}
	}

	if( IcTempAvg < InnerOverTemp.RecvVal )
	{
		sOverTempCnt[0] = 0;
		if(++sOverTempCnt[1] >= InnerOverTemp.RecvCnt )
		{
			sOverTempCnt[1] = InnerOverTemp.RecvCnt;
			AlmFlag.bit.TemperOver = ALM_NORMAL;
		}
	}
}
//BAT_OVT
void ALM_BatOverTempDetect(void)
{
	static uint16_t sBatOVT_Cnt[2] = {0};
	
	if( BatTempAvg > BatOverTemp.GeneVal) 
	{
		sBatOVT_Cnt[0]++;
		sBatOVT_Cnt[1] = 0;
		if( sBatOVT_Cnt[0] >= BatOverTemp.GeneCnt)
		{
			sBatOVT_Cnt[0] = BatOverTemp.GeneCnt;
			AlmFlag.bit.BatTempOver = ALM_FAULT;
		}		
	}
	
	if( BatTempAvg < BatOverTemp.RecvVal)
	{
		sBatOVT_Cnt[1]++;
		sBatOVT_Cnt[0] = 0;
		if( sBatOVT_Cnt[1] >= BatOverTemp.RecvCnt)
		{
			sBatOVT_Cnt[1] = BatOverTemp.RecvCnt;
			AlmFlag.bit.BatTempOver = ALM_NORMAL;
		}		
	}
}

void ALM_Process(void) //5ms
{
	static uint16_t sAlmCnt1 = 0;
	static uint16_t sAlmCnt2 = 0;

	if( 0 == sAlmCnt1 )
	{
		//ALM_AcOverVoltDetect();
		//ALM_AcUnderVoltDetect();
		sAlmCnt1 = 1;
	}
	else
	{
		ALM_BatOverVoltDetect();
		//ALM_BatUnderVoltDetect();
		sAlmCnt1 = 0;
	}
	
	sAlmCnt2++;
	if( sAlmCnt2 >= 200 ) //1S
	{
		//ALM_OverTempDetect();
		ALM_BatOverTempDetect();
		sAlmCnt2 = 0;
	}
}

