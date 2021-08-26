#include "user_ltdc.h"
#include "stm32f4xx_hal.h"

uint16_t ltdc_id;

//��ȡ������
//PG6=R7(M0);PI2=G7(M1);PI7=B7(M2);
//M2:M1:M0
//0 :0 :0 //4.3��480*272 RGB��,ID=0X4342
//0 :0 :1 //7��800*480 RGB��,ID=0X7084
//0 :1 :0 //7��1024*600 RGB��,ID=0X7016
//0 :1 :1 //7��1280*800 RGB��,ID=0X7018
//1 :0 :0 //8��1024*600 RGB��,ID=0X8016
//1 :0 :1 //10.1��1280*800 RGB��,ID=0X1018
//����ֵ:LCD ID:0,�Ƿ�;����ֵ,ID;
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

#if 0
//LTDC�ײ�IO��ʼ����ʱ��ʹ��
//�˺����ᱻHAL_LTDC_Init()����
//hltdc:LTDC���
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_LTDC_CLK_ENABLE();                //ʹ��LTDCʱ��
  __HAL_RCC_DMA2D_CLK_ENABLE();               //ʹ��DMA2Dʱ��
  __HAL_RCC_GPIOB_CLK_ENABLE();               //ʹ��GPIOBʱ��
  __HAL_RCC_GPIOF_CLK_ENABLE();               //ʹ��GPIOFʱ��
  __HAL_RCC_GPIOG_CLK_ENABLE();               //ʹ��GPIOGʱ��
  __HAL_RCC_GPIOH_CLK_ENABLE();               //ʹ��GPIOHʱ��
  __HAL_RCC_GPIOI_CLK_ENABLE();               //ʹ��GPIOIʱ��
  //��ʼ��PB5����������
  GPIO_Initure.Pin = GPIO_PIN_5;              //PB5������������Ʊ���
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;    //�������
  GPIO_Initure.Pull = GPIO_PULLUP;            //����
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //����
  HAL_GPIO_Init(GPIOB, &GPIO_Initure);
  //��ʼ��PF10
  GPIO_Initure.Pin = GPIO_PIN_10;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //����
  GPIO_Initure.Pull = GPIO_NOPULL;
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //����
  GPIO_Initure.Alternate = GPIO_AF14_LTDC;    //����ΪLTDC
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);
  //��ʼ��PG6,7,11
  GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_Initure);
  //��ʼ��PH9,10,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                     GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOH, &GPIO_Initure);
  //��ʼ��PI0,1,2,4,5,6,7,9,10
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | \
                     GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOI, &GPIO_Initure);
}
#endif



