#include "user_common.h"

#ifdef ENABLE_UART1
#ifdef ENABLE_UART1_DMA

extern UART_HandleTypeDef huart1;

#define RECEIVE_LENGTH 96
#define USART_DMA_SENDING 0//发送未完成
#define USART_DMA_SENDOVER 1//发送完成
typedef struct
{
  volatile unsigned char receive_flag: 1; //空闲接收标记
  volatile unsigned char dmaSend_flag: 1; //发送完成标记
  unsigned short rx_len;//接收长度
  unsigned char usartDMA_rxBuf[RECEIVE_LENGTH];//DMA接收缓存
} USER_USART_RECEIVE_T;

//===========================================================================
//=============================private variables ============================
//===========================================================================
USER_USART_RECEIVE_T user_uart_receive;

//===========================================================================
//=============================public  function =============================
//===========================================================================

//开启串口中断
void user_uart1_dma_start(void)
{
  __HAL_UART_CLEAR_OREFLAG(&huart1);
  user_uart_receive.receive_flag = 0;
  user_uart_receive.dmaSend_flag = USART_DMA_SENDOVER;
  HAL_UART_Receive_DMA(&huart1, user_uart_receive.usartDMA_rxBuf, RECEIVE_LENGTH);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

//DMA发送函数
void user_uart1_dma_send_data(uint8_t *pdata, uint16_t Length)
{
  while (user_uart_receive.dmaSend_flag == USART_DMA_SENDING);

  user_uart_receive.dmaSend_flag = USART_DMA_SENDING;
  HAL_UART_Transmit_DMA(&huart1, pdata, Length);
}

__weak void user_uart1_dma_receive_process(unsigned char *data, unsigned short length)
{
  if (length > 0)
  {
    USER_EchoLog("Rec:%s;len:%d\n", data, length);
  }
}

//串口接收空闲中断
void user_uart1_dma_receive_idle(void)
{
  uint32_t temp;

  if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET))
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    HAL_UART_DMAStop(&huart1);
    temp = huart1.hdmarx->Instance->NDTR;
    user_uart_receive.rx_len =  RECEIVE_LENGTH - temp;
    user_uart_receive.receive_flag = 1;
    HAL_UART_Receive_DMA(&huart1, user_uart_receive.usartDMA_rxBuf, RECEIVE_LENGTH);
  }

  // 打印过程中，接收到gcode指令后，返回ok申请下一条
  // 直到当前指令条数为BUFSIZE-1，暂停请求
  if (user_uart_receive.receive_flag) //如果产生了空闲中断
  {
    user_uart1_dma_receive_process(user_uart_receive.usartDMA_rxBuf, user_uart_receive.rx_len);
  }
}

//DMA发送完成中断回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  __HAL_DMA_DISABLE(huart->hdmatx);
  user_uart_receive.dmaSend_flag = USART_DMA_SENDOVER;
}
#endif // ENABLE_UART1_DMA

//===========================================================================
//===========加入以下代码,支持printf函数,而不需要选择use MicroLIB============
//===========================================================================

#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
  int handle;

};
/* FILE is typedef’ d in stdio.h. */
FILE __stdout;
void _sys_exit(int x)
{
  x = x;
}
void _ttywrch(int ch)
{
  ch = ch;
}
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  #if defined(USE_BOOT) || defined(USE_APP1)

  while ((USART1->SR & 0x40) == 0); //循环发送，直到发送完毕

  USART1->DR = (uint8_t)ch; //发送数据
  #elif defined(USE_APP2)
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  #endif
  return ch;
}
#endif // ENABLE_UART1



