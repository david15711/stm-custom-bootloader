/*
 * bsp.h
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#ifndef INC_BSP_H_
#define INC_BSP_H_

#include "def.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_iwdg.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_it.h"

//#include "stm32f1xx_ll_dma.h"
//#include "stm32f1xx_ll_iwdg.h"
//#include "stm32f1xx_ll_rcc.h"
//#include "stm32f1xx_ll_bus.h"
//#include "stm32f1xx_ll_system.h"
//#include "stm32f1xx_ll_exti.h"
//#include "stm32f1xx_ll_cortex.h"
//#include "stm32f1xx_ll_utils.h"
//#include "stm32f1xx_ll_rtc.h"
//#include "stm32f1xx_ll_usart.h"
//#include "stm32f1xx_ll_gpio.h"

void bsp_Init(void);
void delay(uint32_t ms);
uint32_t get_milis(void);

#endif /* INC_BSP_H_ */
