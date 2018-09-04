#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara)
{
	MX_TIM3_Init(pTestPara->timeStep);									//DAC输出定时器,单位ms
	HAL_TIM_Base_Stop_IT(&htim3);    										//关闭DAC输出中断
	MX_TIM2_Init(10000/pTestPara->sampleRate);					//ADC采集定时器，单位100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//关闭ADC采集
	MX_TIM4_Init(1000);																	//固定100ms中断一次
	HAL_TIM_Base_Stop_IT(&htim4); 
}

void SetTimerAction(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testCmd==CMD_START)										//开始采集的命令				
	{		
		if(pTestPara->testStatus==OFF)
		{
			pTestPara->testStatus=ON;												//测试开始,有测试在运行中
			ChangeTimer(pTestPara, TIM_ON);									//打开相应定时器
		}
	}
	else if(pTestPara->testCmd==CMD_STOP)								//停止采集的命令
	{
		if(pTestPara->testStatus==ON)
		{
			pTestPara->testStatus=OFF;					
			ChangeTimer(pTestPara, TIM_OFF);
		}			
	}
}

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF)
{
	if(TIM_ONOFF == TIM_ON)
	{
		//HAL_TIM_Base_Start_IT(&htim3);								//在timer4定时器中断里启动Tim3	
		HAL_TIM_Base_Start_IT(&htim4);								//启动QuietTime定时器
		HAL_TIM_Base_Start_IT(&htim2);								//ADC采集数据并回传中断启动
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);									//采样中断停止
		HAL_TIM_Base_Stop_IT(&htim2);									//ADC采集数据并回传中断停止
		HAL_TIM_Base_Stop_IT(&htim4);	
	}
}


void Test_Relay(void)         //测试当前档位合适
{
	for(uint8_t i=0;i<8;i++)
	{
		AD7988_1_ReadData(&hAD7988_1_1);
		HAL_Delay(1);
		AD7988_1_ReadData(&hAD7988_1_2);	
	}
}


