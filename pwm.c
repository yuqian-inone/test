/**
  ******************************************************************************
  * File Name          : PWM.c
  * Description        : This file provides code for the configuration
  *                      of the PWM instances.
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
#include "ac78xx_pwm.h"
#include "pwm.h"

extern void Error_Handler(char *, int);

void MSP_PWM_Init(PWM_Type* PWMx);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* PWM2 init function */
void ATC_PWM2_Init(void)
{

    MSP_PWM_Init(PWM2);
    PWM_PWMInit(PWM2, PWM_PWMMODE_EDGEALLIGNED, PWM_PWM_HIGHTRUEPULSE);
    PWM_SetModValue(PWM2, 479);
    PWM_SetCntTOFNum(PWM2, 0);
    PWM_SetChannelValue(PWM2, PWM_CHANNEL_CHANNEL1, 0);
    PWM_SetChannelValue(PWM2, PWM_CHANNEL_CHANNEL2, 0);
    PWM_OutputPolaritySet(PWM2, PWM_CHANNEL_CHANNEL1, PWM_POLARITY_HIGH);
    PWM_OutputPolaritySet(PWM2, PWM_CHANNEL_CHANNEL2, PWM_POLARITY_HIGH);
    PWM_ClockSet(PWM2, PWM_CLOCK_SYSTEMCLOCK, 0);

}

void MSP_PWM_Init(PWM_Type* PWMx)
{

    if (PWMx == PWM2)
    {
    /* USER CODE BEGIN MSP_PWM2_Init 0 */

    /* USER CODE END MSP_PWM2_Init 0 */
    
    /**
    GPIO Configuration for PWM2    
    PB10     --> PWM2_CH1
    PB11     --> PWM2_CH2 
    */
    GPIO_SetFunc(GPIO_PB10, GPIO_FUN2);

    GPIO_SetFunc(GPIO_PB11, GPIO_FUN2);

    /* USER CODE BEGIN MSP_PWM2_Init 1 */

    /* USER CODE END MSP_PWM2_Init 1 */
    }
}

/* USER CODE BEGIN 1 */
void PWM_SetVoltRefDuty(uint16_t duty)
{
	 PWM_SetChannelValue(PWM2, PWM_CHANNEL_CHANNEL2, duty); 
}

void PWM_SetCurrRefDuty(uint16_t duty)
{
	PWM_SetChannelValue(PWM2, PWM_CHANNEL_CHANNEL1, duty); 
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
