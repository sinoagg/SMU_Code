#ifndef __APP_H
#define __APP_H

enum ADC_Status
{
	ADC_READY=0,
	ADC_BUSY=1
};

void SetOutput(TestPara_TypeDef* pTestPara);
void OutputVoltage(enum TestMode testMode, int voltage);
void OutputCurrent(enum TestMode testMode, int16_t current, uint8_t currentUnit, uint8_t selectedRange);

#endif

