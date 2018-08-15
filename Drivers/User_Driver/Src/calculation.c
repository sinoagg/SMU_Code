#include "calculation.h"

uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	
		pTestResult->sampleCount++;																								//����������
		//����ʲôģʽ����ѹ��������Ҫ����
		pTestResult->I_sum+=pTestResult->I_sample;															//�ۼӵ�������ֵ
		pTestResult->V_sum+=pTestResult->V_sample;															//�ۼӵ�ѹ����ֵ
		if(pTestResult->sampleCount>=pTestPara->avgNumber)								 	//�������������ƽ��ֵ����
		{
			//��������£�TIM3���ж�ʱ��Ҫ�����ⲿ�ֵĲɼ��ͼ���ʱ��	
			HAL_TIM_Base_Stop_IT(&htim2);																					//ֹͣAD�ɼ����ж�
			//����ƽ��ֵ
			pTestResult->I_avg.numFloat=((float)pTestResult->I_sum)/pTestResult->sampleCount;			//�˴�������Ļ���adcƽ��ֵ
			pTestResult->V_avg.numFloat=((float)pTestResult->V_sum)/pTestResult->sampleCount;			//�˴�������Ļ���adcƽ��ֵ
			//��õ���ֵ
			pTestResult->I_avg.numFloat=(((float)pTestResult->I_avg.numFloat)*2.5/65535-1.25)/MyPow(10,(pRelay->rangeNow-1));
			if(pRelay->DUT_CurrentScale==RELAY_INPUT_SCALING_6X)
				pTestResult->I_avg.numFloat*=6.1;
			//��õ�ѹֵ
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)/65535*2.5;
			pTestResult->V_avg.numFloat-=1.25;
			if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_6X)
				pTestResult->I_avg.numFloat*=6.1;
		
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



