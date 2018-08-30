#include "main.h"
#include "AD5689R.h"
#include "relay.h"
#include "para_init.h"
#include "app.h"
#include "hardware.h"
#include "calculation.h"

void SetRange(Relay_TypeDef* pRelay)          //�ж��Ƿ����Զ�����
{
	if(pRelay->rangeMode!=RELAY_RANGE_AUTO)								//��������Զ�����
	{
		 SetRangeRelayDirect(pRelay->rangeNow);								//���õ�λ���������ʱ�ʲ��ټ���ʱ
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
			limitCurrent*=10;					//�����л�������λ
			tempRange--;							//���̻��������С��λ
		}
		outputValue=current*MyPow(10, tempRange-3*currentUnit-1); //��λmV
		SetRangeRelayDirect(tempRange);
	}
	else
	{
		outputValue=current*MyPow(10, selectedRange-3*currentUnit-1); //��λmV
		SetRangeRelayDirect(selectedRange);
	}
	OutputVoltage(testMode, outputValue);
}

