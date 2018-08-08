#ifndef __APP_H
#define __APP_H

enum ADC_Status
{
	ADC_READY=0,
	ADC_BUSY=1
};

void SetOutput(TestPara_TypeDef* pTestPara);
void OutputVoltage(enum TestMode testMode, int voltage);

#endif
