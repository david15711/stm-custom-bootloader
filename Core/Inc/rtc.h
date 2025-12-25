/*
 * rtc.h
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "hw_def.h"
#include "stm32f1xx_ll_rtc.h"

#ifdef _USE_HW_RTC

void MX_RTC_Init(void);
void RTC_BKP_Set(uint32_t index, uint32_t data);
uint32_t RTC_BKP_Get(uint32_t index);

#endif
#endif /* INC_RTC_H_ */
