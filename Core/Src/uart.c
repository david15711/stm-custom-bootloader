/*
 * usart.c
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#include <uart.h>
#include "dma.h"
#include "qbuffer.h"

uint8_t DMA_buffer[USART_RX_BUF_LEN];
qbuffer_t qbuffer[USART_MAX_CH];
uint8_t second_buffer[USART_RX_BUF_LEN];

void DMA1_Channel6_IRQHandler(void)
{
}

void USART2_IRQHandler(void) // IDLE Interrupt handle.
{
  LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); // debug
  if( (USART2->SR & USART_SR_IDLE) )
  {
    static uint32_t last_pos = 0;
    uint32_t new_pos = USART_RX_BUF_LEN - DMA1_Channel6->CNDTR; // 전체 길이 뺴기 남은 할당량 = 지나온 길이
    uint32_t receive_len = (new_pos - last_pos + USART_RX_BUF_LEN) % USART_RX_BUF_LEN;
    qbufferWrite(qbuffer, &DMA_buffer[last_pos], receive_len);

//    DEBUG
//    uint8_t temp[256] =
//    { 0 };
//    uint32_t read_len;
//    read_len = qbufferRead(qbuffer, temp, 256);
//    uart_printf("%d %d %d %s\r\n", last_pos, new_pos, read_len, temp);

    last_pos = new_pos;
  }
  __IO uint32_t tmpreg = USART2->DR;  // DR 읽기 (이 시점에 플래그 클리어됨)
  (void)tmpreg;
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
void uart_Init(void)
{
  /* USER CODE BEGIN USART2_Init 0 */
  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct =
  { 0 };
  LL_GPIO_InitTypeDef GPIO_InitStruct =
  { 0 };
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
   PA2   ------> USART2_TX
   PA3   ------> USART2_RX
   */
  GPIO_InitStruct.Pin = USART_TX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(USART_TX_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USART_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  LL_GPIO_Init(USART_RX_GPIO_Port, &GPIO_InitStruct);

  /* USART2 DMA Init */
  /* USART2_RX Init */

  /* USER CODE BEGIN USART2_Init 1 */
  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_EnableDMAReq_RX(USART2);
  LL_USART_EnableIT_IDLE(USART2);
//LL_USART_EnableIT_RXNE(USART2); // debug
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */

  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  qbufferCreate(qbuffer, second_buffer, USART_RX_BUF_LEN);
}

uint8_t uart_ReadChar(void)
{
  uint8_t c;
  if( qbufferRead(qbuffer, &c, 1) ) return c;
  else return 0;
}

uint32_t uart_ReadString(uint8_t* buf, uint32_t len)
{
  return qbufferRead(qbuffer, buf, len);
}

void uart_WriteChar(uint8_t c)
{
  while( !(USART2->SR & USART_SR_TXE) );
  USART2->DR = c;
}

uint32_t uart_WriteString(uint8_t* buf, uint32_t len)
{
  for(int i = 0; i < len; i++)
  {
    uart_WriteChar(buf[i]);
  }
  return len;
}

void uart_printf(const char* fmt, ...)
{
  char buf[128]; // 스택 절약을 위해 버퍼 사이즈 조절
  va_list args;

  va_start(args, fmt);
  int len = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  if( len > 0 )
  {
    uart_WriteString((uint8_t*)buf, (uint32_t)len);
  }
}

#if 0
__STATIC_INLINE uint8_t* get_write_ptr(const sl_iostream_uart_context_t * uart_context)
{
  uint8_t* dst;

  #if defined(DMA_PRESENT)
  int remaining;
  Ecode_t ecode;
  ecode = DMADRV_TransferRemainingCount(uart_context->dma.channel, &remaining);
  EFM_ASSERT(ecode == ECODE_OK);

  DMA_DESCRIPTOR_TypeDef* desc = ((DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE)) + uart_context->dma.channel;
  dst = ((uint8_t*)desc->DSTEND + 1) - remaining;

  #elif defined(LDMA_PRESENT)
  dst = (uint8_t *)LDMA->CH[uart_context->dma.channel].DST;
  #endif

  // Check for buffer over/underflow
  EFM_ASSERT(dst <= (uart_context->DMA_bufferfer + uart_context->DMA_bufferfer_len)
             && dst >= uart_context->DMA_bufferfer);

  // Wrap dst around
  if (dst == (uart_context->DMA_bufferfer + uart_context->DMA_bufferfer_len)) {
    dst = uart_context->DMA_bufferfer;
  }

  return dst;
}


write_ptr = get_write_ptr(uart_context);
  }

  // Compute available space
  if (uart_context->rx_read_ptr >= write_ptr) {
    available_space = (size_t)(uart_context->rx_read_ptr - write_ptr);
  } else {
    available_space = (size_t)((uart_context->DMA_bufferfer + uart_context->DMA_bufferfer_len) - write_ptr);
  }

  // Space available in the RX buffer
  if (available_space > 0) {
    // Start the DMA transfer
    ecode = DMADRV_PeripheralMemory(chan,
                                    uart_context->dma.cfg.peripheral_signal,
                                    write_ptr,
                                    uart_context->dma.cfg.src,
                                    true,
                                    available_space,
                                    dmadrvDataSize1,
                                    dma_irq_handler,
                                    uart_context);
    EFM_ASSERT(ecode == ECODE_OK);
  }
  // RX buffer is full
  else {
    uart_context->DMA_bufferfer_full = true;

    // Send first XOFF. Subsequent XOFFs will be sent by the stream's IRQ handler.
    if (uart_context->sw_flow_control) {
      sl_atomic_store(uart_context->remote_xon, false);
      uart_context->tx(uart_context, UARTXOFF);
      // Enable the RXDATAV IRQ to check if we receive a control character when buffer is full
      uart_context->set_next_byte_detect(uart_context);
    }
  }
#endif
