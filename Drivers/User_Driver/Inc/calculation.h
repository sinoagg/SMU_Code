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

extern FloatUnion Adj_MeasIOffset;                     //校准ADC监控电流偏移量
extern FloatUnion Adj_MeasILinear;                     //校准ADC监控电流线性度
extern FloatUnion Adj_MeasVOffset_1D12;            //校准ADC监控电压偏移量
extern FloatUnion Adj_MeasVLinear_1D12;            //校准ADC监控电压线性度
extern FloatUnion Adj_MeasVOffset_5D6;             //校准ADC监控电压偏移量
extern FloatUnion Adj_MeasVLinear_5D6;             //校准ADC监控电压线性度
extern FloatUnion Adj_RelayOffset[10];                 //校准采集各档位电流偏移量
extern FloatUnion Adj_RelayLinear[10];                 //校准电流采集线性度  

float MyPow(float base,uint8_t time);
uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);

#endif
	 


