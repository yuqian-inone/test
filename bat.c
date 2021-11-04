#include <stdint.h>
//#include "main.h"
//#include "ac78xx_irq_cb.h"
//#include "adc.h"
//#include "can.h"
//#include "i2c.h"
//#include "pwm.h"
//#include "timer.h"
//#include "gpio.h"
//#include "ac78xx_adc.h"
//#include "fsm.h"
//#include "alarm.h"
#include "bat.h"

BAT_PARA_STR BatData;

void BAT_Init(void)
{
	BatData.type = BAT_TYPE_PB;
	BatData.logo = 0;
	BatData.ah = 300;
	BatData.VICodeNum = 0;//B01
	BatData.rateV = 24;//24V
}




