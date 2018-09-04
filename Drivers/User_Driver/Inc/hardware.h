#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "stm32f1xx_hal.h"
#include "relay.h"
#include "delay.h"
#include "AD5689R.h"
#include "AD7988_1.h"
#include "para_init.h"
#include "AD8253.h"

extern TIM_HandleTypeDef htim2;				//handle of timer 2
extern TIM_HandleTypeDef htim3;				//handle of timer 3
extern TIM_HandleTypeDef htim4;				//handle of timer 4

extern UART_HandleTypeDef huart1;			//handle of uart1
extern UART_HandleTypeDef huart2;			//handle of uart2
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern AD5689R_HandleTypeDef hAD5689R1;//handle of DAC 1

extern AD7988_1_HandleTypeDef hAD7988_1_1;		//handle of main current monitor ADC 1
extern AD7988_1_HandleTypeDef hAD7988_1_2;		//handle of main current monitor ADC 1
extern MonitorCH_TypeDef hMonitorCH_Voltage;  //handle of monitor voltage
extern MonitorCH_TypeDef hMonitorCH_Current;  //handle of monitor current
extern MonitorCH_TypeDef hMeasureCH_Voltage;  //handle of measure voltage
extern MonitorCH_TypeDef hMeasureCH_Current;  //handle of measure current

extern void ClearSignOutVol(Relay_TypeDef* pRelay);	//��ʼ�������ѹΪ0	

void Hardware_Init(void);
void SystemClock_Config(void);
//void Error_Handler(void);
void MX_DMA_Init(void);
void MX_GPIO_Init(void);
void MX_TIM2_Init(uint16_t time_step);
void MX_TIM3_Init(uint16_t time_step);
void MX_TIM4_Init(uint16_t time_step);

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

#endif
