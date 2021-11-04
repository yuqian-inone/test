/**
  ******************************************************************************
  * File Name          : TIMER.c
  * Description        : This file provides code for the configuration
  *                      of the TIMER instances.
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
#include "ac78xx.h"
#include "ac78xx_irq_cb.h"
#include "ac78xx_timer.h"
#include "timer.h"

extern void Error_Handler(char *, int);

void MSP_TIMER_Init(TIMER_Type* TIMERx);

/* USER CODE BEGIN 0 */
uint16_t Int5msTimer = 0;
uint16_t Int10msTimer = 0;
uint16_t Int100msTimer = 0;
uint16_t Int200msTimer = 0;
uint16_t Int2000msTimer = 0;//上电立即处理
/* USER CODE END 0 */

/* TIMER0 init function */
void ATC_TIMER0_Init(void)
{
    TIMER_ConfigType timerConfig;
    memset(&timerConfig, 0, sizeof(timerConfig));

    TIMER_SetCallback(TIMER0, TIMER0_IRQHandler_Callback);
    MSP_TIMER_Init(TIMER0);
    timerConfig.timerEn = ENABLE;
    timerConfig.interruptEn = ENABLE;
    timerConfig.linkMode = DISABLE;
    timerConfig.loadValue = 4799;
    TIMER_Init(TIMER0, &timerConfig);

}

void MSP_TIMER_Init(TIMER_Type* TIMERx)
{

    if (TIMERx == TIMER0)
    {
    /* USER CODE BEGIN MSP_TIMER0_Init 0 */

    /* USER CODE END MSP_TIMER0_Init 0 */

    /* TIMER0 interrupt Init */
    NVIC_SetPriority(TIMER0_IRQn, 0);
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* USER CODE BEGIN MSP_TIMER0_Init 1 */

    /* USER CODE END MSP_TIMER0_Init 1 */
    }
}

/* USER CODE BEGIN 1 */
void TIMER0_Interrupt(void)//100us
{
	ADC_CalcAvg();
	
	Int5msTimer++;
	if( Int5msTimer >= 47)
	{
		ALM_Process();
		
		FSM_Process();
		
		Int5msTimer = 0;

	}
	
	Int10msTimer++;
	if( Int10msTimer >= 97)
	{
		ADC_ReadInjectSampleVal();
			
		Int10msTimer = 0;
	}
	
	Int200msTimer++;
	if( Int200msTimer >= 1997)
	{
	    GPIO_DetectKeyEnable();
		Int200msTimer = 0;
	}

	Int100msTimer++;
	if( Int100msTimer >= 997)
	{
		ADC_CalcTempEnable();
		
		SYS_SetSysDropCurr();
			
		Uart_SendEnable();
		Int100msTimer = 0;
	}
	
	Int2000msTimer++;
	if( Int2000msTimer >= 19997)
	{
		I2C_EnableTxData();
		
		SYS_HighTempDropCurr();
		
		SYS_CalcChargeSOC();
		
		CAN_SendEnable();
		
		//Uart_SendEnable();

		Int2000msTimer = 0;
	}
	
	WDOG_Feed();
	
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
