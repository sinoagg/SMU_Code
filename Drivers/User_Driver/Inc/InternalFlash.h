#ifndef __INTERNALFLASH_H
#define __INTERNALFLASH_H

#include "stm32f1xx_hal.h"
#include "prj_typedef.h"

#define FLASH_USER_START_ADDR        ((uint32_t)0x08071000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR          ((uint32_t)0x0807FFFF)   /* End   @ of user Flash area */
#define FLASH_USER_PAGE1_START_ADDR  ((uint32_t)0x08071000)   /* Start Page1 @ of user Flash area */
#define FLASH_USER_PAGE1_END_ADDR    ((uint32_t)0x080717FF)   /* End Page1 @ of user Flash area */
#define FLASH_USER_PAGE2_START_ADDR  ((uint32_t)0x08071800)
#define FLASH_USER_PAGE2_END_ADDR  	 ((uint32_t)0x08071FFF)

#define OUTPUT_OFFSET_ADDR 0x00            	//�Ķ������ѹУ׼ƫ�����洢��ַ
#define OUTPUT_LINEAR_ADDR 0x04*4          	//�Ķ������ѹУ׼�������洢��ַ
#define OUTPUT_RELAY_OFFSET_ADDR 0x04*8    //һ���Ŵ����У׼ƫ�����洢��ַ
#define OUTPUT_RELAY_LINEAR_ADDR 0x04*24   	//һ���Ŵ�������У׼�������洢��ַ
#define OUTPUT_NRELAY_LINEAR_ADDR 0x04*36  	//һ���Ŵ󸺵���У׼�������洢��ַ
#define CURVE_FITTING_ADDR	0x00
#define USER_SET_VOLTAGE_ADDR 0x04*4

void GetFlashData_Float(FloatUnion* pDataArray, uint32_t flash_addr, uint8_t length);
void GetFlashData_U32(uint32_t* pdata, uint32_t flash_addr, uint8_t length);

static uint32_t FlashRead32bit(uint32_t ReadAddr);
static void Uint32to4Uint8(uint32_t uint32_data, uint8_t* pBuffer);
void FlashErase(uint32_t start_address,uint32_t end_address);   //����flash�û�ҳ

#endif

