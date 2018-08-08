#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara)
{
	MX_TIM3_Init(pTestPara->timeStep);									//DAC�����ʱ��,��λms
	HAL_TIM_Base_Stop_IT(&htim3);    										//�ر�DAC����ж�
	MX_TIM2_Init(10000/pTestPara->sampleRate);					//ADC�ɼ���ʱ������λ100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//�ر�ADC�ɼ�
}

void SetTimerAction(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testCmd==CMD_START)										//��ʼ�ɼ�������				
	{		
		pTestPara->testStatus=ON;													//���Կ�ʼ,�в�����������
		ChangeTimer(pTestPara, TIM_ON);											//����Ӧ��ʱ��
	}
	else if(pTestPara->testCmd==CMD_STOP)								//ֹͣ�ɼ�������
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
			HAL_TIM_Base_Start_IT(&htim3);								//DAC��������ж�����	
			HAL_Delay(2);																	//DAC���֮��̶��ȴ�2ms�ٿ����ɼ�
			HAL_TIM_Base_Start_IT(&htim2);								//ADC�ɼ����ݲ��ش��ж�����
		}
		else if(pTestPara->testMode==MODE_FIMV_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
		{
			HAL_TIM_Base_Start_IT(&htim3);								//DAC��������ж�����	
			HAL_Delay(2);																	//DAC���֮��̶��ȴ�2ms�ٿ����ɼ�
			HAL_TIM_Base_Start_IT(&htim2);								//ADC�ɼ����ݲ��ش��ж�����
		}
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);										//DAC��������ж�����	
		HAL_TIM_Base_Stop_IT(&htim2);										//ADC�ɼ����ݲ��ش��ж�ֹͣ	
	}
}


void Test_Relay(void)         //���Ե�ǰ��λ����
{
	for(uint8_t i=0;i<8;i++)
	{
		AD7988_1_ReadData(&hAD7988_1_1);
		HAL_Delay(1);
		AD7988_1_ReadData(&hAD7988_1_2);	
	}
}


