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
			//if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_11X)//在FVMI模式下使用
			//	pTestResult->I_avg.numFloat/=11;
			//if(pRelay->DUT_CurrentScale==RELAY_INPUT_SCALING_11X)//在FIMV模式下使用，但不常用，因为可以通过量程调节
			//pTestResult->I_avg.numFloat*=11;//乘多少倍不确定,需要实验测量，而且这个方法比较少用
			//获得电压值
			pTestResult->V_avg.numFloat=((float)pTestResult->V_avg.numFloat)*2.5/65535-1.25;
			pTestResult->V_avg.numFloat*=1000;			//转化为mV单位
			//if(pRelay->DUT_VoltageScale==RELAY_INPUT_SCALING_11X)
			//pTestResult->V_avg.numFloat*=11;
		
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
				//input_I=1/Adj_NegRelayLinear[pRelay->rangeNow-1].numFloat*(input_I-Adj_RelayOffset[pRelay->rangeNow-1].numFloat);//得到原始电流值
	}
	return input_I;
}
static void ZeroCrrentCalibration(void)
{ 
	for(uint8_t i=0;i<9;i++)							//9个挡位
	{
		float temp_value=0, temp_sum=0;
		RelaySetTestMode(MODE_FIMV_SWEEP);//恒流
		SetRangeRelayDirect(i+1);
		HAL_Delay(1000+i*1000);
		for(uint8_t j=0;j<10;j++)		//10次采集取平均值
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC采集数据
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		Adj_RelayOffset[i].numFloat=(float)(temp_sum/10);//AD采集平均值
	}
}
static void CurrentRangeCalibration(void)
{
	float temp;
	float temp_value=0, temp_sum=0;

	if((Uart2RxBuf[1]==0x15)&&(Uart2RxBuf[2]==0x02))
	{
		RelaySetTestMode(MODE_FIMV_SWEEP);//恒流
		ConnectOutput();//
	  HAL_Delay(200); //打开连接
		SetRangeRelayDirect(Uart2RxBuf[3]);
		HAL_Delay(200);//切换档位，注意上位机一定是正确的档位
		OutputVoltage(MODE_FIMV_SWEEP, 1000,TIMER_RESTART_OFF);
		HAL_Delay(200);
		//temp=GetMonitorVoltage(&hMonitorCH_Drain, 2000);
		for(uint8_t j=0;j<10;j++)		//10次采集取平均值
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC采集数据
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD采集平均值
		Cali_Crrent_Pos(Adj_Pos_lt2400_RelayLinear,temp,Uart2RxBuf[3],1);
		
		OutputVoltage(MODE_FIMV_SWEEP, -1000,TIMER_RESTART_OFF);   //输出特定电压
		HAL_Delay(200);
		temp_value=0, temp_sum=0;
		for(uint8_t j=0;j<10;j++)		//10次采集取平均值
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC采集数据
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD采集平均值
		Cali_Crrent_Neg(Adj_Neg_lt2400_RelayLinear,temp,Uart2RxBuf[3],-1);
		
		
		OutputVoltage(MODE_FIMV_SWEEP, 3000,TIMER_RESTART_OFF);
		HAL_Delay(200);
		//temp=GetMonitorVoltage(&hMonitorCH_Drain, 2000);
		for(uint8_t j=0;j<10;j++)		//10次采集取平均值
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC采集数据
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD采集平均值
		Cali_Crrent_Pos(Adj_Pos_gt2400_RelayLinear,temp,Uart2RxBuf[3],3);
		
		OutputVoltage(MODE_FIMV_SWEEP, -3000,TIMER_RESTART_OFF);   //输出特定电压
		HAL_Delay(200);
		temp_value=0, temp_sum=0;
		for(uint8_t j=0;j<10;j++)		//10次采集取平均值
		{
			temp_value=(float)AD7988_1_ReadData(&hAD7988_1_1)*2.5/65535-1.25;//ADC采集数据
			temp_sum+=temp_value;
			HAL_Delay(2);
		}
		temp=(float)(temp_sum/10);//AD采集平均值
		Cali_Crrent_Neg(Adj_Neg_gt2400_RelayLinear,temp,Uart2RxBuf[3],-3);
	}
	  //ClearAllVoltage();
	  DisconnectOutput();
}  
//校准正电流
void Cali_Crrent_Pos(FloatUnion* arrary,float temp,uint8_t index,int voltage)
{
	(*(arrary+index-1)).numFloat=(temp-Adj_RelayOffset[index-1].numFloat)/voltage;
}
//校准负电流
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
/*保存校准数据*/		
void FlashSaveCaliData(uint32_t start_address, uint32_t end_address)
{
	uint32_t data=0;	
	HAL_FLASH_Unlock();                                //存入FLASH校准数据
	FlashErase(start_address,end_address);             //擦除用户页,必须整页擦出
	
//	for(uint8_t i=0; i<4; i++)                         //存入输出电压校准偏移量值
//	{
//		for(uint8_t j=0;j<4;j++)
//		{
//			data<<=8;
//			data|=Adj_OutputOffset[i].numUchar[3-j];
//		}
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_OFFSET_ADDR+i*4, data);
//	}
	
	//由于输出线性度无法正常校准，此处不存
//	for(uint8_t i=0; i<4; i++)                         //存入输出电压校准线性量值
//	{
//		for(uint8_t j=0;j<4;j++)
//		{
//			data<<=8;
//			data|=Adj_OutputLinear[i].numUchar[3-j];
//		}
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_LINEAR_ADDR+i*4, data);
//	}
	
	for(uint8_t i=0; i<9; i++)                         //存入一级放大电流校准偏移量值
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_RelayOffset[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_RELAY_OFFSET_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //存入一级放大电流校准线性量值
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Pos_gt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_GT2400_RELAY_LINEAR_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //存入一级放大电流校准线性量值
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Neg_gt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_GT2400_NRELAY_LINEAR_ADDR+i*4, data);
	}
		for(uint8_t i=0; i<9; i++)                         //存入一级放大电流校准线性量值
	{
		for(uint8_t j=0;j<4;j++)
		{
			data<<=8;
			data|=Adj_Pos_lt2400_RelayLinear[i].numUchar[3-j];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_ADDR+OUTPUT_LT2400_RELAY_LINEAR_ADDR+i*4, data);
	}
	
	for(uint8_t i=0; i<9; i++)                         //存入一级放大电流校准线性量值
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

