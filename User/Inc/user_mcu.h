#ifndef USER_MCU_H
#define USER_MCU_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MCU_NOT_SUPPORT 0
#define MCU_STM32F429IGT6 1
#define MCU_GD32F450IIH6 2

extern volatile uint32_t mcu_id;

extern void user_get_mcu_id(void);
extern void user_print_mcu_info(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_MCU_H

