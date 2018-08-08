#include "calculation.h"

uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	
		pTestResult->sampleCount++;																								//采样数增加
		//不管什么模式，电压电阻都需要测量
		pTestResult->I_sum+=pTestResult->I_sample;															//累加电流采样值
		pTestResult->V_sum+=pTestResult->V_sample;															//累加电压采样值
		if(pTestResult->sampleCount>=pTestPara->avgNumber)								 	//如果采样数到达平均值个数
		{
			//理想情况下，TIM3的中断时间要大于这部分的采集和计算时间	
			HAL_TIM_Base_Stop_IT(&htim2);																					//停止AD采集的中断
			pTestResult->I_avg.numFloat=((float)pTestResult->I_sum)/pTestResult->sampleCount;			//此处算出来的还是adc平均值
			pTestResult->V_avg.numFloat=((float)pTestResult->V_sum)/pTestResult->sampleCount;			//此处算出来的还是adc平均值
			
			pTestResult->I_avg.numFloat=(((float)pTestResult->I_avg.numFloat)*2.5/65535-1.25)/MyPow(10,(pRelay->rangeNow-1));
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)/65535*2.5;
			pTestResult->V_avg.numFloat-=1.25;
			
			/*if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_I/pTestResult->ADC_sampleCount;		//计算电流平均值	
				temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/10/MyPow(10,(Relay.rangeNow-1));     
				pTestResult->test_I.numFloat=temp_voltage*Adj_RelayLinear[Relay.rangeNow-1].numFloat+Adj_RelayOffset[Relay.rangeNow-1].numFloat;
				pTestResult->ADC_sample_sum_I=0;																							//重新累加，清零
			}
			else if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_V/pTestResult->ADC_sampleCount;		//计算电压平均值	
				if(hMeasureCH_Voltage.monitorch_relaystate==RELAY_INPUT_SCALING_5D6)
				{
					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/10;
					temp_voltage=temp_voltage*Adj_MeasVLinear_5D6.numFloat+Adj_MeasVOffset_5D6.numFloat;// 加入校准参数
				}
				else
				{
					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12;	
					temp_voltage=temp_voltage*Adj_MeasVLinear_1D12.numFloat+Adj_MeasVOffset_1D12.numFloat;// 加入校准参数
				}
				pTestResult->test_V.numFloat=temp_voltage;
				pTestResult->ADC_sample_sum_V=0;																							//重新累加，清零
			}
			pTestResult->ADC_sampleCount=0;																							  //重新计数，清零
		*/
		
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



