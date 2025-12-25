/*
 * iwdg.h
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#ifndef INC_IWDG_H_
#define INC_IWDG_H_

#include "hw_def.h"
#include "stm32f1xx_ll_iwdg.h"

#ifdef _USE_HW_IWDG
void MX_IWDG_Init(void);
void IWDG_Feed(void);
#endif

#endif /* INC_IWDG_H_ */
