#include "timer_ctrl.h"

void SetTimerPara(TestPara_TypeDef* pTestPara)
{
	MX_TIM3_Init(pTestPara->timeStep);									//DAC�����ʱ��,��λms
	HAL_TIM_Base_Stop_IT(&htim3);    										//�ر�DAC����ж�
	MX_TIM2_Init(10000/pTestPara->sampleRate);					//ADC�ɼ���ʱ������λ100us
	HAL_TIM_Base_Stop_IT(&htim2);     									//�ر�ADC�ɼ�
	MX_TIM4_Init(1000);																	//�̶�100ms�ж�һ��
	HAL_TIM_Base_Stop_IT(&htim4); 
}

void SetTimerAction(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testCmd==CMD_START)										//��ʼ�ɼ�������				
	{		
		if(pTestPara->testStatus==OFF)
		{
			pTestPara->testStatus=ON;												//���Կ�ʼ,�в�����������
			ChangeTimer(pTestPara, TIM_ON);									//����Ӧ��ʱ��
		}
	}
	else if(pTestPara->testCmd==CMD_STOP)								//ֹͣ�ɼ�������
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
		//HAL_TIM_Base_Start_IT(&htim3);								//��timer4��ʱ���ж�������Tim3	
		HAL_TIM_Base_Start_IT(&htim4);								//����QuietTime��ʱ��
		HAL_TIM_Base_Start_IT(&htim2);								//ADC�ɼ����ݲ��ش��ж�����
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim3);									//�����ж�ֹͣ
		HAL_TIM_Base_Stop_IT(&htim2);									//ADC�ɼ����ݲ��ش��ж�ֹͣ
		HAL_TIM_Base_Stop_IT(&htim4);	
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


