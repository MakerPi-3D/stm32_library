#include "user_common.h"

#ifdef HAS_FILAMENT_SENSOR

#include "sysconfig_data.h"
#include "gcode.h"
#include "process_m_code.h"
#include "PrintControl.h"
#include "globalvariables.h"
#include "interface.h"
#include "flashconfig.h"
#include "temperature.h"
#include "stepper.h"
#include "commonf.h"
#include "common.h"

static uint8_t filament_check_count = 0;
static uint32_t filament_check_timeout = 0;

void _feature_filament_check_err(void)
{
  if (filament_check_count < 3)
  {
    filament_check_count++;
  }
}

void _feature_filament_reset_check_count(void)
{
  filament_check_count = 0;
  transFilePrintStatus.isFilamentCheck = false;
}

void _feature_filament_check_idex(void)
{
  static char cmd_buf[50] = {0};
  static char cmd_buf2[50] = {0};

  if (user_pin_sig_mat_e0_read() && user_pin_sig_mat_e1_read())
  {
    USER_EchoLogStr("feature_filament_check ==>> (dual) E0 and E1 not filament!\r\n");
    _feature_filament_check_err();
  }
  else
  {
    if (gcode::active_extruder == 0)
    {
      if (user_pin_sig_mat_e0_read())
      {
        if (temperature_get_extruder_target(1) < 80.0f) // 非双色模型
        {
          USER_EchoLogStr("feature_filament_check ==>> (idex_single) E0 not filament!\r\n");
          _feature_filament_reset_check_count();
          user_send_internal_cmd((char *)"T1");
          user_send_internal_cmd((char *)"M104 T0 S50");
          memset(cmd_buf, 0, sizeof(cmd_buf));
          (void)snprintf(cmd_buf, sizeof(cmd_buf), "M109 T1 S%d", (int)temperature_get_extruder_target(0));
          user_send_internal_cmd((char *)cmd_buf);
          memset(cmd_buf2, 0, sizeof(cmd_buf2));
          (void)snprintf(cmd_buf2, sizeof(cmd_buf2), "G92 B%f", st_get_position_length(E_AXIS));
          user_send_internal_cmd((char *)cmd_buf2);
        }
        else
        {
          USER_EchoLogStr("feature_filament_check ==>> (idex_dual) E0 not filament!\r\n");
          _feature_filament_check_err();
        }
      }
      else
      {
        _feature_filament_reset_check_count();
      }
    }
    else if (gcode::active_extruder == 1)
    {
      if (user_pin_sig_mat_e1_read() == GPIO_PIN_SET)
      {
        if (temperature_get_extruder_target(0) < 80.0f) // 非双色模型
        {
          USER_EchoLogStr("feature_filament_check ==>> (idex_single) E1 not filament!\r\n");
          _feature_filament_reset_check_count();
          user_send_internal_cmd((char *)"T0");
          user_send_internal_cmd((char *)"M104 T1 S50");
          memset(cmd_buf, 0, sizeof(cmd_buf));
          (void)snprintf(cmd_buf, sizeof(cmd_buf), "M109 T0 S%d", (int)temperature_get_extruder_target(1));
          user_send_internal_cmd((char *)cmd_buf);
          memset(cmd_buf2, 0, sizeof(cmd_buf2));
          (void)snprintf(cmd_buf2, sizeof(cmd_buf2), "G92 E%f", st_get_position_length(B_AXIS));
          user_send_internal_cmd((char *)cmd_buf2);
        }
        else
        {
          USER_EchoLogStr("feature_filament_check ==>> (idex_dual) E1 not filament!\r\n");
          _feature_filament_check_err();
        }
      }
      else
      {
        _feature_filament_reset_check_count();
      }
    }
  }
}

void _feature_filament_check_single(void)
{
  if (user_pin_sig_mat_e0_read())
  {
    _feature_filament_check_err();
  }
  else
  {
    _feature_filament_reset_check_count();
  }
}


void feature_filament_check(void)
{
  if (t_sys_data_current.enable_material_check) //有断料检测功能
  {
    if (filament_check_timeout < xTaskGetTickCount())
    {
      if (IsPrint() && isM109HeatingComplete() && gcode::g28_complete_flag) //打印开始且加热完成归零后 才去检测是否有料
      {
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL && t_sys.is_idex_extruder == 1) // idex模式
        {
          if (t_sys.idex_print_type == IDEX_PRINT_TYPE_NORMAL) //正常模式
          {
            _feature_filament_check_idex(); // idex
          }
          else if (t_sys.idex_print_type == IDEX_PRINT_TYPE_COPY || t_sys.idex_print_type == IDEX_PRINT_TYPE_MIRROR) // 克隆、镜像模式
          {
            if (user_pin_sig_mat_e0_read() || user_pin_sig_mat_e1_read()) // 任意头没料，报警
            {
              USER_EchoLogStr("feature_filament_check ==>> (copy\\mirror) E0 or E1 not filament!\r\n");
              _feature_filament_check_err();
            }
          }
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER) // 激光模式不检测
        {
          return;
        }
        else
        {
          _feature_filament_check_single(); // 单头、混色头
        }
      }

      if (filament_check_count == 3)
      {
        filament_check_count = 0;
        USER_EchoLogStr("IsNotHaveMatInPrint\r\n");
        transFilePrintStatus.isFilamentCheck = true;
        IsNotHaveMatInPrint = 1; //在打印的时候没料了
        osDelay(100);
        waiting_for_pausing();
      }

      filament_check_timeout = MILLIS() + 1000;
    }
  }
}

#endif // HAS_FILAMENT_SENSOR

