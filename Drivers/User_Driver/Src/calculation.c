#include "calculation.h"

uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	
		pTestResult->sampleCount++;																								//采样数增加
		//不管什么模式，电压电流都需要测量
		pTestResult->I_sum+=pTestResult->I_sample;															//累加电流采样值
		pTestResult->V_sum+=pTestResult->V_sample;															//累加电压采样值
		if(pTestResult->sampleCount>=pTestPara->avgNumber)								 	//如果采样数到达平均值个数
		{
			//理想情况下，TIM3的中断时间要大于这部分的采集和计算时间	
			HAL_TIM_Base_Stop_IT(&htim2);																					//停止AD采集的中断
			//计算平均值
			pTestResult->I_avg.numFloat=((float)pTestResult->I_sum)/pTestResult->sampleCount;			//此处算出来的还是adc平均值
			pTestResult->V_avg.numFloat=((float)pTestResult->V_sum)/pTestResult->sampleCount;			//此处算出来的还是adc平均值
			//获得电流值
			pTestResult->I_avg.numFloat=(((float)pTestResult->I_avg.numFloat)*2.5/65535-1.25)/MyPow(10,(pRelay->rangeNow-1));
			if(pRelay->DUT_CurrentScale==RELAY_INPUT_SCALING_6X)
				pTestResult->I_avg.numFloat*=6.1;
			//获得电压值
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)/65535*2.5;
			pTestResult->V_avg.numFloat-=1.25;
			if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_6X)
				pTestResult->I_avg.numFloat*=6.1;
		
			return 0;				
		}
		return 1;																						//没有有效的结果
}


float MyPow(float base,uint8_t time)
{
	uint8_t i;
	float rtn=1;
	for(i=0;i<time;i++)
		rtn*=base;
	return rtn;
}	



