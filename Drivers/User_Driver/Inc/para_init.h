#ifndef __PARA_INIT_H
#define __PARA_INIT_H

#include "stm32f1xx_hal.h"
#include "prj_typedef.h"
#include "relay.h"


#define TIM_ON 1
#define TIM_OFF 0

#define DEFAULT_SAMPLERATE 2000
#define DEFAULT_SAMPLENUMBER 400
#define DEFAULT_SILENTTIME 1000
#define DEFAULT_MAXRANGESELECT 10
#define DEFAULT_V_STEP 10						//mV
#define DEFAULT_I_STEP 1						

extern FloatUnion Adj_OutputOffset;                //�����ѹУ׼ϵ����ƫ����
extern FloatUnion Adj_OutputLinear;                //�����ѹУ׼ϵ�������Զ�
extern FloatUnion Adj_MeasIOffset;                 //У׼ADC��ص���ƫ����
extern FloatUnion Adj_MeasILinear;                 //У׼ADC��ص������Զ�
extern FloatUnion Adj_MeasVOffset_1D12;            //У׼ADC��ص�ѹƫ����
extern FloatUnion Adj_MeasVLinear_1D12;            //У׼ADC��ص�ѹ���Զ�
extern FloatUnion Adj_MeasVOffset_5D6;             //У׼ADC��ص�ѹƫ����
extern FloatUnion Adj_MeasVLinear_5D6;             //У׼ADC��ص�ѹ���Զ�
extern FloatUnion Adj_RelayOffset[10];             //У׼�ɼ�����λ����ƫ����
extern FloatUnion Adj_RelayLinear[10];             //У׼�������ɼ����Զ�  

extern MonitorCH_TypeDef hMonitorCH_Voltage;  //handle of monitor voltage
extern MonitorCH_TypeDef hMonitorCH_Current;  //handle of monitor current
extern MonitorCH_TypeDef hMeasureCH_Voltage;  //handle of measure voltage
extern MonitorCH_TypeDef hMeasureCH_Current;  //handle of measure current

void InitTestPara(TestPara_TypeDef* pTestPara);
void InitTestResult(TestResult_TypeDef* pTestResult);
void GetTestPara(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay, uint8_t* pUartRxBuf);
void ClearArray(FloatUnion* pArray, uint8_t size);
void SetArray(FloatUnion* pArray, uint8_t size);

#endif 

