/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
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
#include "ac78xx_i2c.h"
#include "i2c.h"

extern void Error_Handler(char *, int);

void MSP_I2C_Init(I2C_Type* I2Cx);

/* USER CODE BEGIN 0 */
uint16_t I2CUpdateMsg = I2C_TRUE;
//0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
uint8_t BCD_decode_NumTab[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x58,0x5E,0x79,0x71};
//电压单位U 电流单位A
uint8_t BCD_decode_UnitTab[3] ={0x3E,0x77,0x73};

/* USER CODE END 0 */

/* I2C2 init function */
void ATC_I2C2_Init(void)
{
    I2C_ConfigType i2cConfig;
    memset(&i2cConfig, 0, sizeof(i2cConfig));

    MSP_I2C_Init(I2C2);
    i2cConfig.sampleCNT = 9;
    i2cConfig.stepCNT = 4;
    i2cConfig.sclAdj = 0;
    i2cConfig.setting.i2cEn = ENABLE;
    i2cConfig.setting.intEn = DISABLE;
    i2cConfig.setting.master = I2C_MASTER;
    i2cConfig.setting.wakeUpEn = DISABLE;
    i2cConfig.setting.swRst = 0;
    i2cConfig.setting.gcaEn = DISABLE;
    i2cConfig.setting.addressExt = DISABLE;
    i2cConfig.setting.syncEn = ENABLE;
    i2cConfig.setting.arbEn = DISABLE;
    i2cConfig.setting.strEn = DISABLE;
    i2cConfig.setting.mntEn = DISABLE;
    i2cConfig.setting.mntIE = DISABLE;
    i2cConfig.setting.dmaRxEn = DISABLE;
    i2cConfig.setting.dmaTxEn = DISABLE;
    i2cConfig.setting.SSIE = DISABLE;
    i2cConfig.setting.rxOFIntEn = DISABLE;
    i2cConfig.setting.txUFIntEn = DISABLE;
    i2cConfig.setting.nackIntEn = DISABLE;
    i2cConfig.addr1 = 0;
    I2C_Initialize(I2C2, &i2cConfig);

}

void MSP_I2C_Init(I2C_Type* I2Cx)
{

    if (I2Cx == I2C2)
    {
    /* USER CODE BEGIN MSP_I2C2_Init 0 */

    /* USER CODE END MSP_I2C2_Init 0 */
    
    /**
    GPIO Configuration for I2C2    
    PD1     --> I2C2_SCL
    PD2     --> I2C2_SDA 
    */
    GPIO_SetFunc(GPIO_PD1, GPIO_FUN3);

    GPIO_SetFunc(GPIO_PD2, GPIO_FUN3);

    /* USER CODE BEGIN MSP_I2C2_Init 1 */

    /* USER CODE END MSP_I2C2_Init 1 */
    }
}

/* USER CODE BEGIN 1 */
//电池电压 充电电流 SOC  故障代码
void I2C_WriteData(uint16_t data,uint16_t type)
{
	uint8_t temp0 = 0;
	uint8_t temp1 = 0xBF;
	
	uint8_t temp2;
	uint8_t temp3;
	uint8_t aData[3] = {0};
	
	I2C_CH455Init();//抗干扰，刷新
	
	if( data<=9)
	{
		//第1位不显示 第2位显示0. 第3位具体值
		I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &temp0, 1);
		
		if( type == I2C_SOC_TYPE )//SOC 此处第2位不显示
		{
			I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &temp0, 1);
		}
		else
		{
		    I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &temp1, 1);
		}

		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[data], 1);   		
	}
	else if( data <= 99 )
	{
		//第1位不显示 第2位data 的个位数加点，第3位显示小数位
		aData[0] = data * 0.1;//个位
		aData[1] = data- (aData[0] * 10); //小数位
		if(type == I2C_SOC_TYPE)
		{
			temp2 = BCD_decode_NumTab[aData[0]];//不加点
		}
		else
		{
		    temp2 = BCD_decode_NumTab[aData[0]] | 0x80;//加点
		}

		I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &temp0, 1);
		I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &temp2, 1);
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[aData[1]], 1); 
	}
	else if( data <= 999)
	{
		//第1位显示十位，第2位显示个位加点,第3位显示小数位
		aData[0] = data * 0.01;//十位
		temp2 = data - ((int16_t)aData[0] * 100);//减去十位数，得到两位数
		
		aData[1] = temp2 * 0.1;//个位
		if(type == I2C_SOC_TYPE)
		{
			temp3 = BCD_decode_NumTab[aData[1]];//不加点
		}
		else
		{
		    temp3 = BCD_decode_NumTab[aData[1]] | 0x80;
		}
		aData[2] = temp2 - (aData[1] * 10);//小数位
		
		I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &BCD_decode_NumTab[aData[0]], 1);
		I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &temp3, 1);
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[aData[2]], 1); 		
	}
	else
	{
		
	}
	
	
	if( type == I2C_VOLT_TYPE)
	{
		//第4个数码管显示单位U
		I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &BCD_decode_UnitTab[0], 1);  
	}
	else if( type == I2C_CURR_TYPE)
	{
		//第4个数码管显示单位A
		I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &BCD_decode_UnitTab[1], 1);  
	}
	else if( type == I2C_SOC_TYPE)
	{
		//SOC 第4个数码管显示单位P
		I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &BCD_decode_UnitTab[2], 1);  
	}
	else
	{
		return;
	}
}
//显示故障代码
void I2C_WriteErrCodeData(void)
{
	uint8_t temp = 0;
	
	I2C_CH455Init();//抗干扰，刷新
	
	//第1位显示字母E
	I2C_MasterWrite(I2C2, CH455_DIG0_ADDR, &BCD_decode_NumTab[14], 1);
	//第2位显示数字0
	I2C_MasterWrite(I2C2, CH455_DIG1_ADDR, &BCD_decode_NumTab[0], 1);
	//第3位
	if( ALM_FAULT == AlmFlag.bit.BatUnderVolt)
	{
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[1], 1);//E01
	}
	else if( (ALM_FAULT == AlmFlag.bit.AcUnderVolt) || (ALM_FAULT == AlmFlag.bit.AcOverVolt))
	{
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[2], 1);//E02
	}
	
	else if( ALM_FAULT == AlmFlag.bit.TemperOver)
	{
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[3], 1);//E03
	}
	else if( ALM_FAULT == AlmFlag.bit.BatTempOver )
	{
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[4], 1);//E04
	}
	else if( ALM_FAULT == AlmFlag.bit.BatOverVolt)
	{
		I2C_MasterWrite(I2C2, CH455_DIG2_ADDR, &BCD_decode_NumTab[6], 1);//E04
	}
	else
	{
		
	}
	//第4位不显示
	I2C_MasterWrite(I2C2, CH455_DIG3_ADDR, &temp, 1);
}

void I2C_CH455DataShowProcess(void)//main函数，3S运行周期
{
	static uint8_t I2CWriteCnt = 0;
    if( (I2C_FALSE == I2CUpdateMsg)||(GPIO_KEY_OFF != GpioKeyState))
	{
		return;
	}
	
	if( 0 == I2CWriteCnt)
	{
		//显示电压
		I2C_WriteData(OutVoltAvgFlt,I2C_VOLT_TYPE);//OutVoltAvgFlt
		I2CWriteCnt = 1;
	}
	else if( 1 == I2CWriteCnt)
	{
		//显示电流
		I2C_WriteData(OutCurrAvgFlt,I2C_CURR_TYPE);//OutCurrAvgFlt
		I2CWriteCnt = 2;

	}
	else if( 2 == I2CWriteCnt)
	{
		//显示SOC 
		I2C_WriteData(SysSOCVal,I2C_SOC_TYPE);
		if(ALM_NORMAL == ALM_GetCurrentAllAlmFlag())//无故障
		{
			I2CWriteCnt = 0;
		}
		else
		{
			I2CWriteCnt = 3;
		}
	}
	else
	{
		//显示故障
		I2C_WriteErrCodeData();
		I2CWriteCnt = 0;
	}
	I2C_DisableTxData();
}

void I2C_CH455Init(void)
{
	uint8_t temp = 0x21;//8段显示，使能CH455
	
	I2C_MasterWrite(I2C2, 0x24, &temp, 1);	
}

void I2C_CH455Stop(void)
{
	uint8_t temp = 0x20;//8段显示,禁止455
	
	I2C_MasterWrite(I2C2, 0x24, &temp, 1);
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT AutoChips *****END OF FILE****/
