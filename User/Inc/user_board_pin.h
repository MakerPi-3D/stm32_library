#ifndef USER_BOARD_PIN_H
#define USER_BOARD_PIN_H

/* Includes ------------------------------------------------------------------*/
#include "user_board.h"
#include "user_mcu.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USER_GPIO_READ(MCU, TYPE)   HAL_GPIO_ReadPin(MCU##_##TYPE##_GPIO_Port, MCU##_##TYPE##_Pin)
#define USER_GPIO_WRITE(MCU, TYPE, value)   HAL_GPIO_WritePin(MCU##_##TYPE##_GPIO_Port, MCU##_##TYPE##_Pin, value)
#define USER_GPIO_CTRL(TYPE, isOn) \
  {\
    GPIO_PinState value = isOn ? GPIO_PIN_SET : GPIO_PIN_RESET;\
    if (mcu_id == MCU_GD32F450IIH6) \
    {\
      USER_GPIO_WRITE(GD32, TYPE, value);\
    }\
    else if (mcu_id == MCU_STM32F429IGT6)\
    {\
      USER_GPIO_WRITE(STM32, TYPE, value);\
    }\
  }
#define USER_GPIO_GET(TYPE) \
  {\
    GPIO_PinState result = GPIO_PIN_RESET;\
    if (mcu_id == MCU_GD32F450IIH6) \
    {\
      result = USER_GPIO_READ(GD32, TYPE);\
    }\
    else if (mcu_id == MCU_STM32F429IGT6)\
    {\
      result = USER_GPIO_READ(STM32, TYPE);\
    }\
    return result;\
  }

__inline static void user_pin_light_bar_ctrl(bool isOn) USER_GPIO_CTRL(LIGHT_BAR, isOn)
__inline static void user_pin_lcd_backlight_ctrl(bool isOn) USER_GPIO_CTRL(LCD_BACKLIGHT, isOn)
__inline static void user_pin_tp_sck_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_SCK, isOn)
__inline static void user_pin_tp_cs_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_CS, isOn)
__inline static void user_pin_tp_mosi_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_MOSI, isOn)
__inline static void user_pin_tp_pen_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_PEN, isOn)

__inline static GPIO_PinState user_pin_tp_pen_read(void) USER_GPIO_GET(TOUCH_PEN)
__inline static GPIO_PinState user_pin_tp_miso_read(void) USER_GPIO_GET(TOUCH_MISO)
__inline static GPIO_PinState user_pin_sig_mat_read(void) USER_GPIO_GET(SIG_MAT)
__inline static GPIO_PinState user_pin_sig_door_read(void) USER_GPIO_GET(SIG_DOOR)

#ifdef __cplusplus
} //extern "C" {
#endif

#endif /* USER_BOARD_PIN_H */
