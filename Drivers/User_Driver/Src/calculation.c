//#include "calculation.h"
#include "calculation.h"
#include "para_init.h"
#include "relay.h"
#include "AD7988_1.h"
#include "hardware.h"
#include "app.h"
#include "InternalFlash.h"



extern uint8_t Uart2RxBuf[32];
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
			//if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_11X)//��FVMIģʽ��ʹ��
			//	pTestResult->I_avg.numFloat/=11;
			//if(pRelay->DUT_CurrentScale==RELAY_INPUT_SCALING_11X)//��FIMVģʽ��ʹ�ã��������ã���Ϊ����ͨ�����̵���
			//pTestResult->I_avg.numFloat*=11;//�˶��ٱ���ȷ��,��Ҫʵ�������������������Ƚ�����
			//��õ�ѹֵ
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)*2.5/65535-1.25;
			pTestResult->V_avg.numFloat*=1000;			//ת��ΪmV��λ
			//if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_11X)
			//pTestResult->V_avg.numFloat*=11;
		
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
float CommitAdjustment(float input_I, Relay_TypeDef* pRelay,int voltage)
{
	if(input_I>0)
	{
		  if(voltage<2400)
			input_I=1/Adj_Pos_lt2400_RelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat/MyPow(10,(pRelay->rangeNow-1)));	
					//input_I=1/Adj_PosRelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat);
				//input_I=1/Adj_PosRelayLinear[pRelay->rangeNow-1].numFloat*(input_I);
        else
			input_I=1/Adj_Pos_gt2400_RelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat/MyPow(10,(pRelay->rangeNow-1)));
	}		
	else
	{
		  if(voltage>(-2400))
			input_I=1/Adj_Neg_lt2400_RelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat/MyPow(10,(pRelay->rangeNow-1)));
			else
			input_I=1/Adj_Neg_gt2400_RelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat/MyPow(10,(pRelay->rangeNow-1)));
				//input_I=1/Adj_NegRelayLinear[pRelay->rangeNow-1].numFloat*(input_I);
				//input_I=1/Adj_NegRelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat);//�õ�ԭʼ����ֵ
	}
	return input_I;
}
static void ZeroCrrentCalibration(void)
{ 
	for(uint8_t i=0;i<9;i++)							//9����λ
	{
		float temp_value=0, temp_sum=0;
		RelaySetTestMode(MODE_FIMV_SWEEP);//����
		SetRangeRelayDirect(i+1);
		HAL_Delay(1000+i*1000);
		for(uint8_t j=0;j<10;j++)		//10�βɼ�ȡƽ��ֵ
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC�ɼ�����
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		Adj_RelayOffset[i].numFloat=(float)(temp_sum/10);//AD�ɼ�ƽ��ֵ
	}
}
static void CurrentRangeCalibration(void)
{
	float temp;
	float temp_value=0, temp_sum=0;

	if((Uart2RxBuf[1]==0x15)&&(Uart2RxBuf[2]==0x02))
	{
		RelaySetTestMode(MODE_FIMV_SWEEP);//����
		ConnectOutput();//
	  HAL_Delay(200); //������
		SetRangeRelayDirect(Uart2RxBuf[3]);
		HAL_Delay(200);//�л���λ��ע����λ��һ������ȷ�ĵ�λ
		OutputVoltage(MODE_FIMV_SWEEP, 1000,TIMER_RESTART_OFF);
		HAL_Delay(200);
		//temp=GetMonitorVoltage(&hMonitorCH_Drain, 2000);
		for(uint8_t j=0;j<10;j++)		//10�βɼ�ȡƽ��ֵ
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC�ɼ�����
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD�ɼ�ƽ��ֵ
		Cali_Crrent_Pos(Adj_Pos_lt2400_RelayLinear,temp,Uart2RxBuf[3],1);
		
		OutputVoltage(MODE_FIMV_SWEEP, -1000,TIMER_RESTART_OFF);   //����ض���ѹ
		HAL_Delay(200);
		temp_value=0, temp_sum=0;
		for(uint8_t j=0;j<10;j++)		//10�βɼ�ȡƽ��ֵ
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC�ɼ�����
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD�ɼ�ƽ��ֵ
		Cali_Crrent_Neg(Adj_Neg_lt2400_RelayLinear,temp,Uart2RxBuf[3],-1);
		
		
		OutputVoltage(MODE_FIMV_SWEEP, 3000,TIMER_RESTART_OFF);
		HAL_Delay(200);
		//temp=GetMonitorVoltage(&hMonitorCH_Drain, 2000);
		for(uint8_t j=0;j<10;j++)		//10�βɼ�ȡƽ��ֵ
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC�ɼ�����
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD�ɼ�ƽ��ֵ
		Cali_Crrent_Pos(Adj_Pos_gt2400_RelayLinear,temp,Uart2RxBuf[3],3);
		
		OutputVoltage(MODE_FIMV_SWEEP, -3000,TIMER_RESTART_OFF);   //����ض���ѹ
		HAL_Delay(200);
		temp_value=0, temp_sum=0;
		for(uint8_t j=0;j<10;j++)		//10�βɼ�ȡƽ��ֵ
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC�ɼ�����
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD�ɼ�ƽ��ֵ
		Cali_Crrent_Neg(Adj_Neg_gt2400_RelayLinear,temp,Uart2RxBuf[3],-3);
	}
	  //ClearAllVoltage();
	  DisconnectOutput();
}  
//У׼������
void Cali_Crrent_Pos(FloatUnion* arrary,float temp,uint8_t index,int voltage)
{
	(*(arrary+index-1)).numFloat=(temp-Adj_RelayOffset[index-1].numFloat)/voltage;
}
//У׼������
void Cali_Crrent_Neg(FloatUnion* arrary,float temp,uint8_t index,int voltage)
{
	(*(arrary+index-1)).numFloat=(temp*1000-Adj_RelayOffset[index-1].numFloat)/(voltage);
}
void RunCalibration(uint8_t* UartRxBuf)
{
	switch(UartRxBuf[2])
	{
		case OUTPUT_VOL_CALIBRATION:
			//OutputVoltageCalibration();
			break;
		case ZERO_CURRENT_CALIBRATION:
			ZeroCrrentCalibration();
			break;
		case CURRENT_RANGE_CALIBRATION:
			CurrentRangeCalibration();
			break;
		case SAVE_CALIBRATION:
			FlashSaveCaliData(FLASH_USER_PAGE1_START_ADDR, FLASH_USER_PAGE1_END_ADDR);	
			break;
	}
}
/*����У׼����*/		
void FlashSaveCaliData(uint32_t start_address, uint32_t end_address)
{
	uint32_t data=0;	
	HAL_FLASH_Unlock();                                //����FLASHУ׼����
	FlashErase(start_address,end_address);             //�����û�ҳ,������ҳ����
	
//	for(uint8_t i=0; i<4; i++)                         //���������ѹУ׼ƫ����ֵ
//	{
//		for(uint8_t j=0;j<4;j++)
//		{
//			data<<=8;
//			data|=Adj_OutputOffset[i].numUchar[3-j];
//		}
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_OFFSET_ADDR+i*4, data);
//	}
	
	//����������Զ��޷�����У׼���˴�����
//	for(uint8_t i=0; i<4; i++)                         //���������ѹУ׼������ֵ
//	{
//		for(uint8_t j=0;j<4;j++)
//		{
//			data<<=8;
//			data|=Adj_OutputLinear[i].numUchar[3-j];
//		}
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_LINEAR_ADDR+i*4, data);
//	}
	
	for(uint8_t i=0; i<9; i++)                         //����һ���Ŵ����У׼ƫ����ֵ
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_RelayOffset[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_RELAY_OFFSET_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //����һ���Ŵ����У׼������ֵ
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Pos_gt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_GT2400_RELAY_LINEAR_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //����һ���Ŵ����У׼������ֵ
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Neg_gt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_GT2400_NRELAY_LINEAR_ADDR+i*4, data);
	}
		for(uint8_t i=0; i<9; i++)                         //����һ���Ŵ����У׼������ֵ
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Pos_lt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_LT2400_RELAY_LINEAR_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //����һ���Ŵ����У׼������ֵ
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Neg_lt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_LT2400_NRELAY_LINEAR_ADDR+i*4, data);
	}
	HAL_FLASH_Lock();
}

