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

void SetRange(Relay_TypeDef* pRelay)          //�ж��Ƿ����Զ�����
{
	if(pRelay->rangeMode!=RELAY_RANGE_AUTO)			//��������Զ�����
	{
		 SetRangeRelayDirect(pRelay->rangeNow);		//���õ�λ���������ʱ�ʲ��ټ���ʱ
	}
	else
	{
		SetRangeRelayDirect(pRelay->maxRange);		//���õ���ߵ�λ����ƽ��
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
	//�����λ�б仯&&�ڲ��������������ɼ�ʱ�䶨ʱ��&&����ɨ��ģʽ��
	if(tempRelayVoltageScale!=Relay.DUT_VoltageScale&&timerRestart==TIMER_RESTART_ON&&(testMode==MODE_FVMI_SWEEP||testMode==MODE_FIMV_SWEEP))
	{
		HAL_TIM_Base_Stop_IT(&htim2);			//�˴�д����ֹ�ڻ���ʱ���ݴ������
		HAL_TIM_Base_Stop_IT(&htim3);
		HAL_Delay(1000);
		HAL_TIM_Base_Start_IT(&htim3);		//�����Ѿ�����ܶ�ʱ�䣬����ֱ����������
		StartNextSampling();							//����Timer2�Ĳɼ�
	}
}

void StartNextSampling(void)
{
	//delay_us(1000);												//��TIM3�Ŀ�ʼ��ʱʱ������
	htim2.Instance->CNT=0;								//����������
	//HAL_UART_AbortReceive_IT(&huart2); 	//Timer2���ȼ�����
	//HAL_NVIC_DisableIRQ(USART2_IRQn);		//�ɼ���ʼ��ֹͣ��Ӧ�����ж�		
	HAL_TIM_Base_Start_IT(&htim2);				//���һ�βſ�ʼAD�ɼ����ж�
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
			limitCurrent*=10;					//�����л�������λ
			tempRange--;							//���̻��������С��λ
		}
		outputValue=current*MyPow(10, tempRange-3*currentUnit-1); //��λmV
		SetRangeRelayDirect(tempRange);
	}
	else
	{
		outputValue=current*MyPow(10, selectedRange-3*currentUnit-1); //��λmV
		SetRangeRelayDirect(selectedRange);
	}
	OutputVoltage(testMode, outputValue, TIMER_RESTART_ON);
}

uint8_t GetMsgType(uint8_t *UartRxBuf)
{
	if(UartRxBuf[0]==DEV_ADDR||UartRxBuf[0]==BROCST_ADDR)			//������豸��ַ�����ǹ㲥��ַ
	{
		uint8_t xorCheck=0;
		for(uint8_t j=0;j<UART_RX_LEN-1;j++)                //�����յ������ݰ���Э�����У��
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
