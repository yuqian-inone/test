/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "ac78xx_adc.h"
#include "adc.h"
#include "ac78xx_dma.h"

uint16_t adcDmaBuffer[4] = {0};

extern void Error_Handler(char *, int);

void MSP_ADC_Init(ADC_Type* ADCx);

/* USER CODE BEGIN 0 */
extern void ADC_GetIcTemp25Ref(void);
//电网电压、输出电压、输出电流采样，采样周期100us
uint16_t AcVoltAvgCnt = 0;
uint32_t AcVoltAvg = 0;
uint32_t AcVoltAvgSum = 0;

uint16_t OutVoltAvgCnt = 1;
uint32_t OutVoltAvg = 0;
uint32_t OutVoltAvgSum = 0;

uint16_t OutVoltAvgFltCnt = 0;
uint16_t OutVoltAvgFlt = 0;//用于显示
uint32_t OutVoltAvgFltSum = 0;

uint16_t OutCurrAvgCnt = 2;
uint32_t OutCurrAvg = 0;
uint32_t OutCurrAvgSum = 0;

uint16_t OutCurrAvgFltCnt = 16;
uint16_t OutCurrAvgFlt = 0;//用于显示
uint32_t OutCurrAvgFltSum = 0;

/*温度采样 100ms 采样周期*/
uint16_t BatTempAvgCnt = 3;
int16_t BatTempAvg = 0;
uint32_t BatTempAvgSum = 0;

uint16_t IcTempAvgCnt = 0;
int16_t IcTempAvg = 0;
uint32_t IcTempAvgSum = 0;
int16_t IcTempRef = 0;

uint16_t adcInjectValue[3]={0};
uint16_t CalcTempEnable = 0;


/* USER CODE END 0 */

/* ADC init function */
void ATC_ADC_Init(void)
{
    ADC_InitType adcConfig;
    memset(&adcConfig, 0, sizeof(adcConfig));
    ADC_TrigSourceType adcTrigSourceConfig;
    memset(&adcTrigSourceConfig, 0, sizeof(adcTrigSourceConfig));

    MSP_ADC_Init(ADC);
    adcConfig.dataAlign = ADC_DataAlign_Right;
    adcConfig.scanMode = ENABLE;
    adcConfig.continousMode = ENABLE;
    adcConfig.disContinousModeOnRegularGroup = DISABLE;
    adcConfig.disContinousModeOnInjectGroup = DISABLE;
    adcConfig.injectAutoMode = DISABLE;
    adcConfig.discNum = 0;
    ADC_Init(ADC, &adcConfig);
    ADC_SetClockPrescaler(ADC, 9);
    adcTrigSourceConfig.regularTrigSource = ADC_TrigSource_Internal;
    adcTrigSourceConfig.injectTrigSource = ADC_TrigSource_Internal;
    ADC_TrigSourceInit(ADC, &adcTrigSourceConfig);
    ADC_SetRegularGroupLength(ADC, 3);
    ADC_SetRegularGroupSequence(ADC, 1, ADC_CHANNEL_AD4);
    ADC_SetRegularGroupSequence(ADC, 2, ADC_CHANNEL_AD5);
    ADC_SetRegularGroupSequence(ADC, 3, ADC_CHANNEL_AD6);
    ADC_SetInjectGroupLength(ADC, 2);
    ADC_SetInjectGroupSequence(ADC, 1, ADC_CHANNEL_AD7);
    ADC_SetInjectGroupSequence(ADC, 2, ADC_CHANNEL_AD17_TEMPSENSOR);
    ADC_DMACmd(ADC, ENABLE);
    ADC_Cmd(ADC, ENABLE);
    ADC_ChannelSampleTimeSel(ADC, ADC_CHANNEL_AD4, ADC_SampleTime_14Cycle);
    ADC_ChannelSampleTimeSel(ADC, ADC_CHANNEL_AD5, ADC_SampleTime_14Cycle);
    ADC_ChannelSampleTimeSel(ADC, ADC_CHANNEL_AD6, ADC_SampleTime_14Cycle);
    ADC_ChannelSampleTimeSel(ADC, ADC_CHANNEL_AD7, ADC_SampleTime_14Cycle);

}

void MSP_ADC_Init(ADC_Type* ADCx)
{

    DMA_ConfigType dmaConfig;
    memset(&dmaConfig, 0, sizeof(dmaConfig));
    /* USER CODE BEGIN MSP_ADC_Init 0 */

    /* USER CODE END MSP_ADC_Init 0 */
    
    /**
    GPIO Configuration for ADC    
    PA11     --> ADC_IN4
    PA12     --> ADC_IN5
    PA13     --> ADC_IN6
    PA14     --> ADC_IN7 
    */
    GPIO_SetFunc(ADC_VAC_Pin, GPIO_FUN1);

    GPIO_SetFunc(ADC_IOUT_Pin, GPIO_FUN1);

    GPIO_SetFunc(ADC_VOUT_Pin, GPIO_FUN1);

    GPIO_SetFunc(ADC_ETEMP_Pin, GPIO_FUN1);

    /* ADC DMA Init */
    /* ADC Init */
    /* USER CODE BEGIN INIT DMA BUFFER ARRAY */

    /* USER CODE END INIT DMA BUFFER ARRAY */
    dmaConfig.memStartAddr = (uint32_t)&adcDmaBuffer[0];
    dmaConfig.memEndAddr = (uint32_t)&adcDmaBuffer[4];
    dmaConfig.periphStartAddr = (uint32_t)&(ADC->RDR);
    dmaConfig.setting.transferNum = 3;
    dmaConfig.setting.finishIntEn = DISABLE;
    dmaConfig.setting.halfFinishIntEn = DISABLE;
    dmaConfig.setting.errorIntEn = DISABLE;
    dmaConfig.setting.channelEn = ENABLE;
    dmaConfig.setting.MEM2MEM = DISABLE;
    dmaConfig.setting.channelPriority = 0;
    dmaConfig.setting.memSize = 1;
    dmaConfig.setting.periphSize = 1;
    dmaConfig.setting.memIncrement = 1;
    dmaConfig.setting.periphIncrement = 0;
    dmaConfig.setting.circular = ENABLE;
    dmaConfig.setting.direction = 0;
    dmaConfig.setting.memByteMode = 0;
    DMA_Initialize(DMA1, &dmaConfig);

    /* ADC interrupt Init */
    DMA_SetEventCallback(DMA1, DMA1_IRQHandler_Callback);
    NVIC_SetPriority(DMA1_IRQn, 0);
    NVIC_EnableIRQ(DMA1_IRQn);

    /* USER CODE BEGIN MSP_ADC_Init 1 */
    ADC_GetIcTemp25Ref();

    /* USER CODE END MSP_ADC_Init 1 */
}

/* USER CODE BEGIN 1 */
void ADC_CalcAvg(void)//100us中断运行
{
	uint32_t u32temp;
	
	AcVoltAvgCnt++;
	AcVoltAvgSum += adcDmaBuffer[0];
	if( AcVoltAvgCnt >= 256 )
	{
		u32temp = AcVoltAvgSum>>8;
		//计算真实值
		AcVoltAvg = u32temp * 222;//放大1000倍
		AcVoltAvgSum = 0;
		AcVoltAvgCnt = 0;
	}

	//输出电流
	OutCurrAvgCnt++;
	OutCurrAvgSum += adcDmaBuffer[1];
	if( OutCurrAvgCnt >= 256 )
	{
		u32temp = OutCurrAvgSum>>8;
		//计算真实值
		OutCurrAvg = (u32temp * 675)>>6;//0.0103 放大1024倍
		OutCurrAvgFltCnt++;
		OutCurrAvgFltSum += OutCurrAvg;
		if( OutCurrAvgFltCnt >= 32)
		{
			u32temp = OutCurrAvgFltSum >> 5;//长时滤波用于显示
			OutCurrAvgFlt = (u32temp * 10)>>10;//放大10倍
			OutCurrAvgFltCnt = 0;
			OutCurrAvgFltSum = 0;
		}
		OutCurrAvgSum = 0;
		OutCurrAvgCnt = 0;			
	}
	
	//输出电压
	OutVoltAvgCnt++;
	OutVoltAvgSum += adcDmaBuffer[2];
	if( OutVoltAvgCnt >= 256 )
	{
		u32temp = OutVoltAvgSum>>8;
		//计算真实值
		OutVoltAvg = (u32temp * 79)>>1;//0.03853 放大1024倍
		OutVoltAvgFltCnt++;
		OutVoltAvgFltSum += OutVoltAvg;
		if(OutVoltAvgFltCnt >= 32)
		{
			u32temp = OutVoltAvgFltSum >> 5; //长时滤波用于显示
			OutVoltAvgFlt = (u32temp * 10) >> 10;//放大10倍
			OutVoltAvgFltSum = 0;
			OutVoltAvgFltCnt = 0;
		}
		OutVoltAvgSum = 0;
		OutVoltAvgCnt = 0;
	}
}

void ADC_ReadInjectSampleVal(void)//10ms 中断运行
{
	if(ADC_STR_IEOC_Msk == (ADC->STR & ADC_STR_IEOC_Msk))
	{
		adcInjectValue[0] = ADC->IDR1;
		adcInjectValue[1] = ADC->IDR2;
		adcInjectValue[2] = ADC->IDR3;
		ADC->STR &= (~ADC_STR_IEOC_Msk);
		ADC_SoftwareStartInjectConvCmd(ADC,ADC_ENABLE);
	}
}

void ADC_CalcTempAvg(void)//main函数中运行
{
	int16_t i16temp;
	if( ADC_FALSE == CalcTempEnable)
	{
		return;	
	}
	
	//计算芯片内部温度
	IcTempAvgCnt++;
	IcTempAvgSum += adcInjectValue[1];
	if( IcTempAvgCnt >= 8)
	{	
		i16temp = (int16_t)(IcTempAvgSum>>3);
		i16temp = (int16_t)(((int32_t)i16temp * 5000)>>12);//AD值转换温度的mV电压
		
		IcTempAvg = (int16_t)(((int32_t)(ICTEMP_25_MV - i16temp) * 1257)>>11) + IcTempRef;
		IcTempAvgCnt = 0;
		IcTempAvgSum = 0;
	}
	
	//查表计算电池温度
	BatTempAvgCnt++;
	BatTempAvgSum += adcInjectValue[0];
	if( BatTempAvgCnt >= 8 )
	{
		i16temp = (uint16_t)(BatTempAvgSum>>3);
		//查表计算真实值
		BatTempAvgCnt = 0;
		BatTempAvgSum = 0;
	}		
	ADC_CalcTempDisable();
}

//修正25度误差
void ADC_GetIcTemp25Ref(void)
{
	uint32_t u32temp;
	
	u32temp = (*(uint32_t *)ICTEMP_25_ADDR & 0x7e000)>>13;
	if(1 == (u32temp >> 5))
	{
		IcTempRef = 25 - (int16_t)((u32temp & 0x1F) >> 1);
	}
	else
	{
		IcTempRef = 25 + (int16_t)((u32temp & 0x1F) >> 1);
	}
}



/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
