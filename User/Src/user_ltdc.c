#include "user_ltdc.h"
#include "stm32f4xx_hal.h"

uint16_t ltdc_id;

//读取面板参数
//PG6=R7(M0);PI2=G7(M1);PI7=B7(M2);
//M2:M1:M0
//0 :0 :0 //4.3寸480*272 RGB屏,ID=0X4342
//0 :0 :1 //7寸800*480 RGB屏,ID=0X7084
//0 :1 :0 //7寸1024*600 RGB屏,ID=0X7016
//0 :1 :1 //7寸1280*800 RGB屏,ID=0X7018
//1 :0 :0 //8寸1024*600 RGB屏,ID=0X8016
//1 :0 :1 //10.1寸1280*800 RGB屏,ID=0X1018
//返回值:LCD ID:0,非法;其他值,ID;
void user_ltdc_read_panel_id(void)
{
  uint8_t idx = 0;
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  GPIO_Initure.Pin = GPIO_PIN_6;
  GPIO_Initure.Mode = GPIO_MODE_INPUT;
  GPIO_Initure.Pull = GPIO_PULLUP;
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_Initure);
  GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOI, &GPIO_Initure);
  idx = (uint8_t)HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6);
  idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_2) << 1;
  idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) << 2;

  switch (idx)
  {
  case 0:
    ltdc_id = 0X4342;
    break;

  case 1:
    ltdc_id = 0X7084;
    break;

  case 2:
    ltdc_id = 0X7016;
    break;

  case 3:
    ltdc_id = 0X7018;
    break;

  case 4:
    ltdc_id = 0X8016;
    break;

  case 5:
    ltdc_id = 0X1018;
    break;

  default:
    ltdc_id = 0;
    break;
  }
}



