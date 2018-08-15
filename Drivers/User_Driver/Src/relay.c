#include "relay.h"
#include "hardware.h"

Relay_TypeDef Relay;

void GetRelayPara(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay)
{
	if(pRelay->rangeMode==RELAY_RANGE_AUTO)
		pRelay->rangeNow=RELAY_RANGE_1G;
	else 
		pRelay->rangeNow=pRelay->rangeMode;
	pRelay->tempMaxRange=pRelay->maxRange;
	pRelay->tempMinRange=pRelay->minRange;
	RelaySetTestMode(pTestPara->testMode);
	SetRangeRelay(pRelay->rangeNow);
	if(pRelay->outputConnect) ConnectOutput();
	else DisconnectOutput();
}	

void RelaySetInputScaling(enum TestMode testMode, uint8_t scale) //���÷�����ѹ�Ŵ���
{
	if(testMode==MODE_FVMI_SWEEP||testMode==MODE_FVMI_NO_SWEEP)
	{
		if(scale==RELAY_INPUT_SCALING_1X)
		{
			HAL_GPIO_WritePin(Ctrl11_GPIO_Port, Ctrl11_Pin , GPIO_PIN_RESET); 
		}
		else
		{
			HAL_GPIO_WritePin(Ctrl11_GPIO_Port, Ctrl11_Pin , GPIO_PIN_SET); 
		}
	}
	
	if(testMode==MODE_FIMV_SWEEP||testMode==MODE_FIMV_NO_SWEEP)
	{
		if(scale==RELAY_INPUT_SCALING_1X)
		{
			HAL_GPIO_WritePin(Ctrl12_GPIO_Port, Ctrl12_Pin , GPIO_PIN_RESET); 
		}
		else
		{
			HAL_GPIO_WritePin(Ctrl12_GPIO_Port, Ctrl12_Pin , GPIO_PIN_SET); 
		}
	}
}

void RelaySetTestMode(enum TestMode testMode) //���ò���ģʽ��ѹ���ߺ���
{
	if(testMode==MODE_FVMI_SWEEP||testMode==MODE_FVMI_NO_SWEEP)
	{
		HAL_GPIO_WritePin(Ctrl13_GPIO_Port, Ctrl13_Pin , GPIO_PIN_RESET); 
	}
	else if(testMode==MODE_FIMV_SWEEP||testMode==MODE_FIMV_NO_SWEEP)
	{
		HAL_GPIO_WritePin(Ctrl13_GPIO_Port, Ctrl13_Pin , GPIO_PIN_SET); 
	}
}

void ConnectOutput(void)												//����̵�������
{
	HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_SET);
}

void DisconnectOutput(void)											//����̵����Ͽ�
{
	HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_RESET);
}

void RelayClear(void)   //��ʼ��10���̵������ƶ�
{
	/*���̵�λ�̵�������*/
	HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_RESET); 
}

void SetRangeRelay(uint8_t rangeSelect)                                   //FVMI���òɼ���ѹ�Ŵ�Χ��FIMV���õ�����λ
{
	RelayClear();
	switch(rangeSelect)
	{
		case 1:																															//1R��λ���Ŵ�1��  
			HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_SET);
			break;
		case 2:                                                             //10R��λ���Ŵ�10��
			HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_SET);
			break;
		case 3:                                                             //100R��λ���Ŵ�10^2��
			HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_SET);
			break;
		case 4:                                                             //1K��λ���Ŵ�10^3��
			HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_SET);
			break;
		case 5:                                                             //10K��λ���Ŵ�10^4��
			HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_SET);
			break;
		case 6:                                                             //100K��λ���Ŵ�10^5��
			HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_SET);
			break;
		case 7:                                                             //1M��λ���Ŵ�10^6��
			HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_SET);
			break;
		case 8:                                                             //10M��λ���Ŵ�10^7��
			HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_SET);
			break;
		case 9:                                                             //100M��λ���Ŵ�10^8��
			HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_SET);
			break;
		case 10:                                                            //1G��λ���Ŵ�10^9��
			HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_SET);
			break;
		default:
			break;
	}
	
}
/******************************************************************/
/*��������λ�Ƿ���ȷ												                    */
/*����ֵ�� 0 Ϊ��ȷ��1Ϊ����																			*/
/******************************************************************/
uint8_t RelayCheck(enum TestMode testMode, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	if(Relay.rangeMode!=RELAY_RANGE_AUTO)
		return 0;
	else
	{
		if(testMode==MODE_FVMI_NO_SWEEP||testMode==MODE_FVMI_SWEEP) 				//���ΪFVMI�Զ�����ģʽ		
		{	
			if((pRelay->rangeChangeTimes>9) && (pRelay->tempMaxRange>pRelay->tempMinRange))					//���������������9��,�����λ�Զ�����һ��
			{
				pRelay->tempMaxRange--;
				pRelay->rangeNow=pRelay->tempMaxRange;
				SetRangeRelay(--pRelay->rangeNow);
				HAL_Delay(RANGE_CHANGE_DELAY);
				return 1;
			}
			if((pTestResult->I_sample>0xE666)||(pTestResult->I_sample<0x2000))				//����ɼ����ĵ���ֵ����
			{
				if(pRelay->rangeNow>pRelay->tempMinRange)														//�����λ��1�����ϣ���Ȼ���Խ���
				{		
					HAL_TIM_Base_Stop_IT(&htim2);
					SetRangeRelay(--(pRelay->rangeNow));
					pRelay->rangeChangeTimes++;
					HAL_Delay(RANGE_CHANGE_DELAY);
					return 1;
				}
				else																															//�����λ��1�����޷���������������
					return 0;
			}
			else if((pTestResult->I_sample<0x8200)&&(pTestResult->I_sample>0x7DFF))			//����ɼ����ĵ�����С
			{
				if(pRelay->rangeNow<pRelay->tempMaxRange)	                      //�����λ��������£���Ȼ��������
				{
					SetRangeRelay(++(pRelay->rangeNow));
					pRelay->rangeChangeTimes++;
					HAL_Delay(RANGE_CHANGE_DELAY);
					return 1;
				}
				else																															//�����λ��7�����޷���������������
					return 0;
			}
			else 																																//��ѹ��Χ����
				return 0;
		}
		/*else if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)  //���ΪFIMVģʽ
		{
			if((origin_adcvalue_V>0xFD00)||(origin_adcvalue_V<0x300))  			   //����ɼ����ĵ�������
		{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
			
			HAL_Delay(RANGE_CHANGE_DELAY);			
			return 1;
		}
		else if((origin_adcvalue_V<0x8800)&&(origin_adcvalue_V>0x78FF))			//����ɼ����ĵ�����С
		{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
			pResultPara->ADC_sample_count=0;																//��������Ҫ���¼���
			pResultPara->ADC_sample_sum_V=0;																//��������Ҫ���¼���
			HAL_Delay(RANGE_CHANGE_DELAY);			
			return 1;
		}*/
		else
			return 0;
	}
}

/******************************************************************/
/*����ص�λ�Ƿ���ȷ					                               		*/
/*����ֵ��origin_adcvalue																	        */
/******************************************************************/
/*float Relay_Check_Monitor(MonitorCH_TypeDef *phMonitorCH,	uint8_t monitor_mode)
{
	float origin_adcvalue;
	origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);	 //ADC��ѹ�ɼ�����	
	if((origin_adcvalue>0xFD00)||(origin_adcvalue<0x300))  			   //����ɼ����ĵ�������
	{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
			if(monitor_mode==RELAY_TEST_MODE_FVMI)
			  Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_1D12);
			HAL_Delay(20);			
			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
	}
	else if((origin_adcvalue<0x8800)&&(origin_adcvalue>0x78FF))			//����ɼ����ĵ�����С
	{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
			if(monitor_mode==RELAY_TEST_MODE_FVMI)
				Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_5D6);
			HAL_Delay(20);	
			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
		}
	return origin_adcvalue;
}*/


