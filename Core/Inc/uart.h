/*
 * usart.h
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "hw_def.h"

#ifdef _USE_HW_USART
void uart_Init(void);
uint8_t uart_ReadChar(void);
uint32_t uart_ReadString(uint8_t* buf, uint32_t len);
void uart_WriteChar(uint8_t c);
uint32_t uart_WriteString(uint8_t* buf, uint32_t len);
void uart_printf(const char* fmt, ...);
#endif

#endif /* INC_UART_H_ */
