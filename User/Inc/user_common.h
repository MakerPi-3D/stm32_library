#ifndef USER_COMMON_H
#define USER_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "./user_config.h"
#include "./user_board.h"
#include "./user_mcu.h"
#include "./user_util.h"
#include "./user_debug.h"
#include "./user_board_pin.h"
#include "./user_sys.h"

#ifdef ENABLE_UART1
  #include "user_uart1.h"
#endif

#ifdef ENABLE_IWDG
  #include "user_iwdg.h"
#endif

#ifdef ENABLE_SD
  #include "user_sd.h"
#endif

#ifdef ENABLE_DELAY
  #include "user_delay.h"
#endif

#ifdef ENABLE_FREERTOS
  #include "cmsis_os.h"
  #include "./user_os_msg.h"
#endif

#ifdef ENABLE_SDRAM
  #include "./user_sdram.h"
#endif

#ifdef ENABLE_NAND
  #include "./user_nand.h"
  #include "../../Nand/Inc/nand.h"
  #include "../../Nand/Inc/ftl.h"
#endif

#ifdef ENABLE_IAP
  #include "../../Iap/Inc/iap_menu.h"
  #include "../../Iap/Inc/flash_if.h"
#endif

#ifdef ENABLE_USB_HOST
  #include "fatfs.h"
  #include "usb_host.h"
  #include "./user_usb_host.h"
#endif

#ifdef ENABLE_USB_DEVICE
  #include "./user_usb_device.h"
#endif

#ifdef ENABLE_LCD
  #include "../../Lcd/Inc/lcd.h"
  #include "../../Lcd/Inc/font.h"
  #include "./user_ltdc.h"
#endif

#ifdef HAS_FILAMENT_SENSOR
  #include "../../Feature/Inc/filament_check.h"
#endif

#ifdef HAS_FAN_CONTROL
  #include "./user_fan.h"
  #include "../../Feature/Inc/fan_control.h"
#endif

#ifdef HAS_BUZZER_CONTROL
  #include "./user_buzzer.h"
#endif

#ifdef HAS_LED_CONTROL
  #include "./user_led.h"
  #include "../../Feature/Inc/led_control.h"
#endif

#ifdef HAS_POWER_RECOVERY
  #include "../../Feature/Inc/power_recovery.h"
  #include "../../Feature/Inc/power_data.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void user_init0(void);
extern void user_init(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_COMMON_H

