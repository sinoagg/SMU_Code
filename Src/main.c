 /******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "para_init.h"
#include "delay.h"
#include "AD5689R.h"
#include "AD7988_1.h"
#include "prj_typedef.h"
#include "InternalFlash.h"
#include "hardware.h"
#include "timer_ctrl.h"
#include "calculation.h"
#include "PC485_TX_protocol.h"
#include "app.h"

#define ID "3102002032031000001"
//#define UNDER_DEBUG
/* USER CODE END Includes */
void OutputNextVoltage(void);
void OutputNextCurrent(void);
void OutputZeroVoltage(void);

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
enum MsgType msgType=MSG_TYPE_NULL;
enum ADC_Status ADC_status=ADC_BUSY;                      	//ADC�ɼ�״̬
TestPara_TypeDef TestPara;                                	//����������ȡ
TestResult_TypeDef TestResult;                            	//���������ȡ														
uint8_t Uart2RxBuf[512]={0};                              	//����PC�˵�����������ж�����
uint8_t Uart2TxBuf[512]={0};                              	//��PC���Ͳɼ���������
uint8_t RxComplete=0;
uint8_t TxComplete=1;
int16_t quietTimeTick=0;                      							//ADC�ɼ�״̬
uint8_t firstDataReady=0;																		//��һ�����ݾ�λ��ֻ�е�һ�����ݾ�λ��ɨ���ѹ�������ӣ��������ɨ���ѹ������һ��������


int main(void)
{
	uint8_t* pTxBuf=Uart2TxBuf;
	Hardware_Init();
	//Get adjustment data
	InitTestPara(&TestPara);
	OutputZeroVoltage();	//������ѹ
	HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN); //�򿪽���
	
	while (1)
  {	
		if(msgType!=MSG_TYPE_NULL)																			//����豸�յ��µ�����
		{
			HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);									//ָʾ����˸
			if(msgType==MSG_TYPE_SETTING)																  //�������������
			{
				GetTestPara(&TestPara, &Relay, Uart2RxBuf);								//���ղ�ת�����Բ���
				GetRelayPara(&TestPara, &Relay);													//���ò���ģʽ���̺ͷŴ�̵���
				if(TestPara.testMode==MODE_FVMI_SWEEP||TestPara.testMode==MODE_FVMI_NO_SWEEP)
					OutputVoltage(TestPara.testMode, TestPara.V_Now, TIMER_RESTART_OFF);
				else if(TestPara.testMode==MODE_FIMV_SWEEP||TestPara.testMode==MODE_FIMV_NO_SWEEP)
					OutputCurrent(TestPara.testMode, TestPara.I_Now, TestPara.I_Unit, Relay.rangeMode);
			}
			else if(msgType==MSG_TYPE_RUN)                            		//������ж�����
			{
				TestPara.testCmd=CMD_START;
				TestResult.endOfTest=0;
				pTxBuf=Uart2TxBuf;
				firstDataReady=0;
				ADC_status=ADC_BUSY;
				InitTestResult(&TestResult);													//��ղ������	
				SetTimerPara(&TestPara);															//���ö�ʱ������
				SetTimerAction(&TestPara);	                					//��ʱ������							
			}
			else if(msgType==MSG_TYPE_STOP)													//�����ֹͣ����
			{
				TestPara.testCmd=CMD_STOP;
				SetTimerAction(&TestPara);														//ֹͣ��ʱ��
				ADC_status=ADC_BUSY;																	//ֹͣ����ʹ���
				OutputZeroVoltage();									
			}
			//else if(msgType==MSG_TYPE_QUERY)												//����ǲ�ѯ����
			//{
					
			//}
			else if(msgType==MSG_TYPE_REPORT_ID)
			{
				Uart2TxBuf[0]=DEV_ADDR;
				sprintf((char*)Uart2TxBuf+1, (const char*)ID);
				HAL_Delay(Uart2TxBuf[UART_TX_LEN-1]*50+50);														//���һλ�����к� ��50ms��ʼ��һֱ��550ms������ʱ
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//ʹ�ܴ���1����
				HAL_UART_Transmit(&huart2, Uart2TxBuf, UART_TX_LEN, 1000);
				HAL_Delay(10);
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
			}
			msgType=MSG_TYPE_NULL;																									//�����Ϣ
		}	
			
		if(RxComplete==1)
		{
			RxComplete=0;
			msgType=(enum MsgType)GetMsgType(Uart2RxBuf);
			
			if(msgType==MSG_TYPE_QUERY)
			{
				if((pTxBuf!=Uart2TxBuf)&&(TxComplete==1))
				{
					TxComplete=0; //��ʾDMA�������ڷ���
					//HAL_NVIC_DisableIRQ(USART2_IRQn);//ֻ�����ݷ��ͺú����Ӧ�����ж�
					Uart2TxBuf[0]=DEV_ADDR;                        												//�豸��ַ
					HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//ʹ�ܷ���			
					HAL_UART_Transmit_DMA(&huart2, Uart2TxBuf, pTxBuf-Uart2TxBuf);		//�������в���ֵ
					HAL_Delay(2);
					pTxBuf=Uart2TxBuf;																												//���н���ѷ��ͣ�����
				}
			}	
			HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN+1);		//�˴��������ӽ��ճ���	
		}
		
		if(ADC_status==ADC_READY)																		//�����ȡ1��ADC�������
		{
			if(RelayCheck(TestPara.testMode, &TestResult, &Relay)==0)	//�����λ��ȷ
		  {
				if(Do_Calculation(&TestPara, &TestResult, &Relay)==0)		//�õ�һ��ƽ�����
				{
					HAL_TIM_Base_Stop_IT(&htim2);													//��ǰ���ݻ�ȡ��ϣ�ֹͣ������ʱ��
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);                                     //ADC״ָ̬ʾ��
					Relay.rangeChangeTimes=0;
					//TestResult.I_avg.numFloat=Commit_Adjustment(TestResult.I_avg.numFloat, &Relay);	//�Բɼ����ĵ����������ݴ���
					if(quietTimeTick==-1 && (pTxBuf-Uart2TxBuf)<500)									//һ����ഫ10������
					{
						pTxBuf=prepareTxData(&TestPara, &TestResult, pTxBuf);
						firstDataReady=1;	
						InitTestResult(&TestResult);												//���꼴�����㣬�����߿���ֱ��ʹ��
						if(TestPara.testMode==MODE_FVMI_SWEEP)
							OutputNextVoltage();
						else if(TestPara.testMode==MODE_FIMV_SWEEP)
							OutputNextCurrent();
					}
					else
						InitTestResult(&TestResult);
				}
			}
			else
			{
				InitTestResult(&TestResult);											//������Ҫ��ղ������
				if(TestPara.testMode==MODE_FVMI_SWEEP||TestPara.testMode==MODE_FIMV_SWEEP)
				{	
					htim3.Instance->CNT=0;
					HAL_TIM_Base_Start_IT(&htim3);
				}	//��IV��VIģʽ�£���������õ�λ�ز⣬���ر�֤ʱ��һ��
				StartNextSampling();
			}
			ADC_status=ADC_BUSY;			//��������������꣬���ܿ�ʼ��һ�β���
		}
	}
}
/* USER CODE BEGIN 4 */

void OutputZeroVoltage(void)
{
	ConnectOutput();
	OutputVoltage(MODE_FVMI_SWEEP, 0, TIMER_RESTART_OFF);
}

void EnterEndofTest(void)
{
	HAL_TIM_Base_Stop_IT(&htim3);																		//DAC��������жϹر�		
	HAL_TIM_Base_Stop_IT(&htim2);
	ADC_status=ADC_BUSY;										//�����������Ļ�������Ȼ��ready״̬
	TestPara.testStatus=OFF;
	OutputVoltage(MODE_FVMI_SWEEP, 0, TIMER_RESTART_OFF);															//�ָ���0��ѹ��ʼ״̬
}

void OutputNextVoltage(void)		
{
	if(TestPara.V_Start<=TestPara.V_End) 
	{						
		if(firstDataReady==1)			//ֻ�й��˾�Ĭʱ��������Ӳ��ҵ�һ�����ݾ�λ
			TestPara.V_Now+=abs(TestPara.V_Step); //�����µĲ��Ե�ѹ
		if(TestPara.V_Now>TestPara.V_End) EnterEndofTest();
		else
		{
			OutputVoltage(MODE_FVMI_SWEEP, TestPara.V_Now, TIMER_RESTART_ON);
		}
	
		if(TestPara.V_Now+abs(TestPara.V_Step)>TestPara.V_End) TestResult.endOfTest=1;
	}
	else
	{
			if(firstDataReady==1)
				TestPara.V_Now-=abs(TestPara.V_Step); //�����µ�©����ѹ
			if(TestPara.V_Now<TestPara.V_End)	EnterEndofTest();
			else
			{
				OutputVoltage(MODE_FVMI_SWEEP, TestPara.V_Now, TIMER_RESTART_ON);			//sweep��no sweep�ڴ˴���Ч
			}
		if(TestPara.V_Now-abs(TestPara.V_Step)<TestPara.V_End) TestResult.endOfTest=1;																											
	}
}

void OutputNextCurrent(void)		
{
	if(TestPara.I_Start<=TestPara.I_End) 
	{						
		if(firstDataReady==1)			//ֻ�й��˾�Ĭʱ��������Ӳ��ҵ�һ�����ݾ�λ
			TestPara.I_Now+=abs(TestPara.I_Step); //�����µ�©����ѹ
		if(TestPara.I_Now>TestPara.I_End) EnterEndofTest();
		else
		{
			OutputCurrent(TestPara.testMode, TestPara.I_Now, TestPara.I_Unit, Relay.rangeMode);
		}
	
		if(TestPara.I_Now+abs(TestPara.I_Step)>TestPara.I_End) TestResult.endOfTest=1;
	}
	else
	{
			if(firstDataReady==1)
				TestPara.I_Now-=abs(TestPara.I_Step); //�����µ�©����ѹ
			if(TestPara.I_Now<TestPara.I_End)	EnterEndofTest();
			else
			{
				OutputCurrent(TestPara.testMode, TestPara.I_Now, TestPara.I_Unit, Relay.rangeMode);	
			}
		if(TestPara.I_Now-abs(TestPara.I_Step)<TestPara.I_End) TestResult.endOfTest=1;																											
	}
}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		if (htim->Instance == htim2.Instance)
    {
			if(ADC_status==ADC_BUSY)//����ʲô���������ɼ���ѹ�͵���
			{	
				TestResult.V_sample=AD7988_1_ReadData(&hAD7988_1_2);										//ADC��ѹ�ɼ�����	
				TestResult.I_sample=AD7988_1_ReadData(&hAD7988_1_1);										//ADC�����ɼ�����
				ADC_status=ADC_READY;
			}
    }
	  else if (htim->Instance == htim3.Instance)
		{
			StartNextSampling();
		}
		else if (htim->Instance == htim4.Instance)		//DAC��ʱ��
		{	
			quietTimeTick++;
			if(quietTimeTick>=TestPara.quietTime)
			{
				HAL_TIM_Base_Stop_IT(&htim4);
				HAL_TIM_Base_Stop_IT(&htim2);
				ADC_status=ADC_BUSY;
				InitTestResult(&TestResult);
				quietTimeTick=-1;										//�Ѿ�����quietTime����ʱ��
				//MX_TIM4_Init(10);									//������ʱ��4�Ķ�ʱ���1ms��Ϊ֮���Tx�����׼��
				HAL_TIM_Base_Start_IT(&htim3);			//��ʼɨ��
				StartNextSampling();
			}
		}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
