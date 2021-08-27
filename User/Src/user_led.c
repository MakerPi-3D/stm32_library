#include "user_common.h"

#ifdef HAS_LED_CONTROL

#if defined(STM32F429xx)
#elif defined(STM32F407xx)
  #include "config_model_tables.h"
  #include "sysconfig_data.h"
  #include "globalvariables.h"
#endif

void user_led_control_init(void)
{
  #if defined(STM32F429xx)
  #elif defined(STM32F407xx)
  #endif
}

//主板灯
void user_led_board_toggle(void)
{
  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    HAL_GPIO_TogglePin(GD32_LIGHT_GPIO_Port, GD32_LIGHT_Pin);
  }

  #elif defined(STM32F407xx)
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
  #endif
}

//警示灯闪烁
void user_led_caution_toggle(void)
{
  #if defined(STM32F429xx)
  //None
  #elif defined(STM32F407xx)

  if (t_sys_data_current.model_id != M41G)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
  }

  #endif
}

//警示灯
void user_led_control_caution_status(bool isOn)
{
  #if defined(STM32F429xx)
  //None
  #elif defined(STM32F407xx)

  if (t_sys_data_current.model_id != M41G)
  {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, isOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  #endif
}

//照明灯条
void user_led_control_lighting_status(bool isOn)
{
  #if defined(STM32F429xx)
  user_pin_light_bar_ctrl(isOn);
  #elif defined(STM32F407xx)
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, isOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
  #endif
}

#endif // HAS_LED_CONTROL












