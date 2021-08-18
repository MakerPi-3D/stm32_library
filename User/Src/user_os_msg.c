#include "user_common.h"

#ifdef ENABLE_FREERTOS

extern osMessageQId GcodeCommandHandle;

#ifdef ENABLE_OS_POOL
  osPoolDef(m_pool_t, 32, os_message_t);
  osPoolId m_os_pool_id;
#else
  #define CMD_BUF_SIZE 32
  #define MAX_CMD_SIZE (96+GCODE_BUF_OFFSET_GCODE_STR)
  static char os_put_msg[CMD_BUF_SIZE][MAX_CMD_SIZE];
  volatile uint8_t os_put_msg_head = 0;
#endif

void user_os_init(void)
{
  #ifdef ENABLE_OS_POOL
  m_os_pool_id = osPoolCreate(osPool(m_pool_t));  //创建内存池
  #endif
}

void user_send_str(uint8_t cmd_str_type, char *msg)
{
  #ifdef ENABLE_OS_POOL
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  message->cmd_type = cmd_str_type;
  message->cmd_length = strlen(msg);
  message->file_pos = 0;
  message->layer_count = 0;
  message->current_layer = 0;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
  #else
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = cmd_str_type;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = strlen(msg);
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 0);
  #endif
}

void user_send_internal_cmd(const char *msg)
{
  #ifdef ENABLE_OS_POOL
  printf("1::%s\n", msg);
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  printf("2::%s\n", msg);
  message->cmd_type = GCODE_TYPE_INTERNAL;
  message->cmd_length = strlen(msg);
  message->file_pos = 0;
  message->layer_count = 0;
  message->current_layer = 0;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
  #else
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = GCODE_TYPE_INTERNAL;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = strlen(msg);
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 50);
  #endif
}

void user_send_file_cmd(const char *msg, uint32_t file_pos, long layer_count, long current_layer)
{
  #ifdef ENABLE_OS_POOL
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  message->cmd_type = GCODE_TYPE_FILE;
  message->cmd_length = strlen(msg);
  message->file_pos = file_pos;
  message->layer_count = layer_count;
  message->current_layer = current_layer;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
  #else
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
  #endif
}

void user_clear_queue_str(void)
{
  osEvent evt = osMessageGet(GcodeCommandHandle, 0);

  while (evt.status == osEventMessage)
  {
    evt = osMessageGet(GcodeCommandHandle, 0);
  }
}

void user_os_print_task_remaining_space(void)
{
  static unsigned long RefreshGuiTimeOut = 0;
  extern osThreadId InitTaskHandle;
  extern osThreadId RefDataTaskHandle;
  extern osThreadId ReadUdiskTaskHandle;
  extern osThreadId PrintTaskHandle;
  extern osThreadId GUITaskHandle;
  extern osThreadId RespondGUITaskHandle;
  extern USBH_HandleTypeDef hUsbHostFS;

  if (RefreshGuiTimeOut < xTaskGetTickCount())
  {
    int uxHighWaterMark = uxTaskGetStackHighWaterMark(InitTaskHandle);
    printf("\r\nInitTaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(RefDataTaskHandle);
    printf("\r\nRefDataTaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(ReadUdiskTaskHandle);
    printf("\r\nReadUdiskTaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(PrintTaskHandle);
    printf("\r\nPrintTaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(GUITaskHandle);
    printf("\r\nGUITaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(RespondGUITaskHandle);
    printf("\r\nRespondGUITaskHandle remaining space: %d\r\n", uxHighWaterMark);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(hUsbHostFS.thread);
    printf("\r\nUSBH_Thread remaining space: %d\r\n", uxHighWaterMark);
    RefreshGuiTimeOut = xTaskGetTickCount() + 5000;
  }
}


#endif //ENABLE_FREERTOS

