/*
 * hw_def.h
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#ifndef INC_HW_DEF_H_
#define INC_HW_DEF_H_

#include "def.h"
#include "bsp.h"

#define FLASH_FW_SIZE                     (112*0x400)  // 96KB
#define FLASH_FW_ADDR_START               0x08004000
#define FLASH_FW_ADDR_END                 (FLASH_FW_ADDR_START + FLASH_FW_SIZE)

#define _USE_HW_RTC
#define _USE_HW_DMA
#define _USE_HW_IWDG
#define _USE_HW_USART
#define _USE_HW_YMODEM

#define USART_MAX_CH 1
#define USART_RX_BUF_LEN 256

enum DEF_UART
{
  _DEF_UART2, _DEF_UART3
};

#define _USE_HW_CLI
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_CMD_LIST_MAX    16
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    32

#endif /* INC_HW_DEF_H_ */
