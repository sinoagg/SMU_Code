#include "PC485_TX_protocol.h"

#define DEV_ADDR 0x01																						//��λ���������ݳ���

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
	for(uint8_t j=0;j<UART_TX_LEN-1;j++)                //�����յ������ݰ���Э�����У��
	{
		xorCheck^=*(pUartTxBuf+j);
	}
	*(pUartTxBuf+19)=xorCheck;
}

//void PC485_TX_Message(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, uint8_t EndOfTest)
//{
//	pTestResult->test_V.numFloat*=1000;                   //��ѹתΪmV���д���
//	if(pTestPara->testMode==MODE_FVMI_SWEEP||pTestPara->testMode==MODE_FVMI_NO_SWEEP)
//	{
//		UART2_TxBuf[2]=pTestResult->test_I.numUchar[3];     //�ɼ�����
//		UART2_TxBuf[3]=pTestResult->test_I.numUchar[2];
//		UART2_TxBuf[4]=pTestResult->test_I.numUchar[1];
//		UART2_TxBuf[5]=pTestResult->test_I.numUchar[0];
//		UART2_TxBuf[7]=pTestResult->test_V.numUchar[3];     //��ص�ѹ
//		UART2_TxBuf[8]=pTestResult->test_V.numUchar[2];	
//		UART2_TxBuf[9]=pTestResult->test_V.numUchar[1];																		
//		UART2_TxBuf[10]=pTestResult->test_V.numUchar[0];																		
//	}
//	else if(pTestPara->testMode==MODE_FIMV_SWEEP||pTestPara->testMode==MODE_FIMV_NO_SWEEP)
//	{
//		UART2_TxBuf[2]=pTestResult->test_V.numUchar[3];     //�ɼ���ѹ
//		UART2_TxBuf[3]=pTestResult->test_V.numUchar[2];
//		UART2_TxBuf[4]=pTestResult->test_V.numUchar[1];
//		UART2_TxBuf[5]=pTestResult->test_V.numUchar[0];
//		UART2_TxBuf[7]=pTestResult->test_I.numUchar[3];     //��ص���
//		UART2_TxBuf[8]=pTestResult->test_I.numUchar[2];	
//		UART2_TxBuf[9]=pTestResult->test_I.numUchar[1];																		
//		UART2_TxBuf[10]=pTestResult->test_I.numUchar[0];		
//	}
//	UART2_TxBuf[0]=EndOfTest;																							//�����Ƿ�Ϊ���һ������	
//	UART2_TxBuf[1]=EndOfTest;																							//�����Ƿ�Ϊ���һ������																
//	UART2_TxBuf[6]=Relay.relay_range;
//	
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //ʹ�ܴ���2����
//	HAL_UART_AbortReceive_IT(&huart1);
//	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)
//	{
//		Error_Handler();
//	}		
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//ʹ�ܽ���
//}

//void PC485_Respond(void)
//{
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET); //ʹ�ܴ���2����
//	HAL_Delay(2);
//	HAL_UART_AbortReceive_IT(&huart1);
//	UART2_TxBuf[0]=0xF1;
//	UART2_TxBuf[1]=0xF1;
//	if(HAL_UART_Transmit(&huart2, UART2_TxBuf, 11, 1000)!=HAL_OK)         //ACKӦ��
//	{
//		Error_Handler();
//	}
//	HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);//ʹ�ܽ���
//}

//void PC485_Update(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult)
//{
//	if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FIMV_NO_SWEEP) // I-t R-t V-t ģʽ
//	{ 
//		PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);		          //PC485�ϴ�����
//	}
//	else if(pTestPara->testMode==MODE_FVMI_SWEEP)                        //I-V
//	{
//		if(pTestPara->V_Start<=pTestPara->V_End)                    //��ʼ��ѹֵС�ڽ�����ѹ����ɨ
//		{
//			if(pTestPara->V_Now<=pTestPara->V_End)                //�����ǰ��ѹС�ڽ���ֵ       
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
//				pTestPara->V_Now+=pTestPara->V_Step;						      //������һ�������ѹ
//				if(pTestPara->V_Now>pTestPara->V_End)								//ɨ�����																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
//					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
//				}
//			}
//		}
//		else                                                                //��ʼ��ѹֵ���ڽ�����ѹ�Ƿ�ɨ
//		{
//			if(pTestPara->V_Now>=pTestPara->V_End)                //�����ǰ��ѹֵ���ڽ�����ѹ
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
//				pTestPara->V_Now-=pTestPara->V_Step;						      //������һ�������ѹ
//				if(pTestPara->V_Now<pTestPara->V_End)		            //ɨ�����																										//ɨ�����																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
//					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
//				}
//			}
//		}
//	}	
//	else if(pTestPara->testMode==MODE_FIMV_SWEEP)                        //V-I
//	{
//		if(pTestPara->I_Start<=pTestPara->I_End)                    //��ʼ����ֵС�ڽ�����������ɨ
//		{
//			if(pTestPara->I_Now<=pTestPara->I_End)                //�����ǰ����С�ڽ���ֵ       
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
//				pTestPara->I_Now+=pTestPara->V_Step;						      //������һ���������
//				if(pTestPara->I_Now>pTestPara->I_End)								//ɨ�����																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
//					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
//				}
//			}
//		}
//		else                                                                //��ʼ����ֵ���ڽ�������ֵ�Ƿ�ɨ
//		{
//			if(pTestPara->I_Now>=pTestPara->I_End)                //�����ǰ����ֵ���ڽ�������
//			{
//				PC485_TX_Message(pTestPara, pTestResult, NOTENDOFTEST);         //����δ�����ı�־�͵�ѹ����ֵ
//				pTestPara->I_Now-=pTestPara->I_Step;						      //������һ���������
//				if(pTestPara->I_Now<pTestPara->I_End)		            //ɨ�����																										//ɨ�����																												
//				{
//					HAL_TIM_Base_Stop_IT(&htim3);																	//DAC��������жϹر�		
//					HAL_TIM_Base_Stop_IT(&htim2);	                                //ADC�ɼ��жϹر�
//					HAL_Delay(100);
//					pTestPara->testStatus=OFF;
//					PC485_TX_Message(pTestPara, pTestResult, ENDOFTEST);			    //��������,�������һ��F3��ͷ������
//					ClearSignOutVol(&Relay);																			//�����ѹ����ͶϿ����
//				}
//			}
//		}
//	}
//}



