#include "para_init.h"
#include "prj_typedef.h"
#include "calculation.h"
#include "InternalFlash.h"
uint8_t silenttime_past=0;							              //静默时间过去标志位，0表示未过
uint8_t ADCread_status=0;								              //ADC是否读取的标志位
uint32_t sample_count=0;								              //sample number counter
uint32_t sample_totalval=0;							              //所有采样值的和
FloatUnion Adj_OutputOffset;                    //输出电压校准系数：偏移量
FloatUnion Adj_OutputLinear;                    //输出电压校准系数：线性度
FloatUnion Adj_MeasIOffset;                     //校准ADC监控电流偏移量
FloatUnion Adj_MeasILinear;                     //校准ADC监控电流线性度
FloatUnion Adj_MeasVOffset_1D12;                //校准ADC监控电压偏移量
FloatUnion Adj_MeasVLinear_1D12;                //校准ADC监控电压线性度
FloatUnion Adj_MeasVOffset_5D6;                 //校准ADC监控电压偏移量
FloatUnion Adj_MeasVLinear_5D6;                 //校准ADC监控电压线性度
FloatUnion Adj_RelayOffset[10];                 //校准采集各档位电流偏移量

FloatUnion Adj_Pos_gt2400_RelayLinear[10];                 //校准电流采集线性度
FloatUnion Adj_Neg_gt2400_RelayLinear[10];                 //校准电流采集线性度
FloatUnion Adj_Pos_lt2400_RelayLinear[10];                 //校准电流采集线性度
FloatUnion Adj_Neg_lt2400_RelayLinear[10];                 //校准电流采集线性度
//FloatUnion Adj_RelayLinear[10];                 //校准电流采集线性度   


extern uint8_t UART2_RxBuf[17];

void InitTestPara(TestPara_TypeDef* pTestPara)
{ 
	pTestPara->testStatus=OFF;		                       	//测试状态关闭
	
	#ifdef MANUAL_ADJUSTMENT                           		//SMU 3号
		Adj_OutputOffset.numFloat=0;//0.00009;        //校准输出端偏移量
		Adj_OutputLinear.numFloat=1;//1.003;           //校准输出端线性度
	
		Adj_MeasIOffset.numFloat=0;                  				//校准ADC监控电流偏移量
		Adj_MeasILinear.numFloat=1;                  				//校准ADC监控电流线性度
	
		Adj_MeasVOffset_1D12.numFloat=-0.004243;     				//校准ADC监控电压偏移量
		Adj_MeasVLinear_1D12.numFloat=0.9999428;      			//校准ADC监控电压线性度
	  Adj_MeasVOffset_5D6.numFloat=-0.0008253;       			//校准ADC监控电压偏移量
		Adj_MeasVLinear_5D6.numFloat=0.9998;      					//校准ADC监控电压线性度
		
		Adj_RelayOffset[0].numFloat=-6.1799e-4;             //校准采集各档位电流偏移量                                    
		Adj_RelayOffset[1].numFloat=-5.7221e-5;                                          
		Adj_RelayOffset[2].numFloat=-6.1799e-6;                                        
		Adj_RelayOffset[3].numFloat=-6.1799e-7;                                        
		Adj_RelayOffset[4].numFloat=-6.1799e-8;                                           
		Adj_RelayOffset[5].numFloat=-6.1799e-9;                                       
		Adj_RelayOffset[6].numFloat=-1.0758e-9;                                         
		Adj_RelayOffset[7].numFloat=-5.6e-10;                                       
		Adj_RelayOffset[8].numFloat=0;
    Adj_RelayOffset[9].numFloat=-5.1e-10;		

						
		Adj_Pos_gt2400_RelayLinear[0].numFloat=0.918864;              //校准电流采集线性度            
		Adj_Pos_gt2400_RelayLinear[1].numFloat=0.9913357;                            
		Adj_Pos_gt2400_RelayLinear[2].numFloat=1.0013;                   
		Adj_Pos_gt2400_RelayLinear[3].numFloat=1.0017;                              
		Adj_Pos_gt2400_RelayLinear[4].numFloat=1.00132;                               
		Adj_Pos_gt2400_RelayLinear[5].numFloat=0.998;                                
		Adj_Pos_gt2400_RelayLinear[6].numFloat=1;                           
		Adj_Pos_gt2400_RelayLinear[7].numFloat=1.008756;                             
		Adj_Pos_gt2400_RelayLinear[8].numFloat=1;     
		Adj_Pos_gt2400_RelayLinear[9].numFloat=1.0101; 
		
		Adj_Neg_gt2400_RelayLinear[0].numFloat=0.918864;              //校准电流采集线性度            
		Adj_Neg_gt2400_RelayLinear[1].numFloat=0.9913357;                            
		Adj_Neg_gt2400_RelayLinear[2].numFloat=1.0013;                   
		Adj_Neg_gt2400_RelayLinear[3].numFloat=1.0017;                              
		Adj_Neg_gt2400_RelayLinear[4].numFloat=1.00132;                               
		Adj_Neg_gt2400_RelayLinear[5].numFloat=0.998;                                
		Adj_Neg_gt2400_RelayLinear[6].numFloat=1;                           
		Adj_Neg_gt2400_RelayLinear[7].numFloat=1.008756;                             
		Adj_Neg_gt2400_RelayLinear[8].numFloat=1;     
		Adj_Neg_gt2400_RelayLinear[9].numFloat=1.0101; 

		Adj_Pos_lt2400_RelayLinear[0].numFloat=0.918864;              //校准电流采集线性度            
		Adj_Pos_lt2400_RelayLinear[1].numFloat=0.9913357;                            
		Adj_Pos_lt2400_RelayLinear[2].numFloat=1.0013;                   
		Adj_Pos_lt2400_RelayLinear[3].numFloat=1.0017;                              
		Adj_Pos_lt2400_RelayLinear[4].numFloat=1.00132;                               
		Adj_Pos_lt2400_RelayLinear[5].numFloat=0.998;                                
		Adj_Pos_lt2400_RelayLinear[6].numFloat=1;                           
		Adj_Pos_lt2400_RelayLinear[7].numFloat=1.008756;                             
		Adj_Pos_lt2400_RelayLinear[8].numFloat=1;     
		Adj_Pos_lt2400_RelayLinear[9].numFloat=1.0101; 
		
		Adj_Neg_lt2400_RelayLinear[0].numFloat=0.918864;              //校准电流采集线性度            
		Adj_Neg_lt2400_RelayLinear[1].numFloat=0.9913357;                            
		Adj_Neg_lt2400_RelayLinear[2].numFloat=1.0013;                   
		Adj_Neg_lt2400_RelayLinear[3].numFloat=1.0017;                              
		Adj_Neg_lt2400_RelayLinear[4].numFloat=1.00132;                               
		Adj_Neg_lt2400_RelayLinear[5].numFloat=0.998;                                
		Adj_Neg_lt2400_RelayLinear[6].numFloat=1;                           
		Adj_Neg_lt2400_RelayLinear[7].numFloat=1.008756;                             
		Adj_Neg_lt2400_RelayLinear[8].numFloat=1;     
		Adj_Neg_lt2400_RelayLinear[9].numFloat=1.0101; 

		
//		Adj_RelayLinear[0].numFloat=0.918864;              //校准电流采集线性度            
//		Adj_RelayLinear[1].numFloat=0.9913357;                            
//		Adj_RelayLinear[2].numFloat=1.0013;                   
//		Adj_RelayLinear[3].numFloat=1.0017;                              
//		Adj_RelayLinear[4].numFloat=1.00132;                               
//		Adj_RelayLinear[5].numFloat=0.998;                                
//		Adj_RelayLinear[6].numFloat=1;                           
//		Adj_RelayLinear[7].numFloat=1.008756;                             
//		Adj_RelayLinear[8].numFloat=1;     
//		Adj_RelayLinear[9].numFloat=1.0101; 

	#endif
	
	#ifdef NO_ADJUSTMENT
		Adj_OutputOffset.numFloat=0;
		Adj_OutputLinear.numFloat=1;
    Adj_MeasIOffset.numFloat=0;                     //校准ADC监控电流偏移量
		Adj_MeasILinear.numFloat=1;                     //校准ADC监控电流线性度
		Adj_MeasVOffset_1D12.numFloat=0;                //校准ADC监控电压偏移量
		Adj_MeasVLinear_1D12.numFloat=1;                //校准ADC监控电压线性度
		Adj_MeasVOffset_5D6.numFloat=0;                 //校准ADC监控电压偏移量
		Adj_MeasVLinear_5D6.numFloat=1;                 //校准ADC监控电压线性度
		ClearArray(Adj_RelayOffset,10);
		SetArray(Adj_RelayLinear,10);
	#endif

	#ifdef AUTO_ADJUSTMENT
//#define OUTPUT_GT2400_RELAY_LINEAR_ADDR 0x04*24   	//放大正电流校准线性量存储地址
//#define OUTPUT_LT2400_RELAY_LINEAR_ADDR 0x04*34   	//放大正电流校准线性量存储地址
//#define OUTPUT_GT2400_NRELAY_LINEAR_ADDR 0x04*44  	//放大负电流校准线性量存储地址
//#define OUTPUT_LT2400_NRELAY_LINEAR_ADDR 0x04*54   	//放大负电流校准线性量存储地址
		//一次读出32bit
		//GetFlashData_Float(Adj_OutputOffset, FLASH_USER_PAGE1_START_ADDR+OUTPUT_OFFSET_ADDR, 4);					//读取Output OFFSET，4个float型数 
		//GetFlashData_Float(Adj_OutputLinear, FLASH_USER_PAGE1_START_ADDR+OUTPUT_LINEAR_ADDR, 4);					//读取Output LINEAR，4个float型数
		GetFlashData_Float(Adj_RelayOffset, FLASH_USER_PAGE1_START_ADDR+OUTPUT_RELAY_OFFSET_ADDR, 9);			//读取RELAY OFFSET，9个float型数
		GetFlashData_Float(Adj_Pos_gt2400_RelayLinear, FLASH_USER_PAGE1_START_ADDR+OUTPUT_GT2400_RELAY_LINEAR_ADDR, 9);	//读取POSITIVE RELAY LINEAR，9个float型数 
		GetFlashData_Float(Adj_Neg_gt2400_RelayLinear, FLASH_USER_PAGE1_START_ADDR+OUTPUT_GT2400_NRELAY_LINEAR_ADDR, 9);	//读取NEGATIVE RELAY LINEAR，9个float型数  
		GetFlashData_Float(Adj_Pos_lt2400_RelayLinear, FLASH_USER_PAGE1_START_ADDR+OUTPUT_LT2400_RELAY_LINEAR_ADDR, 9);	//读取POSITIVE RELAY LINEAR，9个float型数 
		GetFlashData_Float(Adj_Neg_lt2400_RelayLinear, FLASH_USER_PAGE1_START_ADDR+OUTPUT_LT2400_NRELAY_LINEAR_ADDR, 9);	//读取NEGATIVE RELAY LINEAR，9个float型数 
	#endif
	
}

void InitTestResult(TestResult_TypeDef* pTestResult)
{
		pTestResult->I_sample=0;
		pTestResult->I_avg.numFloat=0;
		pTestResult->I_sum=0;
		pTestResult->V_sample=0;
		pTestResult->V_avg.numFloat=0;
		pTestResult->V_sum=0;
		pTestResult->sampleCount=0;
}
void GetTestPara(TestPara_TypeDef* pTestPara, Relay_TypeDef* pRelay, uint8_t* pUartRxBuf)
{
	if(pTestPara->testStatus==OFF)																	//如果设备没有测试进行中
	{
		pTestPara->testMode=(enum TestMode)*(pUartRxBuf+2);
		pTestPara->timeStep=(uint16_t)*(pUartRxBuf+18)<<8|*(pUartRxBuf+19);
		pTestPara->sampleRate=(uint16_t)*(pUartRxBuf+22)<<8|*(pUartRxBuf+23);
		pTestPara->avgNumber=(uint16_t)*(pUartRxBuf+24)<<8|*(pUartRxBuf+25);
		pTestPara->quietTime=(uint16_t)*(pUartRxBuf+16)<<8|*(pUartRxBuf+17);
		pTestPara->quietTime/=100;		
		pTestPara->I_Start=(uint16_t)*(pUartRxBuf+10)<<8|*(pUartRxBuf+11);
		pTestPara->I_End=(uint16_t)*(pUartRxBuf+12)<<8|*(pUartRxBuf+13);
		pTestPara->V_Start=(uint16_t)*(pUartRxBuf+4)<<8|*(pUartRxBuf+5);
		pTestPara->V_End=(uint16_t)*(pUartRxBuf+6)<<8|*(pUartRxBuf+7);
		pTestPara->V_Step=(uint16_t)*(pUartRxBuf+8)<<8|*(pUartRxBuf+9);
		pTestPara->I_Step=(uint16_t)*(pUartRxBuf+14)<<8|*(pUartRxBuf+15);

		pTestPara->startDelay=pTestPara->timeStep - 1000*pTestPara->avgNumber/pTestPara->sampleRate-10;			//给计算传输留出10ms空闲
		if(pTestPara->startDelay<0) pTestPara->startDelay=0;
		pTestPara->I_Now=pTestPara->I_Start;
		pTestPara->V_Now=pTestPara->V_Start;
		pTestPara->I_Unit=(*(pUartRxBuf+26))>>4;
		pRelay->rangeMode=(*(pUartRxBuf+26))&0x0F;
		pRelay->maxRange=*(pUartRxBuf+27);
		pRelay->minRange=*(pUartRxBuf+28);
		pRelay->outputConnect=(*(pUartRxBuf+3))&0x01;		//此字节最低位代表输出继电器开关状态
	}	
}

void ClearArray(FloatUnion* pArray, uint8_t size)
{
	uint8_t i;
	for(i=0;i<size;i++)
	{
		(*(pArray+i)).numFloat=0;
	}
}

void SetArray(FloatUnion* pArray, uint8_t size)
{
	uint8_t i;
	for(i=0;i<size;i++)
	{
		(*(pArray+i)).numFloat=1;
	}
}



