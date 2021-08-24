#ifndef __USER_OS_MSG_H
#define __USER_OS_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_FREERTOS

#define GCODE_TYPE_FILE 0
#define GCODE_TYPE_UART 1
#define GCODE_TYPE_INTERNAL 2

extern void user_send_str(uint8_t cmd_str_type, char *msg);
extern void user_send_internal_cmd(const char *msg);
extern void user_send_file_cmd(const char *msg, uint32_t file_pos, long layer_count, long current_layer);
extern void user_clear_queue_str(void);
extern void user_os_print_task_info(void);

#ifdef ENABLE_OS_POOL
#define GCODE_BUFFER_MAX_SIZE (96)
extern osPoolId m_os_pool_id;
typedef struct
{
  uint8_t cmd_type;            /* Cmd Type: file/uart/internal */
  uint8_t cmd_length;          /* Cmd string length */
  uint32_t file_pos;           /* Print file position */
  uint32_t layer_count;        /* Print model layer count */
  uint32_t current_layer;      /* Print model current layer */
  char cmd_str[GCODE_BUFFER_MAX_SIZE];  /* Cmd string buffers */
} os_message_t;

#else
#define GCODE_BUF_OFFSET_TYPE 0             // uint8_t
#define GCODE_BUF_OFFSET_GCODE_LENGTH 1     // uint8_t
#define GCODE_BUF_OFFSET_FILE_POS 2         // uint32_t
#define GCODE_BUF_OFFSET_LAYER_COUNT 6      // long
#define GCODE_BUF_OFFSET_CURRENT_LAYER 10   // long
#define GCODE_BUF_OFFSET_GCODE_STR 14       // char*

#endif
extern void user_os_init(void);

#endif

#ifdef __cplusplus
} //extern "C"
#endif

#endif // __USER_OS_MSG_H

