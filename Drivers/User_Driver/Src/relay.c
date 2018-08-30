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
	pRelay->DUT_CurrentScale=RELAY_INPUT_SCALING_1X;
	pRelay->DUT_VoltageScale=RELAY_INPUT_SCALING_1X;
	RelaySetTestMode(pTestPara->testMode);
	ClearAllRangeRelay();
	SetRangeRelayDirect(pRelay->rangeNow);
	if(pRelay->outputConnect) ConnectOutput();			//�˴���Ҫ�����Ƿ���������̵�������ѹģʽ�£��̵�����Ҫһֱ���ӣ�����ģʽ�£��̵���ֻ������Ҫʱ����
	else DisconnectOutput();
}	

void RelaySetInputScaling(enum TestMode testMode, uint8_t scale) //���÷�����ѹ�Ŵ���
{
	if(testMode==MODE_FVMI_SWEEP||testMode==MODE_FVMI_NO_SWEEP||testMode==MODE_MV) //��FVMIʵ���У���ѹ�ǲ��뷴���ģ���MVʵ���أ�֪ʶ��ѹ�������뷴��
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

void ClearAllRangeRelay(void)   //��ʼ��10���̵������ƶ�
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

void SetRangeRelayDirect(uint8_t selectedRange)
{
	switch(selectedRange)
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

void SetRangeRelay(uint8_t selectedRange, uint8_t currentRange)                                   //FVMI���òɼ���ѹ�Ŵ�Χ��FIMV���õ�����λ
{
	SetRangeRelayDirect(selectedRange);
	HAL_Delay(RANGE_CHANGE_DELAY);
	ClearRangeRelay(currentRange);
}

void ClearRangeRelay(uint8_t selectedRange)                                   //FVMI���òɼ���ѹ�Ŵ�Χ��FIMV���õ�����λ
{
	switch(selectedRange)
	{
		case 1:																															//1R��λ���Ŵ�1��  
			HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_RESET);
			break;
		case 2:                                                             //10R��λ���Ŵ�10��
			HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_RESET);
			break;
		case 3:                                                             //100R��λ���Ŵ�10^2��
			HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_RESET);
			break;
		case 4:                                                             //1K��λ���Ŵ�10^3��
			HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_RESET);
			break;
		case 5:                                                             //10K��λ���Ŵ�10^4��
			HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_RESET);
			break;
		case 6:                                                             //100K��λ���Ŵ�10^5��
			HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_RESET);
			break;
		case 7:                                                             //1M��λ���Ŵ�10^6��
			HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_RESET);
			break;
		case 8:                                                             //10M��λ���Ŵ�10^7��
			HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_RESET);
			break;
		case 9:                                                             //100M��λ���Ŵ�10^8��
			HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_RESET);
			break;
		case 10:                                                            //1G��λ���Ŵ�10^9��
			HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_RESET);
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
		if(testMode==MODE_FVMI_NO_SWEEP||testMode==MODE_FVMI_SWEEP) 						//���ΪFVMI�Զ�����ģʽ		
		{	
			if((pRelay->rangeChangeTimes>9) && (pRelay->tempMaxRange>pRelay->tempMinRange))					//���������������9��,�����λ�Զ�����һ��
			{
				pRelay->tempMaxRange--;
				SetRangeRelay(pRelay->tempMaxRange, pRelay->rangeNow);
				pRelay->rangeNow=pRelay->tempMaxRange;
				HAL_Delay(RANGE_CHANGE_DELAY);
				return 1;
			}
			if((pTestResult->I_sample>0xFD00)||(pTestResult->I_sample<0x0300))				//����ɼ����ĵ���ֵ����
			{
				if(pRelay->rangeNow>pRelay->tempMinRange)														//�����λ��1�����ϣ���Ȼ���Խ���
				{		
					HAL_TIM_Base_Stop_IT(&htim2);
					SetRangeRelay(pRelay->rangeNow-1, pRelay->rangeNow);
					pRelay->rangeNow--;
					pRelay->rangeChangeTimes++;
					HAL_Delay(RANGE_CHANGE_DELAY);
					return 1;
				}
				else																															//�����λ��1�����޷���������������
					return 0;
			}
			else if((pTestResult->I_sample<0x8600)&&(pTestResult->I_sample>0x7B00))			//����ɼ����ĵ�����С
			{
				if(pRelay->rangeNow<pRelay->tempMaxRange)	                      //�����λ��������£���Ȼ��������
				{
					SetRangeRelay(pRelay->rangeNow+1, pRelay->rangeNow);
					pRelay->rangeNow++;
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
		
		else if(testMode==MODE_FIMV_NO_SWEEP||testMode==MODE_FIMV_SWEEP||testMode==MODE_MV)  //���ΪFIMVģʽ
		{
			if((pTestResult->V_sample>0xFD00)||(pTestResult->V_sample<0x0300)) 		//����ɼ����ĵ�ѹ����
			{
				HAL_TIM_Base_Stop_IT(&htim2);
				RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_11X);		//ʵ�ʵ�ѹ��ADC��ѹ��11��
				HAL_Delay(RANGE_CHANGE_DELAY);			
				return 1;
			}
			else if((pTestResult->V_sample<0x8600)&&(pTestResult->V_sample>0x7B00))					//����ɼ����ĵ�����С
			{
				HAL_TIM_Base_Stop_IT(&htim2);
				RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_1X);		//ʵ�ʵ�ѹ��ADC��ѹ��1��
				HAL_Delay(RANGE_CHANGE_DELAY);			
				return 1;
			}
			else
				return 0;
		}
		else 
			return 0;
	}
}



