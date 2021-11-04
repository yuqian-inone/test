/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
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
#include "ac78xx_can.h"
#include "can.h"

extern void Error_Handler(char *, int);

void MSP_CAN_Init(CAN_Type* CANx);

/* USER CODE BEGIN 0 */
uint8_t CanOnSys = BMS_ON_CHARGER;//test
uint8_t CanSendEnable = CAN_FALSE;
uint32_t iq10CanSetCurr = (uint32_t)40<<10;
uint32_t iq10CanSetVolt = (uint32_t)30<<10;

/* USER CODE END 0 */

/* CAN1 init function */
void ATC_CAN1_Init(void)
{
    CAN_Config canConfig;
    memset(&canConfig, 0, sizeof(canConfig));
    CAN_BaudrateConfig baudrate;
    memset(&baudrate, 0, sizeof(baudrate));

    CAN_SetEventCallBack(CAN1, CAN1_IRQHandler_Callback);
    MSP_CAN_Init(CAN1);
    canConfig.canMode = CAN_MODE_NORMAL;
    canConfig.clockSrc = CAN_CLKSRC_AHB;
    canConfig.interruptEnable = ENABLE;
    canConfig.autoReset = DISABLE;
    canConfig.TPSS = ENABLE;
    canConfig.TSSS = ENABLE;
    canConfig.TSMODE = 0;
    canConfig.ROM = 0;
    baudrate.S_PRESC = 7;
    baudrate.S_SEG_1 = 16;
    baudrate.S_SEG_2 = 5;
    baudrate.S_SJW = 3;

    CAN_Initialize(CAN1, &canConfig, &baudrate);
    CAN_Standby(CAN1, DISABLE);

}

void MSP_CAN_Init(CAN_Type* CANx)
{

    if (CANx == CAN1)
    {
    /* USER CODE BEGIN MSP_CAN1_Init 0 */

    /* USER CODE END MSP_CAN1_Init 0 */
    
    /**
    GPIO Configuration for CAN1    
    PA4     --> CAN1_RX
    PA5     --> CAN1_TX 
    */
    GPIO_SetFunc(GPIO_PA4, GPIO_FUN2);

    GPIO_SetFunc(GPIO_PA5, GPIO_FUN2);

    /* CAN1 interrupt Init */
    NVIC_SetPriority(CAN1_IRQn, 1);
    NVIC_EnableIRQ(CAN1_IRQn);

    /* USER CODE BEGIN MSP_CAN1_Init 1 */

    /* USER CODE END MSP_CAN1_Init 1 */
    }
}

/* USER CODE BEGIN 1 */
uint8_t CanSendOneMessage(pCAN_MSG_T p_send)
{	
	uint16_t wait_cnt=0;
	#if 1
	while(1 == CAN1->BIT.TPE)
	{
		wait_cnt++;
		if(wait_cnt > 1500)
		{
			return 0;
		}
	}
	#endif
	CAN1->BIT.TBSEL = 0;
	CAN1->TBUF.ID = p_send->strId.b29ID;
	CAN1->TBUF.DLC = p_send->u8Len;
	CAN1->TBUF.RTR = p_send->strId.b1RTR;
	CAN1->TBUF.IDE = p_send->strId.b1IDE;
	
	CAN1->TBUF.Data[0] = (uint32_t)p_send->u8Datas[0] | ((uint32_t)p_send->u8Datas[1]<<8) | 
							((uint32_t)p_send->u8Datas[2]<<16) | ((uint32_t)p_send->u8Datas[3]<<24);
	CAN1->TBUF.Data[1] = (uint32_t)p_send->u8Datas[4] | ((uint32_t)p_send->u8Datas[5]<<8) | 
							((uint32_t)p_send->u8Datas[6]<<16) | ((uint32_t)p_send->u8Datas[7]<<24);
	
	
	if(CAN1->BIT.STBY)
	{
		return 0;
	}
	CAN1->BIT.TPE = 1;
	return 1;
}
extern uint16_t UartRcvdata;
void CAN_SendData(void)//100ms
{
    uint16_t temp; 
	CAN_MSG_T SendMsg;
	
	if( CAN_FALSE == CanSendEnable)
	{
		return;
	}

	SendMsg.strId.b29ID = 0x111;//ID
#if 1
	//0表示标准帧，1表示扩展帧
	SendMsg.strId.b1IDE = 0;//标准帧
	SendMsg.u8Len = 8;//数据长度
	SendMsg.strId.b1RTR = 0;//数据帧
	
    temp = (uint16_t)(AcVoltAvg >> 3);
	SendMsg.u8Datas[0] = temp;//低8位
	SendMsg.u8Datas[1] = temp>>8;//高8位
	
	temp = (uint16_t)((OutVoltAvg * 10)>>10);
	SendMsg.u8Datas[2] = temp;
	SendMsg.u8Datas[3] = temp>>8;
	
	temp = (uint16_t)((OutCurrAvg * 10)>>10);
	SendMsg.u8Datas[4] = temp;
	SendMsg.u8Datas[5] = temp>>8;
	
	SendMsg.u8Datas[6] = UartRcvdata;
	SendMsg.u8Datas[7] = IcTempAvg;
#endif	
	//CAN_MessageSend(CAN1, &SendMsg, TRANSMIT_PRIMARY);
	CanSendOneMessage(&SendMsg);

	CAN_SendDisable();
}

void CAN1_IntRecvMsg(void)//中断
{
    CAN_MSG_INFO RecvMsg;
	CAN_MessageRead(CAN1,&RecvMsg);
	  
	if( 1 == RecvMsg.IDE)//扩展帧
	{

	}
	
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
