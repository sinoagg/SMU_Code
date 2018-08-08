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

#define DEV_ADDR 0x01																									//此设备485总线地址，同总线产品地址不能相同
#define UART_RX_LEN 30																								//上位机配置数据长度
#define UART_TX_LEN 20	

#define ID "3102002032031000001"
//#define UNDER_DEBUG
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
enum MsgType msgType=MSG_TYPE_NULL;
enum ADC_Status ADC_status=ADC_BUSY;                      	//ADC采集状态
TestPara_TypeDef TestPara;                                	//测量参数存取
TestResult_TypeDef TestResult;                            	//测量结果存取														
uint8_t Uart2RxBuf[32]={0};                              		//接收PC端的设置命令和行动命令
uint8_t Uart2TxBuf[128]={0};                              	//向PC发送采集到的数据

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* USER CODE BEGIN 2 */
	Hardware_Init();
	//Get adjustment data
	InitTestPara(&TestPara);
	uint8_t resultNum=0;
	#ifdef UNDER_DEBUG
		ConnectOutput();
		SetRelay(9);
	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
		
  /* USER CODE BEGIN 3 */	
		if(msgType!=MSG_TYPE_NULL)																			//如果设备收到新的命令
		{
			//HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);									//指示灯闪烁
			if(msgType==MSG_TYPE_SETTING)																  //如果是设置命令
			{
					GetTestPara(&TestPara, &Relay, Uart2RxBuf);								//接收并转化测试参数
					InitTestResult(&TestResult);															//清空测量结果
					SetTimerPara(&TestPara);																	//设置定时器参数和选定大小档位
					GetRelayPara(&TestPara, &Relay);													//设置测量模式量程和放大继电器
					//此处先测试电压值，后测试电流值
					OutputVoltage(TestPara.testMode, TestPara.V_Now);
			}
			else if(msgType==MSG_TYPE_RUN)                            //如果是行动命令
			{
				TestPara.testCmd=CMD_START;
				resultNum=0;
				ConnectOutput();
				//HAL_Delay(pTestPara->quietTime);										//等待静默时间
				//Test_Relay();																			  //既充当延时又测试档位，目前只是测试ADC值
				SetTimerAction(&TestPara);	                					//定时器动作							
			}
			else if(msgType==MSG_TYPE_STOP)													//如果是停止命令
			{
				TestPara.testCmd=CMD_STOP;
				ADC_status=ADC_BUSY;																	  //停止USB传输
				SetTimerAction(&TestPara);														//停止定时器
				DisconnectOutput();
			}
			else if(msgType==MSG_TYPE_QUERY)												//如果是查询命令
			{
					if(resultNum>0&&TestPara.testStatus==ON)
					{
						Uart2TxBuf[0]=DEV_ADDR;                        												//设备地址
						HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);		//使能发送
						HAL_UART_Transmit(&huart2, Uart2TxBuf, resultNum*UART_TX_LEN, 1000);	//发送所有测量值
						resultNum=0;																													//所有结果已发送，归零
						delay_us(10);
						HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);	//使能485接收
					}
			}
			else if(msgType==MSG_TYPE_REPORT_ID)
			{
				Uart2TxBuf[0]=DEV_ADDR;
				sprintf((char*)Uart2TxBuf+1, (const char*)ID);
				HAL_Delay(Uart2TxBuf[UART_TX_LEN-1]*50+50);														//最后一位是序列号 从50ms开始，一直到550ms特征延时
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);//使能串口1发送
				HAL_UART_Transmit(&huart2, Uart2TxBuf, UART_TX_LEN, 1000);
				HAL_Delay(10);
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
			}
			msgType=MSG_TYPE_NULL;																				//清空消息
		}
		HAL_UART_Receive_IT(&huart2,Uart2RxBuf,UART_RX_LEN);	              //持续接收串口1 RS485数据
		
		if(ADC_status==ADC_READY)																							//如果读取1次ADC过程完成
		{
			ADC_status=ADC_BUSY;
			if(RelayCheck(TestPara.testMode, &TestResult, &Relay)==0)//如果档位正确
		  {
				if(Do_Calculation(&TestPara, &TestResult, &Relay)==0)	//得到一次平均结果
				{
					//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);                                     //ADC状态指示灯
					if(resultNum<5)
					{
						prepareTxData(&TestPara, &TestResult, Uart2TxBuf+resultNum*UART_TX_LEN);
						resultNum++;
					}
					InitTestResult(&TestResult);
				}
			}
			else
				InitTestResult(&TestResult);											//换挡需要清空测量结果
		}
	}
  /* USER CODE END 3 */

}
/* USER CODE BEGIN 4 */
//void UartTx()
//{
	
//}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		if (htim->Instance == htim2.Instance)
    {
			//不管什么情况，均需采集电压和电流
			TestResult.V_sample=AD7988_1_ReadData(&hAD7988_1_2);										//ADC电压采集数据	
			TestResult.I_sample=AD7988_1_ReadData(&hAD7988_1_1);										//ADC电流采集数据
			ADC_status=ADC_READY;
    }
	  else if (htim->Instance == htim3.Instance)
		{
			if(TestPara.testMode==MODE_FVMI_SWEEP)
			{
				//TestPara.V_Now+=TestPara.V_Step;
				//OutputVoltage(TestPara.testMode, TestPara.V_Now);
			}
			else if(TestPara.testMode==MODE_FIMV_SWEEP)
			{
				//OutputCurrent(TestPara.testMode, TestPara.I_Now);
			}
			HAL_TIM_Base_Start_IT(&htim2);															//输出一次才开始AD采集的中断
			//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);                     //ADC状态指示灯
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	if(huart->Instance == huart2.Instance)													//485接口收到指令
	{
		if(Uart2RxBuf[0]==DEV_ADDR||Uart2RxBuf[0]==0x00)			//如果是设备地址或者是广播地址
		{
			uint8_t xorCheck=0;
			for(uint8_t j=0;j<UART_RX_LEN-1;j++)                //将接收到的数据按照协议进行校验
			{
				xorCheck^=Uart2RxBuf[j];
			}
			if(xorCheck==Uart2RxBuf[UART_RX_LEN-1])
			{
					msgType=(enum MsgType)Uart2RxBuf[1];
		  }
	  }
	}	
}

/* USER CODE END 4 */
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
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
