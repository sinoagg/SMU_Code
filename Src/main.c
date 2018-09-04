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
enum ADC_Status ADC_status=ADC_BUSY;                      	//ADC采集状态
TestPara_TypeDef TestPara;                                	//测量参数存取
TestResult_TypeDef TestResult;                            	//测量结果存取														
uint8_t Uart2RxBuf[512]={0};                              	//接收PC端的设置命令和行动命令
uint8_t Uart2TxBuf[512]={0};                              	//向PC发送采集到的数据
uint8_t RxComplete=0;
uint8_t TxComplete=1;
int16_t quietTimeTick=0;                      							//ADC采集状态
uint8_t firstDataReady=0;																		//第一个数据就位，只有第一个数据就位后扫描电压才能增加，否则出现扫描电压跳过第一个点的情况


int main(void)
{
	uint8_t* pTxBuf=Uart2TxBuf;
	Hardware_Init();
	//Get adjustment data
	InitTestPara(&TestPara);
	OutputZeroVoltage();	//输出零电压
	HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN); //打开接收
	
	while (1)
  {	
		if(msgType!=MSG_TYPE_NULL)																			//如果设备收到新的命令
		{
			HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);									//指示灯闪烁
			if(msgType==MSG_TYPE_SETTING)																  //如果是设置命令
			{
				GetTestPara(&TestPara, &Relay, Uart2RxBuf);								//接收并转化测试参数
				GetRelayPara(&TestPara, &Relay);													//设置测量模式量程和放大继电器
				if(TestPara.testMode==MODE_FVMI_SWEEP||TestPara.testMode==MODE_FVMI_NO_SWEEP)
					OutputVoltage(TestPara.testMode, TestPara.V_Now, TIMER_RESTART_OFF);
				else if(TestPara.testMode==MODE_FIMV_SWEEP||TestPara.testMode==MODE_FIMV_NO_SWEEP)
					OutputCurrent(TestPara.testMode, TestPara.I_Now, TestPara.I_Unit, Relay.rangeMode);
			}
			else if(msgType==MSG_TYPE_RUN)                            		//如果是行动命令
			{
				TestPara.testCmd=CMD_START;
				TestResult.endOfTest=0;
				pTxBuf=Uart2TxBuf;
				firstDataReady=0;
				ADC_status=ADC_BUSY;
				InitTestResult(&TestResult);													//清空测量结果	
				SetTimerPara(&TestPara);															//设置定时器参数
				SetTimerAction(&TestPara);	                					//定时器动作							
			}
			else if(msgType==MSG_TYPE_STOP)													//如果是停止命令
			{
				TestPara.testCmd=CMD_STOP;
				SetTimerAction(&TestPara);														//停止定时器
				ADC_status=ADC_BUSY;																	//停止计算和传输
				OutputZeroVoltage();									
			}
			//else if(msgType==MSG_TYPE_QUERY)												//如果是查询命令
			//{
					
			//}
			else if(msgType==MSG_TYPE_REPORT_ID)
			{
				Uart2TxBuf[0]=DEV_ADDR;
				sprintf((char*)Uart2TxBuf+1, (const char*)ID);
				HAL_Delay(Uart2TxBuf[UART_TX_LEN-1]*50+50);														//最后一位是序列号 从50ms开始，一直到550ms特征延时
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//使能串口1发送
				HAL_UART_Transmit(&huart2, Uart2TxBuf, UART_TX_LEN, 1000);
				HAL_Delay(10);
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
			}
			msgType=MSG_TYPE_NULL;																									//清空消息
		}	
			
		if(RxComplete==1)
		{
			RxComplete=0;
			msgType=(enum MsgType)GetMsgType(Uart2RxBuf);
			
			if(msgType==MSG_TYPE_QUERY)
			{
				if((pTxBuf!=Uart2TxBuf)&&(TxComplete==1))
				{
					TxComplete=0; //表示DMA有数据在发送
					//HAL_NVIC_DisableIRQ(USART2_IRQn);//只有数据发送好后才响应串口中断
					Uart2TxBuf[0]=DEV_ADDR;                        												//设备地址
					HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);	//使能发送			
					HAL_UART_Transmit_DMA(&huart2, Uart2TxBuf, pTxBuf-Uart2TxBuf);		//发送所有测量值
					HAL_Delay(2);
					pTxBuf=Uart2TxBuf;																												//所有结果已发送，归零
				}
			}	
			HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART_RX_LEN+1);		//此处必须增加接收长度	
		}
		
		if(ADC_status==ADC_READY)																		//如果读取1次ADC过程完成
		{
			if(RelayCheck(TestPara.testMode, &TestResult, &Relay)==0)	//如果档位正确
		  {
				if(Do_Calculation(&TestPara, &TestResult, &Relay)==0)		//得到一次平均结果
				{
					HAL_TIM_Base_Stop_IT(&htim2);													//当前数据获取完毕，停止采样定时器
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);                                     //ADC状态指示灯
					Relay.rangeChangeTimes=0;
					//TestResult.I_avg.numFloat=Commit_Adjustment(TestResult.I_avg.numFloat, &Relay);	//对采集到的电流进行数据处理
					if(quietTimeTick==-1 && (pTxBuf-Uart2TxBuf)<500)									//一次最多传10组数据
					{
						pTxBuf=prepareTxData(&TestPara, &TestResult, pTxBuf);
						firstDataReady=1;	
						InitTestResult(&TestResult);												//用完即刻清零，否则后边可能直接使用
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
				InitTestResult(&TestResult);											//换挡需要清空测量结果
				if(TestPara.testMode==MODE_FVMI_SWEEP||TestPara.testMode==MODE_FIMV_SWEEP)
				{	
					htim3.Instance->CNT=0;
					HAL_TIM_Base_Start_IT(&htim3);
				}	//在IV和VI模式下，如果换挡该挡位重测，不必保证时间一致
				StartNextSampling();
			}
			ADC_status=ADC_BUSY;			//以上情况都处理完，才能开始下一次采样
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
	HAL_TIM_Base_Stop_IT(&htim3);																		//DAC输出数据中断关闭		
	HAL_TIM_Base_Stop_IT(&htim2);
	ADC_status=ADC_BUSY;										//如果不做处理的话可能仍然是ready状态
	TestPara.testStatus=OFF;
	OutputVoltage(MODE_FVMI_SWEEP, 0, TIMER_RESTART_OFF);															//恢复到0电压初始状态
}

void OutputNextVoltage(void)		
{
	if(TestPara.V_Start<=TestPara.V_End) 
	{						
		if(firstDataReady==1)			//只有过了静默时间才能增加并且第一个数据就位
			TestPara.V_Now+=abs(TestPara.V_Step); //设置新的测试电压
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
				TestPara.V_Now-=abs(TestPara.V_Step); //设置新的漏极电压
			if(TestPara.V_Now<TestPara.V_End)	EnterEndofTest();
			else
			{
				OutputVoltage(MODE_FVMI_SWEEP, TestPara.V_Now, TIMER_RESTART_ON);			//sweep和no sweep在此处等效
			}
		if(TestPara.V_Now-abs(TestPara.V_Step)<TestPara.V_End) TestResult.endOfTest=1;																											
	}
}

void OutputNextCurrent(void)		
{
	if(TestPara.I_Start<=TestPara.I_End) 
	{						
		if(firstDataReady==1)			//只有过了静默时间才能增加并且第一个数据就位
			TestPara.I_Now+=abs(TestPara.I_Step); //设置新的漏极电压
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
				TestPara.I_Now-=abs(TestPara.I_Step); //设置新的漏极电压
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
			if(ADC_status==ADC_BUSY)//不管什么情况，均需采集电压和电流
			{	
				TestResult.V_sample=AD7988_1_ReadData(&hAD7988_1_2);										//ADC电压采集数据	
				TestResult.I_sample=AD7988_1_ReadData(&hAD7988_1_1);										//ADC电流采集数据
				ADC_status=ADC_READY;
			}
    }
	  else if (htim->Instance == htim3.Instance)
		{
			StartNextSampling();
		}
		else if (htim->Instance == htim4.Instance)		//DAC定时器
		{	
			quietTimeTick++;
			if(quietTimeTick>=TestPara.quietTime)
			{
				HAL_TIM_Base_Stop_IT(&htim4);
				HAL_TIM_Base_Stop_IT(&htim2);
				ADC_status=ADC_BUSY;
				InitTestResult(&TestResult);
				quietTimeTick=-1;										//已经到达quietTime计数时间
				//MX_TIM4_Init(10);									//调整定时器4的定时间隔1ms，为之后的Tx间隔做准备
				HAL_TIM_Base_Start_IT(&htim3);			//开始扫描
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
