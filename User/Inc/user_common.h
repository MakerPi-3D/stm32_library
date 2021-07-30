#ifndef USER_COMMON_H
#define USER_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "user_config.h"
#include "user_board.h"
#include "user_mcu.h"
#include "iap_menu.h"
#include "user_util.h"

#ifdef ENABLE_UART1
  #include "user_uart1.h"
  #include "user_debug.h"
#endif

#ifdef ENABLE_FREERTOS
  #include "cmsis_os.h"
  #include "user_os_msg.h"
#endif

#ifdef ENABLE_SDRAM
  #include "sdram.h"
#endif

#ifdef ENABLE_USB_HOST
  #include "fatfs.h"
  #include "usb_host.h"
  #include "user_usb_host.h"
#endif

#if defined(USE_APP1)
  #include "user_ltdc.h"
  #include "lcd.h"
  #include "ftl.h"
  #include "delay.h"
  #include "fatfs.h"
#elif defined(USE_APP2)
  #include "lcd.h"
  #include "delay.h"
#endif //USE_BOOT

#ifdef __cplusplus
extern "C" {
#endif

extern void user_init0(void);
extern void user_init(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_COMMON_H

