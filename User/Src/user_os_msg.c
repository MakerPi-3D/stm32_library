#include "user_common.h"

#ifdef ENABLE_FREERTOS

extern osMessageQId GcodeCommandHandle;

#define CMD_BUF_SIZE 32
#define MAX_CMD_SIZE (96+GCODE_BUF_OFFSET_GCODE_STR)


static char os_put_msg[CMD_BUF_SIZE][MAX_CMD_SIZE];
volatile uint8_t os_put_msg_head = 0;

void user_send_str(uint8_t cmd_str_type, char *msg)
{
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = cmd_str_type;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = strlen(msg);
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 0);
}

void user_send_internal_cmd(const char *msg)
{
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = GCODE_TYPE_INTERNAL;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = strlen(msg);
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 0);
}

void user_send_file_cmd(const char *msg, uint32_t file_pos, long layer_count, long current_layer)
{
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = GCODE_TYPE_FILE;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = strlen(msg);
  uint32_t *file_pos_tmp = (uint32_t *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_FILE_POS];
  *file_pos_tmp = file_pos;
  long *layer_count_tmp = (long *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_LAYER_COUNT];
  *layer_count_tmp = layer_count;
  long *current_layer_tmp = (long *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_CURRENT_LAYER];
  *current_layer_tmp = current_layer;
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 0);
}

void user_clear_queue_str(void)
{
  osEvent evt = osMessageGet(GcodeCommandHandle, 0);

  while (evt.status == osEventMessage)
  {
    evt = osMessageGet(GcodeCommandHandle, 0);
  }
}

#endif //ENABLE_FREERTOS

