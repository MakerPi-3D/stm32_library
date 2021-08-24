#include "user_common.h"
#include "main.h"


#if defined(STM32F407xx)
  #include "globalvariables.h"
#endif

#if defined(USE_APP1) || defined(USE_APP2)
static void user_NVIC_SetVectorTable(uint32_t Offset)
{
  /* Check the parameters */
  assert_param((Offset) < 0x000FFFF);
  #ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | (Offset & (uint32_t)0x1FFFFF80);
  #else
  SCB->VTOR = FLASH_BASE | (Offset & (uint32_t)0x1FFFFF80);
  #endif
}
#endif

void user_init0(void)
{
  #if defined(USE_APP1)
  //�����ж�������  33k:0x08008400  128k-33k=95K:0x17C00   0x8000000  0x100000
  user_NVIC_SetVectorTable(0x00008400);
  __enable_irq(); // ���ж�
  #elif defined(USE_APP2)
  //ִ���������������Ĳ���
  user_NVIC_SetVectorTable(0x00020000);
  __enable_irq(); // ���ж�
  #endif
  user_get_mcu_id();
}

void user_init(void)
{
  user_print_mcu_info();
  user_board_init();
  #if defined(USE_BOOT)
  USER_EchoLog("Bootloader ==>> start");
  #elif defined(USE_APP1)
  delay_init(168);
  u8 res = 0;
  USER_EchoLog("APP1 ==>> start");
  SDRAM_Init();
  APP_LCD_StartInit(); //LCD��ʼ��
  APP_LCD_Clear(BLACK);
  HAL_Delay(100);
  LCD_LED = 1;      //��������
  //  FTL_Init();
  //  exfuns_init();
  res = f_mount(&USERFatFS, "1:", 1);     //����NAND FLASH.
  printf("res ..%d\n", res);

  if (res == 0X0D) //NAND FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��NAND FLASH
  {
    BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
    res = f_mkfs("1:", 1, 0, work, sizeof(work)); //��ʽ��FLASH,2,�̷�;1,����Ҫ������,8������Ϊ1����
    delay_ms(1000);
  }

  #elif defined(USE_APP2)
  #if defined(STM32F407xx)
  user_delay_init(168);
  globalvariables_init();
  // ���ڳ�ʼ��
  user_uart1_dma_start();
  // ���Ź���ʼ��
  user_iwdg_init();
  user_sd_init();
  feature_fan_control_init();
  #elif defined(STM32F429xx)
  user_os_init();
  delay_init(168);
  SDRAM_Init();
  LCD_Init();//��������ʼ��
  #ifdef ENABLE_UART1_DMA
  user_uart1_dma_start();
  #endif
  #endif
  #endif
}




