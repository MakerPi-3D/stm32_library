#include "user_common.h"

#ifdef HAS_POWER_RECOVERY
#include "flashconfig.h"
#if defined(STM32F429xx)
  #include "user_ccm.h"
  #include "temperature.h"
  #include "sysconfig_data.h"
  #include "planner.h"
  #include "PrintControl.h"
  #include "midwaychangematerial.h"
  #include "stepper.h"
  #include "stepper_pin.h"
  #include "globalvariables.h"
  #include "process_command.h"

  char powerOffFilePathName[100];                    /*!< 断电文件路径名 */
  static float e_position = 0, b_position = 0;
#elif defined(STM32F407xx)
  #include "globalvariables.h"
  #include "gcode_global_params.h"
  #include "temperature.h"
  #include "PrintControl.h"

  #include "globalvariables_ccmram.h"
#endif

FLASH_POWEROFF_RECOVERY_T flash_poweroff_recovery_t2;

#ifdef __cplusplus
extern "C" {
#endif

/*
从片内flash中读出断电保存的数据
*/

static void flash_read_poweroff_data(void)
{
  uint32_t address;
  uint32_t *pd;
  uint16_t i;
  address = FLASH_POWEROFF_DATA_START_ADDR;
  pd = (uint32_t *)(&flash_poweroff_recovery_t2);

  for (i = 0; i < sizeof(flash_poweroff_recovery_t2) / 4; i++)
  {
    *pd = *((volatile uint32_t *) address);
    address += 4;
    pd ++;
  }
}

void feature_pow_data_init(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    flash_read_poweroff_data();

    if (0U != flash_poweroff_recovery_t2.serialFlag) // 串口标志位开，断电标志关闭
    {
      flash_poweroff_recovery_t2.flag = 0U;
    }

    t_power_off.bed_target_temp = flash_poweroff_recovery_t2.bedTargetTemp;
    t_power_off.nozzle_target_temp = flash_poweroff_recovery_t2.nozzleTargetTemp;
    t_power_off.fan_speed = flash_poweroff_recovery_t2.fanSpeed;
    t_power_off.feed_multiply = flash_poweroff_recovery_t2.feedMultiply;
    t_power_off.feed_rate = flash_poweroff_recovery_t2.feedRate;
    t_power_off.x_pos = flash_poweroff_recovery_t2.xPos;
    t_power_off.y_pos = flash_poweroff_recovery_t2.yPos;
    t_power_off.z_pos = flash_poweroff_recovery_t2.zPos;
    t_power_off.e_pos = flash_poweroff_recovery_t2.ePos;
    t_power_off.b_pos = flash_poweroff_recovery_t2.bPos;
    t_power_off.sd_pos = flash_poweroff_recovery_t2.sdPos;
    t_power_off.serial_flag = flash_poweroff_recovery_t2.serialFlag;
    (void)strcpy(t_power_off.path_file_name, flash_param_t.pathFileName);
    (void)strcpy(t_power_off.file_name, flash_param_t.fileName);
    t_power_off.flag = flash_poweroff_recovery_t2.flag;
    #if defined(STM32F429xx)

    if ((uint8_t)(flash_param_t.pathFileName[0] - '0') == 0)
      t_power_off.is_file_from_sd = 0;

    t_power_off.blockdetectflag = flash_poweroff_recovery_t2.blockflag;
    #elif defined(STM32F407xx)

    if ((uint8_t)(flash_param_t.pathFileName[0] - '0') == 0)
      t_power_off.is_file_from_sd = 1;

    t_power_off.blockdetectflag = flash_poweroff_recovery_t2.blockflag;
    t_sys.enable_color_buf = flash_poweroff_recovery_t2.enable_color_buf;
    t_sys.print_time_save = flash_poweroff_recovery_t2.print_time_save;
    #endif
  }
}

// 重置断电标志位
void feature_pow_data_reset_flag(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    memset(&t_power_off, 0, sizeof(t_power_off));
    flash_poweroff_recovery_t.flag = 0;
    flash_save_poweroff_data();
  }
}

void feature_pow_data_start_print_init(void)
{
  #if defined(STM32F429xx)
  #elif defined(STM32F407xx)

  for (int i = 0; i < 4; i++)
  {
    reset_run_status(&po_save_running_status[i]);
  }

  #endif
}

void _feature_pow_data_sync_data(void)
{
  #if defined(STM32F429xx)
  //  if (f_open(&power_off_file, POWER_OFF_REC_FILE, FA_READ | FA_WRITE) == FR_OK) //打开保存数据的文件
  //  {
  //    (void)f_write(&power_off_file, ccm_param::poweroff_data, POWER_OFF_BUF_SIZE, (UINT *)&poweroff_data_size);
  //    (void)f_sync(&power_off_file);
  //    (void)f_close(&power_off_file);
  //    #if (1 == DEBUG_POWEROFF_CUSTOM)
  //    USER_DbgLog("PowerOffOperation::syncData Done !");
  //    USER_DbgLog("%40s\r\n%s", "poweroff_data_str_in_syncData", ccm_param::poweroff_data);
  //    #endif
  //  }
  flash_poweroff_recovery_t.bedTargetTemp = (int)temperature_get_bed_target();                     /*!< 热床目标温度 */
  flash_poweroff_recovery_t.nozzleTargetTemp = (int)temperature_get_extruder_target(0);            /*!< 喷嘴目标温度 */
  flash_poweroff_recovery_t.fanSpeed = plan_get_fans();                                            /*!< 风扇速度 */
  flash_poweroff_recovery_t.feedMultiply = plan_get_fmultiply();                                   /*!< 进料速度百分比 */
  flash_poweroff_recovery_t.feedRate = (float)GetFeedRate();                                       /*!< 进料速度 */
  flash_poweroff_recovery_t.xPos = (BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(X_AXIS) : plan_get_axis_position((int)X_AXIS);                                 /*!< x位置 */
  flash_poweroff_recovery_t.yPos = ((BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(Y_AXIS) : plan_get_axis_position((int)Y_AXIS));                                 /*!< y位置 */
  flash_poweroff_recovery_t.zPos = (BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(Z_AXIS) : plan_get_axis_position((int)Z_AXIS);                                 /*!< z位置 */
  flash_poweroff_recovery_t.ePos =  e_position;                                /*!< e位置 */
  flash_poweroff_recovery_t.bPos =  b_position;                                /*!< b位置 */
  flash_poweroff_recovery_t.sdPos = plan_get_sd_position();                                        /*!< 文件位置 */
  flash_poweroff_recovery_t.flag = 1;                                               /*!< 断电标志位 */
  flash_poweroff_recovery_t.serialFlag = plan_get_serial_flag();                                   /*!< 串口标志位 */
  flash_poweroff_recovery_t.blockflag = IsNotHaveMatInPrint;                                       /*!< 堵料标志位 */
  flash_save_poweroff_data();
  #elif defined(STM32F407xx)
  #endif
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == GPIO_PIN_3) && t_sys_data_current.enable_powerOff_recovery) // PD3
  {
    if (IsPrint())
    {
      #if defined(STM32F429xx)

      if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_SET)
      #elif defined(STM32F407xx)
      if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET)
      #endif
      {
        #if defined(STM32F429xx)

        for (int i = 0; i < MAX_NUM_AXIS; i++)
        {
          stepper_axis_enable(i, false);
        }

        t_power_off.is_power_off = 1; // 断电状态，更新标志
        #endif
        flash_save_poweroff_data();
        HAL_NVIC_SystemReset(); //复位
      }

      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    }
  }
}

// 设置断电数据缓存
static void _feature_pow_data_sync_buf(void)
{
  #if defined(STM32F429xx)
  static unsigned long setDataTimeOut = 0;
  static uint32_t sd_position2 = 0, sd_position3 = 0;
  //  static float e_position = 0, b_position = 0; //2017517缓存E、B电机的位置，保存上一次的值，因为sd的位置是上上次的

  if (setDataTimeOut < xTaskGetTickCount())
  {
    for (int i = 0; i < POWER_OFF_BUF_SIZE; i++) //初始化填入空白符到BUF
      ccm_param::poweroff_data[i] = ' ';

    (void)snprintf(&ccm_param::poweroff_data[0], sizeof(ccm_param::poweroff_data), \
                   "D1:%s\r\nD2:%d\r\nD3:%d\r\nD4:%d\r\nD5:%d\r\nD6:%f\r\nD7:%f\r\nD8:%f\r\nD9:%f\r\nD10:%d\r\nD11:%d\r\nD12:%d\r\nD13:%d\r\nD14:1\r\nD15:%d\r\nD16:%f\r\nD17:%f\r\nD18:%ld", \
                   powerOffFilePathName, (int)temperature_get_bed_target(), (int)temperature_get_extruder_target(0), plan_get_fans(), plan_get_fmultiply(),
                   (float)GetFeedRate(), ((BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(Z_AXIS) : plan_get_axis_position((int)Z_AXIS)),
                   e_position, b_position, (int)plan_get_sd_position(), sd_position2, sd_position3,
                   IsNotHaveMatInPrint, (plan_get_serial_flag()),
                   ((BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(X_AXIS) : plan_get_axis_position((int)X_AXIS)),
                   ((BED_LEVEL_PRESSURE_SENSOR == t_sys_data_current.enable_bed_level) ? st_get_position_length(Y_AXIS) : plan_get_axis_position((int)Y_AXIS)), printControl.getTime()); //2017516记录三个sd位置
    sd_position3 = sd_position2;
    sd_position2 = plan_get_sd_position();

    if (t_sys_data_current.enable_color_mixing)
    {
      e_position = plan_get_axis_position((int)E_AXIS) - 8.0f;//减4为了补偿在返回打印途中“漏掉”的耗材
      b_position = plan_get_axis_position((int)B_AXIS) - 8.0f;
    }
    else
    {
      e_position = plan_get_axis_position((int)E_AXIS) - 0.0f;//减4为了补偿在返回打印途中“漏掉”的耗材
      b_position = plan_get_axis_position((int)B_AXIS) - 0.0f;
    }

    setDataTimeOut = xTaskGetTickCount() + 10;
  }

  #if (1 == DEBUG_POWEROFF_CUSTOM)
  //  USER_DbgLog("%40s\r\n%s", "poweroff_data_in_setDataBuf",ccm_param::poweroff_data);
  USER_DbgLog("X%f Y%f Z%f E%f \r\n",
              plan_get_axis_position(X_AXIS),
              plan_get_axis_position(Y_AXIS),
              plan_get_axis_position(Z_AXIS),
              plan_get_axis_position(E_AXIS));
  #endif
  #elif defined(STM32F407xx)
  static uint32_t set_data_timeout = 0UL;
  copy_run_status_to_other(&po_save_running_status[2], &po_save_running_status[3]);
  copy_run_status_to_other(&po_save_running_status[1], &po_save_running_status[2]);
  copy_run_status_to_other(&po_save_running_status[0], &po_save_running_status[1]);
  copy_run_status_to_other(&runningStatus[block_buffer_tail], &po_save_running_status[0]);

  if (set_data_timeout < xTaskGetTickCount())
  {
    unsigned char save_index = 0U;

    for (int i = 3; i >= 0; --i)
    {
      if (0 != po_save_running_status[i].sd_position)
      {
        save_index = i;
        break;
      }
    }

    flash_poweroff_recovery_t.bedTargetTemp = (int)po_save_running_status[save_index].bed_temp;             /*!< 热床目标温度 */
    flash_poweroff_recovery_t.nozzleTargetTemp = (int)po_save_running_status[save_index].extruder0_temp;    /*!< 喷嘴目标温度 */
    flash_poweroff_recovery_t.fanSpeed = po_save_running_status[save_index].fan_speed;                      /*!< 风扇速度 */
    flash_poweroff_recovery_t.feedMultiply = po_save_running_status[save_index].feed_multiply;              /*!< 进料速度百分比 */
    flash_poweroff_recovery_t.feedRate = (float)po_save_running_status[save_index].feed_rate;               /*!< 进料速度 */
    flash_poweroff_recovery_t.xPos = po_save_running_status[save_index].axis_position[X_AXIS];              /*!< x位置 */
    flash_poweroff_recovery_t.yPos = po_save_running_status[save_index].axis_position[Y_AXIS];              /*!< y位置 */
    flash_poweroff_recovery_t.zPos = po_save_running_status[save_index].axis_position[Z_AXIS];              /*!< z位置 */
    flash_poweroff_recovery_t.ePos =  po_save_running_status[save_index].axis_position[E_AXIS];             /*!< e位置 */
    flash_poweroff_recovery_t.bPos =  po_save_running_status[save_index].axis_position[B_AXIS];             /*!< b位置 */
    flash_poweroff_recovery_t.sdPos = po_save_running_status[save_index].sd_position;                       /*!< 文件位置 */
    flash_poweroff_recovery_t.flag = 1;                                                                     /*!< 断电标志位 */
    flash_poweroff_recovery_t.serialFlag = po_save_running_status[save_index].is_serial;                    /*!< 串口标志位 */
    flash_poweroff_recovery_t.blockflag = t_gui_p.IsNotHaveMatInPrint;                                      /*!< 堵料标志位 */
    flash_poweroff_recovery_t.print_time_save = (long)printControl.getTime();
    flash_poweroff_recovery_t.enable_color_buf = t_sys.enable_color_buf;
    set_data_timeout = xTaskGetTickCount() + 10U;
  }

  #endif
}

void feature_pow_data_set_file_path_name(const char *filePathName)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    #if defined(STM32F429xx)
    memcpy(powerOffFilePathName, filePathName, strlen(filePathName));
    powerOffFilePathName[strlen(filePathName)] = 0;
    //  snprintf(poweroff_data, sizeof(ccm_param::poweroff_data), "D1:%s\r\n",filePathName); // 设置文件名
    //  poweroff_data_size = strlen(ccm_param::poweroff_data); // 获取文件名字符串长度
    #elif defined(STM32F407xx)
    #endif
  }
}


void feature_pow_data_set(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    #if defined(STM32F429xx)
    static int _current_block_buffer_tail = -1;
    static int bed_target = 0;
    static int hotend_target = 0;
    static bool _isPausePrint = false;

    if (IsPrint())
    {
      if (_current_block_buffer_tail == ccm_param::block_buffer_tail && (bed_target != (int)temperature_get_bed_target() || hotend_target != (int)temperature_get_extruder_target(0)))
      {
        _feature_pow_data_sync_buf();
        bed_target = (int)temperature_get_bed_target();
        hotend_target = (int)temperature_get_extruder_target(0);
      }

      if (_current_block_buffer_tail != ccm_param::block_buffer_tail) // 當前運動隊列變化，保存數據
      {
        _feature_pow_data_sync_buf();
        //        taskENTER_CRITICAL();
        //        (void)f_write(&power_off_file, ccm_param::poweroff_data, POWER_OFF_BUF_SIZE, (UINT *)&poweroff_data_size);
        //        taskEXIT_CRITICAL();
        _current_block_buffer_tail = ccm_param::block_buffer_tail;
        #if (1 == DEBUG_POWEROFF_CUSTOM)
        USER_DbgLog("PowerOffOperation::setData IsPrint !");
        #endif
      }

      _isPausePrint = false;
    }
    else if (IsPausePrint() || IsMidWayChangeMat()) // 暂停打印或者中途换料，先保存断电数据
    {
      if (!_isPausePrint) // 暫停打印只保存一次
      {
        _feature_pow_data_sync_buf();
        //        taskENTER_CRITICAL();
        //        (void)f_write(&power_off_file, ccm_param::poweroff_data, POWER_OFF_BUF_SIZE, (UINT *)&poweroff_data_size);
        _feature_pow_data_sync_data();
        //        taskEXIT_CRITICAL();
        #if (1 == DEBUG_POWEROFF_CUSTOM)
        USER_DbgLog("PowerOffOperation::setData IsPausePrint !");
        #endif
      }

      _current_block_buffer_tail = -1;
      _isPausePrint = true;
    }
    else
    {
      _current_block_buffer_tail = -1;
      _isPausePrint = false;
    }

    #elif defined(STM32F407xx)
    static int _current_block_buffer_tail = -1;
    static bool _is_pause_print = false;

    if ((0U != t_sys_data_current.enable_powerOff_recovery) && (0U == t_power_off.is_power_off)) // 断电开启，非断电状态
    {
      if ((0 != IsPrint())) // 打印状态
      {
        static int bed_target = 0;
        static int hotend_target = 0;
        bool is_sync_data = false;

        if ((bed_target != static_cast<int>(sg_grbl::temperature_get_bed_target())) ||
            (hotend_target != static_cast<int>(sg_grbl::temperature_get_extruder_target(0)))) // 目标温度变化
        {
          is_sync_data = true;
          bed_target = static_cast<int>(sg_grbl::temperature_get_bed_target());
          hotend_target = static_cast<int>(sg_grbl::temperature_get_extruder_target(0));
        }

        if (_current_block_buffer_tail != block_buffer_tail) // 當前運動隊列變化，保存數據
        {
          is_sync_data = true;
          _current_block_buffer_tail = block_buffer_tail;
        }

        if (is_sync_data)
        {
          _feature_pow_data_sync_buf();
        }

        _is_pause_print = false;
      }
      else if ((0 != IsPausePrint()) || gcode::m600_is_midway_change_material) // 暂停打印或者中途换料，先保存断电数据
      {
        if (!_is_pause_print) // 暫停打印只保存一次
        {
          _feature_pow_data_sync_buf();
          flash_save_poweroff_data();
        }

        _current_block_buffer_tail = -1;
        _is_pause_print = true;
      }
      else
      {
        _current_block_buffer_tail = -1;
        _is_pause_print = false;
      }
    }

    #endif
  }
}

//断电续打取消删除SD卡中的文件
void feature_pow_data_delete_file_from_sd(void)
{
  if (t_sys_data_current.enable_powerOff_recovery)
  {
    if (t_power_off.is_file_from_sd)   //断电续打文件在SD卡中，删除文件
    {
      taskENTER_CRITICAL();
      (void)f_unlink(t_power_off.path_file_name);
      taskEXIT_CRITICAL();
    }

    feature_pow_data_reset_flag();
  }
}


//断电续打取消删除SD卡中的文件
void feature_pow_data_save_file_path_name(char *path_file_name, char *file_name)
{
  #if defined(STM32F429xx)
  #elif defined(STM32F407xx)

  if ((0U != t_sys_data_current.enable_powerOff_recovery) && (0U == t_power_off.is_power_off)) // 断电开启，非断电状态
  {
    /*确定选择的打印文件后，将文件名和路径名保存到flash，以便断电续打时调用*/
    strcpy(flash_param_t.pathFileName, path_file_name);
    strcpy(flash_param_t.fileName, file_name);
  }

  #endif
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_POWER_RECOVERY













