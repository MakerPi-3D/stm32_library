#include "user_common.h"

#ifdef HAS_FAN_CONTROL

#if defined(STM32F429xx)


#elif defined(STM32F407xx)

extern TIM_HandleTypeDef htim5;

void feature_fan_control_init(void)
{
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
  htim5.Instance->CCR2 = 0;
}

void feature_fan_control_e_pwm(int pwm_value)
{
  if (pwm_value > 255)
  {
    pwm_value = 255;
  }

  pwm_value = (pwm_value * 1000) / 255;
  htim5.Instance->CCR2 = pwm_value;
}



#endif

#endif // HAS_FILAMENT_SENSOR

