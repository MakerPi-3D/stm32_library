#include "user_common.h"

#ifdef HAS_POWER_RECOVERY

#if defined(STM32F429xx)
  #include "user_ccm.h"
  #include "flashconfig.h"
  #include "controlxyz.h"
  #include "process_m_code.h"
  #include "process_command.h"
  #include "globalvariables.h"
  #include "PrintControl.h"
  #include "sysconfig_data.h"
  #include "gcode.h"
  #include "planner.h"
#elif defined(STM32F407xx)
  #include "controlxyz.h"
  #include "globalvariables.h"
  #include "Alter.h"
  #include "gcode.h"
  #include "stepper.h"
  #include "planner.h"
  #include "gcode_global_params.h"
  #include "config_model_tables.h"
  #include "PrintControl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static bool isPowerOffRecoverPrint = false;                       /*!< 是否为断电恢复操作 */
static bool isPowerOffRecoverPrintFinish = false;                 /*!< 是否为断电恢复操作 */
static bool isXYHome = false;

void feature_pow_rec_ready_to_print(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    #if defined(STM32F429xx)
    stepper_quick_stop(); // 电机快速停止
    resetM109HeatingComplete(); //设置为未加热
    ccm_param::motion_3d.updown_g28_first_time = 0;
    user_send_internal_cmd((char *)"G90");
    user_send_internal_cmd((char *)"M82");
    user_send_internal_cmd((char *)"M109 S130"); // 先加热到130度，再移动喷嘴防止喷嘴与打印模具粘在一起。

    if (flash_param_t.idex_print_type == IDEX_PRINT_TYPE_COPY || flash_param_t.idex_print_type == IDEX_PRINT_TYPE_MIRROR)
    {
      user_send_internal_cmd((char *)"M109 T1 S130");
    }

    OS_DELAY(600);
    xy_to_zero();
    isPowerOffRecoverPrint = 1;
    #elif defined(STM32F407xx)
    stepper_quick_stop(); // 电机快速停止
    #if LASER_MODE

    if (t_sys_data_current.IsLaser)
      t_gui_p.m109_heating_complete = 1U;
    else
    #endif
      t_gui_p.m109_heating_complete = 0U; //设置为未加热

    if (t_power_off.sd_pos != 0U)
    {
      motion_3d.updown_g28_first_time = 0;
      user_send_internal_cmd("G90 isInternal");
      user_send_internal_cmd("M82 isInternal");
      #if LASER_MODE

      if (!t_sys_data_current.IsLaser)
      #endif
      {
        user_send_internal_cmd("M109 S180 isInternal");// 先加热到180度，再移动喷嘴防止喷嘴与打印模具粘在一起。
      }

      z_down_to_bottom(); // Z下降到底部 XY归零命令
    }
    else
    {
      motion_3d.updown_g28_first_time = 0U;
      user_send_internal_cmd("G90 isInternal");
      user_send_internal_cmd("M82 isInternal");
      user_send_internal_cmd("G28 isInternal");
    }

    isPowerOffRecoverPrint = 1;
    isXYHome = false;
    t_power_off.is_power_off = 1U; //标志为断电状态，防止把当前sdPos写入到poweroff_data中，解决断电多次重新打印现象
    #endif
  }
}

static void _feature_pow_rec_finish(void)
{
  #if defined(STM32F429xx)

  if (isPowerOffRecoverPrintFinish)
  {
    IsFinishedPowerOffRecoverReady = 1; // UI界面更新标志位
    PowerOffRecStartPrint(); //开始从文件读取内容继续去打印
    isPowerOffRecoverPrintFinish = false;
  }

  #elif defined(STM32F407xx)

  if (isPowerOffRecoverPrintFinish)
  {
    t_gui_p.IsFinishedPowerOffRecoverReady = 1U; // UI界面更新标志位

    if (0U == t_power_off.serial_flag)
    {
      PowerOffRecStartPrint(); //开始从文件读取内容继续去打印
    }

    isPowerOffRecoverPrintFinish = false;
    t_power_off.is_power_off = 0U;
  }

  #endif
}

static void _feature_pow_rec_temp_set(void)
{
  #if defined(STM32F429xx)
  #elif defined(STM32F407xx)
  static CHAR gcodeM140CommandBuf[50] = {0};
  static CHAR gcodeM104CommandBuf[50] = {0};
  (void)memset(gcodeM140CommandBuf, 0, sizeof(gcodeM140CommandBuf));
  (void)snprintf(gcodeM140CommandBuf, sizeof(gcodeM140CommandBuf), "M140 S%d isInternal", t_power_off.bed_target_temp);
  user_send_internal_cmd(gcodeM140CommandBuf);//设置Z最大位置
  (void)memset(gcodeM104CommandBuf, 0, sizeof(gcodeM104CommandBuf));
  (void)snprintf(gcodeM104CommandBuf, sizeof(gcodeM104CommandBuf), "M104 S%d isInternal", t_power_off.nozzle_target_temp);
  user_send_internal_cmd(gcodeM104CommandBuf);//设置Z最大位置
  #endif
}

static void _feature_pow_rec_eb_set(void)
{
  #if defined(STM32F429xx)
  eb_compensate_8mm(t_sys_data_current.enable_color_mixing);
  g92_set_axis_position((int)E_AXIS, t_power_off.e_pos);

  if (t_sys_data_current.enable_color_mixing)
  {
    g92_set_axis_position((int)B_AXIS, t_power_off.b_pos);
  }

  #elif defined(STM32F407xx)

  if (0U != t_power_off.blockdetectflag) //20170927,堵料时断电续打须进丝一段时间
  {
    user_send_internal_cmd("G92 E0 isInternal");
    user_send_internal_cmd("G1 F150 E150 H0 isInternal");
    t_power_off.blockdetectflag = 0U;
  }

  eb_compensate_16mm(t_sys_data_current.enable_color_mixing);
  g92_set_axis_position(static_cast<INT>(E_AXIS), t_power_off.e_pos);

  if (0U != t_sys_data_current.enable_color_mixing)
  {
    g92_set_axis_position(static_cast<INT>(B_AXIS), t_power_off.b_pos);
  }

  #endif
}

static void _feature_pow_rec_z_set(void)
{
  #if defined(STM32F429xx)
  static char gcodeG1FCommandBuf[50] = {0};
  static char gcodeG1ZCommandBuf[50] = {0};
  static char gcodeG1XYCommandBuf[50] = {0};
  user_send_internal_cmd((char *)"M2003 S0"); // 关闭坐标转换
  memset(gcodeG1ZCommandBuf, 0, sizeof(gcodeG1ZCommandBuf));
  (void)snprintf(gcodeG1ZCommandBuf, sizeof(gcodeG1ZCommandBuf), "G92 Z%f R%u", t_power_off.z_pos, t_power_off.sd_pos);
  user_send_internal_cmd((char *)gcodeG1ZCommandBuf); //Z位置
  memset(gcodeG1XYCommandBuf, 0, sizeof(gcodeG1XYCommandBuf));
  (void)snprintf(gcodeG1XYCommandBuf, sizeof(gcodeG1XYCommandBuf), "G1 F2400 X%f Y%f Z%f", t_power_off.x_pos, t_power_off.y_pos, t_power_off.z_pos); //添加上文件位置
  user_send_internal_cmd((char *)gcodeG1XYCommandBuf);
  user_send_internal_cmd((char *)"M2003 S1");    // 开启坐标转换
  user_send_internal_cmd((char *)"M2003 S0");    // 关闭坐标转换
  memset(gcodeG1FCommandBuf, 0, sizeof(gcodeG1FCommandBuf));

  if (t_power_off.feed_rate > 2400)
    t_power_off.feed_rate = 2400; // 限制移动速度最大为40mm/s

  (void)snprintf(gcodeG1FCommandBuf, sizeof(gcodeG1FCommandBuf), "G1 F%f", t_power_off.feed_rate);
  user_send_internal_cmd((char *)gcodeG1FCommandBuf); //出料速度
  user_send_internal_cmd((char *)"M2003 S1");    // 开启坐标转换
  #elif defined(STM32F407xx)

  // 需要判断是否归零
  if ((motion_3d_model.xyz_move_max_pos[0] < t_power_off.x_pos) || (motion_3d_model.xyz_move_max_pos[1] < t_power_off.y_pos))
  {
    user_send_internal_cmd("G28 isInternal");
  }
  else
  {
    static CHAR gcodeG1ZCommandBuf[50] = {0};
    (void)memset(gcodeG1ZCommandBuf, 0, sizeof(gcodeG1ZCommandBuf));

    if ((M4141 == t_sys_data_current.model_id) || (M4141S_NEW == t_sys_data_current.model_id) || (M4141S == t_sys_data_current.model_id))
    {
      (void)snprintf(gcodeG1ZCommandBuf, sizeof(gcodeG1ZCommandBuf), "G1 F400 Z%f P%u I0 H0 isInternal", t_power_off.z_pos, t_power_off.sd_pos); //添加上文件位置
    }
    else
    {
      (void)snprintf(gcodeG1ZCommandBuf, sizeof(gcodeG1ZCommandBuf), "G1 F600 Z%f P%u I0 H0 isInternal", t_power_off.z_pos, t_power_off.sd_pos); //添加上文件位置
    }

    user_send_internal_cmd(gcodeG1ZCommandBuf);//Z位置
    static CHAR gcodeG1XYCommandBuf[50] = {0};
    (void)memset(gcodeG1XYCommandBuf, 0, sizeof(gcodeG1XYCommandBuf));
    (void)snprintf(gcodeG1XYCommandBuf, sizeof(gcodeG1XYCommandBuf), "G1 F2400 X%f Y%f H0 isInternal", t_power_off.x_pos, t_power_off.y_pos); //添加上文件位置
    user_send_internal_cmd(gcodeG1XYCommandBuf);//Z位置
  }

  static CHAR gcodeG1FCommandBuf[50] = {0};
  (void)memset(gcodeG1FCommandBuf, 0, sizeof(gcodeG1FCommandBuf));

  if (t_power_off.feed_rate > 2400.0F)
  {
    t_power_off.feed_rate = 2400.0F; // 限制移动速度最大为40mm/s
  }

  (void)snprintf(gcodeG1FCommandBuf, sizeof(gcodeG1FCommandBuf), "G1 F%f H0 isInternal", t_power_off.feed_rate * 60.0f * 100.0f / t_power_off.feed_multiply);
  user_send_internal_cmd(gcodeG1FCommandBuf);//出料速度
  #endif
}

static void _feature_pow_rec_heating(void)
{
  #if defined(STM32F429xx)
  static char gcodeM104OrM190CommandBuf[50] = {0};
  static char gcodeM109CommandBuf[50] = {0};
  memset(gcodeM104OrM190CommandBuf, 0, sizeof(gcodeM104OrM190CommandBuf));

  if (!t_custom_services.disable_hot_bed)
  {
    if (t_power_off.bed_target_temp > 50)
    {
      user_send_internal_cmd((char *)"M190 S50");
      (void)snprintf(gcodeM104OrM190CommandBuf, sizeof(gcodeM104OrM190CommandBuf), "M140 S%d", t_power_off.bed_target_temp);
    }
    else
    {
      (void)snprintf(gcodeM104OrM190CommandBuf, sizeof(gcodeM104OrM190CommandBuf), "M190 S%d", t_power_off.bed_target_temp);
    }

    user_send_internal_cmd((char *)gcodeM104OrM190CommandBuf); //热床加温命令
  }

  memset(gcodeM109CommandBuf, 0, sizeof(gcodeM109CommandBuf));
  (void)snprintf(gcodeM109CommandBuf, sizeof(gcodeM109CommandBuf), "M109 S%d", t_power_off.nozzle_target_temp);
  user_send_internal_cmd((char *)gcodeM109CommandBuf); //喷嘴加温命令
  #elif defined(STM32F407xx)
  static CHAR gcodeM104OrM190CommandBuf[50] = {0};
  static CHAR gcodeM109CommandBuf[50] = {0};
  (void)memset(gcodeM104OrM190CommandBuf, 0, sizeof(gcodeM104OrM190CommandBuf));

  if (0U == t_custom_services.disable_hot_bed)
  {
    if (t_power_off.bed_target_temp > 50U)
    {
      user_send_internal_cmd("M190 S50 isInternal");
      (void)snprintf(gcodeM104OrM190CommandBuf, sizeof(gcodeM104OrM190CommandBuf), "M140 S%d isInternal", t_power_off.bed_target_temp);
    }
    else
    {
      (void)snprintf(gcodeM104OrM190CommandBuf, sizeof(gcodeM104OrM190CommandBuf), "M190 S%d isInternal", t_power_off.bed_target_temp);
    }

    user_send_internal_cmd(gcodeM104OrM190CommandBuf);//热床加温命令
  }

  (void)memset(gcodeM109CommandBuf, 0, sizeof(gcodeM109CommandBuf));
  (void)snprintf(gcodeM109CommandBuf, sizeof(gcodeM109CommandBuf), "M109 S%d isInternal", t_power_off.nozzle_target_temp);
  user_send_internal_cmd(gcodeM109CommandBuf);//喷嘴加温命令
  #endif
}

static void _feature_pow_rec_close_to_saved_z(void)
{
  #if defined(STM32F429xx)
  static char gcodeG1CommandBuf[50] = {0};
  user_send_internal_cmd((char *)"M2003 S0");    // 关闭坐标转换
  memset(gcodeG1CommandBuf, 0, sizeof(gcodeG1CommandBuf));
  //  float zUpValue = ((t_sys_data_current.poweroff_rec_z_max_value - t_power_off.z_pos) > 50) ? t_power_off.z_pos + 50 : t_sys_data_current.poweroff_rec_z_max_value;
  (void)snprintf(gcodeG1CommandBuf, sizeof(gcodeG1CommandBuf), "G92  Z%f", t_power_off.z_pos);
  user_send_internal_cmd((char *)gcodeG1CommandBuf); //设置Z最大位置
  user_send_internal_cmd((char *)"M2003 S1"); // 开启坐标转换
  #elif defined(STM32F407xx)
  static CHAR gcodeG1CommandBuf[50] = {0};
  (void)memset(gcodeG1CommandBuf, 0, sizeof(gcodeG1CommandBuf));
  float zUpValue = t_sys_data_current.poweroff_rec_z_max_value;

  if (t_sys_data_current.poweroff_rec_z_max_value > (50.0F + t_power_off.z_pos)) // float
  {
    zUpValue = 50.0F + t_power_off.z_pos; // float
  }

  if ((M4141 == t_sys_data_current.model_id) || (M4141S_NEW == t_sys_data_current.model_id) || (M4141S == t_sys_data_current.model_id))
  {
    (void)snprintf(gcodeG1CommandBuf, sizeof(gcodeG1CommandBuf), "G1 F200 Z%.2f I0 H0 isInternal", zUpValue);
  }
  else
  {
    (void)snprintf(gcodeG1CommandBuf, sizeof(gcodeG1CommandBuf), "G1 F600 Z%.2f I0 H0 isInternal", zUpValue);
  }

  user_send_internal_cmd(gcodeG1CommandBuf);//设置Z最大位置
  #endif
}

static void _feature_pow_rec_process_loop(void)
{
  #if defined(STM32F429xx)
  static uint8_t powerOffRecoverPrintStatus = 0;
  static char gcodeM140CommandBuf[50] = {0};
  static char gcodeM104CommandBuf[50] = {0};
  gcode::g28_complete_flag = false; //设置为归零

  if (powerOffRecoverPrintStatus == 0 && (isM109HeatingComplete()) && 0 == planner_moves_planned())   //加热完成和Z轴向下完成
  {
    t_power_off.is_power_off = 1; //标志为断电状态，防止把当前sdPos写入到poweroff_data中，解决断电多次重新打印现象
    gcode::g28_complete_flag = false; //设置为归零
    //    ccm_param::motion_3d.updown_g28_first_time = 1; // 设置已经执行了上下共限位归零操作
    //    z_check_and_set_bottom(ccm_param::motion_3d.enable_poweroff_up_down_min_min, t_sys_data_current.poweroff_rec_z_max_value); // 检测z底部位置
    //    z_check_and_set_bottom(ccm_param::motion_3d.enable_poweroff_up_down_min_min, t_power_off.z_pos);
    memset(gcodeM140CommandBuf, 0, sizeof(gcodeM140CommandBuf));
    (void)snprintf(gcodeM140CommandBuf, sizeof(gcodeM140CommandBuf), "M140 S%d", t_power_off.bed_target_temp);
    user_send_internal_cmd((char *)gcodeM140CommandBuf); //设置Z最大位
    memset(gcodeM104CommandBuf, 0, sizeof(gcodeM104CommandBuf));
    (void)snprintf(gcodeM104CommandBuf, sizeof(gcodeM104CommandBuf), "M104 S%d", t_power_off.nozzle_target_temp);
    user_send_internal_cmd((char *)gcodeM104CommandBuf);

    if (flash_param_t.idex_print_type == IDEX_PRINT_TYPE_COPY || flash_param_t.idex_print_type == IDEX_PRINT_TYPE_MIRROR)
    {
      memset(gcodeM104CommandBuf, 0, sizeof(gcodeM104CommandBuf));
      (void)snprintf(gcodeM104CommandBuf, sizeof(gcodeM104CommandBuf), "M104 T1 S%d", t_power_off.nozzle_target_temp);
      user_send_internal_cmd((char *)gcodeM104CommandBuf);
    }

    powerOffRecoverPrintStatus = 1;
    isPowerOffRecoverPrintFinish = false;
  }
  else if (powerOffRecoverPrintStatus == 1 && 0 == planner_moves_planned())
  {
    //        recovery_print_close_to_saved_z();
    powerOffRecoverPrintStatus = 2;
  }
  else if (powerOffRecoverPrintStatus == 2 && 0 == planner_moves_planned())
  {
    resetM109HeatingComplete();  //设置为未加热
    _feature_pow_rec_heating();
    powerOffRecoverPrintStatus = 3;
  }
  else if (powerOffRecoverPrintStatus == 3 && (isM109HeatingComplete()))     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    _feature_pow_rec_eb_set();
    powerOffRecoverPrintStatus = 4;
  }
  else if (powerOffRecoverPrintStatus == 4 && 0 == planner_moves_planned())     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    _feature_pow_rec_z_set();
    powerOffRecoverPrintStatus = 5;
  }
  else if (powerOffRecoverPrintStatus == 5 && 0 == planner_moves_planned())     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    feature_set_extruder_fan_speed(t_power_off.fan_speed);         //风扇速度
    SetFeedMultiply(t_power_off.feed_multiply); //打印速度
    powerOffRecoverPrintStatus = 0;
    isPowerOffRecoverPrintFinish = true;
  }

  #elif defined(STM32F407xx)
  static UINT8 powerOffRecoverPrintStatus = 0U;

  if ((powerOffRecoverPrintStatus == 0U) && (1U == t_gui_p.m109_heating_complete) && (0 == sg_grbl::planner_moves_planned()))   //加热完成和Z轴向下完成
  {
    t_gui_p.G28_ENDSTOPS_COMPLETE = 1U; //设置为归零
    motion_3d.updown_g28_first_time = 1U; // 设置已经执行了上下共限位归零操作
    z_check_and_set_bottom(motion_3d.enable_poweroff_up_down_min_min, t_sys_data_current.poweroff_rec_z_max_value); // 检测z底部位置
    #if LASER_MODE

    if (!t_sys_data_current.IsLaser)
    #endif
    {
      static CHAR gcodeM140CommandBuf[50] = {0};
      (void)memset(gcodeM140CommandBuf, 0, sizeof(gcodeM140CommandBuf));
      (void)snprintf(gcodeM140CommandBuf, sizeof(gcodeM140CommandBuf), "M140 S%d isInternal", t_power_off.bed_target_temp);
      user_send_internal_cmd(gcodeM140CommandBuf);
      static CHAR gcodeM104CommandBuf[50] = {0};
      (void)memset(gcodeM104CommandBuf, 0, sizeof(gcodeM104CommandBuf));
      (void)snprintf(gcodeM104CommandBuf, sizeof(gcodeM104CommandBuf), "M104 S%d isInternal", t_power_off.nozzle_target_temp);
      user_send_internal_cmd(gcodeM104CommandBuf);//设置Z最大位置
    }

    powerOffRecoverPrintStatus = 1U;
    isPowerOffRecoverPrintFinish = false;
  }
  else if ((powerOffRecoverPrintStatus == 1U) && (0 == sg_grbl::planner_moves_planned()))
  {
    _feature_pow_rec_close_to_saved_z();
    powerOffRecoverPrintStatus = 2U;
  }
  else if ((powerOffRecoverPrintStatus == 2U) && (0 == sg_grbl::planner_moves_planned()))
  {
    #if LASER_MODE

    if (!t_sys_data_current.IsLaser)
    #endif
    {
      t_gui_p.m109_heating_complete = 0U;  //设置为未加热
      _feature_pow_rec_heating();
    }

    powerOffRecoverPrintStatus = 3U;
  }
  else if ((powerOffRecoverPrintStatus == 3U) && (1U == t_gui_p.m109_heating_complete))     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    #if LASER_MODE

    if (!t_sys_data_current.IsLaser)
    #endif
      _feature_pow_rec_eb_set();

    powerOffRecoverPrintStatus = 4U;
  }
  else if ((powerOffRecoverPrintStatus == 4U) && (0 == sg_grbl::planner_moves_planned()))     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    _feature_pow_rec_z_set();

    if (0U != t_power_off.serial_flag)
    {
      user_send_internal_cmd("M1005 S6 isInternal");//打开串口打印状态
    }

    powerOffRecoverPrintStatus = 5U;
  }
  else if ((powerOffRecoverPrintStatus == 5U) && (0 == sg_grbl::planner_moves_planned()))     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    feature_set_extruder_fan_speed(t_power_off.fan_speed);         //风扇速度
    gcode::feed_multiply = t_power_off.feed_multiply; //打印速度
    powerOffRecoverPrintStatus = 0U;
    isPowerOffRecoverPrintFinish = true;
    #if LASER_MODE

    if (t_sys_data_current.IsLaser)
      t_gui_p.m109_heating_complete = 0U;  //设置为未加热

    #endif
  }
  else
  {
    // TODO
  }

  #endif
}

void feature_pow_rec_process(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    static uint8_t isStartPowerOffRecoverPrint = 0;
    #if defined(STM32F429xx)

    if (isPowerOffRecoverPrint && 0 == planner_moves_planned()) // 等待平台降到最低处
    {
      isStartPowerOffRecoverPrint = 1;
      isPowerOffRecoverPrint = 0;
    }

    if (isStartPowerOffRecoverPrint)
    {
      if (t_power_off.sd_pos != 0)
      {
        _feature_pow_rec_process_loop();
      }
      else
      {
        isPowerOffRecoverPrintFinish = true;
      }

      if (isPowerOffRecoverPrintFinish)
        isStartPowerOffRecoverPrint = 0;
    }

    _feature_pow_rec_finish();
    #elif defined(STM32F407xx)

    if ((0U != isPowerOffRecoverPrint) && (0U == isXYHome) && (0 == sg_grbl::planner_moves_planned()))
    {
      xy_to_zero();
      sg_grbl::st_synchronize();
      isXYHome = true;
    }

    if ((0U != isPowerOffRecoverPrint) && (0 != isXYHome) && (0 == sg_grbl::planner_moves_planned())) // 等待平台降到最低处
    {
      isStartPowerOffRecoverPrint = 1U;
      isPowerOffRecoverPrint = 0;
    }

    if (0U != isStartPowerOffRecoverPrint)
    {
      if (t_power_off.sd_pos != 0U)
      {
        _feature_pow_rec_process_loop();
      }
      else
      {
        if (0U != t_power_off.serial_flag)
        {
          _feature_pow_rec_temp_set();

          if (sg_grbl::planner_moves_planned() > 0)
          {
            return;
          }
        }

        isPowerOffRecoverPrintFinish = true;
      }

      if (0U != isPowerOffRecoverPrintFinish)
      {
        isStartPowerOffRecoverPrint = 0U;
      }
    }

    _feature_pow_rec_finish();
    #endif
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_POWER_RECOVERY













