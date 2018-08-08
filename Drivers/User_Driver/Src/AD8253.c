#include "AD8253.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "delay.h"

void ADC_VoltageScaling(enum AD8253_Scale scale)  //设置ADC2采集电压缩小倍数
{
	if(scale==SCALE10X)
	{
		HAL_GPIO_WritePin(PGA2_A0_GPIO_Port, PGA2_A0_Pin , GPIO_PIN_SET);
		HAL_GPIO_WritePin(PGA2_A1_GPIO_Port, PGA2_A1_Pin , GPIO_PIN_RESET);
	}
	else if(scale==SCALE1X)
	{
		HAL_GPIO_WritePin(PGA2_A0_GPIO_Port, PGA2_A0_Pin , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PGA2_A1_GPIO_Port, PGA2_A1_Pin , GPIO_PIN_RESET);			
	}
	delay_us(10);
	HAL_GPIO_WritePin(PGA2_WR_GPIO_Port, PGA2_WR_Pin , GPIO_PIN_RESET);
	delay_us(10);
	HAL_GPIO_WritePin(PGA2_WR_GPIO_Port, PGA2_WR_Pin , GPIO_PIN_SET);
}

void ADC_CurrentScaling(enum AD8253_Scale scale)  //设置ADC1采集电流缩小倍数
{
	if(scale==SCALE10X)
	{
		HAL_GPIO_WritePin(PGA1_A0_GPIO_Port, PGA1_A0_Pin , GPIO_PIN_SET);
		HAL_GPIO_WritePin(PGA1_A1_GPIO_Port, PGA1_A1_Pin , GPIO_PIN_RESET);		
	}
	else if(scale==SCALE1X)
	{ 
		HAL_GPIO_WritePin(PGA1_A0_GPIO_Port, PGA1_A0_Pin , GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(PGA1_A1_GPIO_Port, PGA1_A1_Pin , GPIO_PIN_RESET);		
	}
	delay_us(10);
	HAL_GPIO_WritePin(PGA1_WR_GPIO_Port, PGA1_WR_Pin , GPIO_PIN_RESET);
	delay_us(10);
	HAL_GPIO_WritePin(PGA1_WR_GPIO_Port, PGA1_WR_Pin , GPIO_PIN_SET);
}
