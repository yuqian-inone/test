/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * This software/firmware and related documentation ("AutoChips Software") are
  * protected under relevant copyright laws. The information contained herein is
  * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
  * the prior written permission of AutoChips inc. and/or its licensors, any
  * reproduction, modification, use or disclosure of AutoChips Software, and
  * information contained herein, in whole or in part, shall be strictly
  * prohibited.
  *
  * AutoChips Inc. (C) 2021. All rights reserved.
  *
  * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
  * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
  * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
  * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
  * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
  * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
  * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
  * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
  * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
  * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
  * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
  * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
  * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
  * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
  * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
  * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
  * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
  * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
  *
  ******************************************************************************  
  */

/* --------------------------------- Includes ---------------------------------*/
#include "gpio.h"
#include "ac78xx_irq_cb.h"
/* USER CODE BEGIN 0 */
#include "ac78xx_i2c.h"
#include "bat.h"
#include "i2c.h"
#include "eflash.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
uint16_t GpioKeyState = GPIO_KEY_OFF;
uint16_t GpioKeyDetectEn = GPIO_FALSE;

//不显示，E,Q,O,F
uint8_t CH455EQVal[5] = {0,0x79,0x67,0x3F,0x71};
uint8_t BatCurveCode[BAT_CODE_NUM_MAX] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10};
/* USER CODE END 1 */

/** Configure pins as 
              * Analog 
              * Input 
              * Output
              * EVENT_OUT
              * EXTI
*/
void ATC_GPIO_Init(void)
{
    GPIO_SetFunc(KEY_Pin, GPIO_FUN0);
    GPIO_SetFunc(FAN_EN_Pin, GPIO_FUN0);
    GPIO_SetFunc(AC_RLY_EN_Pin, GPIO_FUN0);
    GPIO_SetFunc(DC_RLY_EN_Pin, GPIO_FUN0);
    GPIO_SetFunc(LED_R_Pin, GPIO_FUN0);
    GPIO_SetFunc(LED_G_Pin, GPIO_FUN0);
    GPIO_SetFunc(CH375B_INT_Pin, GPIO_FUN0);
    GPIO_SetDir(KEY_Pin, GPIO_IN);
    GPIO_SetDir(FAN_EN_Pin, GPIO_OUT);
    GPIO_SetDir(AC_RLY_EN_Pin, GPIO_OUT);
    GPIO_SetDir(DC_RLY_EN_Pin, GPIO_OUT);
    GPIO_SetDir(LED_R_Pin, GPIO_OUT);
    GPIO_SetDir(LED_G_Pin, GPIO_OUT);
    GPIO_SetDir(CH375B_INT_Pin, GPIO_IN);
    GPIO_SetPullupEnable(KEY_Pin, ENABLE);
    GPIO_SetPinValue(FAN_EN_Pin, GPIO_LEVEL_LOW);
    GPIO_SetPinValue(AC_RLY_EN_Pin, GPIO_LEVEL_LOW);
    GPIO_SetPinValue(DC_RLY_EN_Pin, GPIO_LEVEL_LOW);
    GPIO_SetPinValue(LED_R_Pin, GPIO_LEVEL_LOW);
    GPIO_SetPinValue(LED_G_Pin, GPIO_LEVEL_LOW);
    GPIO_SetE4E2(FAN_EN_Pin, GPIO_DRIVING_4MA);
    GPIO_SetE4E2(AC_RLY_EN_Pin, GPIO_DRIVING_4MA);
    GPIO_SetE4E2(DC_RLY_EN_Pin, GPIO_DRIVING_4MA);
    GPIO_SetE4E2(LED_R_Pin, GPIO_DRIVING_4MA);
    GPIO_SetE4E2(LED_G_Pin, GPIO_DRIVING_4MA);
    GPIO_SetEventCallback(CH375B_INT_Pin, EXTI0_IRQHandler_Callback);
    GPIO_EnableGPIOExtInt(CH375B_INT_Pin, EXTI_Trigger_Falling);

    /* EXTI interrupt init*/
    NVIC_SetPriority(EXTI0_IRQn, 0);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 2 */
void GPIO_Init(void)
{
	OPEN_AC_RLY;
	
	OPEN_DC_RLY;
	
	FAN_DISABLE;
	
	LED_R_ON;
	
	LED_G_ON;
}

//进入EQ模式闪烁字符
void GPIO_CH455EQMode(uint16_t mode)
{
	static uint8_t sEqModeCnt = 0;
	
	if( 0 == sEqModeCnt)
	{
        I2C_CH455Stop();
		sEqModeCnt = 1;
	}
	else
	{
		I2C_CH455Init();
		//udelay(10);
		
		if( CH455_EQ_DISPLAY == mode)
		{
		    I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &CH455EQVal[0], 1);//不显示
		    I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &CH455EQVal[1], 1);//E
			I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &CH455EQVal[2], 1);//Q		
			I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &CH455EQVal[0], 1);//不显示
		}
        else
		{	
			I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &CH455EQVal[0], 1);//不显示
			I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &CH455EQVal[3], 1);//O
			I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &CH455EQVal[4], 1);//F		
			I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &CH455EQVal[4], 1);//F	
		}	
		sEqModeCnt = 0;
	}
}

// VI曲线代码显示
void GPIO_CH455VICodeDisplay(uint16_t code)
{
	uint8_t temp = 0;
	uint8_t aNum[2];
	uint16_t BatVIcode;
	
	BatVIcode = code;
	
	I2C_CH455Init();
	//udelay(10);
	
	I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &BCD_decode_NumTab[0xB], 1);//B
	
	if( BatVIcode <= 9)
	{
		I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &BCD_decode_NumTab[0], 1);//o
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[BatVIcode], 1);		
	}
	else if( BatVIcode <= 99 )
	{
		aNum[0] = BatVIcode * 0.1;//十位
		aNum[1] = BatVIcode - (aNum[0] * 10); //个位
		I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &BCD_decode_NumTab[aNum[0]], 1);//o
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[aNum[1]], 1);	
	}
	else
	{
		
	}
	I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &temp, 1);//不显示	
}

void GPIO_CH455VICodeControl(uint16_t code,uint16_t state)
{
	static uint16_t sCh455VIModeFlag = 0;
	if( 0 == sCh455VIModeFlag)
	{
		GPIO_CH455VICodeDisplay(code);
		sCh455VIModeFlag = 1;
	}
	else
	{
		if( CH455_BCODE_HOLD == state)
		{
			GPIO_CH455VICodeDisplay(code);
		}
		else
		{
			I2C_CH455Stop();
		}
		sCh455VIModeFlag = 0;
	}
}
//按键判断状态
void GPIO_GetKeyState(void)//200ms main
{
	uint16_t EQmode;
    uint16_t gpio_idr;
	uint16_t gpio_timer;
	uint16_t VIcode;
	static uint16_t sGpioKeyCnt[3] = {0};
	static uint16_t sCodeNumUpFlag = 0;
	
	if( GPIO_FALSE == GpioKeyDetectEn)
	{
		return;
	}
	
	gpio_idr = GPIO_GetGPIOxIDR(GPIOA);
	
	if( KEY_SET_VI_IN == GpioKeyState)//一旦进入曲线设置模式，开始设置
	{
		sGpioKeyCnt[0] = 0;
		//判断是否设置完成
		if(GPIO_LOW == (gpio_idr & GPIO_PA0_DATA))//按键按下超过5S 
		{
			sGpioKeyCnt[1]++;
		}
		else
		{
			if( sGpioKeyCnt[1] <= 3)//600ms
			{
				
			}
			else if( sGpioKeyCnt[1] <= 10)//600ms-2S
			{
				sCodeNumUpFlag = 1;//曲线号递增
			}
			else if( sGpioKeyCnt[1] <= 20 )//2S-4S
			{
				//无动作
			}
			else //超过4S
			{
				sGpioKeyCnt[1]= 0;
				//EFLASH_WriteVICode(BatData.NowVICode);//保存在eflash中
				GpioKeyState = KEY_SET_VI_OUT;//设置完成 退出
				return;
			}
			sGpioKeyCnt[1]= 0;
		}
		//显示曲线代码
		VIcode = BatData.VICodeNum;//当前曲线代码号
		if( 1 == sCodeNumUpFlag)
		{
			VIcode++;
			if( VIcode >= BAT_CODE_NUM_MAX)
			{
				VIcode = 0;
			}
			
		}
		BatData.VICodeNum = VIcode;//更新当前曲线代码号
		GPIO_CH455VICodeControl(BatCurveCode[BatData.VICodeNum],CH455_BCODE_HOLD);//显示曲线代码
	}
	else if( KEY_SET_VI_OUT == GpioKeyState)
	{
		GPIO_CH455VICodeControl(BatCurveCode[BatData.VICodeNum],CH455_BCODE_FLASH);//显示设置好的曲线
		sGpioKeyCnt[1]++;
		if( sGpioKeyCnt[1] > 20)//闪烁4S后退出
		{
			sGpioKeyCnt[1] = 0;
			GpioKeyState = GPIO_KEY_OFF;//退出
		}

	}
	else if( KEY_EQ_MODE_IN == GpioKeyState)//一旦进入工装模式，检测退出
	{
		EQmode = CH455_EQ_DISPLAY;
		if( GPIO_LOW == (gpio_idr & GPIO_PA0_DATA))
		{
			sGpioKeyCnt[1]++;
			if( sGpioKeyCnt[1] >= 45)//10S
			{
				sGpioKeyCnt[1] = 45;
				GpioKeyState = KEY_EQ_MODE_OUT;//退出EQ模式
                EQmode = CH455_EQ_OFF_DISPLAY;	
			}
		}
		else
		{
			sGpioKeyCnt[1] = 0;
		}
		
		GPIO_CH455EQMode(EQmode);//字符闪烁
	}
	else if( KEY_EQ_MODE_OUT == GpioKeyState)
	{
		sGpioKeyCnt[0] = 0;
		sGpioKeyCnt[1] = 0;
		sGpioKeyCnt[2] = 0;
		GPIO_CH455EQMode(CH455_EQ_OFF_DISPLAY);//OFF 闪烁 断电重启
	}
	else 
	{	
		sGpioKeyCnt[1] = 0;
		sGpioKeyCnt[2] = 0;
		if( GPIO_LOW == (gpio_idr & GPIO_PA0_DATA))//按键被按下
		{
			sGpioKeyCnt[0]++;
			if( sGpioKeyCnt[0] > 45)//超过10S
			{
				GpioKeyState = KEY_EQ_MODE_IN;//面板按键按下,超过10S，进入EQ模式
			}
		}
		else
		{
			gpio_timer = sGpioKeyCnt[0];//读取
			sGpioKeyCnt[0] = 0;
			
			if( gpio_timer < 20)//小于5S 不动作
			{
				
			}
			else if( gpio_timer < 45)//5-10S
			{			
				GpioKeyState = KEY_SET_VI_IN;//切换曲线，数码管显示曲线代码
			}
			else
			{
			
			}
		}
	}	
	GPIO_DetectKeyDisable();
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
