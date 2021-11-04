/**
  ******************************************************************************
  * @file    ac78xx_irq_cb.c
  * @brief   IRQ callback.
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
#include "ac78xx_timer.h"
#include "ac78xx_can.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* --------------------------------- Typedefs ----------------------------------*/
/* USER CODE BEGIN Typedefs */

/* USER CODE END Typedefs */

/* --------------------------------- Defines -----------------------------------*/
/* USER CODE BEGIN Defines */
 
/* USER CODE END Defines */

/* --------------------------------- Macros ------------------------------------*/
/* USER CODE BEGIN Macros */

/* USER CODE END Macros */

/* --------------------------------- Variables ---------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t UartRcvdata = 0;
/* USER CODE END Variables */

/* --------------------------------- Function Prototypes -----------------------*/
/* USER CODE BEGIN Function_Prototypes */
extern void TIMER0_Interrupt(void);
extern void CAN1_IntRecvMsg(void);
/* USER CODE END Function_Prototypes */

/* --------------------------------- User Code ---------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/******************************************************************************/
/* ac78xx Peripheral Interrupt Handlers callbacks                             */
/******************************************************************************/

/**
* @brief This function handle UART4 interrupt.
* @return 0: success, other: error value
*/
int32_t UART4_IRQHandler_Callback(uint8_t port, uint32_t lsr0, uint32_t lsr1)
{
    if ((UART4->UARTn_IER.ERXNE == 1) && (lsr0 & LSR0_DR))
    {
        /* USER CODE BEGIN UART4 RECEIVE DATA */
		UartRcvdata = UART_ReceiveData(UART4);

        /* USER CODE END UART4 RECEIVE DATA */
    }
    if ((UART4->UARTn_IER.ETXE == 1) && (lsr0 & LSR0_THRE))
    {
        /* USER CODE BEGIN UART4 SEND DATA */

        /* USER CODE END UART4 SEND DATA */
    }

    /* USER CODE BEGIN UART4_IRQHandler_Callback*/

    /* USER CODE END UART4_IRQHandler_Callback*/
    return 0;
}

/**
* @brief This function handle EXTI Line0 interrupt.
*/
void EXTI0_IRQHandler_Callback(uint32_t gpioNum)
{

    /* USER CODE BEGIN EXTI0_IRQHandler_Callback*/

    /* USER CODE END EXTI0_IRQHandler_Callback*/
}

/**
* @brief This function handle DMA Channel1 interrupt.
* @param[in] DMAx: DMA type pointer,x can be 1 to 12
* @param[in] wparam: DMA channel's status(Finish, Half Finish and Error Flags)
* @param[in] lparam: reserved
* @return 0: success, other: error value
*/
int32_t DMA1_IRQHandler_Callback(DMA_Type *DMAx, uint32_t wparam, uint32_t lparam)
{
    if ((wparam & 0x01) == 0x01)
    {
        /* USER CODE BEGIN DMA1 FINISH */

        /* USER CODE END DMA1 FINISH */
    }

    if ((wparam & 0x02) == 0x02)
    {
        /* USER CODE BEGIN HALF DMA1 FINISH */

        /* USER CODE END HALF DMA1 FINISH */
    }

    if ((wparam & 0x04) == 0x04)
    {
        /* USER CODE BEGIN HALF DMA1 TRANSFER ERROR */

        /* USER CODE END HALF DMA1 TRANSFER ERROR */
    }

    /* USER CODE BEGIN DMA1_IRQHandler_Callback*/

    /* USER CODE END DMA1_IRQHandler_Callback*/
    return 0;
}

/**
* @brief This function handle Timer 0 interrupt.
* @param[in] arg: timer callback argument
* @return none
*/
void TIMER0_IRQHandler_Callback(uint8_t arg)
{
    TIMER_ClrIntFlag(TIMER_GetTimerCtrl(arg));

    /* USER CODE BEGIN TIMER0_IRQHandler_Callback*/
    TIMER0_Interrupt();
    /* USER CODE END TIMER0_IRQHandler_Callback*/
}

/**
* @brief This function handle CAN 1 interrupt.
* @param[in] event: can event
* @param[in] wparam: reserved
* @param[in] lparam: reserved
* @return 0: success, other: error value
*/
int32_t CAN1_IRQHandler_Callback(uint32_t event, uint32_t wparam, uint32_t lparam)
{
    if (event & CAN_EVENT_RECVMSG)
    {
        /* USER CODE BEGIN CAN1 EVENT RECEIVE MESSAGE */
        CAN1_IntRecvMsg();
        /* USER CODE END CAN1 EVENT RECEIVE MESSAGE */
    }

    /* USER CODE BEGIN CAN1_IRQHandler_Callback*/

    /* USER CODE END CAN1_IRQHandler_Callback*/
    return 0;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
