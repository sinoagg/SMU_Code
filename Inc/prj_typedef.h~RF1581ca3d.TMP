#ifndef __PRJ_TYPEDEF_H
#define __PRJ_TYPEDEF_H

#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"

enum MsgType
{
	MSG_TYPE_NULL=0,
	MSG_TYPE_SETTING=0x11,					
	MSG_TYPE_RUN=0x12,
	MSG_TYPE_STOP=0x13,
	MSG_TYPE_QUERY=0x14,
	MSG_TYPE_CALIBRATION=0x15,
	MSG_TYPE_REPORT_ID=0xFF
};

enum TestStatus
{
	ON=0,
	OFF=1,
	PAUSED=2
};

enum TestCMD
{
	CMD_START=0x11,
	CMD_PAUSE=0x12,
	CMD_STOP=0x13,
	CMD_CONTINUE=0x14
};

enum TestMode
{
	MODE_CALIBRATION=0,
	MODE_FVMI_SWEEP=1,
	MODE_FIMV_SWEEP=2,
	MODE_FVMI_NO_SWEEP=3,
	MODE_FIMV_NO_SWEEP=4
};

typedef union										//电流值数据组织形式
{
	uint8_t numUchar[4]; 
	float numFloat;
}FloatUnion;

enum CaliMode										//校准模式
{
	SHORT=0,
	LOAD100=1,
	LOAD10k=2,
	LOAD1M=3,
	LOAD100M=4,
	OUTPUTVOLTAGE_ZERO=0x80,
	OUTPUTVOLTAGE_FULL=0x81,
	OPEN=0xFF
};

typedef struct
{
	enum TestStatus testStatus;					//测试状态
	enum TestMode testMode;
	enum TestCMD	testCmd;
	enum CaliMode caliMode;
	uint16_t timeStep;								//timestep decides the time delay between each output point and tansmission, default is 1000ms
	uint16_t sampleRate;							//samplerate decides main current monitor ADC sampling speed, default is 1000  		
	int16_t V_Start;									//voltage gate start
	int16_t V_End;										//voltage gate end
	int16_t V_Now;								//the current voltage of gate output
	int16_t I_Start;									//voltage drain start
	int16_t I_End;										//voltage drain end
	int16_t I_Now;								//the current voltage of drain output
	int16_t V_Step;										//step for sweeping use
	int16_t I_Step;
	uint16_t avgNumber;					//默认16个点取平均值
	uint16_t quietTime;							//默认静默时间是2s
	int16_t startDelay;
	uint16_t sampleTime;
}TestPara_TypeDef;

typedef struct
{
	uint16_t I_sample;				//单次测量值
	uint32_t I_sum;						//单次测量值累加			
	FloatUnion I_avg;						//单次测量值平均
	uint16_t V_sample;					
	uint32_t V_sum;		
	FloatUnion V_avg;	
	uint32_t sampleCount;
	uint8_t endOfTest;
}TestResult_TypeDef;

enum SamplingStatus
{
	SAMPLING_READY=0,
	SAMPLING_BUSY=1
};

#endif

