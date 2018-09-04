#ifndef __APP_H
#define __APP_H

#include "prj_typedef.h"
#include "PC485_TX_protocol.h"

#define TIMER_RESTART_ON 1				//Timer是否重启，在扫描测量过程中需要重启，在测试结束归0的时候不需要重启
#define TIMER_RESTART_OFF 0

#define DEV_ADDR 0x01							//此设备485总线地址，同总线产品地址不能相同
#define BROCST_ADDR	0x00					//广播地址，同总线上所有设备都需要响应

enum ADC_Status
{
	ADC_READY=0,
	ADC_BUSY=1
};

void SetOutput(TestPara_TypeDef* pTestPara);
void OutputVoltage(enum TestMode testMode, int voltage, int timerRestart);
void OutputCurrent(enum TestMode testMode, int16_t current, uint8_t currentUnit, uint8_t selectedRange);
void StartNextSampling(void);
void ClearUartBuf(uint8_t length, uint8_t *pUartRxBuf);
uint8_t GetMsgType(uint8_t *UartRxBuf);

#endif

