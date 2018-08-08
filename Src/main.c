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

#define DEV_ADDR 0x01																									//���豸485���ߵ�ַ��ͬ���߲�Ʒ��ַ������ͬ
#define UART_RX_LEN 30																								//��λ���������ݳ���
#define UART_TX_LEN 20	

#define ID "3102002032031000001"
//#define UNDER_DEBUG
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
enum MsgType msgType=MSG_TYPE_NULL;
enum ADC_Status ADC_status=ADC_BUSY;                      	//ADC�ɼ�״̬
TestPara_TypeDef TestPara;                                	//����������ȡ
TestResult_TypeDef TestResult;                            	//���������ȡ														
uint8_t Uart2RxBuf[32]={0};                              		//����PC�˵�����������ж�����
uint8_t Uart2TxBuf[128]={0};                              	//��PC���Ͳɼ���������

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
		if(msgType!=MSG_TYPE_NULL)																			//����豸�յ��µ�����
		{
			//HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);									//ָʾ����˸
			if(msgType==MSG_TYPE_SETTING)																  //�������������
			{
					GetTestPara(&TestPara, &Relay, Uart2RxBuf);								//���ղ�ת�����Բ���
					InitTestResult(&TestResult);															//��ղ������
					SetTimerPara(&TestPara);																	//���ö�ʱ��������ѡ����С��λ
					GetRelayPara(&TestPara, &Relay);													//���ò���ģʽ���̺ͷŴ�̵���
					//�˴��Ȳ��Ե�ѹֵ������Ե���ֵ
					OutputVoltage(TestPara.testMode, TestPara.V_Now);
			}
			else if(msgType==MSG_TYPE_RUN)                            //������ж�����
			{
				TestPara.testCmd=CMD_START;
				resultNum=0;
				ConnectOutput();
				//HAL_Delay(pTestPara->quietTime);										//�ȴ���Ĭʱ��
				//Test_Relay();																			  //�ȳ䵱��ʱ�ֲ��Ե�λ��Ŀǰֻ�ǲ���ADCֵ
				SetTimerAction(&TestPara);	                					//��ʱ������							
			}
			else if(msgType==MSG_TYPE_STOP)													//�����ֹͣ����
			{
				TestPara.testCmd=CMD_STOP;
				ADC_status=ADC_BUSY;																	  //ֹͣUSB����
				SetTimerAction(&TestPara);														//ֹͣ��ʱ��
				DisconnectOutput();
			}
			else if(msgType==MSG_TYPE_QUERY)												//����ǲ�ѯ����
			{
					if(resultNum>0&&TestPara.testStatus==ON)
					{
						Uart2TxBuf[0]=DEV_ADDR;                        												//�豸��ַ
						HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);		//ʹ�ܷ���
						HAL_UART_Transmit(&huart2, Uart2TxBuf, resultNum*UART_TX_LEN, 1000);	//�������в���ֵ
						resultNum=0;																													//���н���ѷ��ͣ�����
						delay_us(10);
						HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);	//ʹ��485����
					}
			}
			else if(msgType==MSG_TYPE_REPORT_ID)
			{
				Uart2TxBuf[0]=DEV_ADDR;
				sprintf((char*)Uart2TxBuf+1, (const char*)ID);
				HAL_Delay(Uart2TxBuf[UART_TX_LEN-1]*50+50);														//���һλ�����к� ��50ms��ʼ��һֱ��550ms������ʱ
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_SET);//ʹ�ܴ���1����
				HAL_UART_Transmit(&huart2, Uart2TxBuf, UART_TX_LEN, 1000);
				HAL_Delay(10);
				HAL_GPIO_WritePin(RS485_RE2_GPIO_Port, RS485_RE2_Pin, GPIO_PIN_RESET);
			}
			msgType=MSG_TYPE_NULL;																				//�����Ϣ
		}
		HAL_UART_Receive_IT(&huart2,Uart2RxBuf,UART_RX_LEN);	              //�������մ���1 RS485����
		
		if(ADC_status==ADC_READY)																							//�����ȡ1��ADC�������
		{
			ADC_status=ADC_BUSY;
			if(RelayCheck(TestPara.testMode, &TestResult, &Relay)==0)//�����λ��ȷ
		  {
				if(Do_Calculation(&TestPara, &TestResult, &Relay)==0)	//�õ�һ��ƽ�����
				{
					//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);                                     //ADC״ָ̬ʾ��
					if(resultNum<5)
					{
						prepareTxData(&TestPara, &TestResult, Uart2TxBuf+resultNum*UART_TX_LEN);
						resultNum++;
					}
					InitTestResult(&TestResult);
				}
			}
			else
				InitTestResult(&TestResult);											//������Ҫ��ղ������
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
			//����ʲô���������ɼ���ѹ�͵���
			TestResult.V_sample=AD7988_1_ReadData(&hAD7988_1_2);										//ADC��ѹ�ɼ�����	
			TestResult.I_sample=AD7988_1_ReadData(&hAD7988_1_1);										//ADC�����ɼ�����
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
			HAL_TIM_Base_Start_IT(&htim2);															//���һ�βſ�ʼAD�ɼ����ж�
			//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);                     //ADC״ָ̬ʾ��
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	if(huart->Instance == huart2.Instance)													//485�ӿ��յ�ָ��
	{
		if(Uart2RxBuf[0]==DEV_ADDR||Uart2RxBuf[0]==0x00)			//������豸��ַ�����ǹ㲥��ַ
		{
			uint8_t xorCheck=0;
			for(uint8_t j=0;j<UART_RX_LEN-1;j++)                //�����յ������ݰ���Э�����У��
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
