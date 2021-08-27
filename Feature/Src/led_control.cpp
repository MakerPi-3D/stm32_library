#include "user_common.h"

#ifdef HAS_LED_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F429xx)
#include "user_ccm.h"
#include "globalvariables.h"
#include "planner.h"
#include "process_m_code.h"
#include "temperature.h"

extern int IsHeating(void);
extern int IsPrint(void);

#elif defined(STM32F407xx)
#include "config_model_tables.h"
#include "sysconfig_data.h"
#include "globalvariables.h"
#include "planner.h"
#include "temperature.h"

extern int IsHeating(void);
extern int IsPrint(void);

#endif

static volatile bool led_lighting_status = false;

//主板灯
static void _feature_led_control_board(void)
{
  static unsigned long led_last_time = 0UL;

  if (led_last_time < xTaskGetTickCount())
  {
    user_led_board_toggle();
    led_last_time = xTaskGetTickCount() + 500U; // 500ms闪烁一次
  }
}

//警示灯闪烁
static void _feature_led_control_caution_twinkle(uint32_t delay_time)
{
  static unsigned long CautionLightTimeControl = 0UL;

  if (CautionLightTimeControl < xTaskGetTickCount())
  {
    user_led_caution_toggle();
    CautionLightTimeControl = xTaskGetTickCount() + delay_time;
  }
}

//照明灯条控制
static void _feature_led_control_lighting(void)
{
  #if defined(STM32F429xx)
  static uint8_t LEDLight_status = 0;
  static unsigned long LEDLight_timeoutToStatus = 0;

  if (planner_moves_planned() || IsHeating()) // 移动或加热，灯条开启
  {
    if (LEDLight_status == 0)
    {
      user_led_control_lighting_status(true);
      LEDLight_status = 1;
    }

    LEDLight_timeoutToStatus = xTaskGetTickCount() + 1 * 60 * 1000UL; //5 minute
  }
  else
  {
    if (LEDLight_status == 1 && LEDLight_timeoutToStatus < xTaskGetTickCount())
    {
      user_led_control_lighting_status(false);
      LEDLight_status = 0;
    }
  }

  #elif defined(STM32F407xx)
  static uint8_t LEDLight_status = 0U;
  static unsigned long LEDLight_timeoutToStatus = 0UL;

  if ((0 != sg_grbl::planner_moves_planned()) || (0 != IsHeating())) // 移动或加热，灯条开启
  {
    if (LEDLight_status == 0U)
    {
      led_lighting_status = true;
      user_led_control_lighting_status(led_lighting_status);
      LEDLight_status = 1U;
    }

    LEDLight_timeoutToStatus = xTaskGetTickCount() + (60UL * 1000UL); //1 minute timeout
  }
  else
  {
    if ((LEDLight_status == 1U) && (LEDLight_timeoutToStatus < xTaskGetTickCount()))
    {
      led_lighting_status = false;
      user_led_control_lighting_status(led_lighting_status);
      LEDLight_status = 0U;
    }
  }

  #endif
}

//警示灯控制
static void _feature_led_control_caution_light(void)
{
  #if defined(STM32F429xx)
  static uint8_t LastStatus = 0;
  static bool doorOpenBeep = false;//标志门检打开了蜂鸣器

  if (doorStatus && IsPrint()) //打印中门未关闭
  {
    _feature_led_control_caution_twinkle(100);//0.1S闪烁
    LastStatus = 1;
    IsDisplayDoorOpenInfo = 0;
  }
  else if (IsPrint() && isM109HeatingComplete()) //加热完成并开始打印
  {
    LastStatus = 2;
    IsDisplayDoorOpenInfo = 0;
  }
  else if ((int)temperature_get_extruder_current(0) > 60 || IsPrint()) //温度大于60度 或 打印前的加热阶段
  {
    if (doorStatus)
    {
      isOpenBeep = 1;
      doorOpenBeep = true;
      _feature_led_control_caution_twinkle(100);//0.1S闪烁
    }
    else
    {
      if (doorOpenBeep)
      {
        isOpenBeep = 0;
        doorOpenBeep = false;
      }

      _feature_led_control_caution_twinkle(500);//0.5S闪烁
    }

    if (((int)temperature_get_extruder_current(0) > 60) && !IsPrint() && doorStatus) //待机、预热、进丝、退丝 时 温度大于60度且门打开显示提示信息
    {
      IsDisplayDoorOpenInfo = 1;
    }
    else
    {
      IsDisplayDoorOpenInfo = 0;
    }

    LastStatus = 3;
  }
  else //待机且温度小于60度
  {
    if ((LastStatus == 3) && doorStatus)
    {
      isOpenBeep = 0;
    }

    LastStatus = 4;
    IsDisplayDoorOpenInfo = 0;
  }

  #elif defined(STM32F407xx)
  static uint8_t LastStatus = 0U;
  static bool doorOpenBeep = false;//标志门检打开了蜂鸣器

  if ((0U != t_gui_p.doorStatus) && (0 != IsPrint())) //打印中门未关闭
  {
    _feature_led_control_caution_twinkle(100U);//0.1S闪烁
    LastStatus = 1U;
    t_gui_p.IsDisplayDoorOpenInfo = 0U;
  }
  else if ((0 != IsPrint()) && (1U == t_gui_p.m109_heating_complete)) //加热完成并开始打印
  {
    user_led_control_caution_status(true);
    LastStatus = 2U;
    t_gui_p.IsDisplayDoorOpenInfo = 0U;
  }
  else if ((sg_grbl::temperature_get_extruder_current(0) > 60.0F) || (0 != IsPrint())) //温度大于60度 或 打印前的加热阶段
  {
    if (0U != t_gui_p.doorStatus)
    {
      t_gui_p.isBeepAlarm = 1;
      user_buzzer_control(false);
      doorOpenBeep = true;
      _feature_led_control_caution_twinkle(100U);//0.1S闪烁
    }
    else
    {
      if (doorOpenBeep)
      {
        t_gui_p.isBeepAlarm = 0;
        doorOpenBeep = false;
      }

      _feature_led_control_caution_twinkle(500U);//0.5S闪烁
    }

    if ((sg_grbl::temperature_get_extruder_current(0) > 60.0F) && (0 == IsPrint()) && (0U != t_gui_p.doorStatus)) //待机、预热、进丝、退丝 时 温度大于60度且门打开显示提示信息
    {
      t_gui_p.IsDisplayDoorOpenInfo = 1U;
    }
    else
    {
      t_gui_p.IsDisplayDoorOpenInfo = 0U;
    }

    LastStatus = 3U;
  }
  else //待机且温度小于60度
  {
    if ((LastStatus == 3U) && (0U != t_gui_p.doorStatus))
    {
      t_gui_p.isBeepAlarm = 0;
    }

    user_led_control_caution_status(false);
    LastStatus = 4U;
    t_gui_p.IsDisplayDoorOpenInfo = 0U;
  }

  #endif
}

//照明灯条状态设置
void feature_led_set_lighting_status(bool value)
{
  led_lighting_status = value;
}

//照明灯条状态获取
bool feature_led_get_lighting_status(void)
{
  return led_lighting_status;
}

//LED控制入口
void feature_led_control(void)
{
  #if defined(STM32F429xx)
  _feature_led_control_board(); // 核心板LED一直开启

  if (t_custom_services.enable_led_light) // 有LED灯条功能，开启LED灯条照明
    _feature_led_control_lighting();

  if (t_custom_services.enable_warning_light &&
      ccm_param::motion_3d.enable_check_door_open) // 有警示灯和门检测功能，开启警示灯
    _feature_led_control_caution_light();

  #elif defined(STM32F407xx)
  _feature_led_control_board(); // 核心板LED一直开启

  if (0U != t_custom_services.enable_led_light) // 有LED灯条功能，开启LED灯条照明
  {
    _feature_led_control_lighting();
  }

  if ((0U != t_custom_services.enable_warning_light) &&
      (0U != motion_3d.enable_check_door_open)) // 有警示灯和门检测功能，开启警示灯
  {
    _feature_led_control_caution_light();
  }

  #endif
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_LED_CONTROL

