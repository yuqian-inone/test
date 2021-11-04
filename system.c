#include <stdint.h>
//#include "main.h"
//#include "ac78xx_irq_cb.h"
#include "adc.h"
#include "can.h"
//#include "i2c.h"
#include "pwm.h"
//#include "timer.h"
//#include "gpio.h"
//#include "ac78xx_adc.h"
//#include "fsm.h"
//#include "alarm.h"
#include "system.h"


int32_t iq10VoltRefVal = 0;

int32_t iq10CurrRefVal = 0;
int32_t iq10MaxCurrRef = 0;
int32_t iq5TempDropPwr = SYS_RATE_PWR;
uint8_t TempDropPwrFlag = DROP_NO_TRIGGER;
uint16_t SysSOCVal = 0;

void SYS_Init(void)
{
	iq10VoltRefVal = 0;
	iq10CurrRefVal = 0;
	iq10MaxCurrRef = 0;
	
	TempDropPwrFlag = DROP_NO_TRIGGER;
	iq5TempDropPwr = SYS_RATE_PWR;;
	
	SysSOCVal = 0;
}

//电压基准占空比计算
void SYS_SetVoltLoopRefDuty(int32_t iq10Volt)//提供给铅酸电池控制逻辑的接口
{
	uint16_t Vduty;
    if (iq10Volt >= IQ10_VOLT_MAX_REF)
    {
    	iq10Volt = IQ10_VOLT_MAX_REF;
    }
	else if( iq10Volt <= 0)
	{
		iq10Volt = 0;
	}
	else
	{
		
	}
	if( iq10VoltRefVal <= iq10Volt)
	{
		iq10VoltRefVal += 100;
	}
	else
	{
		iq10VoltRefVal -= 100;
	}
	IW_UPDNLMT(iq10VoltRefVal,0,IQ10_VOLT_MAX_REF);
	Vduty = (iq10VoltRefVal * 3)>>10;//系数3 100K频率值480 D=10%占空比 数字47
	IW_UPDNLMT(Vduty,0,VOLT_DUTY_MAX);
	PWM_SetVoltRefDuty(Vduty);
}
//电流基准占空比计算
void SYS_SetCurrLoopRefDuty(int32_t iq10Curr)
{
	uint16_t Iduty;
	if( iq10Curr >= iq10MaxCurrRef )
    {
    	iq10Curr = iq10MaxCurrRef;
    }
	else if( iq10Curr <= 0)
	{
		iq10Curr = 0;
	}
	
	if( iq10CurrRefVal <= iq10Curr )
	{
		iq10CurrRefVal += 20;//50ms  0.2A
	}
	else
	{
		iq10CurrRefVal -= 20;
	}
	IW_UPDNLMT(iq10CurrRefVal,0,iq10MaxCurrRef);
	Iduty = (uint16_t)((iq10CurrRefVal * 2893)>>18);//系数11.3 100K频率值480 D=10%占空比 数字47
	IW_UPDNLMT(Iduty,0,CURR_DUTY_MAX);
	PWM_SetCurrRefDuty(Iduty);
}

//温度降额
void SYS_HighTempDropCurr(void) //2S运行
{
	static uint16_t sTempDropCnt1 = 0;
	static uint16_t sTempDropCnt2 = 0;

	if( IcTempAvg < DROP_RVC_TEMP)
	{
		sTempDropCnt1 = 0;
		sTempDropCnt2 = 0;
		TempDropPwrFlag = DROP_NO_TRIGGER;
		iq5TempDropPwr = SYS_RATE_PWR;
	}

	else if(IcTempAvg < DROP_BASE_TEMP) // 73 - 83
	{
		sTempDropCnt1 = 0;
		sTempDropCnt2 = 0;
		if( DROP_NO_TRIGGER == TempDropPwrFlag)
		{
			iq5TempDropPwr = SYS_RATE_PWR;
		}
		else
		{
			return;
		}
	}
	else if ( IcTempAvg < DROP1_TEMP)   //83~88
	{		
		if( DROP_NO_TRIGGER == TempDropPwrFlag)
		{
			sTempDropCnt1++;
			sTempDropCnt2 = 0;
			if( sTempDropCnt1 >= 5)
			{
				TempDropPwrFlag = DROP_TRIGGER;
				iq5TempDropPwr = TEMP_DROP1_PWR;
				sTempDropCnt1 = 5;
			}
		}
		else
		{
			return;		
		}
	}
	else if( IcTempAvg < DROP2_TEMP) // 88~93
	{
		sTempDropCnt2++;
		sTempDropCnt1 = 0;
		if( sTempDropCnt2 >= 5)
		{
			TempDropPwrFlag = DROP_TRIGGER;
			iq5TempDropPwr = TEMP_DROP2_PWR;
			sTempDropCnt2 = 5;
		}
	}
	else  // > 93
	{
        return;
	}
}

void SYS_SetSysDropCurr(void)//100ms 运行
{
	int32_t iq5Power;
	int32_t iq10Curr;
	int32_t iq5AcDropPwr;
	
	//输入电压降额
	if( AcVoltAvg <= ((uint32_t)150*1000))//110V 电网
	{
	    iq5AcDropPwr = AC220V_PWR;
	}
	else
	{
		iq5AcDropPwr = AC220V_PWR;
	}

	iq5Power = IW_MIN_TWO(iq5AcDropPwr,iq5TempDropPwr);
	if( 0 != iq5Power )
	{
		iq10Curr = (iq5Power << 10) / (OutVoltAvg>>5);
	}
	else
	{
		iq10Curr = 2048;//无意义值，任意给
	}
	iq10Curr = IW_MIN_TWO(iq10Curr,iq10CanSetCurr);
	IW_UPDNLMT(iq10Curr,0,IQ10_CURR_MAX_REF);
	iq10MaxCurrRef = iq10Curr;
}

//SOC估算
void SYS_CalcChargeSOC(void)//2S计算一次
{
	uint16_t tempsoc;
	//判断电池类型 
	
	//以下是24V铅酸电池SOC预估，开路电压
	if( SysSOCVal >= 80)
	{
		return; //超过80%的SOC计算方式依据充电曲线
	}
	
	if( OutVoltAvg < PB_CHG_EMPTY_VOLT)
	{
		SysSOCVal = 10;
	}
	tempsoc = ((OutVoltAvg - PB_CHG_EMPTY_VOLT)*2133)>>16;
	if( tempsoc < SysSOCVal)
	{
		return;
	}
	else
	{
		if( tempsoc < 80)
		{
			SysSOCVal = tempsoc;
		}
	}
}







