#include <stdint.h>
//#include "main.h"
//#include "ac78xx_irq_cb.h"
#include "adc.h"
#include "can.h"
//#include "i2c.h"
#include "pwm.h"
#include "system.h"
#include "gpio.h"
//#include "ac78xx_adc.h"
#include "fsm.h"
#include "alarm.h"


uint16_t FsmState = FSM_INIT;


void FSM_Init(void)
{
	FsmState = FSM_INIT;
}


void FSM_Process(void)
{
    static uint16_t sFsmCounter = 0;
	static uint16_t sFsmFaultCnt = 0;
//	static uint16_t sFsmTest = 0;
	    
	switch(FsmState)
	{
        case FSM_INIT:
        	PWM_SetVoltRefDuty(0);
		    PWM_SetCurrRefDuty(0);
	        sFsmCounter++;
	    	if(sFsmCounter >= FSM_1_SECONDS)
	        {
	    	    sFsmCounter = 0;
	        	FsmState = FSM_IDLE;

	    	}
	        break;
	        
        case FSM_IDLE:
			if( ALM_NORMAL == AlmFlag.bit.AcUnderVolt)
			{
				CLOSE_AC_RLY;
				sFsmCounter = 0;
        	    FsmState = FSM_START;
			}
			else
			{
				OPEN_AC_RLY;
			}
        	break;
        	
        case FSM_START:
             if( BMS_ON_CHARGER == CanOnSys)
        	{
        		if(ALM_NORMAL == ALM_GetCurrentAllAlmFlag())
        		{
					sFsmCounter++;
					if( sFsmCounter >= FSM_2_SECONDS)
					{
						sFsmCounter = 0;
        	            CLOSE_DC_RLY;//延迟吸合避免压差过大吸合带来的冲击电流
        	            FsmState = FSM_RUN;
					}
        		}
        		else
        		{
        			sFsmCounter = 0;
					sFsmFaultCnt = 0;
        			FsmState = FSM_FAULT;
        		}
        	}

        	break;	
        	
        case FSM_RUN:
    		if(ALM_NORMAL == ALM_GetCurrentAllAlmFlag())
    		{
				//PWM_SetVoltRefDuty(0);
		        //PWM_SetCurrRefDuty(0);
				SYS_SetVoltLoopRefDuty((int32_t)29<<10);//test
				SYS_SetCurrLoopRefDuty((int32_t)30<<10);//test
				FAN_ENABLE;//test
    		}
    		else
    		{
				sFsmCounter = 0;
				sFsmFaultCnt = 0;
    			FsmState = FSM_FAULT;
    		}
    		
    	    if(BMS_OFF_CHARGER == CanOnSys )
    	    {
				sFsmCounter = 0;
				sFsmFaultCnt = 0;
    	    	FsmState = FSM_FAULT;
    	    }
	        break;
	        
        case FSM_FAULT:
            PWM_SetVoltRefDuty(0);
		    PWM_SetCurrRefDuty(0);
        	
        	if (ALM_FAULT == AlmFlag.bit.TemperOver)
        	{
				FAN_ENABLE;
			}
			else
			{
				FAN_DISABLE;
			}
			sFsmFaultCnt++;
			if( sFsmFaultCnt >= FSM_1_SECONDS)
			{
				if( (ALM_FAULT == AlmFlag.bit.BatUnderVolt) && (OutCurrAvg < 1000))//输出电流小于1A
				{
					OPEN_DC_RLY;					
				}
				
				if( (ALM_FAULT == AlmFlag.bit.AcUnderVolt) && (OutCurrAvg < 1000))//输出电流小于1A
				{
					OPEN_AC_RLY;
				}
				sFsmFaultCnt = FSM_1_SECONDS;
		    }
        	if(ALM_NORMAL == ALM_GetCurrentAllAlmFlag())
        	{
        		sFsmCounter++;
            	if( sFsmCounter >= FSM_2_SECONDS)
            	{
					sFsmFaultCnt = 0;
            		sFsmCounter = 0;
					SYS_Init();
            		FsmState = FSM_IDLE;
            	}
        	}      	
	        break;

        default:
	        break;
	}	
}

