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

void RelaySetInputScaling(enum TestMode testMode, uint8_t scale) //设置反馈电压放大倍数
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

void RelaySetTestMode(enum TestMode testMode) //设置测量模式恒压或者恒流
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

void ConnectOutput(void)												//输出继电器连接
{
	HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_SET);
}

void DisconnectOutput(void)											//输出继电器断开
{
	HAL_GPIO_WritePin(Ctrl14_GPIO_Port, Ctrl14_Pin , GPIO_PIN_RESET);
}

void RelayClear(void)   //初始化10个继电器控制端
{
	/*量程挡位继电器控制*/
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

void SetRangeRelay(uint8_t rangeSelect)                                   //FVMI设置采集电压放大范围，FIMV设置电流档位
{
	RelayClear();
	switch(rangeSelect)
	{
		case 1:																															//1R档位，放大1倍  
			HAL_GPIO_WritePin(Ctrl1_GPIO_Port, Ctrl1_Pin , GPIO_PIN_SET);
			break;
		case 2:                                                             //10R档位，放大10倍
			HAL_GPIO_WritePin(Ctrl2_GPIO_Port, Ctrl2_Pin , GPIO_PIN_SET);
			break;
		case 3:                                                             //100R档位，放大10^2倍
			HAL_GPIO_WritePin(Ctrl3_GPIO_Port, Ctrl3_Pin , GPIO_PIN_SET);
			break;
		case 4:                                                             //1K档位，放大10^3倍
			HAL_GPIO_WritePin(Ctrl4_GPIO_Port, Ctrl4_Pin , GPIO_PIN_SET);
			break;
		case 5:                                                             //10K档位，放大10^4倍
			HAL_GPIO_WritePin(Ctrl5_GPIO_Port, Ctrl5_Pin , GPIO_PIN_SET);
			break;
		case 6:                                                             //100K档位，放大10^5倍
			HAL_GPIO_WritePin(Ctrl6_GPIO_Port, Ctrl6_Pin , GPIO_PIN_SET);
			break;
		case 7:                                                             //1M档位，放大10^6倍
			HAL_GPIO_WritePin(Ctrl7_GPIO_Port, Ctrl7_Pin , GPIO_PIN_SET);
			break;
		case 8:                                                             //10M档位，放大10^7倍
			HAL_GPIO_WritePin(Ctrl8_GPIO_Port, Ctrl8_Pin , GPIO_PIN_SET);
			break;
		case 9:                                                             //100M档位，放大10^8倍
			HAL_GPIO_WritePin(Ctrl9_GPIO_Port, Ctrl9_Pin , GPIO_PIN_SET);
			break;
		case 10:                                                            //1G档位，放大10^9倍
			HAL_GPIO_WritePin(Ctrl10_GPIO_Port, Ctrl10_Pin , GPIO_PIN_SET);
			break;
		default:
			break;
	}
	
}
/******************************************************************/
/*检查测量档位是否正确												                    */
/*返回值： 0 为正确，1为换档																			*/
/******************************************************************/
uint8_t RelayCheck(enum TestMode testMode, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay)
{
	if(Relay.rangeMode!=RELAY_RANGE_AUTO)
		return 0;
	else
	{
		if(testMode==MODE_FVMI_NO_SWEEP||testMode==MODE_FVMI_SWEEP) 				//如果为FVMI自动换挡模式		
		{	
			if((pRelay->rangeChangeTimes>9) && (pRelay->tempMaxRange>pRelay->tempMinRange))					//如果换挡次数多于9次,将最大挡位自动降低一档
			{
				pRelay->tempMaxRange--;
				pRelay->rangeNow=pRelay->tempMaxRange;
				SetRangeRelay(--pRelay->rangeNow);
				HAL_Delay(RANGE_CHANGE_DELAY);
				return 1;
			}
			if((pTestResult->I_sample>0xE666)||(pTestResult->I_sample<0x2000))				//如果采集到的电流值过大
			{
				if(pRelay->rangeNow>pRelay->tempMinRange)														//如果档位在1档以上，仍然可以降档
				{		
					HAL_TIM_Base_Stop_IT(&htim2);
					SetRangeRelay(--(pRelay->rangeNow));
					pRelay->rangeChangeTimes++;
					HAL_Delay(RANGE_CHANGE_DELAY);
					return 1;
				}
				else																															//如果档位在1档，无法降档，换挡结束
					return 0;
			}
			else if((pTestResult->I_sample<0x8200)&&(pTestResult->I_sample>0x7DFF))			//如果采集到的电流过小
			{
				if(pRelay->rangeNow<pRelay->tempMaxRange)	                      //如果档位在最大档以下，仍然可以升档
				{
					SetRangeRelay(++(pRelay->rangeNow));
					pRelay->rangeChangeTimes++;
					HAL_Delay(RANGE_CHANGE_DELAY);
					return 1;
				}
				else																															//如果档位在7档，无法升档，换挡结束
					return 0;
			}
			else 																																//电压范围正常
				return 0;
		}
		/*else if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)  //如果为FIMV模式
		{
			if((origin_adcvalue_V>0xFD00)||(origin_adcvalue_V<0x300))  			   //如果采集到的电流过大
		{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
			
			HAL_Delay(RANGE_CHANGE_DELAY);			
			return 1;
		}
		else if((origin_adcvalue_V<0x8800)&&(origin_adcvalue_V>0x78FF))			//如果采集到的电流过小
		{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_5D6;
			Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_5D6);
			pResultPara->ADC_sample_count=0;																//换挡后需要重新计数
			pResultPara->ADC_sample_sum_V=0;																//换挡后需要重新计算
			HAL_Delay(RANGE_CHANGE_DELAY);			
			return 1;
		}*/
		else
			return 0;
	}
}

/******************************************************************/
/*检查监控档位是否正确					                               		*/
/*返回值：origin_adcvalue																	        */
/******************************************************************/
/*float Relay_Check_Monitor(MonitorCH_TypeDef *phMonitorCH,	uint8_t monitor_mode)
{
	float origin_adcvalue;
	origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);	 //ADC电压采集数据	
	if((origin_adcvalue>0xFD00)||(origin_adcvalue<0x300))  			   //如果采集到的电流过大
	{
			phMonitorCH->monitorch_relaystate=RELAY_INPUT_SCALING_1D12;
			if(monitor_mode==RELAY_TEST_MODE_FVMI)
			  Relay_ADC_Voltage_Scaling(RELAY_INPUT_SCALING_1D12);
			else if(monitor_mode==RELAY_TEST_MODE_FIMV)
				Relay_ADC_Current_Scaling(RELAY_INPUT_SCALING_1D12);
			HAL_Delay(20);			
			origin_adcvalue=AD7988_1_ReadData(phMonitorCH->phAD7988_1_x);
	}
	else if((origin_adcvalue<0x8800)&&(origin_adcvalue>0x78FF))			//如果采集到的电流过小
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


