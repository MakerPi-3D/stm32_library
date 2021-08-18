#ifndef LCD_COMMON_H
#define LCD_COMMON_H
#include "sys.h"
#include "stm32f4xx_hal.h"
#include "wbtypes.h"
#ifdef __cplusplus
extern "C" {
#endif

//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#if defined(ENABLE_LTDC)
#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��
#elif defined(ENABLE_FSMC)
#define DFT_SCAN_DIR  R2L_D2U
#endif

#if defined(ENABLE_FSMC)
#ifdef HAL_SRAM_MODULE_ENABLED
extern SRAM_HandleTypeDef hsram1;
extern void LCD_WR_REG(__IO UINT16 regval);
extern void LCD_WR_DATA(__IO UINT16 data);
extern UINT16 LCD_RD_DATA(void);
extern void LCD_WriteRAM_Prepare(void);
extern void LCD_WriteReg(__IO UINT16 LCD_Reg, __IO UINT16 LCD_RegValue);
extern UINT16 LCD_ReadReg(__IO UINT16 LCD_Reg);
extern UINT16 LCD_RD_REG(void);
extern u16 LCD_BGR2RGB(u16 c);
extern void opt_delay(u8 i);
#endif
#endif

#ifdef __cplusplus
} //extern "C"
#endif
#endif // LCD_COMMON_H

