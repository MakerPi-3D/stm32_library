#include "user_common.h"

#if defined(ENABLE_NAND)

void user_nand_init(void)
{
  retUSER = f_mount(&USERFatFS, (const char *)USERPath, 1);

  if (retUSER == FR_OK)
  {
    USER_EchoLog("user_mount_nand ok!");
  }
  else
  {
    USER_EchoLog("user_mount_nand not ok!");

    if (retUSER == 0X0D) //NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
    {
      retUSER = f_mkfs(USERPath, 1, 4096, NULL, 0); //格式化FLASH,2,盘符;1,不需要引导区,8个扇区为1个簇
    }
  }
}

#if 0
//NAND FALSH底层驱动,引脚配置，时钟使能
//此函数会被HAL_NAND_Init()调用
void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_FMC_CLK_ENABLE();             //使能FMC时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();           //使能GPIOD时钟
  __HAL_RCC_GPIOE_CLK_ENABLE();           //使能GPIOE时钟
  __HAL_RCC_GPIOG_CLK_ENABLE();           //使能GPIOG时钟
  //初始化PD6 R/B引脚
  GPIO_Initure.Pin = GPIO_PIN_6;
  GPIO_Initure.Mode = GPIO_MODE_INPUT;        //输入
  GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);
  //初始化PG9 NCE3引脚
  GPIO_Initure.Pin = GPIO_PIN_9;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //输入
  GPIO_Initure.Pull = GPIO_NOPULL;        //上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
  GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC
  HAL_GPIO_Init(GPIOG, &GPIO_Initure);
  //初始化PD0,1,4,5,11,12,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | \
                     GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_Initure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);
  //初始化PE7,8,9,10
  GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);
}
#endif

#endif // #if defined(ENABLE_NAND)

