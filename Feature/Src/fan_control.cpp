#include "user_common.h"

#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F429xx)
#include "config_model_tables.h"
#include "sysconfig_data.h"
#include "flashconfig.h"
#include "temperature.h"
#include "planner.h"

extern int IsPrint(void);
extern uint8_t GetIsDisableStepperStatus(void);

static volatile int user_extruder_fan_speed = 0;

#elif defined(STM32F407xx)

#endif

// 电机风扇控制
static void feature_fan_control_eb_motor(void)
{
  #if defined(STM32F429xx)
  static uint8_t motor_fan_status = 0;
  static unsigned long motor_timeoutToStatus = 0;
  static unsigned long motor_timeToReset = 0;

  if ((planner_moves_planned() || IsPrint()) && !GetIsDisableStepperStatus())
  {
    if (motor_fan_status == 0)
    {
      user_fan_control_eb_motor(true);
      motor_fan_status = 1;
    }

    motor_timeoutToStatus = xTaskGetTickCount() + 1 * 60 * 1000UL; //1 minute

    // add by suzhiwei 20160711
    // 每5min复位3秒，防止风扇停转
    if (xTaskGetTickCount() > motor_timeToReset && xTaskGetTickCount() <= motor_timeToReset + 3000UL)
    {
      user_fan_control_eb_motor(false);
    }
    else if (xTaskGetTickCount() > motor_timeToReset + 3000UL)     //5sec+ 5min
    {
      motor_timeToReset = xTaskGetTickCount() + 300000UL;
      user_fan_control_eb_motor(true);
    }
  }
  else
  {
    if (motor_fan_status == 1 && motor_timeoutToStatus < xTaskGetTickCount())
    {
      user_fan_control_eb_motor(false);
      motor_fan_status = 0;
    }
  }

  #elif defined(STM32F407xx)
  #endif
}

// 喷嘴加热块风扇控制
static void feature_fan_control_nozzle_heat_block(void)
{
  #if defined(STM32F429xx)

  if (temperature_get_extruder_current(0) >= 50 || (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL && temperature_get_extruder_current(1) >= 50))
  {
    user_fan_control_nozzle_heat_block(true);
  }
  else
  {
    user_fan_control_nozzle_heat_block(false);
  }

  #elif defined(STM32F407xx)
  #endif
}

int feature_get_extruder_fan_speed(void)
{
  return user_extruder_fan_speed;
}

void feature_set_extruder_fan_speed(int value)
{
  #if defined(STM32F429xx)

  if (user_extruder_fan_speed == value) return; //防止重复设置

  user_extruder_fan_speed = value;

  if (mcu_id == MCU_GD32F450IIH6)
  {
    user_fan_control_e_pwm(user_extruder_fan_speed);

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      //激光头使用风扇B-PWM接口，设置数据大于0，全功率设置，避免激光启动不了
      user_fan_control_b_pwm(user_extruder_fan_speed > 0 ? 255 : user_extruder_fan_speed);
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      user_fan_control_b_pwm(user_extruder_fan_speed);
    }
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    if (P2_Pro == t_sys_data_current.model_id)
    {
      USER_GPIO_WRITE(STM32, FAN_EB_MOTOR, (value > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET)); // 使用eb电机风扇接口
    }
    else if (P3_Pro == t_sys_data_current.model_id || F400TP == t_sys_data_current.model_id)
    {
      user_fan_control_e_pwm(user_extruder_fan_speed);
    }
  }

  #elif defined(STM32F407xx)
  #endif
}

void feature_fan_control(void)
{
  feature_fan_control_eb_motor();
  feature_fan_control_nozzle_heat_block();
}


#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_FAN_CONTROL

