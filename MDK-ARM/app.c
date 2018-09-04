#include "main.h"
#include "AD5689R.h"
#include "relay.h"
#include "para_init.h"
#include "app.h"
#include "hardware.h"
#include "calculation.h"

extern uint8_t Uart2RxBuf[];

void ClearUartBuf(uint8_t length, uint8_t *pUartRxBuf)
{
	uint8_t i;
	for(i=0;i<length;i++)
	{
		*(pUartRxBuf+i)=0;
	}
}

void SetRange(Relay_TypeDef* pRelay)          //判断是否是自动换挡
{
	if(pRelay->rangeMode!=RELAY_RANGE_AUTO)			//如果不是自动换挡
	{
		 SetRangeRelayDirect(pRelay->rangeNow);		//设置档位，后边有延时故不再加延时
	}
	else
	{
		SetRangeRelayDirect(pRelay->maxRange);		//设置到最高挡位容易平衡
	}
}

void SetOutput(TestPara_TypeDef* pTestPara)
{
	if(pTestPara->testMode==MODE_FIMV_NO_SWEEP||pTestPara->testMode==MODE_FIMV_SWEEP)
	{
	  OutputVoltage(MODE_FVMI_SWEEP, pTestPara->V_Now, TIMER_RESTART_ON);
	}
	else if(pTestPara->testMode==MODE_FVMI_NO_SWEEP||pTestPara->testMode==MODE_FVMI_SWEEP)
	{
		//OutputCurrent(pTestPara->testMode, pTestPara->I_Now);
	}
}

void OutputVoltage(enum TestMode testMode, int voltage, int timerRestart)
{
	float outputValue;
	uint8_t tempRelayVoltageScale=Relay.DUT_VoltageScale;
	if(voltage>-2400 && voltage<2400)			//protect output overflow considering adjustment
	{
		Relay.DUT_VoltageScale=RELAY_INPUT_SCALING_1X;
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_1X);			
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat))/5*65535;
	}
	else
	{
		Relay.DUT_VoltageScale=RELAY_INPUT_SCALING_11X;
		RelaySetInputScaling(testMode, RELAY_INPUT_SCALING_11X);
		outputValue=(2.5-((float)voltage/1000*Adj_OutputLinear.numFloat+Adj_OutputOffset.numFloat)/11)/5*65535;
	}
	AD5689R_WriteIR(&hAD5689R1, CH_A, outputValue);
	//如果挡位有变化&&在测量过程中重启采集时间定时器&&是在扫描模式下
	if(tempRelayVoltageScale!=Relay.DUT_VoltageScale&&timerRestart==TIMER_RESTART_ON&&(testMode==MODE_FVMI_SWEEP||testMode==MODE_FIMV_SWEEP))
	{
		HAL_TIM_Base_Stop_IT(&htim2);			//此处写法防止在换挡时数据传输出错
		HAL_TIM_Base_Stop_IT(&htim3);
		HAL_Delay(1000);
		HAL_TIM_Base_Start_IT(&htim3);		//换挡已经耽误很多时间，所以直接启动测量
		StartNextSampling();							//开启Timer2的采集
	}
}

void StartNextSampling(void)
{
	//delay_us(1000);												//与TIM3的开始计时时间拉开
	htim2.Instance->CNT=0;								//计数器清零
	//HAL_UART_AbortReceive_IT(&huart2); 	//Timer2优先级更高
	//HAL_NVIC_DisableIRQ(USART2_IRQn);		//采集开始后停止响应串口中断		
	HAL_TIM_Base_Start_IT(&htim2);				//输出一次才开始AD采集的中断
}

void OutputCurrent(enum TestMode testMode, int16_t current, uint8_t currentUnit, uint8_t selectedRange)
{
	float outputValue;
	if(selectedRange==RELAY_RANGE_AUTO)
	{
		float realCurrent=(float)abs(current)*MyPow(0.1, 3*currentUnit+3);
		float limitCurrent=1.25e-9;
		uint8_t tempRange=9;
		while(limitCurrent<realCurrent)
		{
			limitCurrent*=10;					//电流切换到更大挡位
			tempRange--;							//量程换到电阻更小挡位
		}
		outputValue=current*MyPow(10, tempRange-3*currentUnit-1); //单位mV
		SetRangeRelayDirect(tempRange);
	}
	else
	{
		outputValue=current*MyPow(10, selectedRange-3*currentUnit-1); //单位mV
		SetRangeRelayDirect(selectedRange);
	}
	OutputVoltage(testMode, outputValue, TIMER_RESTART_ON);
}

uint8_t GetMsgType(uint8_t *UartRxBuf)
{
	if(UartRxBuf[0]==DEV_ADDR||UartRxBuf[0]==BROCST_ADDR)			//如果是设备地址或者是广播地址
	{
		uint8_t xorCheck=0;
		for(uint8_t j=0;j<UART_RX_LEN-1;j++)                //将接收到的数据按照协议进行校验
		{
			xorCheck^=UartRxBuf[j];
		}
		if(xorCheck==UartRxBuf[UART_RX_LEN-1])
		{
				return UartRxBuf[1];
		}
		else 
			return (uint8_t)MSG_TYPE_NULL;
	}
	else 
		return (uint8_t)MSG_TYPE_NULL;
}
