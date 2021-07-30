#ifndef __USER_OS_MSG_H
#define __USER_OS_MSG_H

#include "user_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_FREERTOS

  #define GCODE_TYPE_FILE 0
  #define GCODE_TYPE_UART 1
  #define GCODE_TYPE_INTERNAL 2

  #define GCODE_BUF_OFFSET_TYPE 0             // uint8_t
  #define GCODE_BUF_OFFSET_GCODE_LENGTH 1     // uint8_t
  #define GCODE_BUF_OFFSET_FILE_POS 2         // uint32_t
  #define GCODE_BUF_OFFSET_LAYER_COUNT 6      // long
  #define GCODE_BUF_OFFSET_CURRENT_LAYER 10   // long
  #define GCODE_BUF_OFFSET_GCODE_STR 14       // char*

  extern void user_send_str(uint8_t cmd_str_type, char *msg);
  extern void user_send_internal_cmd(const char *msg);
  extern void user_send_file_cmd(const char *msg, uint32_t file_pos, long layer_count, long current_layer);
  extern void user_clear_queue_str(void);

#endif

#ifdef __cplusplus
} //extern "C"
#endif

#endif // __USER_OS_MSG_H

