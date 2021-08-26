#include "user_common.h"

#ifdef HAS_BUZZER_CONTROL

#if defined(STM32F429xx)
  extern TIM_HandleTypeDef htim11;
#elif defined(STM32F407xx)
  #include "globalvariables.h"
  #include "pins.h"
#endif

static volatile bool is_alarm_on = false;

void user_buzzer_init(void)
{
  #if defined(STM32F429xx)
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
  #elif defined(STM32F407xx)
  #endif
}

void user_buzzer_control(const bool is_on)
{
  #if defined(STM32F429xx)
  unsigned int compare = is_on ? 1000 : 0;
  //compare = (compare / 0.9 + 50) * 10;
  htim11.Instance->ARR  = compare;
  htim11.Instance->CCR1 = compare / 2;
  #elif defined(STM32F407xx)
  t_gui_p.isOpenBeep = is_on;
  digitalWrite(BEEPER_PIN, is_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
  #endif
}

void user_buzzer_beep(unsigned short time)
{
  static unsigned long timeout_beeper = 0;
  static bool b = false;

  if (0 == time)
  {
    user_buzzer_control(false);
    return;
  }

  if (timeout_beeper < xTaskGetTickCount())
  {
    timeout_beeper = xTaskGetTickCount() + time;
    b = !b;

    if (b) user_buzzer_control(true);
    else user_buzzer_control(false);
  }
}

void user_buzzer_buzz(unsigned short msticks)
{
  user_buzzer_control(true);
  (void)osDelay(msticks);
  user_buzzer_control(false);
  (void)osDelay(msticks);
}

void user_buzzer_set_alarm_status(const bool is_on)
{
  is_alarm_on = is_on;
}

void user_buzzer_beep_alarm(void)
{
  static bool IsCloseBeep = true;

  if (is_alarm_on)
  {
    user_buzzer_beep(500);
    IsCloseBeep = false;
  }
  else
  {
    if (!IsCloseBeep)
    {
      user_buzzer_beep(0);
      IsCloseBeep = true;
    }
  }
}

#endif



