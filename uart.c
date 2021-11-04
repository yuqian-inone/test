/**
  ******************************************************************************
  * File Name          : UART.c
  * Description        : This file provides code for the configuration
  *                      of the UART instances.
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
#include "ac78xx_uart.h"
#include "uart.h"

extern void Error_Handler(char *, int);

void MSP_UART_Init(UART_Type* UARTx);

/* USER CODE BEGIN 0 */
uint8_t UartSendEn = UART_FALSE;
/* USER CODE END 0 */

/* UART4 init function */
void ATC_UART4_Init(void)
{
    UART_SettingType uartConfig;
    memset(&uartConfig, 0, sizeof(uartConfig));

    UART_SetEventCallback(UART4, UART4_IRQHandler_Callback);
    MSP_UART_Init(UART4);
    uartConfig.baudrate = 9600;
    uartConfig.dataBits = BIT8;
    uartConfig.stopBits = STOP_1BIT;
    uartConfig.parity = NOPARITY;
    uartConfig.fifoByte = DISABLE;
    uartConfig.dmaEn = UART_DMA_TXRX_NONE;
    uartConfig.rateStep = UART_SMP_CNT1;
    UART_Init(UART4, &uartConfig);
    UART_SetRxIntEn(UART4, ENABLE);

}

void MSP_UART_Init(UART_Type* UARTx)
{

    if (UARTx == UART4)
    {
    /* USER CODE BEGIN MSP_UART4_Init 0 */

    /* USER CODE END MSP_UART4_Init 0 */
    
    /**
    GPIO Configuration for UART4    
    PC14     --> UART4_RX
    PC15     --> UART4_TX 
    */
    GPIO_SetFunc(GPIO_PC14, GPIO_FUN2);

    GPIO_SetFunc(GPIO_PC15, GPIO_FUN2);

    /* UART4 interrupt Init */
    NVIC_SetPriority(UART4_IRQn, 3);
    NVIC_EnableIRQ(UART4_IRQn);

    /* USER CODE BEGIN MSP_UART4_Init 1 */

    /* USER CODE END MSP_UART4_Init 1 */
    }
}

/* USER CODE BEGIN 1 */
void UART_SendOneByte(void)
{
	static uint8_t stest = 0;
	if( UART_FALSE == UartSendEn)
	{
		return;
	}
	if( 0 == stest)
	{
	    UART_SendData(UART4, 0x16);
		stest = 1;
	}
	else
	{
		//UART_SendData(UART4, 0x04);
		stest = 0;
	}
	
	Uart_SendDisable();
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
