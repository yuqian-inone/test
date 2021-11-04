
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* --------------------------------- Includes -------------------------------*/
#include "main.h"
#include "ac78xx_irq_cb.h"
#include "adc.h"
#include "can.h"
#include "i2c.h"
#include "pwm.h"
#include "timer.h"
#include "uart.h"
#include "wdg.h"
#include "gpio.h"
/* USER CODE BEGIN Includes */
#include "ac78xx_adc.h"
#include "ac78xx_wdg.h"   
#include "pwm.h"
#include "alarm.h"
#include "fsm.h"
#include "system.h"
#include "can.h"
#include "uart.h"
#include "eflash.h"
/* USER CODE END Includes */

/* --------------------------------- Typedefs -------------------------------*/
/* USER CODE BEGIN Typedefs */

/* USER CODE END Typedefs */

/* --------------------------------- Defines --------------------------------*/
/* USER CODE BEGIN Defines */

/* USER CODE END Defines */

/* --------------------------------- Macros ---------------------------------*/
/* USER CODE BEGIN Macros */

/* USER CODE END Macros */

/* --------------------------------- Variables ------------------------------*/

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* --------------------------------- Function Prototypes --------------------*/
void SystemClock_Config(void);
void MSP_Init(void);

/* USER CODE BEGIN Function_Prototypes */
uint16_t uart_rd = 0;
/* USER CODE END Function_Prototypes */

/* USER CODE BEGIN 0 */
extern void I2C_CH455InitDisplay(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
 
    /* USER CODE END 1 */

    /* MCU Configuration------------------------------------------------------*/

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    ATC_GPIO_Init();
    ATC_I2C2_Init();
    ATC_ADC_Init();
    ATC_TIMER0_Init();
	
    ATC_PWM2_Init();
	PWM_SetVoltRefDuty(0);
    PWM_SetCurrRefDuty(0);
	
    ATC_WDG_Init();
    ATC_CAN1_Init();
    ATC_UART4_Init();

    /* USER CODE BEGIN 2 */
	InitDelay();
	
	EFLASH_Init();
	
	ADC_SoftwareStartRegularConvCmd(ADC,ADC_ENABLE);//ADC规则采样软件触发
	ADC_SoftwareStartInjectConvCmd(ADC,ADC_ENABLE);//ADC注入采样触发
	
	GPIO_Init();
		
	ALM_Init();
	
	SYS_Init();
	
	FSM_Init();
	
	I2C_CH455Init();
	
    /* USER CODE END 2 */

    /* Infinite loop */
    while(1)
    {

        /* USER CODE BEGIN WHILE */
		WDOG_Feed();
			
		UART_SendOneByte();
			
		ADC_CalcTempAvg();
		
		I2C_CH455DataShowProcess();
		
		CAN_SendData();
		
		GPIO_GetKeyState();
		
        /* USER CODE END WHILE */

    }

    /* USER CODE BEGIN 3 */

    /* USER CODE END 3 */
}

/**
    * @brief System Clock Configuration
    * @retval None
    */
void SystemClock_Config(void)
{
    SPM_EnablePLL(1);
    CKGEN_SetPLLReference(PLL_REF_INTERAL_OSC);
    CKGEN_SetPllPrevDiv(PLL_PREDIV_1);
    CKGEN_SetPllFeedbackDiv(96);
    CKGEN_SetPllPostDiv(PLL_POSDIV_4);
    CKGEN_SetSysclkDiv(SYSCLK_DIVIDER_1);
    CKGEN_SetSysclkSrc(SYSCLK_SRC_PLL_OUTPUT);
    CKGEN_SetAPBClockDivider(APBCLK_DIVIDER_2);
    CKGEN_SetSFlashClock(SFLASH_CLK_SEL_APB, SFLASH_DIVIDER_2);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to handle the driver error */
    while(1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
