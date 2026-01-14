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
bool uart_Init(int ch, int baud);
bool uart_isOpen(int ch);
uint8_t uart_ReadChar(int ch);
uint32_t uart_ReadString(int ch, uint8_t* buf, uint32_t len);
void uart_WriteChar(int ch, uint8_t c);
uint32_t uart_WriteString(int ch, uint8_t* buf, uint32_t len);
void uart_printf(int ch, const char* fmt, ...);
#endif

#endif /* INC_UART_H_ */
