#ifndef _SDRAM_H
#define _SDRAM_H

#include "user_common.h"

#ifdef ENABLE_SDRAM

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F429������
//SDRAM��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
//extern SDRAM_HandleTypeDef SDRAM_Handler;//SDRAM���
#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) //SDRAM��ʼ��ַ

//SDRAM���ò���
#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)

void SDRAM_Init(void);
void SDRAM_MPU_Config(void);
u8 SDRAM_Send_Cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer, u32 WriteAddr, u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer, u32 ReadAddr, u32 n);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // ENABLE_SDRAM

#endif
