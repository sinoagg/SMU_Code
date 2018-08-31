#include "PC485_TX_protocol.h"

#define DEV_ADDR 0x01																						//上位机配置数据长度

void prepareTxData(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t* pUartTxBuf)
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
}

//void PC485_TX_Message(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t EndOfTest)
//{
//	pTestResult->test_V.numFloat*=1000;                   //电压转为mV进行传输
//	if(pTestPara->testMode==MODE_FVMI_SWEEP||pTestPara->testMode==MODE_FVMI_NO_SWEEP)
//	{
//		UART2_TxBuf[2]=pTestResult->test_I.numUchar[3];     //采集电流
//		UART2_TxBuf[3]=pTestResult->test_I.numUchar[2];
//		UART2_TxBuf[4]=pTestResult->test_I.numUchar[1];
//		UART2_TxBuf[5]=pTestResult->test_I.numUchar[0];
//		UART2_TxBuf[7]=pTestResult->test_V.numUchar[3];     //监控电压
//		UART2_TxBuf[8]=pTestResult->test_V.numUchar[2];	
//		UART2_TxBuf[9]=pTestResult->test_V.numUchar[1];																		
//		UART2_TxBuf[10]=pTestResult->test_V.numUchar[0];																		
//	}
//	else if(pTestPara->testMode==MODE_FIMV_SWEEP||pTestPara->testMode==MODE_FIMV_NO_SWEEP)
//	{
//		UART2_TxBuf[2]=pTestResult->test_V.numUchar[3];     //采集电压
//		UART2_TxBuf[3]=pTestResult->test_V.numUchar[2];
//		UART2_TxBuf[4]=pTestResult->test_V.numUchar[1];
//		UART2_TxBuf[5]=pTestResult->test_V.numUchar[0];
//		UART2_TxBuf[7]=pTestResult->test_I.numUchar[3];     //监控电流
//		UART2_TxBuf[8]=pTestResult->test_I.numUchar[2];	
//		UART2_TxBuf[9]=pTestResult->test_I.numUchar[1];																		
//		UART2_TxBuf[10]=pTestResult->test_I.numUchar[0];		
//	}
//	UART2_TxBuf[0]=EndOfTest;																							//数据是否为最后一组数据	
//	UART2_TxBuf[1]=EndOfTest;																							//数据是否为最后一组数据																
//	UART2_TxBuf[6]=Relay.relay_range;
//	
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //使能串口2发送
//	HAL_UART_AbortReceive_IT(&huart1);
//	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)
//	{
//		Error_Handler();
//	}		
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//使能接收
//}

//void PC485_Respond(void)
//{
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //使能串口2发送
//	HAL_Delay(2);
//	HAL_UART_AbortReceive_IT(&huart1);
//	UART2_TxBuf[0]=0xF1;
//	UART2_TxBuf[1]=0xF1;
//	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)         //ACK应答
//	{
//		Error_Handler();
//	}
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//使能接收
//}

//void PC485_Update(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
//{
//	if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FIMV_NO_SWEEP) // I-t R-t V-t 模式
//	{ 
//		PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);		          //PC485上传数据
//	}
//	else if(pTestPara->testMode==MODE_FVMI_SWEEP)                        //I-V
//	{
//		if(pTestPara->V_Start<=pTestPara->V_End)                    //起始电压值小于结束电压是正扫
//		{
//			if(pTestPara->V_Now<=pTestPara->V_End)                //如果当前电压小于结束值       
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
//				pTestPara->V_Now+=pTestPara->V_Step;						      //设置下一次输出电压
//				if(pTestPara->V_Now>pTestPara->V_End)								//扫描结束																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
//					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
//				}
//			}
//		}
//		else                                                                //起始电压值大于结束电压是反扫
//		{
//			if(pTestPara->V_Now>=pTestPara->V_End)                //如果当前电压值大于结束电压
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
//				pTestPara->V_Now-=pTestPara->V_Step;						      //设置下一次输出电压
//				if(pTestPara->V_Now<pTestPara->V_End)		            //扫描结束																										//扫描结束																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
//					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
//				}
//			}
//		}
//	}	
//	else if(pTestPara->testMode==MODE_FIMV_SWEEP)                        //V-I
//	{
//		if(pTestPara->I_Start<=pTestPara->I_End)                    //起始电流值小于结束电流是正扫
//		{
//			if(pTestPara->I_Now<=pTestPara->I_End)                //如果当前电流小于结束值       
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
//				pTestPara->I_Now+=pTestPara->V_Step;						      //设置下一次输出电流
//				if(pTestPara->I_Now>pTestPara->I_End)								//扫描结束																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
//					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
//				}
//			}
//		}
//		else                                                                //起始电流值大于结束电流值是反扫
//		{
//			if(pTestPara->I_Now>=pTestPara->I_End)                //如果当前电流值大于结束电流
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //传递未结束的标志和电压电流值
//				pTestPara->I_Now-=pTestPara->I_Step;						      //设置下一次输出电流
//				if(pTestPara->I_Now<pTestPara->I_End)		            //扫描结束																										//扫描结束																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC输出数据中断关闭		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC采集中断关闭
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //结束测试,发送最后一组F3开头的命令
//					ClearSignOutVol(&Relay);																			//输出电压清零和断开输出
//				}
//			}
//		}
//	}
//}



