#include "main.h"
#include "AD5689R.h"
#include "relay.h"
#include "para_init.h"
#include "app.h"
#include "hardware.h"


void SetRange(Relay_TypeDef* pRelay)          //判断是否是自动换挡
{
	if(pRelay->rangeMode!=AUTO_RANGE)								//如果不是自动换挡
	{
		 SetRelay(pRelay->rangeNow);								//设置档位，后边有延时故不再加延时
	}
	else
	{
		SetRelay(pRelay->maxRange);
	}
}

void SetOutput(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)
	{
	  OutputVoltage(pTestPara->testMode, pTestPara->V_Now);
	}
	else if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
	{
		//OutputCurrent(pTestPara->testMode, pTestPara->I_Now);
	}
}

void OutputVoltage(enum TestMode testMode, int voltage)
{
	float outputValue;
	if(voltage>-2400||voltage<2400)			//protect output overflow considering adjustment
	{
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_1X);
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat))/5*65535;
	}
	else
	{
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_6X);
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat)/6)/5*65535;
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, outputValue);	
}

void OutputCurrent(enum TestMode testMode, int current)
{
	float outputValue;
	
}

