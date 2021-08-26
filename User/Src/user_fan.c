#include "user_common.h"

#ifdef HAS_FAN_CONTROL

#if defined(STM32F429xx)
  #include "config_model_tables.h"
  #include "sysconfig_data.h"

  extern TIM_HandleTypeDef htim2;
  extern TIM_HandleTypeDef htim3;

#elif defined(STM32F407xx)
  #include "config_model_tables.h"
  #include "sysconfig_data.h"
  #include "globalvariables.h"
  #include "pins.h"
  extern TIM_HandleTypeDef htim5;
#endif

void user_fan_control_init(void)
{
  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //B喷嘴风扇
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); //E喷嘴风扇
    htim2.Instance->CCR1 = 2120;
    htim2.Instance->CCR2 = 0;
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); //E喷嘴风扇
  }

  #elif defined(STM32F407xx)
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); //E喷嘴风扇
  htim5.Instance->CCR2 = 0;

  // 加热块扇热风扇（5V）初始化
  if (t_sys_data_current.model_id != M41G)
  {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }

  #endif
}

//E喷嘴风扇
void user_fan_control_e_pwm(int pwm_value)
{
  unsigned short pwn_tmp = pwm_value;

  if (pwm_value > 255)
    pwn_tmp = 255;

  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    pwn_tmp = pwn_tmp * 2120 / 255;
    htim2.Instance->CCR2 = pwn_tmp;
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    pwn_tmp = pwn_tmp * 1000 / 255;
    htim3.Instance->CCR3 = pwn_tmp;
  }

  #elif defined(STM32F407xx)
  pwn_tmp = (pwn_tmp * 1000) / 255;
  htim5.Instance->CCR2 = pwn_tmp;
  #endif
}

//B喷嘴风扇
void user_fan_control_b_pwm(int pwm_value)
{
  unsigned short pwn_tmp = pwm_value;

  if (pwm_value > 255)
    pwn_tmp = 255;

  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    pwn_tmp = (255 - pwn_tmp) * 2120 / 255;
    htim2.Instance->CCR1 = pwn_tmp;
  }

  #elif defined(STM32F407xx)
  #endif
}

//EB电机风扇
void user_fan_control_eb_motor(bool isOn)
{
  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    USER_GPIO_WRITE(GD32, FAN_EB_MOTOR, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    if (P3_Pro == t_sys_data_current.model_id || P2_Pro == t_sys_data_current.model_id || F400TP == t_sys_data_current.model_id) //没有EB电机风扇
    {
    }
    else
    {
      USER_GPIO_WRITE(STM32, FAN_EB_MOTOR, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
    }
  }

  #elif defined(STM32F407xx)
  digitalWrite(MOTOR_FAN_PIN, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
  #endif
}

//喷嘴加热块扇热风扇
void user_fan_control_nozzle_heat_block(bool isOn)
{
  #if defined(STM32F429xx)

  if (mcu_id == MCU_GD32F450IIH6)
  {
    USER_GPIO_WRITE(GD32, FAN_FILTER, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET)); // 使用过滤风扇接口
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    if (P2_Pro == t_sys_data_current.model_id)
    {
      user_fan_control_e_pwm(isOn ? 204 : 0); // P2使用E喷嘴扇热风扇
    }
    else if (P3_Pro == t_sys_data_current.model_id || F400TP == t_sys_data_current.model_id)
    {
      USER_GPIO_WRITE(STM32, FAN_EB_MOTOR, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET)); // 使用EB电机风扇接口
    }
  }

  #elif defined(STM32F407xx)

  if (t_sys_data_current.enable_color_mixing) //混色打开5V风扇
  {
    if (t_sys_data_current.model_id != M41G) //M41G 5V_FAN做断料检测IO
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, isOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
  }

  #endif
}

void user_fan_control_board_cool(bool isOn)
{
  #if defined(STM32F429xx)
  //Alway on
  #elif defined(STM32F407xx)
  digitalWrite(BOARD_FAN_PIN, isOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
  #endif
}



#endif // HAS_FILAMENT_SENSOR

