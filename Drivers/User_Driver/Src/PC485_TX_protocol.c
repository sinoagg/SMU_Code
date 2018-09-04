#include "PC485_TX_protocol.h"
#include "app.h"

uint8_t* prepareTxData(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t* pUartTxBuf)
{
	uint8_t xorCheck=0;
	*pUartTxBuf=DEV_ADDR;
	*(pUartTxBuf+1)=pTestResult->endOfTest;
	*(pUartTxBuf+2)=(uint8_t)((pTestPara->V_Now)>>8);
	*(pUartTxBuf+3)=(uint8_t)((pTestPara->V_Now)&0xFF);
	*(pUartTxBuf+4)=(uint8_t)((pTestPara->I_Now)>>8);
	*(pUartTxBuf+5)=(uint8_t)((pTestPara->I_Now)&0xFF);
	*(pUartTxBuf+6)=pTestResult->V_avg.numUchar[3];																		
	*(pUartTxBuf+7)=pTestResult->V_avg.numUchar[2];																		
	*(pUartTxBuf+8)=pTestResult->V_avg.numUchar[1];																	
	*(pUartTxBuf+9)=pTestResult->V_avg.numUchar[0];
	*(pUartTxBuf+10)=pTestResult->I_avg.numUchar[3];																		
	*(pUartTxBuf+11)=pTestResult->I_avg.numUchar[2];																		
	*(pUartTxBuf+12)=pTestResult->I_avg.numUchar[1];																	
	*(pUartTxBuf+13)=pTestResult->I_avg.numUchar[0];																	
	*(pUartTxBuf+18)=0;
	for(uint8_t j=0;j<UART_TX_LEN-1;j++)                //将接收到的数据按照协议进行校验
	{
		xorCheck^=*(pUartTxBuf+j);
	}
	*(pUartTxBuf+19)=xorCheck;
	return (pUartTxBuf+20);
	
}




