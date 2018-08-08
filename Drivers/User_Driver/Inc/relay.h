#ifndef __RELAY_H
#define __RELAY_H	 

#include "stm32f1xx_hal.h"	   
#include "main.h"
#include "prj_typedef.h"
#include "AD7988_1.h"

#define RANGE_CHANGE_DELAY 100							//delay time between each relay change in ms

#define RELAY_INPUT_SCALING_1X 0            //放大1倍
#define RELAY_INPUT_SCALING_6X 1           //放大6倍

#define AUTO_RANGE 0                        //自动换挡模式
#define RELAY_RANGE_1R 1
#define RELAY_RANGE_10R 2
#define RELAY_RANGE_100R 3
#define RELAY_RANGE_1K 4
#define RELAY_RANGE_10K 5
#define RELAY_RANGE_100K 6
#define RELAY_RANGE_1M 7
#define RELAY_RANGE_10M 8
#define RELAY_RANGE_100M 9
#define RELAY_RANGE_1G 10

#define DEFAULT_RELAY_INPUT_SCALING RELAY_INPUT_SCALING_6X
#define DEFAULT_RELAY_TEST_MODE RELAY_TEST_MODE_FIMV
#define DEFAULT_RELAY_CHANGE_MODE AUTO_RANGE
#define DEFAULT_RELAY_MAX_RANGE RELAY_RANGE_1G
#define DEFAULT_RELAY_MIN_RANGE RELAY_RANGE_1R
#define DEFAULT_RELAY_RANGE RELAY_RANGE_100K


typedef struct
{
	uint8_t rangeMode;								//自动或手动换挡：0为自动换挡，1-10为手动换挡的档位值
	uint8_t	maxRange;									//最大挡位
	uint8_t minRange;									//最小挡位
	uint8_t rangeNow;									//目前所用挡位
	uint8_t originState;              //原始档位计数
	uint8_t originRange;              //原始档位
	uint8_t rangeChangeSatus;         //换挡结束标志，0为继续，1为结束
}Relay_TypeDef;

typedef struct
{
	uint8_t monitorch_relaystate;
	AD7988_1_HandleTypeDef *phAD7988_1_x;
}MonitorCH_TypeDef;

extern Relay_TypeDef Relay;

void RelayClear(void);
void SetRelay(uint8_t range_select);
void GetRelayPara(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay);
uint8_t RelayCheck(enum TestMode testMode, TestResult_TypeDef* pTestResult, Relay_TypeDef* pRelay);
void RelaySetInputScaling(enum TestMode testMode, uint8_t scale);
void RelaySetTestMode(enum TestMode testmode);
void ConnectOutput(void);											//输出继电器连接
void DisconnectOutput(void);									//输出继电器断开

#endif

