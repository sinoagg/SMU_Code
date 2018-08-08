#ifndef __CALCULATION_H
#define __CALCULATION_H

#include "stm32f1xx_hal.h"	  
#include "prj_typedef.h"
#include "relay.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern MonitorCH_TypeDef hMonitorCH_Voltage;  //handle of monitor voltage
extern MonitorCH_TypeDef hMonitorCH_Current;  //handle of monitor current
extern MonitorCH_TypeDef hMeasureCH_Voltage;  //handle of measure voltage

extern FloatUnion Adj_MeasIOffset;                     //У׼ADC��ص���ƫ����
extern FloatUnion Adj_MeasILinear;                     //У׼ADC��ص������Զ�
extern FloatUnion Adj_MeasVOffset_1D12;            //У׼ADC��ص�ѹƫ����
extern FloatUnion Adj_MeasVLinear_1D12;            //У׼ADC��ص�ѹ���Զ�
extern FloatUnion Adj_MeasVOffset_5D6;             //У׼ADC��ص�ѹƫ����
extern FloatUnion Adj_MeasVLinear_5D6;             //У׼ADC��ص�ѹ���Զ�
extern FloatUnion Adj_RelayOffset[10];                 //У׼�ɼ�����λ����ƫ����
extern FloatUnion Adj_RelayLinear[10];                 //У׼�����ɼ����Զ�  

float MyPow(float base,uint8_t time);
uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);

#endif
	 


