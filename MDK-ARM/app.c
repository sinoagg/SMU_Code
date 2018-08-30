#include "main.h"
#include "AD5689R.h"
#include "relay.h"
#include "para_init.h"
#include "app.h"
#include "hardware.h"
#include "calculation.h"

void SetRange(Relay_TypeDef* pRelay)          //判断是否是自动换挡
{
	if(pRelay->rangeMode!=RELAY_RANGE_AUTO)								//如果不是自动换挡
	{
		 SetRangeRelayDirect(pRelay->rangeNow);								//设置档位，后边有延时故不再加延时
	}
	else
	{
		SetRangeRelayDirect(pRelay->maxRange);
	}
}

void SetOutput(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)
	{
	  OutputVoltage(MODE_FVMI_SWEEP, pTestPara->V_Now);
	}
	else if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
	{
		//OutputCurrent(pTestPara->testMode, pTestPara->I_Now);
	}
}

void OutputVoltage(enum TestMode testMode, int voltage)
{
	float outputValue;
	if(voltage>-2400 && voltage<2400)			//protect output overflow considering adjustment
	{
		Relay.DUT_VoltageScale=RELAY_INPUT_SCALING_1X;
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_1X);			
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat))/5*65535;
	}
	else
	{
		Relay.DUT_VoltageScale=RELAY_INPUT_SCALING_11X;
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_11X);
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat)/11)/5*65535;
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, outputValue);	
}

void OutputCurrent(enum TestMode testMode, int16_t current, uint8_t currentUnit, uint8_t selectedRange)
{
	float outputValue;
	if(selectedRange==RELAY_RANGE_AUTO)
	{
		float realCurrent=(float)abs(current)*MyPow(0.1, 3*currentUnit+3);
		float limitCurrent=1.25e-9;
		uint8_t tempRange=9;
		while(limitCurrent<realCurrent)
		{
			limitCurrent*=10;					//电流切换到更大挡位
			tempRange--;							//量程换到电阻更小挡位
		}
		outputValue=current*MyPow(10, tempRange-3*currentUnit-1); //单位mV
		SetRangeRelayDirect(tempRange);
	}
	else
	{
		outputValue=current*MyPow(10, selectedRange-3*currentUnit-1); //单位mV
		SetRangeRelayDirect(selectedRange);
	}
	OutputVoltage(testMode, outputValue);
}

