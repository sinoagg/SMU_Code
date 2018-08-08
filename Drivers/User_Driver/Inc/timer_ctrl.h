#ifndef __TIMER_CTRL_H
#define __TIMER_CTRL_H

#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"
#include "prj_typedef.h"
#include "relay.h"
#include "InternalFlash.h"
#include "hardware.h"

#define TIM_ON 1
#define TIM_OFF 0

extern uint8_t silenttime_past;							//��Ĭʱ���ȥ��־λ��0��ʾδ��
extern uint8_t ADCread_status;							//ADC�Ƿ��ȡ�ı�־λ
extern TestPara_TypeDef TestPara;           //����������ȡ
extern TestResult_TypeDef TestResult;       //���������ȡ
extern uint16_t origin_adcvalue_I;			    //������ص�����ADC��������Ӧ7798_1_1
extern uint16_t origin_adcvalue_V;					//������ص�ѹ��ADC����, ��Ӧ7798_1_2
extern MonitorCH_TypeDef hMeasureCH_Voltage;

void SetTimerPara(TestPara_TypeDef* pTestPara);
void SetTimerAction(TestPara_TypeDef* pTestPara);

void SetInitialVoltage(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay);

void Test_Relay(void);

static void ChangeTimer(TestPara_TypeDef* pTestPara, uint8_t TIM_ONOFF);

#endif 

