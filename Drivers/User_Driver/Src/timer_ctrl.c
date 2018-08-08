#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara)
{
	MX_TIM3_Init(pTestPara->timeStep);									//DAC输出定时器,单位ms
	HAL_TIM_Base_Stop_IT(&htim3);    										//关闭DAC输出中断
	MX_TIM2_Init(10000/pTestPara->sampleRate);					//ADC采集定时器，单位100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//关闭ADC采集
}

void SetTimerAction(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testCmd==CMD_START)										//开始采集的命令				
	{		
		pTestPara->testStatus=ON;													//测试开始,有测试在运行中
		ChangeTimer(pTestPara, TIM_ON);											//打开相应定时器
	}
	else if(pTestPara->testCmd==CMD_STOP)								//停止采集的命令
	{
		pTestPara->testStatus=OFF;					
		ChangeTimer(pTestPara, TIM_OFF);		
	}
}

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF)
{
	if(TIM_ONOFF == TIM_ON)
	{
		if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FVMI_NO_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DAC输出数据中断启动	
			HAL_Delay(2);																	//DAC输出之后固定等待2ms再开启采集
			HAL_TIM_Base_Start_IT(&htim2);								//ADC采集数据并回传中断启动
		}
		else if(pTestPara->testMode==MODE_FIMV_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DAC输出数据中断启动	
			HAL_Delay(2);																	//DAC输出之后固定等待2ms再开启采集
			HAL_TIM_Base_Start_IT(&htim2);								//ADC采集数据并回传中断启动
		}
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);										//DAC输出数据中断启动	
		HAL_TIM_Base_Stop_IT(&htim2);										//ADC采集数据并回传中断停止	
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


