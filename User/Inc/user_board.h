#ifndef USER_BOARD_H
#define USER_BOARD_H

#if defined(STM32F429xx)

  /* Includes ------------------------------------------------------------------*/
  #include "main.h"
  #include "fatfs.h"

  #define STM32_HEAT_NOZ_B_Pin GPIO_PIN_2
  #define STM32_HEAT_NOZ_B_GPIO_Port GPIOE
  #define STM32_SIG_X2_Pin GPIO_PIN_8
  #define STM32_SIG_X2_GPIO_Port GPIOB
  #define STM32_LCD_BACKLIGHT_Pin GPIO_PIN_5
  #define STM32_LCD_BACKLIGHT_GPIO_Port GPIOB
  #define STM32_B_DIR_Pin GPIO_PIN_4
  #define STM32_B_DIR_GPIO_Port GPIOB
  #define STM32_HEAT_CAVITY_Pin GPIO_PIN_3
  #define STM32_HEAT_CAVITY_GPIO_Port GPIOB
  #define STM32_Y_EN_Pin GPIO_PIN_7
  #define STM32_Y_EN_GPIO_Port GPIOD
  #define STM32_E_STP_Pin GPIO_PIN_12
  #define STM32_E_STP_GPIO_Port GPIOC
  #define STM32_FAN_NOZ_B_Pin GPIO_PIN_15
  #define STM32_FAN_NOZ_B_GPIO_Port GPIOA
  #define STM32_SIG_DOOR_Pin GPIO_PIN_4
  #define STM32_SIG_DOOR_GPIO_Port GPIOE
  #define STM32_BUZZER_Pin GPIO_PIN_9
  #define STM32_BUZZER_GPIO_Port GPIOB
  #define STM32_X2_STP_Pin GPIO_PIN_7
  #define STM32_X2_STP_GPIO_Port GPIOB
  #define STM32_X2_DIR_Pin GPIO_PIN_6
  #define STM32_X2_DIR_GPIO_Port GPIOB
  #define STM32_E_EN_Pin GPIO_PIN_12
  #define STM32_E_EN_GPIO_Port GPIOG
  #define STM32_B_EN_Pin GPIO_PIN_10
  #define STM32_B_EN_GPIO_Port GPIOG
  #define STM32_SIG_Z_MAX_Pin GPIO_PIN_11
  #define STM32_SIG_Z_MAX_GPIO_Port GPIOC
  #define STM32_SIG_Z_MIN_Pin GPIO_PIN_10
  #define STM32_SIG_Z_MIN_GPIO_Port GPIOC
  #define STM32_TOUCH_MOSI_Pin GPIO_PIN_3
  #define STM32_TOUCH_MOSI_GPIO_Port GPIOI
  #define STM32_LIGHT_BAR_Pin GPIO_PIN_13
  #define STM32_LIGHT_BAR_GPIO_Port GPIOC
  #define STM32_TOUCH_CS_Pin GPIO_PIN_8
  #define STM32_TOUCH_CS_GPIO_Port GPIOI
  #define STM32_B_STP_Pin GPIO_PIN_3
  #define STM32_B_STP_GPIO_Port GPIOD
  #define STM32_X2_EN_Pin GPIO_PIN_2
  #define STM32_X2_EN_GPIO_Port GPIOD
  #define STM32_HEAT_BED_Pin GPIO_PIN_14
  #define STM32_HEAT_BED_GPIO_Port GPIOC
  #define STM32_SIG_X_Pin GPIO_PIN_2
  #define STM32_SIG_X_GPIO_Port GPIOH
  #define STM32_Z_STP_Pin GPIO_PIN_9
  #define STM32_Z_STP_GPIO_Port GPIOC
  #define STM32_FAN_EB_MOTOR_Pin GPIO_PIN_8
  #define STM32_FAN_EB_MOTOR_GPIO_Port GPIOA
  #define STM32_X_STP_Pin GPIO_PIN_3
  #define STM32_X_STP_GPIO_Port GPIOH
  #define STM32_Z_DIR_Pin GPIO_PIN_8
  #define STM32_Z_DIR_GPIO_Port GPIOC
  #define STM32_Z_EN_Pin GPIO_PIN_7
  #define STM32_Z_EN_GPIO_Port GPIOC
  #define STM32_X_DIR_Pin GPIO_PIN_4
  #define STM32_X_DIR_GPIO_Port GPIOH
  #define STM32_HEAT_NOZ_E_Pin GPIO_PIN_6
  #define STM32_HEAT_NOZ_E_GPIO_Port GPIOC
  #define STM32_X_EN_Pin GPIO_PIN_5
  #define STM32_X_EN_GPIO_Port GPIOH
  #define STM32_STEERING_ENGINE_Pin GPIO_PIN_6
  #define STM32_STEERING_ENGINE_GPIO_Port GPIOF
  #define STM32_TOUCH_MISO_Pin GPIO_PIN_3
  #define STM32_TOUCH_MISO_GPIO_Port GPIOG
  #define STM32_TOUCH_SCK_Pin GPIO_PIN_6
  #define STM32_TOUCH_SCK_GPIO_Port GPIOH
  #define STM32_E_DIR_Pin GPIO_PIN_8
  #define STM32_E_DIR_GPIO_Port GPIOH
  #define STM32_FAN_FILTER_Pin GPIO_PIN_13
  #define STM32_FAN_FILTER_GPIO_Port GPIOD
  #define STM32_TEMP_B_Pin GPIO_PIN_0
  #define STM32_TEMP_B_GPIO_Port GPIOA
  #define STM32_TOUCH_PEN_Pin GPIO_PIN_7
  #define STM32_TOUCH_PEN_GPIO_Port GPIOH
  #define STM32_TEMP_BED_Pin GPIO_PIN_6
  #define STM32_TEMP_BED_GPIO_Port GPIOA
  #define STM32_SIG_MAT_E0_Pin GPIO_PIN_5
  #define STM32_SIG_MAT_E0_GPIO_Port GPIOA
  #define STM32_SIG_MAT_E1_Pin GPIO_PIN_5
  #define STM32_SIG_MAT_E1_GPIO_Port GPIOA
  #define STM32_Y_STP_Pin GPIO_PIN_12
  #define STM32_Y_STP_GPIO_Port GPIOB
  #define STM32_SIG_Y_Pin GPIO_PIN_13
  #define STM32_SIG_Y_GPIO_Port GPIOB
  #define STM32_TEMP_E_Pin GPIO_PIN_3
  #define STM32_TEMP_E_GPIO_Port GPIOA
  #define STM32_TEMP_CAVITY_Pin GPIO_PIN_1
  #define STM32_TEMP_CAVITY_GPIO_Port GPIOB
  #define STM32_FAN_NOZ_E_Pin GPIO_PIN_0
  #define STM32_FAN_NOZ_E_GPIO_Port GPIOB
  #define STM32_Y_DIR_Pin GPIO_PIN_10
  #define STM32_Y_DIR_GPIO_Port GPIOB

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void user_board_init(void);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif /* USER_BOARD_H */

