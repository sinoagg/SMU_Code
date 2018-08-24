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

#define DEV_ADDR 0x01																									//´ËÉè±¸485×ÜÏßµØÖ·£¬Í¬×ÜÏß²úÆ·µØÖ·²»ÄÜÏàÍ¬
#define UART_RX_LEN 30																								//ÉÏÎ»»úÅäÖÃÊý¾Ý³¤¶È
#define UART_TX_LEN 20	

#define ID "3102002032031000001"
//#define UNDER_DEBUG
/* USER CODE END Includes */
void OutputNextVoltage(void);
void OutputNextCurrent(void);
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
enum MsgType msgType=MSG_TYPE_NULL;
enum ADC_Status ADC_status=ADC_BUSY;                      	//ADC²É¼¯×´Ì¬
TestPara_TypeDef TestPara;                                	//²âÁ¿²ÎÊý´æÈ¡
TestResult_TypeDef TestResult;                            	//²âÁ¿½á¹û´æÈ¡														
uint8_t Uart2RxBuf[512]={0};                              	//½ÓÊÕPC¶ËµÄÉèÖÃÃüÁîºÍÐÐ¶¯ÃüÁî
uint8_t Uart2TxBuf[512]={0};                              	//ÏòPC·¢ËÍ²É¼¯µ½µÄÊý¾Ý
uint8_t RxComplete=0;
uint8_t TxComplete=0;
int16_t quietTimeTick=0;                      							//ADC²É¼¯×´Ì¬
uint8_t firstDataReady=0;																		//µÚÒ»¸öÊý¾Ý¾ÍÎ»£¬Ö»ÓÐµÚÒ»¸öÊý¾Ý¾ÍÎ»ºóÉ¨ÃèµçÑ¹²ÅÄÜÔö¼Ó£¬·ñÔò³öÏÖÉ¨ÃèµçÑ¹Ìø¹ýµÚÒ»¸öµãµÄÇé¿ö

int main(void)
{
	uint8_t resultNum=0;
	Hardware_Init();
	//Get adjustment data
	InitTestPara(&TestPara);
	OutputVoltage(MODE_FVMI_SWEEP, 0, &Relay);											//»Ö¸´µ½0µçÑ¹³õÊ¼×´Ì
	#ifdef UNDER_DEBUG
		ConnectOutput();
		SetRangeRelay(9);
	#endif
	HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN);
	
	while (1)
  {	
		if(msgType!=MSG_TYPE_NULL)																			//Èç¹ûÉè±¸ÊÕµ½ÐÂµÄÃüÁî
		{
			//HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);									//Ö¸Ê¾µÆÉÁË¸
			if(msgType==MSG_TYPE_SETTING)																  //Èç¹ûÊÇÉèÖÃÃüÁî
			{
					GetTestPara(&TestPara, &Relay, Uart2RxBuf);								//½ÓÊÕ²¢×ª»¯²âÊÔ²ÎÊý
					//´Ë´¦ÏÈ²âÊÔµçÑ¹Öµ£¬ºó²âÊÔµçÁ÷Öµ
					OutputVoltage(TestPara.testMode, TestPara.V_Now, &Relay);
			}
			else if(msgType==MSG_TYPE_RUN)                            //Èç¹ûÊÇÐÐ¶¯ÃüÁî
			{
				TestPara.testCmd=CMD_START;
				TestResult.endOfTest=0;
				resultNum=0;
				firstDataReady=0;
				InitTestResult(&TestResult);													//Çå¿Õ²âÁ¿½á¹û
				GetRelayPara(&TestPara, &Relay);											//ÉèÖÃ²âÁ¿Ä£Ê½Á¿³ÌºÍ·Å´ó¼ÌµçÆ÷	
				ConnectOutput();																			//¼È³äµ±ÑÓÊ±ÓÖ²âÊÔµµÎ»£¬Ä¿Ç°Ö»ÊÇ²âÊÔADCÖµ
				SetTimerPara(&TestPara);															//ÉèÖÃ¶¨Ê±Æ÷²ÎÊýºÍÑ¡¶¨´óÐ¡µµÎ»
				SetTimerAction(&TestPara);	                					//¶¨Ê±Æ÷¶¯×÷							
			}
			else if(msgType==MSG_TYPE_STOP)													//Èç¹ûÊÇÍ£Ö¹ÃüÁî
			{
				TestPara.testCmd=CMD_STOP;
				ADC_status=ADC_BUSY;																	//Í£Ö¹USB´«Êä
				SetTimerAction(&TestPara);														//Í£Ö¹¶¨Ê±Æ÷
				DisconnectOutput();
				OutputVoltage(MODE_FVMI_SWEEP, 0, &Relay);						//»Ö¸´µ½0µçÑ¹³õÊ¼×´Ì
			}
			else if(msgType==MSG_TYPE_QUERY)												//Èç¹ûÊÇ²éÑ¯ÃüÁî
			{
					if(resultNum>0)
					{
						Uart2TxBuf[0]=DEV_ADDR;                        												//Éè±¸µØÖ·
						HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//Ê¹ÄÜ·¢ËÍ
						HAL_UART_Transmit_DMA(&huart2, Uart2TxBuf, resultNum*UART_TX_LEN);		//·¢ËÍËùÓÐ²âÁ¿Öµ
						resultNum=0;																													//ËùÓÐ½á¹ûÒÑ·¢ËÍ£¬¹éÁã
					}
			}
			else if(msgType==MSG_TYPE_REPORT_ID)
			{
				Uart2TxBuf[0]=DEV_ADDR;
				sprintf((char*)Uart2TxBuf+1, (const char*)ID);
				HAL_Delay(Uart2TxBuf[UART_TX_LEN-1]*50+50);														//×îºóÒ»Î»ÊÇÐòÁÐºÅ ´Ó50ms¿ªÊ¼£¬Ò»Ö±µ½550msÌØÕ÷ÑÓÊ±
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//Ê¹ÄÜ´®¿Ú1·¢ËÍ
				HAL_UART_Transmit(&huart2, Uart2TxBuf, UART_TX_LEN, 1000);
				HAL_Delay(10);
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
			}
			msgType=MSG_TYPE_NULL;																									//Çå¿ÕÏûÏ¢
		}	

		if(TxComplete==1)
		{
			TxComplete=0;
			//HAL_Delay(2);			//ËäÈ»µ¥Æ¬»ú·¢ÍêÁË£¬µ«ÊÇ485Ð¾Æ¬²¢Ã»ÓÐ·¢Íê
			HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);	//Ê¹ÄÜ485½ÓÊÕ
		}
		if(RxComplete==1)
		{
			HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN+1);
			RxComplete=0;
			if(Uart2RxBuf[0]==DEV_ADDR||Uart2RxBuf[0]==0x00)			//Èç¹ûÊÇÉè±¸µØÖ·»òÕßÊÇ¹ã²¥µØÖ·
			{
				uint8_t xorCheck=0;
				for(uint8_t j=0;j<UART_RX_LEN-1;j++)                //½«½ÓÊÕµ½µÄÊý¾Ý°´ÕÕÐ­Òé½øÐÐÐ£Ñé
				{
					xorCheck^=Uart2RxBuf[j];
				}
				if(xorCheck==Uart2RxBuf[UART_RX_LEN-1])
				{
						msgType=(enum MsgType)Uart2RxBuf[1];
				}
			}
		}
		
		if(ADC_status==ADC_READY)																		//Èç¹û¶ÁÈ¡1´ÎADC¹ý³ÌÍê³É
		{
			ADC_status=ADC_BUSY;
			if(RelayCheck(TestPara.testMode, &TestResult, &Relay)==0)	//Èç¹ûµµÎ»ÕýÈ·
		  {
				if(Do_Calculation(&TestPara, &TestResult, &Relay)==0)		//µÃµ½Ò»´ÎÆ½¾ù½á¹û
				{
					HAL_TIM_Base_Stop_IT(&htim2);													//µ±Ç°Êý¾Ý»ñÈ¡Íê±Ï£¬Í£Ö¹²ÉÑù¶¨Ê±Æ÷
					//Ö»ÓÐÍ£Ö¹²É¼¯Ê±²ÅÏìÓ¦´®¿ÚÖÐ¶Ï
					HAL_NVIC_EnableIRQ(USART1_IRQn);
					//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);                                     //ADC×´Ì¬Ö¸Ê¾µÆ
					Relay.rangeChangeTimes=0;
					//TestResult.I_avg.numFloat=Commit_Adjustment(TestResult.I_avg.numFloat, &Relay);	//¶Ô²É¼¯µ½µÄµçÁ÷½øÐÐÊý¾Ý´¦Àí
					if(quietTimeTick==-1 && resultNum<10)
					{
						prepareTxData(&TestPara, &TestResult, Uart2TxBuf+resultNum*UART_TX_LEN);
						resultNum++;
						firstDataReady=1;
						if(TestPara.testMode==MODE_FVMI_SWEEP)
						{
							OutputNextVoltage();
						}
						else if(TestPara.testMode==MODE_FIMV_SWEEP)
						{
							OutputNextCurrent();
						}
					}
					InitTestResult(&TestResult);
				}
			}
			else
			{
				ADC_status=ADC_BUSY;
				InitTestResult(&TestResult);											//»»µ²ÐèÒªÇå¿Õ²âÁ¿½á¹û
				HAL_TIM_Base_Start_IT(&htim2);
			}
		}
	}
  /* USER CODE END 3 */

}
/* USER CODE BEGIN 4 */
void EnterEndofTest(void)
{
	HAL_TIM_Base_Stop_IT(&htim3);																		//DACÊä³öÊý¾ÝÖÐ¶Ï¹Ø±Õ		
	HAL_TIM_Base_Stop_IT(&htim2);	
	TestPara.testStatus=OFF;
	OutputVoltage(MODE_FVMI_SWEEP, 0, &Relay);											//»Ö¸´µ½0µçÑ¹³õÊ¼×´Ì¬
	//ClearAllVoltage();			Disconnect();																				//Êä³öµçÑ¹ÇåÁã
}

void StartNextSampling(void)
{
	delay_us(2);											//ÓëTIM3µÄ¿ªÊ¼¼ÆÊ±Ê±¼äÀ­¿ª
	htim2.Instance->CNT=0;						//¼ÆÊýÆ÷ÇåÁã
	HAL_NVIC_DisableIRQ(USART1_IRQn);	//²É¼¯¿ªÊ¼ºóÍ£Ö¹ÏìÓ¦´®¿ÚÖÐ¶Ï		
	HAL_TIM_Base_Start_IT(&htim2);		//Êä³öÒ»´Î²Å¿ªÊ¼AD²É¼¯µÄÖÐ¶Ï
}

void OutputNextVoltage(void)		
{
	if(TestPara.V_Start<=TestPara.V_End) 
	{						
		if(firstDataReady==1)			//Ö»ÓÐ¹ýÁË¾²Ä¬Ê±¼ä²ÅÄÜÔö¼Ó²¢ÇÒµÚÒ»¸öÊý¾Ý¾ÍÎ»
			TestPara.V_Now+=abs(TestPara.V_Step); //ÉèÖÃÐÂµÄÂ©¼«µçÑ¹
		if(TestPara.V_Now>TestPara.V_End) EnterEndofTest();
		else
		{
			OutputVoltage(TestPara.testMode, TestPara.V_Now, &Relay);
		}
	
		if(TestPara.V_Now+abs(TestPara.V_Step)>TestPara.V_End) TestResult.endOfTest=1;
	}
	else
	{
			if(firstDataReady==1)
				TestPara.V_Now-=abs(TestPara.V_Step); //ÉèÖÃÐÂµÄÂ©¼«µçÑ¹
			if(TestPara.V_Now<TestPara.V_End)	EnterEndofTest();
			else
			{
				OutputVoltage(TestPara.testMode, TestPara.V_Now, &Relay);	
			}
		if(TestPara.V_Now-abs(TestPara.V_Step)<TestPara.V_End) TestResult.endOfTest=1;																											
	}
}

void OutputNextCurrent(void)		
{
	if(TestPara.I_Start<=TestPara.I_End) 
	{						
		if(firstDataReady==1)			//Ö»ÓÐ¹ýÁË¾²Ä¬Ê±¼ä²ÅÄÜÔö¼Ó²¢ÇÒµÚÒ»¸öÊý¾Ý¾ÍÎ»
			TestPara.I_Now+=abs(TestPara.I_Step); //ÉèÖÃÐÂµÄÂ©¼«µçÑ¹
		if(TestPara.I_Now>TestPara.I_End) EnterEndofTest();
		else
		{
			OutputVoltage(TestPara.testMode, TestPara.I_Now, &Relay);
		}
	
		if(TestPara.I_Now+abs(TestPara.I_Step)>TestPara.I_End) TestResult.endOfTest=1;
	}
	else
	{
			if(firstDataReady==1)
				TestPara.I_Now-=abs(TestPara.I_Step); //ÉèÖÃÐÂµÄÂ©¼«µçÑ¹
			if(TestPara.I_Now<TestPara.I_End)	EnterEndofTest();
			else
			{
				OutputVoltage(TestPara.testMode, TestPara.I_Now, &Relay);	
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
			//²»¹ÜÊ²Ã´Çé¿ö£¬¾ùÐè²É¼¯µçÑ¹ºÍµçÁ÷
			//TestResult.V_sample=AD7988_1_ReadData(&hAD7988_1_2);										//ADCµçÑ¹²É¼¯Êý¾Ý	
			TestResult.I_sample=AD7988_1_ReadData(&hAD7988_1_1);										//ADCµçÁ÷²É¼¯Êý¾Ý
			ADC_status=ADC_READY;
    }
	  else if (htim->Instance == htim3.Instance)
		{
			StartNextSampling();
		}
		else if (htim->Instance == htim4.Instance)		//DAC¶¨Ê±Æ÷
		{
				quietTimeTick++;
				if(quietTimeTick>=TestPara.quietTime)
				{
					HAL_TIM_Base_Stop_IT(&htim4);
					HAL_TIM_Base_Stop_IT(&htim2);
					ADC_status=ADC_BUSY;
					InitTestResult(&TestResult);
					quietTimeTick=-1;										//ÒÑ¾­µ½´ïquietTime¼ÆÊýÊ±¼ä
					HAL_TIM_Base_Start_IT(&htim3);			//¿ªÊ¼É¨Ãè
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
