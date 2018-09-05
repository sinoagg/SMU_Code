#ifndef __CALCULATION_H
#define __CALCULATION_H

#include "stm32f1xx_hal.h"	  
#include "prj_typedef.h"
#include "relay.h"

#define OUTPUT_VOL_CALIBRATION 0
#define ZERO_CURRENT_CALIBRATION	1
#define CURRENT_RANGE_CALIBRATION 2
#define SAVE_CALIBRATION  0xff


#define FLASH_USER_START_ADDR        ((uint32_t)0x08071000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR          ((uint32_t)0x0807FFFF)   /* End   @ of user Flash area */
#define FLASH_USER_PAGE1_START_ADDR  ((uint32_t)0x08071000)   /* Start Page1 @ of user Flash area */
#define FLASH_USER_PAGE1_END_ADDR    ((uint32_t)0x080717FF)   /* End Page1 @ of user Flash area */

#define OUTPUT_OFFSET_ADDR 0x00            	//四端输出电压校准偏移量存储地址
#define OUTPUT_LINEAR_ADDR 0x04*4          	//四端输出电压校准线性量存储地址
#define OUTPUT_RELAY_OFFSET_ADDR 0x04*8    //放大电流校准偏移量存储地址
#define OUTPUT_GT2400_RELAY_LINEAR_ADDR 0x04*24   	//放大正电流校准线性量存储地址
#define OUTPUT_LT2400_RELAY_LINEAR_ADDR 0x04*34   	//放大正电流校准线性量存储地址
#define OUTPUT_GT2400_NRELAY_LINEAR_ADDR 0x04*44  	//放大负电流校准线性量存储地址
#define OUTPUT_LT2400_NRELAY_LINEAR_ADDR 0x04*54   	//放大负电流校准线性量存储地址



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
//extern FloatUnion Adj_RelayLinear[10];                 //校准电流采集线性度  
extern FloatUnion Adj_Pos_gt2400_RelayLinear[10];                 //校准电流采集线性度
extern FloatUnion Adj_Neg_gt2400_RelayLinear[10];                 //校准电流采集线性度
extern FloatUnion Adj_Pos_lt2400_RelayLinear[10];                 //校准电流采集线性度
extern FloatUnion Adj_Neg_lt2400_RelayLinear[10];                 //校准电流采集线性度

float MyPow(float base,uint8_t time);
uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);
void RunCalibration(uint8_t* UartRxBuf);

void Cali_Crrent_Pos(FloatUnion* arrary,float temp,uint8_t index,int voltage);
void Cali_Crrent_Neg(FloatUnion* arrary,float temp,uint8_t index,int voltage);
float CommitAdjustment(float input_I, Relay_TypeDef* pRelay,int voltage);
void FlashSaveCaliData(uint32_t start_address, uint32_t end_address);   											//保存校准数据


#endif
	 


