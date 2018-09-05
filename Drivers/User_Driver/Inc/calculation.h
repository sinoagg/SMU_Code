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

#define OUTPUT_OFFSET_ADDR 0x00            	//�Ķ������ѹУ׼ƫ�����洢��ַ
#define OUTPUT_LINEAR_ADDR 0x04*4          	//�Ķ������ѹУ׼�������洢��ַ
#define OUTPUT_RELAY_OFFSET_ADDR 0x04*8    //�Ŵ����У׼ƫ�����洢��ַ
#define OUTPUT_GT2400_RELAY_LINEAR_ADDR 0x04*24   	//�Ŵ�������У׼�������洢��ַ
#define OUTPUT_LT2400_RELAY_LINEAR_ADDR 0x04*34   	//�Ŵ�������У׼�������洢��ַ
#define OUTPUT_GT2400_NRELAY_LINEAR_ADDR 0x04*44  	//�Ŵ󸺵���У׼�������洢��ַ
#define OUTPUT_LT2400_NRELAY_LINEAR_ADDR 0x04*54   	//�Ŵ󸺵���У׼�������洢��ַ



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
//extern FloatUnion Adj_RelayLinear[10];                 //У׼�����ɼ����Զ�  
extern FloatUnion Adj_Pos_gt2400_RelayLinear[10];                 //У׼�����ɼ����Զ�
extern FloatUnion Adj_Neg_gt2400_RelayLinear[10];                 //У׼�����ɼ����Զ�
extern FloatUnion Adj_Pos_lt2400_RelayLinear[10];                 //У׼�����ɼ����Զ�
extern FloatUnion Adj_Neg_lt2400_RelayLinear[10];                 //У׼�����ɼ����Զ�

float MyPow(float base,uint8_t time);
uint8_t Do_Calculation(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);
void RunCalibration(uint8_t* UartRxBuf);

void Cali_Crrent_Pos(FloatUnion* arrary,float temp,uint8_t index,int voltage);
void Cali_Crrent_Neg(FloatUnion* arrary,float temp,uint8_t index,int voltage);
float CommitAdjustment(float input_I, Relay_TypeDef* pRelay,int voltage);
void FlashSaveCaliData(uint32_t start_address, uint32_t end_address);   											//����У׼����


#endif
	 


