#ifndef __APP_H
#define __APP_H

enum ADC_Status
{
	ADC_READY=0,
	ADC_BUSY=1
};

void SetOutput(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay);
void OutputVoltage(enum TestMode testMode, int voltage, Relay_TypeDef* pRelay);

#endif

