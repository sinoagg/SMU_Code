#include "calculation.h"

uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	
		pTestResult->sampleCount++;																								//����������
		//����ʲôģʽ����ѹ���趼��Ҫ����
		pTestResult->I_sum+=pTestResult->I_sample;															//�ۼӵ�������ֵ
		pTestResult->V_sum+=pTestResult->V_sample;															//�ۼӵ�ѹ����ֵ
		if(pTestResult->sampleCount>=pTestPara->avgNumber)								 	//�������������ƽ��ֵ����
		{
			//��������£�TIM3���ж�ʱ��Ҫ�����ⲿ�ֵĲɼ��ͼ���ʱ��	
			HAL_TIM_Base_Stop_IT(&htim2);																					//ֹͣAD�ɼ����ж�
			pTestResult->I_avg.numFloat=((float)pTestResult->I_sum)/pTestResult->sampleCount;			//�˴�������Ļ���adcƽ��ֵ
			pTestResult->V_avg.numFloat=((float)pTestResult->V_sum)/pTestResult->sampleCount;			//�˴�������Ļ���adcƽ��ֵ
			
			pTestResult->I_avg.numFloat=(((float)pTestResult->I_avg.numFloat)*2.5/65535-1.25)/MyPow(10,(pRelay->rangeNow-1));
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)/65535*2.5;
			pTestResult->V_avg.numFloat-=1.25;
			
			/*if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_I/pTestResult->ADC_sampleCount;		//�������ƽ��ֵ	
				temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/10/MyPow(10,(Relay.rangeNow-1));     
				pTestResult->test_I.numFloat=temp_voltage*Adj_RelayLinear[Relay.rangeNow-1].numFloat+Adj_RelayOffset[Relay.rangeNow-1].numFloat;
				pTestResult->ADC_sample_sum_I=0;																							//�����ۼӣ�����
			}
			else if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)
			{
				temp_adcvalue=pTestResult->ADC_sample_sum_V/pTestResult->ADC_sampleCount;		//�����ѹƽ��ֵ	
				if(hMeasureCH_Voltage.monitorch_relaystate==RELAY_INPUT_SCALING_5D6)
				{
					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12/10;
					temp_voltage=temp_voltage*Adj_MeasVLinear_5D6.numFloat+Adj_MeasVOffset_5D6.numFloat;// ����У׼����
				}
				else
				{
					temp_voltage=(((float)temp_adcvalue)*2.5/65535-1.25)*12;	
					temp_voltage=temp_voltage*Adj_MeasVLinear_1D12.numFloat+Adj_MeasVOffset_1D12.numFloat;// ����У׼����
				}
				pTestResult->test_V.numFloat=temp_voltage;
				pTestResult->ADC_sample_sum_V=0;																							//�����ۼӣ�����
			}
			pTestResult->ADC_sampleCount=0;																							  //���¼���������
		*/
		
			return 0;				
		}
		return 1;																						//û����Ч�Ľ��
}


float MyPow(float base,uint8_t time)
{
	uint8_t i;
	float rtn=1;
	for(i=0;i<time;i++)
		rtn*=base;
	return rtn;
}	



